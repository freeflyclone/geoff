#include "geoff.h"

#include "Universe.h"
#include "Session.h"
#include "RockField.h"

#include "WebsockSessionManager.h"

using namespace asteroids;

namespace Websock
{
	WebsockSessionManager<Session> g_sessions;
};
using namespace Websock;

namespace asteroids
{
	// we want just one of these, but we want to control when it gets created.
	std::unique_ptr<Universe> g_universe;

	const Universe& Init(int w, int h)
	{
		if (!g_universe)
			g_universe = std::make_unique<Universe>(w, h);

		return *g_universe;
	}
};

//#undef UN_TRACE
//#define UN_TRACE TRACE

Universe::Universe(int width, int height)
	: Size({ static_cast<double>(width), static_cast<double>(height) }),
	m_rockField(std::make_unique<RockField>(width, height))
{
	UN_TRACE(__FUNCTION__);
}

Universe::~Universe()
{
	UN_TRACE(__FUNCTION__);
}

RockField& Universe::GetRockField()
{
	return *m_rockField;
}

void Universe::TickEvent(Session& session)
{
	UN_TRACE(__FUNCTION__);

	OtherSessionsTickEvent(session);

	if (m_rockField)
		m_rockField->TickEvent(session);

	// TODO: output stuff to client
}

void Universe::OtherSessionsTickEvent(Session& session)
{
	UN_TRACE(__FUNCTION__);
}