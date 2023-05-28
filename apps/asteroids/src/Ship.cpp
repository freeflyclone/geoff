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
	Context({ cw, ch, static_cast<uint16_t>(g_universe->sizeW / 2 - cw / 2), static_cast<uint16_t>(g_universe->sizeH / 2 - ch / 2) }),
	Position({ x, y }),
	Velocity({ 0, 0 }),
	Orientation({ angle }),
	Radius({ SHIP_SIZE }),
	m_gun(std::make_unique<Gun>()),
	m_rotation(0),
	m_max_delta_v(SHIP_MAX_DELTA_V),
	m_viewport_margin(VIEWPORT_MARGIN),
	m_thrusting(false),
	m_left(false),
	m_right(false),
	m_slide_viewport(true),
	m_wrap_viewport(false),
	m_enforce_boundaries(true)
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
	case 'A':
	case 37:
		m_left = isDown;
		break;

	case 'D':
	case 39:
		m_right = isDown;
		break;

	case 'I':
	case 'W':
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

	m_rotation = 0;
	m_rotation += m_left ? SHIP_RADS_PER_TICK : 0;
	m_rotation -= m_right ? SHIP_RADS_PER_TICK : 0;

	if (m_thrusting)
	{
		// accelerate
		auto thrustX = (double)  SHIP_THRUST * cos(angle) / (double)FPS;
		auto thrustY = (double) -SHIP_THRUST * sin(angle) / (double)FPS;

		deltaX += thrustX;
		deltaY += thrustY;

		if (deltaX > m_max_delta_v)
			deltaX = m_max_delta_v;
		if (deltaX < -m_max_delta_v)
			deltaX = -m_max_delta_v;

		if (deltaY > m_max_delta_v)
			deltaY = m_max_delta_v;
		if (deltaY < -m_max_delta_v)
			deltaY = -m_max_delta_v;


		//SH_TRACE("Velockity: " << deltaX << "," << deltaY);
	}
	else
	{
		// decelerate
		deltaX -= FRICTION * deltaX / FPS;
		deltaY -= FRICTION * deltaY / FPS;

		//SH_TRACE("Velockity: " << deltaX << "," << deltaY);
	}

	if (m_slide_viewport)
	{
		// left?
		if (posX < (ctxOX + m_viewport_margin))
		{
			if (posX > m_viewport_margin)
				ctxOX = posX - m_viewport_margin;
		}
		// right?
		else if (posX > (ctxOX + ctxW - m_viewport_margin))
		{
			if (posX < g_universe->sizeW - m_viewport_margin)
				ctxOX = posX - ctxW + m_viewport_margin;
		}

		// up?
		if (posY < (ctxOY + m_viewport_margin))
		{
			if (posY > m_viewport_margin)
				ctxOY = posY - m_viewport_margin;
		}
		// down?
		else if (posY > (ctxOY + ctxH - m_viewport_margin))
		{
			if (posY < g_universe->sizeH - m_viewport_margin)
				ctxOY = posY - ctxH + m_viewport_margin;
		}
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

	if (m_enforce_boundaries)
	{
		if (posX < radius)
		{
			posX = radius;
			deltaX = 0.0;
		}
		else if (posX + radius >= g_universe->sizeW)
		{
			posX = g_universe->sizeW - radius;
			deltaX = 0.0;
		}

		if (posY < radius)
		{
			posY = radius;
			deltaY = 0.0;
		}
		else if (posY + radius >= g_universe->sizeH)
		{
			posY = g_universe->sizeH - radius;
			deltaY = 0.0;
		}
	}

	if (m_wrap_viewport)
	{
		// Get Context dimensions & offset as doubles (not uint16_t)
		auto offX = static_cast<double>(ctxOX);
		auto offY = static_cast<double>(ctxOY);
		auto windowW = static_cast<double>(ctxW);
		auto windowH = static_cast<double>(ctxH);

		// handle wrapping at edge of screen
		// (eventually: move the Context within the Universe)a
		if (posX < offX) {
			posX = offX + windowW;
		}
		else if (posX > offX + windowW) {
			posX = offX;
		}

		if (posY < offY) {
			posY = offY + windowH;
		}
		else if (posY > offY + windowH) {
			posY = offY;
		}
	}
}

