#ifndef WEBSOCK_SESSION_H
#define WEBSOCK_SESSION_H

#include <list>
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>

#include "AppBuffer.h"

class GameSession;

class WebsockSession
{
public:
	// used to initiate beast::async_write() when new TX buffer is ready
	// that isn't a response to a query from the client side.
	typedef std::function<void(WebsockSession &)> OnTxReadyCallback_t;

	enum class RequestType_t
	{
		RegisterSession = 0x00,
		ClickEvent = 0x02,
		KeyEvent = 0x04
	};

	WebsockSession(uint32_t sessionID);
	virtual ~WebsockSession();

	// Allow caller to specify a callback when a new AppBuffer for TX is ready
	void OnTxReady(OnTxReadyCallback_t);

	uint32_t SessionID();
	bool IsLittleEndian() { return m_isLittleEndian; }

	void CommsHandler(beast::flat_buffer buffer, std::size_t bytes_transferred);
	void CommitTxBuffer(std::unique_ptr<AppBuffer>& buffer);
	bool GetNextTxBuffer(std::unique_ptr<AppBuffer>& buffer);

	void StartTimer();
	void SetIntervalInUs(uint32_t interval);

private:
	virtual void CommsHandler(const uint8_t* buff, const size_t length);

	// Call the TX ready callback specified by the caller
	void OnTxReady(WebsockSession&);

	uint32_t m_sessionID;
	bool m_isLittleEndian;

	std::deque<std::unique_ptr<AppBuffer>> m_txQue;
	std::recursive_mutex m_session_mutex;

	std::unique_ptr<net::deadline_timer> m_timer;
	bool m_run_timer;
	int m_timer_complete;
	uint32_t m_timer_tick;
	uint32_t m_tick_interval_in_us;
	void TimerTick();
	OnTxReadyCallback_t m_tx_ready_callback;

	std::unique_ptr<GameSession> m_game_session;
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
