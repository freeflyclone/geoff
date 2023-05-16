#include "geoff.h"
#include "Rock.h"
#include "Universe.h"

using namespace asteroids;

//#undef RK_TRACE
//#define RK_TRACE TRACE

Rock::Rock(double x, double y, double dx, double dy, double radius)
	:
	Position({ x,y }),
	Velocity({ dx,dy }),
	m_radius(radius)
{
	RK_TRACE(__FUNCTION__ << ", x: " << posX << ", y: " << posY << ", dx: " << deltaX << ", dy: " << deltaY << ", radius: " << m_radius);
}

Rock::~Rock()
{
	RK_TRACE(__FUNCTION__);
}

void Rock::TickEvent(Session& session)
{
	auto universeW = g_universe->Size::sizeW;
	auto universeH = g_universe->Size::sizeH;

	posX += deltaX;
	posY += deltaY;

	if (posX > universeW)
		posX = 0.0;
	if (posX < 0.0)
		posX = universeW;

	if (posY > universeH)
		posY = 0.0;
	if (posY < 0.0)
		posY = universeH;

	RK_TRACE(__FUNCTION__ << ", x: " << posX << ", y: " << posY << ", dx: " << deltaX << ", dy: " << deltaY << ", radius: " << m_radius);
}