#ifndef WEBSOCK_SESSION_H
#define WEBSOCK_SESSION_H

#include <list>
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>

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

	void TimerTick();

	uint32_t m_sessionID;
	bool m_isLittleEndian;
	std::thread m_thread;
	bool m_thread_done;
};

class WebsockSessionManager
{
public:
	WebsockSessionManager();
	virtual ~WebsockSessionManager();

	virtual uint32_t add_session();

	std::shared_ptr<WebsockSession> find_by_id(uint32_t sessionID);
	void delete_by_id(uint32_t sessionID);

private:
	std::mutex m_sessions_mutex;
	uint32_t m_session_id;
	std::list<std::shared_ptr<WebsockSession>> m_sessions;
};

#endif // WEBSOCK_SESSION_H
