#ifndef CUSTOM_SESSION_H
#define CUSTOM_SESSION_H

#include "GameSession.h"
#include "Asteroids.h"

class CustomSession : public GameSession
{
public:
	CustomSession(uint32_t sessionID);
	~CustomSession();

	friend std::ostream& operator<<(std::ostream& os, const CustomSession& gs);

protected:
	void HandleNewSession(AppBuffer& rxBuffer);
	void HandleKeyEvent(AppBuffer& rxBuffer);
	void HandleClickEvent(AppBuffer& rxBuffer);
	void HandleResizeEvent(AppBuffer& rxBuffer);
	void HandleTimerTick();

	std::unique_ptr<Asteroids::Ship> m_ship;
};

#endif // CUSTOM_SESSION
