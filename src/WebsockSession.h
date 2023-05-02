#ifndef WEBSOCK_SESSION_H
#define WEBSOCK_SESSION_H

#include <list>
#include <cstdint>
#include <memory>
#include <mutex>

class WebsockSession
{
public:
	enum class RequestType_t
	{
		RegisterSession = 0x00,
		ClickEvent = 0x02,
		KeyEvent = 0x04
	};

	WebsockSession(uint32_t sessionID);
	virtual ~WebsockSession();

	uint32_t SessionID();
	virtual void CommsHandler(const uint8_t *buff, const size_t length);

private:
	void RegisterNewSession(AppBuffer& rxBuffer);
	void HandleClickEvent(AppBuffer& rxBuffer);
	void HandleKeyEvent(AppBuffer& rxBuffer);

	uint32_t m_sessionID;
};

class WebsockSessionManager
{
public:
	WebsockSessionManager();
	virtual ~WebsockSessionManager();

	virtual void add_session(uint32_t sessionID);

	std::shared_ptr<WebsockSession> find_by_id(uint32_t sessionID);
	void delete_by_id(uint32_t sessionID);

private:
	std::mutex m_sessions_mutex;
	std::list<std::shared_ptr<WebsockSession>> m_sessions;
};

#endif // WEBSOCK_SESSION_H
