#include "GameSession.h"
#include "WebsockServer.h"

//#define GS_TRACE TRACE
#define GS_TRACE(...)

std::ostream& operator<<(std::ostream& os, const GameSession& gs)
{
	os << std::endl << "-----------------" << std::endl;
	os << "SessionID: " << gs.m_sessionID << std::endl;
	os << "-----------------";
	return os;
}

GameSession::GameSession(uint32_t sessionID, boost::beast::tcp_stream& stream)
	: WebsockSession(sessionID, stream)
{
	GS_TRACE("sessionID: " << SessionID());
}

GameSession::~GameSession()
{
	GS_TRACE("sessionID: " << SessionID());
}

void GameSession::HandleResizeEvent(AppBuffer& rxBuffer)
{
	(void)rxBuffer;
	GS_TRACE("");
}

// Echo New Session data back to client
void GameSession::HandleNewSession(AppBuffer& rxBuffer)
{
	GS_TRACE("NewSession: " << SessionID());
	auto txBuffer = std::make_unique<AppBuffer>(12, rxBuffer.isLittleEndian());

	txBuffer->set_uint8(0xBB);
	txBuffer->set_uint8(static_cast<uint8_t>(MessageType_t::SessionRegistered));
	txBuffer->set_uint32(SessionID());
	txBuffer->set_uint16((uint16_t)GAME_APP_VERSION);

	CommitTxBuffer(txBuffer);
}

// Echo Key events back to client
void GameSession::HandleKeyEvent(AppBuffer& rxBuffer) 
{
	// preserve rxBuffer state for possible additional handlers
	// (ie read data without altering its m_readOffset)
	assert(rxBuffer.size() >= 3);

	bool isDown = rxBuffer.get_uint8(1);
	int keyCode = rxBuffer.get_uint8(2);

	auto txBuffer = std::make_unique <AppBuffer>(8, rxBuffer.isLittleEndian());

	txBuffer->set_uint8(0xBB);
	txBuffer->set_uint8(static_cast<uint8_t>(MessageType_t::KeyMessage));
	txBuffer->set_uint32(SessionID());
	txBuffer->set_uint8(isDown ? 1 : 0);
	txBuffer->set_uint8(static_cast<uint8_t>(keyCode));

	CommitTxBuffer(txBuffer);
};

// Echo Click events back to client.
void GameSession::HandleClickEvent(AppBuffer& rxBuffer) 
{
	// preserve rxBuffer state for possible additional handlers
	// (ie read data without altering its m_readOffset)
	assert(rxBuffer.size() >= 11);

	uint32_t rxSessionID = rxBuffer.get_uint32(1);
	uint16_t clickX = rxBuffer.get_uint16(5);
	uint16_t clickY = rxBuffer.get_uint16(7);

	assert(rxSessionID == SessionID());

	auto txBuffer = std::make_unique <AppBuffer>(12, rxBuffer.isLittleEndian());

	txBuffer->set_uint8(0xBB);
	txBuffer->set_uint8(static_cast<uint8_t>(MessageType_t::ClickMessage));
	txBuffer->set_uint32(SessionID());
	txBuffer->set_uint16(clickX);
	txBuffer->set_uint16(clickY);

	CommitTxBuffer(txBuffer);
};

void GameSession::CommsHandler(AppBuffer & rxBuffer)
{
	// non-destructive read of requestType, keep rxBuffer state intact
	// for sake of additional handlers that *may* be called.
	auto requestType = static_cast<RequestType_t>(rxBuffer.get_uint8(0));
	
	//GS_TRACE("Request Type: " << (int)requestType);

	switch (requestType)
	{
		case RequestType_t::RegisterSession:
			HandleNewSession(rxBuffer);
			break;

		case RequestType_t::ClickEvent:
			HandleClickEvent(rxBuffer);
			break;

		case RequestType_t::KeyEvent:
			HandleKeyEvent(rxBuffer);
			break;

		case RequestType_t::ResizeEvent:
			HandleResizeEvent(rxBuffer);
			break;
	}
}
