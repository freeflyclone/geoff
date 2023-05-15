#include "geoff.h"
#include "WebsockSessionManager.h"

#include "Session.h"

#undef SS_TRACE
#define SS_TRACE TRACE

using namespace as2;

namespace Websock {	WebsockSessionManager<Session> g_sessions; };

Session::Session(uint32_t sessionID)
	:
	GameSession(sessionID),
	m_timer(*this, 1000000)
{
	SS_TRACE(__FUNCTION__);
}

Session::~Session()
{
	SS_TRACE(__FUNCTION__);
}

void Session::HandleNewSession(AppBuffer& rxBuffer)
{
	SS_TRACE(__FUNCTION__);
}

void Session::HandleKeyEvent(AppBuffer& rxBuffer)
{
	SS_TRACE(__FUNCTION__);
}

void Session::HandleClickEvent(AppBuffer& rxBuffer)
{
	SS_TRACE(__FUNCTION__);
}

void Session::HandleResizeEvent(AppBuffer& rxBuffer)
{
	SS_TRACE(__FUNCTION__);
}

void Session::TickEvent()
{
	SS_TRACE(__FUNCTION__);
}
