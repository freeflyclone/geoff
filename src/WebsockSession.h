#ifndef WEBSOCK_SESSION_H
#define WEBSOCK_SESSION_H

#include <list>
#include <cstdint>
#include <memory>
#include <mutex>

class WebsockSession
{
public:
	WebsockSession(uint32_t sessionID);
	~WebsockSession();

	uint32_t session_id();
	void set_little_endian(const bool littleEndian);

	void comms_handler(const uint8_t *buff, const size_t length);

private:
	uint32_t m_sessionID;
	bool m_isLittleEndian;
};

class WebsockSessionManager
{
public:
	WebsockSessionManager();
	~WebsockSessionManager();

	void add_session(uint32_t sessionID);

	std::shared_ptr<WebsockSession> find_by_id(uint32_t sessionID);
	void delete_by_id(uint32_t sessionID);

private:
	std::mutex m_sessions_mutex;
	std::list<std::shared_ptr<WebsockSession>> m_sessions;
};

#endif // WEBSOCK_SESSION_H
