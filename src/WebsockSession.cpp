#include "geoff.h"
#include "WebsockSession.h"

WebsockSession::WebsockSession(uint32_t sessionID, bool isLittleEndian, uint16_t clientAppVersion) :
	m_sessionID(sessionID),
	m_isLittleEndian(isLittleEndian),
	m_clientAppVersion(clientAppVersion)
{
	std::cout << __FUNCTION__ << "(" << __LINE__ << ") : " << m_sessionID << std::endl;
}

WebsockSession::~WebsockSession()
{
	std::cout << __FUNCTION__ << "(" << __LINE__ << ") : " << m_sessionID << std::endl;
}

uint32_t WebsockSession::session_id()
{
	return m_sessionID;
}

WebsockSessionManager::WebsockSessionManager()
	: m_sessions()
{
}

WebsockSessionManager::~WebsockSessionManager()
{
}

void WebsockSessionManager::add_client(uint32_t sessionID, bool isLittleEndian, uint16_t clientAppVersion)
{
	const std::lock_guard<std::mutex> lock(m_sessions_mutex);

	m_sessions.push_back(std::make_shared<WebsockSession>(sessionID, isLittleEndian, clientAppVersion));
}

void WebsockSessionManager::delete_session_by_id(uint32_t sessionID)
{
	const std::lock_guard<std::mutex> lock(m_sessions_mutex);

	auto client = find_session_by_id(sessionID);

	if (client)
		m_sessions.remove(client);
}

std::shared_ptr<WebsockSession> WebsockSessionManager::find_session_by_id(uint32_t sessionID)
{
	for (auto client : m_sessions)
	{
		if (client->session_id() == sessionID)
			return client;
	}

	return nullptr;
}
