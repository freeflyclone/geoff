#include "geoff.h"
#include "WebsockServer.h"

#include "Timer.h"
#include "Session.h"

#define TM_TE_TRACE TRACE

Timer::Timer(Session& session, uint32_t intervalInUs)
	:
	m_session(session),
	m_tick_interval_in_us(intervalInUs)
{
	TM_TRACE(__FUNCTION__);

	m_timer = std::make_unique<net::deadline_timer>(*WebsockServer::GetInstance().IoContext(), boost::posix_time::microseconds(m_tick_interval_in_us));
	m_run_timer = true;
	Ticker();
}

Timer::~Timer()
{
	TM_TRACE(__FUNCTION__);

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

uint32_t Timer::GetTick()
{
	TM_TRACE(__FUNCTION__);
	return m_tick;
}

void Timer::TickEvent()
{
	TM_TRACE(__FUNCTION__);
	m_session.TickEvent();
}

void Timer::Ticker()
{
	TickEvent();

	boost::system::error_code ec;

	if (!m_timer)
	{
		m_timer_complete = true;
		TM_TRACE(__FUNCTION__);
		return;
	}

	if (!m_run_timer)
	{
		m_timer_complete = true;
		TM_TRACE(__FUNCTION__);
		return;
	}

	m_timer->expires_from_now(boost::posix_time::microseconds(m_tick_interval_in_us), ec);
	if (ec)
	{
		TRACE(ec);
		return;
	}

	m_timer->async_wait([this](const boost::system::error_code& e) {
		(void)e;
		Ticker();
	});
}
