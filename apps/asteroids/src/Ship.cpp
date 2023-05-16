#define _USE_MATH_DEFINES
#include <cmath>
#include "geoff.h"

#include "Consts.h"

#include "Ship.h"
#include "Gun.h"
#include "Universe.h" 

using namespace asteroids;

//#undef SH_TRACE
//#define SH_TRACE TRACE

Ship::Ship(uint16_t cw, uint16_t ch, double x, double y, double angle)
	:
	Context({cw, ch, static_cast<uint16_t>(g_universe->sizeW / 2 - cw / 2), static_cast<uint16_t>(g_universe->sizeH / 2 - ch / 2)}),
	Position({ x, y }),
	Velocity({ 0, 0 }),
	Orientation({ angle }),
	Radius({SHIP_SIZE}),
	m_gun(std::make_unique<Gun>()),
	m_thrusting(false),
	m_rotation(0)
{
	SH_TRACE(__FUNCTION__);
}

Ship::~Ship()
{
	SH_TRACE(__FUNCTION__);
}

void Ship::FireGuns()
{
	//SH_TRACE(__FUNCTION__);

	if(m_gun)
		m_gun->Fire(*this);
}

void Ship::KeyEvent(int key, bool isDown)
{
	(void)key;
	(void)isDown;

	//SH_TRACE(__FUNCTION__);

	switch (key)
	{
	case 37:
		m_rotation = (isDown) ? SHIP_RADS_PER_TICK : 0;
		break;

	case 39:
		m_rotation = (isDown) ? -SHIP_RADS_PER_TICK : 0;
		break;

	case 38:
		m_thrusting = isDown;
		break;

	case 32:
			if (isDown)
				FireGuns();
			break;
	}
}

void Ship::ResizeEvent(int w, int h)
{
	ctxW = static_cast<uint16_t>(w);
	ctxH = static_cast<uint16_t>(h);
	ctxOX = static_cast<uint16_t>(g_universe->sizeW / 2) - ctxW / 2;
	ctxOY = static_cast<uint16_t>(g_universe->sizeH / 2) - ctxH / 2;

	SH_TRACE(__FUNCTION__ << ", ctxW: " << ctxW << ", ctxH: " << ctxH << ", ctxOX: " << ctxOX << ", ctxOY: " << ctxOY);
}

void Ship::TickEvent(Session& session)
{
	//SH_TRACE(__FUNCTION__);
	MoveShip();

	if(m_gun)
		m_gun->TickEvent(session);
}

void Ship::MoveShip()
{
	//SH_TRACE(__FUNCTION__ << "Ship: " << posX << "," << posY << " - " << deltaX << "," << deltaY << ", angle: " << angle << ", " << (m_thrusting ? "thrusting" : ""));

	if (m_thrusting)
	{
		// accelerate
		auto thrustX = (double)  SHIP_THRUST * cos(angle) / (double)FPS;
		auto thrustY = (double) -SHIP_THRUST * sin(angle) / (double)FPS;

		deltaX += thrustX;
		deltaY += thrustY;

		//SH_TRACE("Velockity: " << deltaX << "," << deltaY);
	}
	else
	{
		// decelerate
		deltaX -= FRICTION * deltaX / FPS;
		deltaY -= FRICTION * deltaY / FPS;

		//SH_TRACE("Velockity: " << deltaX << "," << deltaY);
	}

	// Rotation is tracked in radians, and we want to clip the rotation
	// angle like we do for screen edges.  That is: keep it between 0
	// and 360 degrees, (in radians of course)
	auto max_angle = M_PI * 2.0;

	// m_rotation toggled in keypress handler
	angle += m_rotation;

	// auto-wrap rotation: ie keep between 0-360 degrees (in radians)
	if (angle >= max_angle)
		angle = 0;
	else if (angle < 0)
		angle = max_angle;

	// apply thrust or friction
	posX += deltaX;
	posY += deltaY;

	// Get Context dimensions & offset as doubles (not uint16_t)
	auto offX = static_cast<double>(ctxOX);
	auto offY = static_cast<double>(ctxOY);
	auto windowW = static_cast<double>(ctxW);
	auto windowH = static_cast<double>(ctxH);

	// handle wrapping at edge of screen
	// (eventually: move the Context within the Universe)
	if (posX < offX - radius) {
		posX = offX + windowW + radius;
	}
	else if (posX > offX + windowW + radius) {
		posX = offX - radius;
	}

	if (posY < offY - radius) {
		posY = offY + windowH + radius;
	}
	else if (posY > offY + windowH + radius) {
		posY = offY - radius;
	}
}

