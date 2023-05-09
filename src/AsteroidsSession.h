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

protected:
	void HandleNewSession(AppBuffer& rxBuffer);
	void HandleKeyEvent(AppBuffer& rxBuffer);
	void HandleClickEvent(AppBuffer& rxBuffer);
	void HandleResizeEvent(AppBuffer& rxBuffer);
	void HandleTimerTick();

	// One of these to the client side each game tick.
	// An internal function which creates a std::unique_ptr<AppBuffer>
	// and returns it (automatically invoking move semantics) to be added
	// to the txQue.
	std::unique_ptr<AppBuffer> NewSessionTickBuffer();

	// from here down: object that persist for the session
	// Maintained with unique_ptr's for automated lifetime scoped memory.
	std::unique_ptr<Asteroids::Ship> m_ship;
};

#endif // ASTEROIDS_SESSION
