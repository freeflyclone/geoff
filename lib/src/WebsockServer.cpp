#include "WebsockServer.h"
#include "AppSession.h"

#include <ios>
#include <iostream>

using namespace Websock;

WebsockServer::WebsockServer() :
	m_serverMutex(),
	m_ioc(nullptr),
	m_interval_in_us(1000000),
	m_tick_timer(nullptr),
	m_tick_count(0),
	m_run_timer(false),
	m_timer_complete(false)
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

	if (!m_tick_timer)
	{
		// run the timer in it's own explicit strand
		m_tick_timer = new net::deadline_timer(net::make_strand(*m_ioc), boost::posix_time::microseconds(m_interval_in_us));

		m_run_timer = true;

		// initiate the timer
		TimerHandler();
	}
}

net::io_context* WebsockServer::IoContext()
{
	return m_ioc;
}

void WebsockServer::OnAccept(OnAcceptCallback_t fn)
{
	fn(g_session_manager.add_session());
}

void WebsockServer::OnClose(uint32_t sessionID)
{
	const std::lock_guard<std::recursive_mutex> lock(m_serverMutex);

	g_session_manager.delete_by_id(sessionID);
}

void WebsockServer::CommsHandler(uint32_t sessionID, beast::flat_buffer in_buffer, std::size_t in_length)
{
	const std::lock_guard<std::recursive_mutex> lock(m_serverMutex);

	auto session = g_session_manager.find_by_id(sessionID);

	if (!session)
	{
		TRACE("session is null.");
		return;
	}

	session->WebsockSession::CommsHandler(in_buffer, in_length);
}

std::shared_ptr<WebsockSession> WebsockServer::FindSessionByID(uint32_t sessionID)
{
	const std::lock_guard<std::recursive_mutex> lock(m_serverMutex);

	return g_session_manager.find_by_id(sessionID);
}

void WebsockServer::SetTimerInterval(uint32_t interval_in_us)
{
	m_interval_in_us = interval_in_us;
}

uint32_t WebsockServer::GetTimerTick()
{
	return m_tick_count;
}

void WebsockServer::OnTimerTick(uint32_t count)
{
	for (auto fn : m_timer_callbacks)
		fn(count);
}

void WebsockServer::TimerHandler()
{
	boost::system::error_code ec;

	assert(m_tick_timer != nullptr);

	OnTimerTick(m_tick_count++);

	m_tick_timer->expires_from_now(boost::posix_time::microseconds(m_interval_in_us), ec);
	if (ec)
	{
		TRACE(ec);
		return;
	}

	m_tick_timer->async_wait([this](const boost::system::error_code& ec) {
		if (ec)
		{
			TRACE("Error: " << ec);
			return;
		}

		TimerHandler();
	});
}

void WebsockServer::ShutdownTimer()
{
	m_tick_timer->cancel();

	// clean up the timer.
	m_run_timer = false;
	m_timer_complete = false;

	int retries = 50;
	while (!m_timer_complete && retries)
	{
		m_run_timer = false;
		std::this_thread::sleep_for(std::chrono::duration(std::chrono::milliseconds(10)));
		retries--;
	}
}

void WebsockServer::AddTimerTickCallback(OnTimerTickCallback_t fn)
{
	m_timer_callbacks.push_back(fn);
}