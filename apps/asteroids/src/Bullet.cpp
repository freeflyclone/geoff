#include "geoff.h"

#include "Consts.h"

#include "Bullet.h"

#include "Universe.h"
using namespace asteroids;

//#undef B_TRACE
//#define B_TRACE TRACE

Bullet::Bullet(double x, double y, double dx, double dy)
	: 
	Position({ x, y }),
	Velocity({ dx, dy }),
	m_ticks_left(static_cast<unsigned int>(BULLET_DURATION * FPS))
{
	B_TRACE(__FUNCTION__);
}

Bullet::~Bullet()
{
	B_TRACE(__FUNCTION__);
}

bool Bullet::TickEvent(Session& session)
{
	(void)session;

	if (m_ticks_left)
		m_ticks_left--;

	if (m_ticks_left)
	{
		posX += deltaX;
		posY += deltaY;

		if (posX > g_universe->sizeW)
			posX = 0.0;
		if (posX < 0.0)
			posX = g_universe->sizeW;

		if (posY > g_universe->sizeH)
			posY = 0.0;
		if (posY < 0.0)
			posY = g_universe->sizeH;

		B_TRACE(__FUNCTION__ << ", posX: " << posX << ", posY: " << posY << ", ticks left: " << m_ticks_left);
	}

	return m_ticks_left == 0;
}
