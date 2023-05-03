#ifndef WEBSOCK_SESSION_H
#define WEBSOCK_SESSION_H

#include <list>
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>

#include "AppBuffer.h"

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
	bool IsLittleEndian() { return m_isLittleEndian; }

	void CommsHandler(beast::flat_buffer buffer, std::size_t bytes_transferred);
	void CommitTxBuffer(std::unique_ptr<AppBuffer>& buffer);
	bool GetNextTxBuffer(std::unique_ptr<AppBuffer>& buffer);

private:
	void RegisterNewSession(AppBuffer& rxBuffer);
	void HandleClickEvent(AppBuffer& rxBuffer);
	void HandleKeyEvent(AppBuffer& rxBuffer);

	uint32_t m_sessionID;
	bool m_isLittleEndian;

	std::deque<std::unique_ptr<AppBuffer>> m_txQue;
	std::recursive_mutex m_session_mutex;
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

	void TimerTick();

	std::thread m_thread;
	bool m_thread_running;
	uint32_t m_tick_count;
};

#endif // WEBSOCK_SESSION_H
