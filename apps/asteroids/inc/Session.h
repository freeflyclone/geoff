#ifndef SESSION_H
#define SESSION_H

#include "geoff.h"
#include "GameSession.h"

#define SS_TRACE(...)

class Session : public GameSession
{
public:
	Session(uint32_t sessionID);
	~Session();

	void HandlerTimerTick();

private:
	void HandleNewSession(AppBuffer& rxBuffer);
	void HandleKeyEvent(AppBuffer& rxBuffer);
	void HandleClickEvent(AppBuffer& rxBuffer);
	void HandleResizeEvent(AppBuffer& rxBuffer);
};

#endif