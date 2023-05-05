#ifndef GAME_SESSION_H
#define GAME_SESSION_H

#include "geoff.h"
#include "WebsockSession.h"

#define GAME_APP_VERSION 3

class GameSession : public WebsockSession
{
public:
	typedef std::function<void(AppBuffer&)> GameSessionPacketHandler_t;

	GameSession(uint32_t sessionID);
	~GameSession();

	void CommsHandler(AppBuffer &) override;

	void AddRegisterNewSessionHandler(GameSessionPacketHandler_t fn);
	void AddClickEventHandler(GameSessionPacketHandler_t fn);
	void AddKeyEventHandler(GameSessionPacketHandler_t fn);

protected:
	std::vector<GameSessionPacketHandler_t> m_newSessionHandlers;
	std::vector<GameSessionPacketHandler_t> m_clickEventHandlers;
	std::vector<GameSessionPacketHandler_t> m_keyEventHandlers;
};

#endif