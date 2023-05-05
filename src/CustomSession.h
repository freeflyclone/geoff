#ifndef CUSTOM_SESSION_H
#define CUSTOM_SESSION_H

#include "GameSession.h"

class CustomSession : public GameSession
{
public:
	CustomSession(uint32_t sessionID);
	~CustomSession();

protected:
	void HandleNewSession(AppBuffer& rxBuffer);
	void HandleKeyEvent(AppBuffer& rxBuffer);
	void HandleClickEvent(AppBuffer& rxBuffer);
	void HandleTimerTick();
};

#endif // CUSTOM_SESSION