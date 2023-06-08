#ifndef WEBSOCK_SERVER_H
#define WEBSOCK_SERVER_H

#include "geoff.h"
#include "AppBuffer.h"
#include "WebsockSession.h"
#include "WebsockSessionManager.h"
#define gameAppVersion 2

class WebsockServer
{
public:
	static WebsockServer& GetInstance();
	typedef std::function<void(uint32_t tick)> OnTimerTickCallback_t;
	typedef std::function<void(uint32_t sessionId)> OnAcceptCallback_t;

	void IoContext(net::io_context* ioc);
	net::io_context* IoContext();

	void OnAccept(OnAcceptCallback_t);
	void OnClose(uint32_t sessionID);

	void CommsHandler(uint32_t sessionID, beast::flat_buffer buffer, std::size_t bytes_transferred);
	std::shared_ptr<WebsockSession> FindSessionByID(uint32_t sessionID);

	void SetTimerInterval(uint32_t interval_in_us);
	uint32_t GetTimerTick();
	void TimerHandler();
	void OnTimerTick(uint32_t count);
	void ShutdownTimer();
	void AddTimerTickCallback(OnTimerTickCallback_t fn);

private:
	// I know what you're thinking: WTF is this?  It's Magic Statics!
	// https://blog.mbedded.ninja/programming/languages/c-plus-plus/magic-statics/
	// 
	// I tried to figure out how to pass a WebsockServer object into the Beast code, and fully face-planted.
	// So I said fuckit, I'm using a Singleton.
	WebsockServer();
	~WebsockServer();

	// You can only get a WebsockServer instance through WebsockServer::GetInstance(), all default constructors hereby deleted.
	WebsockServer(const WebsockServer&) = delete;
	WebsockServer& operator=(const WebsockServer&) = delete;
	WebsockServer(WebsockServer&&) = delete;
	WebsockServer& operator=(WebsockServer&&) = delete;

	std::recursive_mutex m_serverMutex;
	net::io_context *m_ioc;

	uint32_t m_interval_in_us;
	uint32_t m_tick_count;
	bool m_run_timer;
	bool m_timer_complete;
	net::deadline_timer* m_tick_timer;
	std::vector<OnTimerTickCallback_t> m_timer_callbacks;
};

#endif
