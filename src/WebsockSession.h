#ifndef WEBSOCK_SESSION_H
#define WEBSOCK_SESSION_H

#include <list>
#include <cstdint>
#include <memory>
#include <mutex>

class WebsockSession
{
public:
	WebsockSession(uint32_t sessionID, bool isLittleEndian, uint16_t clientAppVersion);
	~WebsockSession();

	uint32_t session_id();

private:
	uint32_t m_sessionID;
	bool m_isLittleEndian;
	uint16_t m_clientAppVersion;
};

class WebsockSessionManager
{
public:
	WebsockSessionManager();
	~WebsockSessionManager();

	void add_client(uint32_t sessionID, bool isLittleEndian, uint16_t clientAppVersion);

	std::shared_ptr<WebsockSession> find_session_by_id(uint32_t sessionID);
	void delete_session_by_id(uint32_t sessionID);

private:
	std::mutex m_sessions_mutex;
	std::list<std::shared_ptr<WebsockSession>> m_sessions;
};

#endif // WEBSOCK_SESSION_H
