#ifndef TIMER_H
#define TIMER_H

#include "geoff.h"
#include "Session.h"

#define TM_TRACE(...)

namespace asteroids
{
	class Session;

	class Timer
	{
	public:
		Timer(uint32_t intervalInUs);
		~Timer();

		uint32_t GetTick();
		void Cancel();
		void TickEvent();

		void Ticker();

	private:
		uint32_t m_tick_interval_in_us;
		uint32_t m_tick;
		bool m_run_timer;
		bool m_timer_complete;
		std::mutex m_timer_mutex;
		net::deadline_timer m_timer;
	};
}
#endif
