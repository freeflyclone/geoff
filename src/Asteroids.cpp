#include "geoff.h"

#include "Asteroids.h"

using namespace Asteroids;

namespace {
	// Take from asteroids.js
	const int FPS = 60;
	const float FRICTION = 0.7f;
	const float SHIP_BLINK_DUR = 0.2f;       // duration in seconds of a single blink during ship's invisibility
	const float SHIP_EXPLODE_DUR = 0.5f;     // duration of the ship's explosion in seconds
	const float SHIP_INV_DUR = 3.0f;         // duration of the ship's invisibility in seconds
	const int SHIP_SIZE = 20;                // ship height in pixels
	const int SHIP_THRUST = 10;              // acceleration of the ship in pixels per second per second
	const int SHIP_TURN_SPEED = 360;         // turn speed in degrees per second
}

void Context::Resize(uint16_t width, uint16_t height)
{
	m_width = width;
	m_height = height;

	TRACE("m_width: " << m_width << ", m_height : " << m_height);
}

Bullet::Bullet(int x, int y, int dx, int dy)
	:
	m_x(x),
	m_y(y),
	m_dx(dx),
	m_dy(dy)
{
}

Bullet::~Bullet()
{
}

Ship::Ship(int windowWidth, int windowHeight, int x, int y, float angle) :
	m_width(windowWidth),
	m_height(windowHeight),
	m_x(x),
	m_y(y),
	m_angle(angle),
	m_radius(SHIP_SIZE),
	m_canShoot(true),
	m_dead(false),
	m_explode_time(0),
	m_rotation(0),
	m_thrusting(false),
	m_thrust({ 0,0 })
{
	//TRACE("");
}

Ship::~Ship()
{
	//TRACE("");
}

void Ship::MoveShip()
{
	if (m_thrusting && !m_dead)
	{
		auto tx = (double) SHIP_THRUST * cos(m_angle) / (double)FPS;
		auto ty = (double)-SHIP_THRUST * sin(m_angle) / (double)FPS;
	}

	m_angle += m_rotation;
}

void Ship::SetPosition(int x, int y)
{
	m_x = x;
	m_y = y;
}

void Ship::KeyEvent(int key, bool isDown)
{
	//TRACE("Key: (" << key << "," << (isDown ? "down" : "up") << ")");

	switch (key)
	{
		case 37:
			m_rotation = (isDown) ? SHIP_TURN_SPEED / 180 * M_PI / FPS : 0;
			break;

		case 39:
			m_rotation = (isDown) ? -SHIP_TURN_SPEED / 180 * M_PI / FPS : 0;
			break;

		case 38:
			m_thrusting = isDown;
			TRACE("m_thrusting: " << m_thrusting);
			break;
	}
}

void Ship::TickEvent()
{
	MoveShip();
}
