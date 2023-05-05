/*
* GameSession
* 
* This class provides an interface to WebsockSession to facilitate its use.
* 
* Services provided:
*	Maintain lists of handler functions for 
*		RegisterSession, 
*		KeyEvent 
*		ClickEvent
*	packets.  Users can add handler functions which will be called in order of
*   addition to the list(s).
* 
*	Maintain list of TimerTick handler functions to be called per timer tick.
*		Probably wise to be judicious #handlers added, depending on SetIntervalInUs() settings.
*
* This is intended to be the main interface a GEOFF based game would start from.
* 
* See WebsockServer.cpp WebsockSessionManager declaration: that's where a class derived
* (specialized) from this one needs to be specified.
* 
* Changing this file does NOT reevaluate websocket_session.h.
*/
#ifndef GAME_SESSION_H
#define GAME_SESSION_H

#include "geoff.h"
#include "WebsockSession.h"

#define GAME_APP_VERSION 3

class GameSession : public WebsockSession
{
public:
	typedef std::function<void(AppBuffer&)> AppBufferProcessor_t;
	typedef std::function<void()> TimerTickCallback_t;

	GameSession(uint32_t sessionID);
	~GameSession();

	void CommsHandler(AppBuffer &) override;
	void OnTimerTick() override;

	void AddRegisterNewSessionHandler(AppBufferProcessor_t fn);
	void AddClickEventHandler(AppBufferProcessor_t fn);
	void AddKeyEventHandler(AppBufferProcessor_t fn);
	void AddTimerTickHandler(TimerTickCallback_t fn);

protected:
	std::vector<AppBufferProcessor_t> m_newSessionHandlers;
	std::vector<AppBufferProcessor_t> m_clickEventHandlers;
	std::vector<AppBufferProcessor_t> m_keyEventHandlers;
	std::vector<TimerTickCallback_t> m_timerTickHandlers;
};

#endif