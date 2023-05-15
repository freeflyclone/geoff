#include "geoff.h"

#include "Universe.h"
#include "Session.h"

using namespace as2;

namespace Websock
{
	WebsockSessionManager<Session> g_sessions;
};
using namespace Websock;

#undef UN_TRACE
#define UN_TRACE TRACE

Universe::Universe(int width, int height)
	: Size({ static_cast<double>(width), static_cast<double>(height) })
{
	UN_TRACE(__FUNCTION__);
}

Universe::~Universe()
{
	UN_TRACE(__FUNCTION__);
}

void Universe::TickEvent(Session& session)
{
	UN_TRACE(__FUNCTION__);

	for (auto pair : g_sessions.get_map())
	{
		pair.second->TickEvent();
		PerSessionTickEvent(session);
	}

	// TODO: output stuff to client
}

void Universe::PerSessionTickEvent(Session& session)
{
	UN_TRACE(__FUNCTION__);

}