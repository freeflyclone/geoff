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

		Player* GetPlayer() { return m_player.get(); }

		void TickEvent(uint32_t sessionID, uint32_t tickCount);

	private:
		void HandleNewSession(AppBuffer& rxBuffer);
		void HandleKeyEvent(AppBuffer& rxBuffer);
		void HandleClickEvent(AppBuffer& rxBuffer);
		void HandleResizeEvent(AppBuffer& rxBuffer);

		uint32_t m_sessionID;
		std::shared_ptr<Player> m_player;
	};

	double DistanceBetweenPoints(Position& p1, Position& p2);
}

#endif
