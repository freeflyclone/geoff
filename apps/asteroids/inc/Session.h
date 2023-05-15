#ifndef SESSION_H
#define SESSION_H

#include "geoff.h"
#include "GameSession.h"

#include "Timer.h"

#define SS_TRACE(...)

namespace as2
{
	class Player;

	class Session : public GameSession
	{
	public:
		Session(uint32_t sessionID);
		~Session();

		void TickEvent();

	private:
		void HandleNewSession(AppBuffer& rxBuffer);
		void HandleKeyEvent(AppBuffer& rxBuffer);
		void HandleClickEvent(AppBuffer& rxBuffer);
		void HandleResizeEvent(AppBuffer& rxBuffer);

		Timer m_timer;
		std::unique_ptr<Player> m_player;
	};
}

#endif