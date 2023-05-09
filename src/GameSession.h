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
*	user input packets from the client side.
*
*	Drive a boost::deadline_timer handler function to be called per timer tick.
*		Using such a timer we're guaranteed to be called sequentially
*		with respect to buffer exchanges on the beast Websocket stream.
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
	GameSession(uint32_t sessionID);
	~GameSession();

	void StartTimer();
	void StopTimer();
	void SetIntervalInUs(uint32_t interval);

	void CommsHandler(AppBuffer &) override;

	friend std::ostream& operator<<(std::ostream& os, const GameSession& gs);

protected:
	virtual void HandleNewSession(AppBuffer& rxBuffer);
	virtual void HandleKeyEvent(AppBuffer& rxBuffer);
	virtual void HandleClickEvent(AppBuffer& rxBuffer);
	virtual void HandleResizeEvent(AppBuffer& rxBuffer);
	virtual void HandleTimerTick();

	void TimerTicker();

	std::unique_ptr<net::deadline_timer> m_timer;

	bool m_run_timer;
	int m_timer_complete;
	uint32_t m_timer_tick;
	uint32_t m_tick_interval_in_us;
};

#endif