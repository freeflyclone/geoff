#include "WebsockServer.h"

#include <ios>
#include <iostream>

WebsockServer::WebsockServer() :
	m_playersMutex(),
	m_sessionID(0),
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
	m_sessions.add_session(m_sessionID);
	fn(m_sessionID);

	m_sessionID++;
	m_sessionID &= 0xFFFFFFFF;
}

void WebsockServer::OnClose(uint32_t sessionID)
{
	m_sessions.delete_by_id(sessionID);
}

void WebsockServer::CommsHandler(uint32_t sessionID, beast::flat_buffer in_buffer, std::size_t in_length)
{
	const std::lock_guard<std::mutex> lock(m_playersMutex);

	auto session = m_sessions.find_by_id(sessionID);

	if (!session)
		return;

	uint8_t* buff = (uint8_t*)static_cast<net::const_buffer>(in_buffer.data()).data();

	session->CommsHandler(buff, in_length);
}

void WebsockServer::CommitTxBuffer(std::shared_ptr<AppBuffer> buffer)
{
	m_txQue.push_back(buffer);
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
