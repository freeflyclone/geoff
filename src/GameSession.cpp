#include "GameSession.h"

GameSession::GameSession(WebsockSession& ws)
	:
	m_wss(ws)
{
	TRACE("sessionID: " << m_wss.SessionID());
}

GameSession::~GameSession()
{
	TRACE("sessionID: " << m_wss.SessionID());
}

void GameSession::CommsHandler(AppBuffer & rxBuffer)
{
	auto requestType = static_cast<WebsockSession::RequestType_t>(rxBuffer.get_uint8());

	switch (requestType)
	{
		case WebsockSession::RequestType_t::RegisterSession:
			RegisterNewSession(rxBuffer);
			break;

		case WebsockSession::RequestType_t::ClickEvent:
			HandleClickEvent(rxBuffer);
			break;

		case WebsockSession::RequestType_t::KeyEvent:
			HandleKeyEvent(rxBuffer);
			break;
	}
}

void GameSession::RegisterNewSession(AppBuffer& rxBuffer)
{
	//TRACE(", sessionID: " << m_sessionID);

	auto txBuffer = std::make_unique<AppBuffer>(12, rxBuffer.isLittleEndian());

	txBuffer->set_uint8(0xBB);
	txBuffer->set_uint8(0x01);
	txBuffer->set_uint32(m_wss.SessionID());
	txBuffer->set_uint16((uint16_t)GAME_APP_VERSION);

	m_wss.CommitTxBuffer(txBuffer);

	m_wss.StartTimer();
}

void GameSession::HandleKeyEvent(AppBuffer& rxBuffer)
{
	//TRACE(", sessionID: " << m_sessionID);
	bool isDown = (rxBuffer.get_uint8() == 1) ? true : false;
	int keyCode = rxBuffer.get_uint8();

	auto txBuffer = std::make_unique <AppBuffer>(8, rxBuffer.isLittleEndian());

	txBuffer->set_uint8(0xBB);
	txBuffer->set_uint8(0x05);
	txBuffer->set_uint32(m_wss.SessionID());
	txBuffer->set_uint8(isDown ? 1 : 0);
	txBuffer->set_uint8(static_cast<uint8_t>(keyCode));

	m_wss.CommitTxBuffer(txBuffer);
}

void GameSession::HandleClickEvent(AppBuffer& rxBuffer)
{
	//TRACE(", sessionID: " << m_sessionID);

	uint32_t rxSessionID = rxBuffer.get_uint32();
	uint16_t playerID = rxBuffer.get_uint16();
	uint16_t clickX = rxBuffer.get_uint16();
	uint16_t clickY = rxBuffer.get_uint16();

	auto txBuffer = std::make_unique <AppBuffer>(12, rxBuffer.isLittleEndian());

	txBuffer->set_uint8(0xBB);
	txBuffer->set_uint8(0x03);
	txBuffer->set_uint32(m_wss.SessionID());
	txBuffer->set_uint16(playerID);
	txBuffer->set_uint16(clickX);
	txBuffer->set_uint16(clickY);

	m_wss.CommitTxBuffer(txBuffer);
}
