#ifndef TIMER_H
#define TIMER_H

#include "geoff.h"

#define TM_TRACE(...)

namespace asteroids
{
	class Session;

	class Timer
	{
	public:
		Timer(Session& session, uint32_t intervalInUs);
		~Timer();

		uint32_t GetTick();
		void TickEvent();

		void Ticker();

	private:
		Session& m_session;
		std::unique_ptr<net::deadline_timer> m_timer;
		uint32_t m_tick_interval_in_us;
		uint32_t m_tick;
		bool m_run_timer;
		bool m_timer_complete;
	};
}
#endif