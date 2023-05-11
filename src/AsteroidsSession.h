#ifndef ASTEROIDS_SESSION_H
#define ASTEROIDS_SESSION_H

#include "GameSession.h"
#include "Asteroids.h"

class AsteroidsSession : public GameSession
{
public:
	AsteroidsSession(uint32_t sessionID);
	~AsteroidsSession();

	friend std::ostream& operator<<(std::ostream& os, const AsteroidsSession& gs);

	// from here down: objects that persist for the lifetime of the AsteroidsSession
	// Maintained with smart pointers for automated lifetime scoped memory.
	std::unique_ptr<Asteroids::Player> m_player;
	std::shared_ptr<Asteroids::Universe> m_universe;

protected:
	void HandleNewSession(AppBuffer& rxBuffer);
	void HandleKeyEvent(AppBuffer& rxBuffer);
	void HandleClickEvent(AppBuffer& rxBuffer);
	void HandleResizeEvent(AppBuffer& rxBuffer);
	void HandleTimerTick();
};

#endif // ASTEROIDS_SESSION
