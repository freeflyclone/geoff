#include "geoff.h"
#include "WebsockSession.h"
#include "WebsockServer.h"

WebsockSession::WebsockSession(uint32_t sessionID) :
	m_sessionID(sessionID)
{
}

WebsockSession::~WebsockSession()
{
}

uint32_t WebsockSession::SessionID()
{
	return m_sessionID;
}

void WebsockSession::CommsHandler(const uint8_t* buff, const size_t length)
{
	if (!buff || length < 2)
		return;

	bool isLittleEndian = buff[0] == 0xAB ? true : false;
	auto requestType = static_cast<WebsockSession::RequestType_t>(buff[1]);

	AppBuffer rxBuffer(buff+2, length-2, isLittleEndian);

	switch (requestType)
	{
		case RequestType_t::RegisterSession:
			RegisterNewSession(rxBuffer);
			break;

		case RequestType_t::KeyEvent:
			HandleKeyEvent(rxBuffer);
			break;

		case RequestType_t::ClickEvent:
			HandleClickEvent(rxBuffer);
			break;
	}
}

void WebsockSession::RegisterNewSession(AppBuffer& rxBuffer)
{
	// skip "clientAppVersion" for now.
	rxBuffer.get_uint16();

	auto txBuffer = std::make_shared<AppBuffer>(12, rxBuffer.isLittleEndian());

	txBuffer->set_uint8(0xBB);
	txBuffer->set_uint8(0x01);
	txBuffer->set_uint32(m_sessionID);
	txBuffer->set_uint16((uint16_t)gameAppVersion);

	WebsockServer::GetInstance().CommitTxBuffer(txBuffer);
}

void WebsockSession::HandleKeyEvent(AppBuffer& rxBuffer)
{
	bool isDown = (rxBuffer.get_uint8() == 1) ? true : false;
	int keyCode = rxBuffer.get_uint8();

	auto txBuffer = std::make_shared <AppBuffer>(8, rxBuffer.isLittleEndian());

	txBuffer->set_uint8(0xBB);
	txBuffer->set_uint8(0x05);
	txBuffer->set_uint32(m_sessionID);
	txBuffer->set_uint8(isDown ? 1 : 0);
	txBuffer->set_uint8(static_cast<uint8_t>(keyCode));

	WebsockServer::GetInstance().CommitTxBuffer(txBuffer);
}

void WebsockSession::HandleClickEvent(AppBuffer& rxBuffer)
{
	uint32_t rxSessionID = rxBuffer.get_uint32();
	uint16_t playerID = rxBuffer.get_uint16();
	uint16_t clickX = rxBuffer.get_uint16();
	uint16_t clickY = rxBuffer.get_uint16();

	auto txBuffer = std::make_shared <AppBuffer>(12, rxBuffer.isLittleEndian());

	txBuffer->set_uint8(0xBB);
	txBuffer->set_uint8(0x03);
	txBuffer->set_uint32(m_sessionID);
	txBuffer->set_uint16(playerID);
	txBuffer->set_uint16(clickX);
	txBuffer->set_uint16(clickY);

	WebsockServer::GetInstance().CommitTxBuffer(txBuffer);
}

WebsockSessionManager::WebsockSessionManager()
	: m_sessions()
{
}

WebsockSessionManager::~WebsockSessionManager()
{
}

void WebsockSessionManager::add_session(uint32_t sessionID)
{
	const std::lock_guard<std::mutex> lock(m_sessions_mutex);

	m_sessions.push_back(std::make_shared<WebsockSession>(sessionID));
}

void WebsockSessionManager::delete_by_id(uint32_t sessionID)
{
	const std::lock_guard<std::mutex> lock(m_sessions_mutex);

	auto client = find_by_id(sessionID);

	if (client)
		m_sessions.remove(client);
}

std::shared_ptr<WebsockSession> WebsockSessionManager::find_by_id(uint32_t sessionID)
{
	for (auto client : m_sessions)
	{
		if (client->SessionID() == sessionID)
			return client;
	}

	return nullptr;
}
