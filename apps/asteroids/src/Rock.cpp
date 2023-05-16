#include "geoff.h"
#include "Rock.h"

using namespace as2;

#undef RK_TRACE
#define RK_TRACE TRACE

Rock::Rock(double x, double y, double dx, double dy, double radius)
	:
	Position({ x,y }),
	Velocity({ dx,dy }),
	m_radius(radius)
{
	RK_TRACE(__FUNCTION__);
}

Rock::~Rock()
{
	RK_TRACE(__FUNCTION__);
}

void Rock::TickEvent(Session& session)
{
	RK_TRACE(__FUNCTION__);
}