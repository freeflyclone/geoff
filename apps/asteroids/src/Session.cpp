#include "geoff.h"
#include "WebsockSessionManager.h"

#include "Session.h"

#undef SS_TRACE
#define SS_TRACE TRACE

namespace Websock
{
	WebsockSessionManager<Session> g_sessions;
};
using namespace Websock;

Session::Session(uint32_t sessionID)
	:
	GameSession(sessionID)
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

void Session::HandlerTimerTick()
{
	SS_TRACE(__FUNCTION__);
}
