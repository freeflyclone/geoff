#include "CustomSession.h"

CustomSession::CustomSession(uint32_t sessionID)
	: GameSession(sessionID)
{
	TRACE("");

	AddRegisterNewSessionHandler(std::bind(&CustomSession::HandleNewSession, this, std::placeholders::_1));
	AddKeyEventHandler(std::bind(&CustomSession::HandleKeyEvent, this, std::placeholders::_1));
	AddClickEventHandler(std::bind(&CustomSession::HandleClickEvent, this, std::placeholders::_1));
	AddTimerTickHandler(std::bind(&CustomSession::HandleTimerTick, this));

	SetIntervalInUs(500000);
}

CustomSession::~CustomSession()
{
	TRACE("");
}

void CustomSession::HandleNewSession(AppBuffer& rxBuffer)
{
	TRACE("");
}

void CustomSession::HandleKeyEvent(AppBuffer& rxBuffer)
{
	//TRACE("");
}

void CustomSession::HandleClickEvent(AppBuffer& rxBuffer)
{
	TRACE("");
}

void CustomSession::HandleTimerTick()
{
	TRACE("");
}

