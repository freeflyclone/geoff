#include "geoff.h"
#include "Rock.h"
#include "Universe.h"

using namespace asteroids;

#undef RK_TRACE
#define RK_TRACE TRACE

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

	Position::posX += Velocity::deltaX;
	Position::posY += Velocity::deltaY;

	if (Position::posX > universeW)
		Position::posX = 0.0;
	if (Position::posX < 0.0)
		Position::posX = universeW;

	if (Position::posY > universeH)
		Position::posY = 0.0;
	if (Position::posY < 0.0)
		Position::posY = universeH;

	RK_TRACE(__FUNCTION__ << ", x: " << posX << ", y: " << posY << ", dx: " << deltaX << ", dy: " << deltaY << ", radius: " << m_radius);
}