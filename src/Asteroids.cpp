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
	const float MUZZLE_VELOCITY = 500;		 // pixels per second
}

void Context::Resize(uint16_t w, uint16_t h)
{
	width = w;
	height = h;

	TRACE("m_width: " << width << ", m_height : " << height);
}

Bullet::Bullet(GameSession &g, double x, double y, double dx, double dy)
	:
	gs(g),
	Position({ x,y }),
	Velocity({ dx,dy }),
	ticksLeft(3 * FPS)
{
	//TRACE("New bullet - x: " << Position::x << ", y:" << Position::y << "dx: " << Velocity::dx << ", dy: " << Velocity::dy);
}

Bullet::~Bullet()
{
}

bool Bullet::TickTock()
{
	if (ticksLeft)
		ticksLeft--;

	if (ticksLeft)
	{
		Position::x += Velocity::dx;
		Position::y += Velocity::dy;

		// TODO: window boundary detection

		//TRACE("x: " << Position::x << ", y: " << Position::y);
	}

	return ticksLeft == 0;
}

void Gun::Fire(double x, double y, double dx, double dy)
{
	bullets.emplace_back(std::make_unique<Bullet>(gs, x, y, dx, dy));
	//TRACE("bullets.size(): " << bullets.size());
}

std::unique_ptr<AppBuffer> Gun::MakeBulletsPacket(bool isLittleEndian)
{
	if (bullets.size() == 0)
		return std::move(nullptr);

	auto bulletDataSize = 2 * sizeof(int16_t);

	auto appBufferSize = 2 + (bullets.size() * bulletDataSize);
	auto txBuff = std::make_unique<AppBuffer>(appBufferSize, isLittleEndian);

	txBuff->set_uint16(static_cast<uint16_t>(bullets.size()));

	size_t i = 0;
	for (auto bullet : bullets)
	{
		auto cx = static_cast<uint16_t>(bullet->x);
		auto cy = static_cast<uint16_t>(bullet->y);

		//TRACE("bullet[" << i << "]: " << bullet->x << "(" << cx << ")," << bullet->y << "(" << cy << ")");

		txBuff->set_uint16(cx);
		txBuff->set_uint16(cy);
		i++;

		//if (i > 1)
			//TRACE("More than one"); 
	}

	return std::move(txBuff);
}

void Gun::TickTock()
{
	if (bullets.empty())
		return;

	bool bulletDone = false;

	// update bullet lifetime ticks
	for (auto bullet : bullets)
	{
		if (bullet->TickTock())
		{
			//TRACE("bulletDone, bullets.size(): " << bullets.size());
			bulletDone = true;
		}
	}

	if (bulletDone)
	{
		bullets.pop_front();
		if (bullets.size() == 0)
		{
			//TRACE("bulletDone, no more bullets");
		}
	}
}

Ship::Ship(GameSession& gs, int windowWidth, int windowHeight, double x, double y, double angle) :
	m_gs(gs),
	Context({ static_cast<uint16_t>(windowWidth), static_cast<uint16_t>(windowHeight), gs }),
	Position({ x,y }),
	Velocity({ 0,0 }),
	m_gun(std::make_unique<Gun>(m_gs)),
	m_angle(angle),
	m_radius(SHIP_SIZE),
	m_canShoot(true),
	m_dead(false),
	m_explode_time(0),
	m_rotation(0),
	m_thrusting(false),
	m_show_position(false)
{
}

Ship::~Ship()
{
	//TRACE("");
}

void Ship::GetXY(int16_t& xPos, int16_t& yPos)
{
	xPos = static_cast<int16_t>(std::floor(Position::x));
	yPos = static_cast<int16_t>(std::floor(Position::y));
}

void Ship::GetAngle(int16_t& angle)
{
	angle = static_cast<int16_t>(std::floor(m_angle * 4096.0));
}

void Ship::MoveShip()
{
	if (m_thrusting && !m_dead)
	{
		Velocity::dx += (double) SHIP_THRUST * cos(m_angle) / (double)FPS;
		Velocity::dy += (double)-SHIP_THRUST * sin(m_angle) / (double)FPS;
	}
	else 
	{
		Velocity::dx -= FRICTION * this->dx / FPS;
		Velocity::dy -= FRICTION * this->dy / FPS;
	}

	auto max_angle = M_PI * 2.0;
	m_angle += m_rotation;

	if (m_angle >= max_angle)
		m_angle = 0;
	else if (m_angle < 0)
		m_angle = max_angle;

	Position::x += Velocity::dx;
	Position::y += Velocity::dy;

	// handle edge of screen
	if (Position::x < 0 - m_radius) {
		Position::x = Context::width + m_radius;
	}
	else if (Position::x > Context::width + m_radius) {
		Position::x = 0 - m_radius;
	}

	if (Position::y < 0 - m_radius) {
		Position::y = Context::height + m_radius;
	}
	else if (Position::y > Context::height + m_radius) {
		Position::y = 0 - m_radius;
	}
}

void Ship::FireShot()
{
	if (!m_gun)
		return;

	auto px = Position::x + 4 / 3 * m_radius * cos(m_angle);
	auto py = Position::y - 4 / 3 * m_radius * sin(m_angle);
	
	auto mvx = (double)MUZZLE_VELOCITY * cos(m_angle) / (double)FPS;
	auto mvy = (double)MUZZLE_VELOCITY * -sin(m_angle) / (double)FPS;

	// fire the gun by telling it where it is and muzzle 2D velocity vector 
	m_gun->Fire(px, py, mvx, mvy);
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
			break;

		case 32:
			if (isDown)
				FireShot();

			m_show_position = isDown;
			break;
	}
}

void Ship::TickEvent()
{
	MoveShip();
	m_gun->TickTock();
}
