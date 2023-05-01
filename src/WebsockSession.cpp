#include "geoff.h"
#include "WebsockSession.h"

WebsockSession::WebsockSession(uint32_t sessionID) :
	m_sessionID(sessionID)
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

void WebsockSession::comms_handler(const uint8_t* buff, const size_t length)
{
	std::cout << __FUNCTION__ << "(" << __LINE__ << ") : " << m_sessionID << ", length: " << length << std::endl;
}

void WebsockSession::set_little_endian(bool littleEndian)
{
	m_isLittleEndian = littleEndian;
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
		if (client->session_id() == sessionID)
			return client;
	}

	return nullptr;
}
