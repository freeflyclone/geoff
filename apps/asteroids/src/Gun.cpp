#include "geoff.h"

#include "Gun.h"
#include "Ship.h"

using namespace as2;

#undef GN_TRACE
#define GN_TRACE TRACE

Gun::Gun(Ship2& s)
	:
	m_ship(s)
{
	GN_TRACE(__FUNCTION__);
}

Gun::~Gun()
{
	GN_TRACE(__FUNCTION__);
}

void Gun::Fire()
{
	GN_TRACE(__FUNCTION__);

	// TODO: calculate firing solution from ship position & angle
}

void Gun::TickEvent(Session& session)
{
	GN_TRACE(__FUNCTION__);
}