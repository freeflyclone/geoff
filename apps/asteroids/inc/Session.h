#ifndef SESSION_H
#define SESSION_H

#include "geoff.h"
#include "GameSession.h"

#include "Structs.h"
#include "Timer.h"

#define SS_TRACE(...)

namespace asteroids
{
	class Player;
	class Session : public GameSession
	{
	public:
		Session(uint32_t sessionID);
		~Session();

		Player& GetPlayer() { return *m_player; }
		Timer& GetTimer() { return m_timer; }

		double Session::DistanceBetweenPoints(Position& p1, Position& p2);

		void TickEvent();

	private:
		void HandleNewSession(AppBuffer& rxBuffer);
		void HandleKeyEvent(AppBuffer& rxBuffer);
		void HandleClickEvent(AppBuffer& rxBuffer);
		void HandleResizeEvent(AppBuffer& rxBuffer);

		std::unique_ptr<Player> m_player;

		// Initialize AFTER m_player, else Player::TickEvent mayhem.
		// at Session::Session() time.
		Timer m_timer;
	};
}

#endif