#include "GameSession.h"

GameSession::GameSession(uint32_t sessionID)
	: WebsockSession(sessionID)
{
	//TRACE("sessionID: " << m_wss.SessionID());

	// Echo New Session data back to client
	AddRegisterNewSessionHandler([this](AppBuffer& rxBuffer) {
		auto txBuffer = std::make_unique<AppBuffer>(12, rxBuffer.isLittleEndian());

		txBuffer->set_uint8(0xBB);
		txBuffer->set_uint8(0x01);
		txBuffer->set_uint32(SessionID());
		txBuffer->set_uint16((uint16_t)GAME_APP_VERSION);

		CommitTxBuffer(txBuffer);

		StartTimer();
	});

	// Echo Key events back to client
	AddKeyEventHandler([this](AppBuffer& rxBuffer) {
		bool isDown = (rxBuffer.get_uint8() == 1) ? true : false;
		int keyCode = rxBuffer.get_uint8();

		auto txBuffer = std::make_unique <AppBuffer>(8, rxBuffer.isLittleEndian());

		txBuffer->set_uint8(0xBB);
		txBuffer->set_uint8(0x05);
		txBuffer->set_uint32(SessionID());
		txBuffer->set_uint8(isDown ? 1 : 0);
		txBuffer->set_uint8(static_cast<uint8_t>(keyCode));

		CommitTxBuffer(txBuffer);
	});

	// Echo Click events back to client.
	AddClickEventHandler([this](AppBuffer& rxBuffer) {
		uint32_t rxSessionID = rxBuffer.get_uint32();
		uint16_t playerID = rxBuffer.get_uint16();
		uint16_t clickX = rxBuffer.get_uint16();
		uint16_t clickY = rxBuffer.get_uint16();

		auto txBuffer = std::make_unique <AppBuffer>(12, rxBuffer.isLittleEndian());

		txBuffer->set_uint8(0xBB);
		txBuffer->set_uint8(0x03);
		txBuffer->set_uint32(SessionID());
		txBuffer->set_uint16(playerID);
		txBuffer->set_uint16(clickX);
		txBuffer->set_uint16(clickY);

		CommitTxBuffer(txBuffer);
	});

	// maybe log timer ticks
	AddTimerTickHandler([this]() {
		//TRACE("sessionID: " << SessionID() << ", tick: " << m_timer_tick);
	});

	// Set interval to 60FPS
	SetIntervalInUs(16667);
}

GameSession::~GameSession()
{
	//TRACE("sessionID: " << m_wss.SessionID());
}

void GameSession::AddRegisterNewSessionHandler(AppBufferProcessor_t fn)
{
	m_newSessionHandlers.push_back(fn);
}

void GameSession::AddClickEventHandler(AppBufferProcessor_t fn)
{
	m_clickEventHandlers.push_back(fn);
}

void GameSession::AddKeyEventHandler(AppBufferProcessor_t fn)
{
	m_keyEventHandlers.push_back(fn);
}

void GameSession::AddTimerTickHandler(TimerTickCallback_t fn)
{
	m_timerTickHandlers.push_back(fn);
}

void GameSession::CommsHandler(AppBuffer & rxBuffer)
{
	auto requestType = static_cast<RequestType_t>(rxBuffer.get_uint8());

	switch (requestType)
	{
		case RequestType_t::RegisterSession:
			for (auto fn : m_newSessionHandlers)
				fn(rxBuffer);
			break;

		case RequestType_t::ClickEvent:
			for (auto fn : m_clickEventHandlers)
				fn(rxBuffer);
			break;

		case RequestType_t::KeyEvent:
			for (auto fn : m_keyEventHandlers)
				fn(rxBuffer);
			break;
	}
}

void GameSession::OnTimerTick() {
	WebsockSession::OnTimerTick();

	for (auto fn : m_timerTickHandlers)
		fn();
}
