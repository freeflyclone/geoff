#ifndef GAME_SESSION_H
#define GAME_SESSION_H

#include "geoff.h"
#include "WebsockSession.h"

#define GAME_APP_VERSION 3

class GameSession
{
public:
	GameSession(WebsockSession&);
	~GameSession();

	void CommsHandler(AppBuffer &);


private:
	void RegisterNewSession(AppBuffer& rxBuffer);
	void HandleClickEvent(AppBuffer& rxBuffer);
	void HandleKeyEvent(AppBuffer& rxBuffer);

	WebsockSession& m_wss;
};

#endif