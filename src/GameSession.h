#ifndef GAME_SESSION_H
#define GAME_SESSION_H

#include "geoff.h"
#include "WebsockSession.h"

#define GAME_APP_VERSION 3

class GameSession : public WebsockSession
{
public:
	GameSession(uint32_t sessionID);
	~GameSession();

	void CommsHandler(AppBuffer &) override;

private:
	void RegisterNewSession(AppBuffer& rxBuffer);
	void HandleClickEvent(AppBuffer& rxBuffer);
	void HandleKeyEvent(AppBuffer& rxBuffer);
};

#endif