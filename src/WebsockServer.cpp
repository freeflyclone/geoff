#include "WebsockServer.h"

#include <ios>
#include <iostream>

WebsockServer::WebsockServer() :
	m_playersMutex(),
	m_sessionID(0),
	m_mapWidth(4096),
	m_mapHeight(2048),
	m_sessions()
{

	srand(12345);

	std::cout << "geoff server ver " << gameAppVersion << " running\n";
}

WebsockServer::~WebsockServer()
{
}

// https://blog.mbedded.ninja/programming/languages/c-plus-plus/magic-statics/
WebsockServer& WebsockServer::GetInstance()
{
	static WebsockServer g;
	return g;
}

void WebsockServer::OnAccept(OnAcceptCallback_t fn)
{
	fn(m_sessionID++);
	m_sessionID &= 0xFFFFFFFF;
}

void WebsockServer::OnClose(uint32_t sessionID)
{
	m_sessions.delete_session_by_id(sessionID);
}

void WebsockServer::RegisterNewClientConnection(uint32_t sessionID, AppBuffer & rxBuffer)
{
	uint16_t clientAppVersion = rxBuffer.get_uint16();

	m_sessions.add_client(sessionID, rxBuffer.isLittleEndian(), clientAppVersion);

	auto txBuffer = std::make_shared<AppBuffer>(12, rxBuffer.isLittleEndian());

	txBuffer->set_uint8(0xBB);
	txBuffer->set_uint8(0x09);
	txBuffer->set_uint32(sessionID);
	txBuffer->set_uint16((uint16_t)gameAppVersion);
	txBuffer->set_uint16(m_mapWidth);
	txBuffer->set_uint16(m_mapHeight);

	m_txQue.push_back(txBuffer);
}

void WebsockServer::HandleKeyEvent(uint32_t sessionID, AppBuffer & rxBuffer)
{
	bool isDown = (rxBuffer.get_uint8() == 1) ? true : false;
	int keyCode = rxBuffer.get_uint8();

	// don't respond to unmapped keys. See "keyMap" in GameFace.js
	if (keyCode == 0)
		return;

	auto txBuffer = std::make_shared <AppBuffer>(8, rxBuffer.isLittleEndian());

	txBuffer->set_uint8(0xBB);
	txBuffer->set_uint8(0x15);
	txBuffer->set_uint32(sessionID);
	txBuffer->set_uint8(isDown ? 1 : 0);
	txBuffer->set_uint8(static_cast<uint8_t>(keyCode));

	m_txQue.push_back(txBuffer);
}

void WebsockServer::HandleClickEvent(uint32_t sessionID, AppBuffer& rxBuffer)
{
	uint32_t rxSessionID = rxBuffer.get_uint32();
	uint16_t playerID = rxBuffer.get_uint16();
	uint16_t clickX = rxBuffer.get_uint16();
	uint16_t clickY = rxBuffer.get_uint16();

	if (rxSessionID != sessionID)
	{
		std::cout << "Oops: rxSessionID != sessionID: " << rxSessionID << " vs " << sessionID << std::endl;
		return;
	}

	auto txBuffer = std::make_shared <AppBuffer>(12, rxBuffer.isLittleEndian());

	txBuffer->set_uint8(0xBB);
	txBuffer->set_uint8(0x13);
	txBuffer->set_uint32(sessionID);
	txBuffer->set_uint16(playerID);
	txBuffer->set_uint16(clickX);
	txBuffer->set_uint16(clickY);

	m_txQue.push_back(txBuffer);
}

void WebsockServer::CommsHandler(uint32_t sessionID, beast::flat_buffer in_buffer, std::size_t in_length)
{
	const std::lock_guard<std::mutex> lock(m_playersMutex);

	if (in_length < 2)
		return;

	uint8_t* buff = (uint8_t*)static_cast<net::const_buffer>(in_buffer.data()).data();

	if (buff[0] == 0xAA || buff[0] == 0xAB)
	{
		bool isLittleEndian = buff[0] == 0xAB ? true : false;
		WebsockServer::RequestType_t request = static_cast<WebsockServer::RequestType_t>(buff[1]);

		// skip header
		buff += 2;
		in_length -= 2;

		AppBuffer rxBuffer = AppBuffer(buff, in_length, isLittleEndian);

		switch (request)
		{
			case RegisterClient:
				RegisterNewClientConnection(sessionID, rxBuffer);
				break;

			case ClickEvent:
				HandleClickEvent(sessionID, rxBuffer);
				break;

			case KeyEvent:
				HandleKeyEvent(sessionID, rxBuffer);
				break;
		}
	}
}

bool WebsockServer::GetNextTxBuffer(std::shared_ptr<AppBuffer> & buff)
{
	if (m_txQue.empty())
		return false;

	// Get next AppBuffer from TX que
	buff = m_txQue.front();
	m_txQue.pop_front();

	return true;
}
