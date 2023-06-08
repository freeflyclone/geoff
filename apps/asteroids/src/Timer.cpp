#include "geoff.h"
#include "WebsockServer.h"
#include "WebsockSessionManager.h"

#include "Timer.h"
#include "Session.h"
#include "Universe.h"
#include "AppSession.h"

using namespace asteroids;

#define TM_TE_TRACE TRACE

namespace Websock
{
	WebsockSessionManager<Session> g_session_manager;
};
using namespace Websock;


Timer::Timer(uint32_t intervalInUs)
	:
	m_tick_interval_in_us(intervalInUs),
	m_tick(0),
	m_timer_mutex(),
	m_timer(*WebsockServer::GetInstance().IoContext(), boost::posix_time::microseconds(m_tick_interval_in_us))
{
	TM_TRACE(__FUNCTION__);

	m_run_timer = true;
	
	Ticker();
}

Timer::~Timer()
{
	TRACE(__FUNCTION__);

	std::lock_guard<std::mutex> lock(m_timer_mutex);

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

void Timer::Cancel()
{
	m_timer.cancel();
}

uint32_t Timer::GetTick()
{
	TM_TRACE(__FUNCTION__);
	return m_tick++;
}

void Timer::TickEvent()
{
	//const std::chrono::time_point now = std::chrono::system_clock::now();
	//auto uSec = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
	//TRACE("now: " << uSec.count());

	TM_TRACE(__FUNCTION__);

	if (!m_run_timer)
		return;

	if(g_universe)
		g_universe->TickEvent(m_tick);

	for (auto pair : g_session_manager.get_map())
	{
		auto sessionID = pair.first;
		auto session = pair.second;

		if (session)
			session->TickEvent(sessionID, m_tick);
	}
}

void Timer::Ticker()
{
	std::lock_guard<std::mutex> lock(m_timer_mutex);

	TickEvent();

	boost::system::error_code ec;

	if (!m_run_timer)
	{
		m_timer_complete = true;
		TM_TRACE(__FUNCTION__);
		return;
	}

	m_timer.expires_from_now(boost::posix_time::microseconds(m_tick_interval_in_us), ec);
	if (ec)
	{
		TRACE(ec);
		return;
	}

	m_timer.async_wait([this](const boost::system::error_code& ec) {
		if (ec)
		{
			TRACE("Error: " << ec);
			return;
		}

		Ticker();
	});
}
