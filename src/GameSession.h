#ifndef GAME_SESSION_H
#define GAME_SESSION_H

#include "geoff.h"
#include "WebsockSession.h"

#define GAME_APP_VERSION 3

class GameSession : public WebsockSession
{
public:
	typedef std::function<void(AppBuffer&)> AppBufferProcessor_t;

	GameSession(uint32_t sessionID);
	~GameSession();

	void CommsHandler(AppBuffer &) override;

	void AddRegisterNewSessionHandler(AppBufferProcessor_t fn);
	void AddClickEventHandler(AppBufferProcessor_t fn);
	void AddKeyEventHandler(AppBufferProcessor_t fn);

protected:
	std::vector<AppBufferProcessor_t> m_newSessionHandlers;
	std::vector<AppBufferProcessor_t> m_clickEventHandlers;
	std::vector<AppBufferProcessor_t> m_keyEventHandlers;
};

#endif