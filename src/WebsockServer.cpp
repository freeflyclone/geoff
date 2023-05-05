#include "WebsockServer.h"
#include "GameSession.h"

#include <ios>
#include <iostream>

namespace {
	static WebsockSessionManager<GameSession> m_sessions;
};

WebsockServer::WebsockServer() :
	m_serverMutex(),
	m_ioc(nullptr)
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

void WebsockServer::IoContext(net::io_context* ioc)
{
	m_ioc = ioc;
}

net::io_context* WebsockServer::IoContext()
{
	return m_ioc;
}

void WebsockServer::OnAccept(OnAcceptCallback_t fn)
{
	fn(m_sessions.add_session());
}

void WebsockServer::OnClose(uint32_t sessionID)
{
	m_sessions.delete_by_id(sessionID);
}

void WebsockServer::CommsHandler(uint32_t sessionID, beast::flat_buffer in_buffer, std::size_t in_length)
{
	const std::lock_guard<std::recursive_mutex> lock(m_serverMutex);

	auto session = m_sessions.find_by_id(sessionID);

	if (!session)
		return;

	session->WebsockSession::CommsHandler(in_buffer, in_length);
}

std::shared_ptr<WebsockSession> WebsockServer::FindSessionByID(uint32_t sessionID)
{
	return m_sessions.find_by_id(sessionID);
}
