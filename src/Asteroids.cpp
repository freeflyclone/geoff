#include "geoff.h"

#include "Asteroids.h"
#include "AsteroidsSession.h"
#include "WebsockSessionManager.h"

using namespace Asteroids;

// The Universe class needs access to the WebsockSessionManager, to know
// about other players;
namespace Websock
{
	extern WebsockSessionManager<AsteroidsSession> g_sessions;
};
using namespace Websock;

// Data that all AsteroidsSession instances need access to
// in single and/or multiplayer mode.
namespace Asteroids
{
	std::mutex g_rocks_mutex;
	RockField::RocksList_t g_rocks;
};

#define CTX_TRACE(...)
//#define CTX_TRACE TRACE

void Context::Resize(uint16_t w, uint16_t h)
{
	width = w;
	height = h;

	CTX_TRACE(__FUNCTION__ << "width: " << width << ", height : " << height);
}

Bullet::Bullet(Gun &g, double x, double y, double dx, double dy)
	:
	Position({ x,y }),
	Velocity({ dx,dy }),
	m_gun(g),
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
		auto ship = GetGun().GetShip();
		auto ctx = (Asteroids::Context&)ship;

		Position::x += Velocity::dx;
		Position::y += Velocity::dy;

		if (Position::x > ctx.width)
			Position::x = 0.0;
		if (Position::x < 0.0)
			Position::x = (double)ctx.width;

		if (Position::y > ctx.height)
			Position::y = 0.0;
		if (Position::y < 0.0)
			Position::y = (double)ctx.height;

		//TRACE("x: " << Position::x << ", y: " << Position::y);
	}

	return ticksLeft == 0;
}

Rock::Rock(RockField& field, double x, double y, double dx, double dy, double radius)
	:
	Position({ x,y }),
	Velocity({ dx,dy }),
	m_field(field),
	m_radius(radius)
{
	//TRACE("New rock - x: " << Position::x << ", y:" << Position::y << "dx: " << Velocity::dx << ", dy: " << Velocity::dy);
}

Rock::~Rock()
{
}

bool Rock::TickTock()
{
	auto rockField = GetRockField();
	auto ctx = (Asteroids::Context&)rockField;

	Position::x += Velocity::dx;
	Position::y += Velocity::dy;

	if (Position::x > ctx.width)
		Position::x = 0.0;
	if (Position::x < 0.0)
		Position::x = (double)ctx.width;

	if (Position::y > ctx.height)
		Position::y = 0.0;
	if (Position::y < 0.0)
		Position::y = (double)ctx.height;

	//TRACE(__FUNCTION__ << "x: " << Position::x << ", y: " << Position::y);

	return true;
}

RockField::RockField(Universe& universe, int w, int h)
	:
	Context({ static_cast<uint16_t>(w), static_cast<uint16_t>(h) }),
	m_universe(universe),
	m_rocks(g_rocks)
{

}

RockField::~RockField()
{
}

void RockField::LaunchOne(double x, double y, double dx, double dy, double radius)
{
	m_rocks.emplace_back(std::make_unique<Rock>(*this, x, y, dx, dy, radius));

	TRACE(__FUNCTION__ << "x: " << x << ", y: " << y << ", radius: " << radius << ", " << m_rocks.size() << " rock(s) exist.");
}

void RockField::ResizeEvent(int w, int h)
{
	Resize(static_cast<uint16_t>(w), static_cast<uint16_t>(h));
}

void RockField::TickEvent(AsteroidsSession&) 
{
	for (auto rock : m_rocks)
	{
		rock->TickTock();
	}
}

#define G_TRACE(...)
//#define G_TRACE TRACE
void Gun::Fire(double x, double y, double dx, double dy)
{
	m_bullets.emplace_back(std::make_unique<Bullet>(*this, x, y, dx, dy));
	G_TRACE(__FUNCTION__ << "m_bullets.size(): " << m_bullets.size());
}

std::unique_ptr<AppBuffer> Gun::MakeBulletsPacket(bool isLittleEndian)
{
	if (m_bullets.size() == 0)
		return std::move(nullptr);

	auto bulletDataSize = 2 * sizeof(int16_t);

	auto appBufferSize = 2 + (m_bullets.size() * bulletDataSize);
	auto txBuff = std::make_unique<AppBuffer>(appBufferSize, isLittleEndian);

	txBuff->set_uint16(static_cast<uint16_t>(m_bullets.size()));

	size_t i = 0;
	for (auto bullet : m_bullets)
	{
		auto cx = static_cast<uint16_t>(bullet->x);
		auto cy = static_cast<uint16_t>(bullet->y);

		G_TRACE("bullet[" << i << "]: " << bullet->x << "(" << cx << ")," << bullet->y << "(" << cy << ")");

		txBuff->set_uint16(cx);
		txBuff->set_uint16(cy);
		i++;
	}

	return txBuff;
}

void Gun::TickTock()
{
	if (m_bullets.empty())
		return;

	bool bulletDone = false;

	// Rocks and/or Bullets to be destroyed AFTER all collision checks are complete.
	// (deleting these in the collision detection loops causes mayhem)
	RockField::RocksList_t collidedRocks;
	Gun::BulletsList_t collidedBullets                                                                                                                                                                                               ;

	// TODO: this is crap accessing technique.  Fix it with more elegance.
	RockField::RocksList_t& rocks = GetShip().m_player.m_session.m_universe->m_rockField.m_rocks;

	// Check each Player Bullet...
	for (auto bullet : m_bullets)
	{
		bool bulletHit = false;

		//...against bullet lifetime expiration...
		if (bullet->TickTock())
		{
			G_TRACE(__FUNCTION__ << "bulletDone, m_bullets.size(): " << m_bullets.size());
			bulletDone = true;
		}

		//... and against ALL Universe rocks
		for (auto rock : rocks)
		{
			auto distance = m_ship.m_player.m_session.DistanceBetweenPoints(*bullet, *rock);
			if (distance < rock->Radius())
			{
				G_TRACE("Hit: @ x: " << bullet->x << ", y: " << bullet->y);
				bulletHit = true;
				collidedRocks.push_back(rock);
				break;
			}
		}

		if (bulletHit)
		{
			collidedBullets.push_back(bullet);
		}
		G_TRACE("There are " << rocks.size() << " rocks.");
	}

	if (bulletDone)
	{
		m_bullets.pop_front();
		if (m_bullets.size() == 0)
		{
			G_TRACE(__FUNCTION__ << "bulletDone, no more m_bullets");
		}
	}

	// remove any collidedRocks
	for (auto rock : collidedRocks)
		rocks.remove(rock);

	// remove any collidedBullets
	for (auto bullet : collidedBullets)
		m_bullets.remove(bullet);
}

#define SH_TRACE TRACE
//#define SH_TRACE(...)

Ship::Ship(Player& player, int windowWidth, int windowHeight, double x, double y, double angle) :
	Context({ static_cast<uint16_t>(windowWidth), static_cast<uint16_t>(windowHeight)}),
	Position({ x,y }),
	Velocity({ 0,0 }),
	m_player(player),
	m_gun(std::make_unique<Gun>(*this)),
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
	//SH_TRACE("");
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
	//SH_TRACE(__FUNCTION__ << ", Key: (" << key << "," << (isDown ? "down" : "up") << ")");

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

//#define P_TRACE TRACE
#define P_TRACE(...)

Player::Player(AsteroidsSession& session, int width, int height)
	:
	Context({ static_cast<uint16_t>(width), static_cast<uint16_t>(height) }),
	m_session(session),
	m_ship(*this, width, height, width / 2, height / 2, static_cast<float>(M_PI / 2.0f))
{
	P_TRACE(__FUNCTION__);
}

Player::~Player()
{
	P_TRACE(__FUNCTION__);
}

void Player::KeyEvent(int key, bool isDown)
{
	P_TRACE(__FUNCTION__);
	m_ship.KeyEvent(key, isDown);
}

void Player::ResizeEvent(int w, int h)
{
	Context::Resize(static_cast<uint16_t>(w), static_cast<uint16_t>(h));
	
	// Any Context derived child objects should also have their
	// Context::Resize() methods called here.
	m_ship.Resize(static_cast<uint16_t>(w), static_cast<uint16_t>(h));
}

void Player::TickEvent(AsteroidsSession& session)
{
	P_TRACE(__FUNCTION__ << ", sessionID: " << session.SessionID());

	m_ship.TickEvent();

	int16_t shipX, shipY, shipA;

	m_ship.GetXY(shipX, shipY);
	m_ship.GetAngle(shipA);

	size_t outSize = 18;

	// Handle m_bullets from gun
	std::unique_ptr<AppBuffer> bulletsBuffer;
	auto gun = m_ship.m_gun;

	// ensure gun actually exists
	if (gun)
	{
		// Get size of bullet buffer (if any m_bullets are active)
		// and adjust "outsize" to allow room for m_bullets
		bulletsBuffer = std::move(gun->MakeBulletsPacket(session.IsLittleEndian()));
		if (bulletsBuffer.get())
			outSize += bulletsBuffer->size();
	}

	auto txBuff = std::make_unique<AppBuffer>(outSize, session.IsLittleEndian());

	txBuff->set_uint8(0xBB);
	txBuff->set_uint8(static_cast<uint8_t>(WebsockSession::MessageType_t::PlayerTickMessage));
	txBuff->set_uint32(session.SessionID());
	txBuff->set_uint32(session.GetTimerTick());
	txBuff->set_uint16(shipX);
	txBuff->set_uint16(shipY);
	txBuff->set_uint16(shipA);

	// default bullet count to 0
	txBuff->set_uint16(0);

	if (outSize > 18)
	{
		auto offset = txBuff->allocate(static_cast<int>(bulletsBuffer->size()));
		txBuff->set_uint16(16, bulletsBuffer->get_uint16(0));
		memcpy(txBuff->data() + offset, bulletsBuffer->data() + 2, bulletsBuffer->size() - 2);

		//TRACE("");
	}

	session.CommitTxBuffer(txBuff);
}

//#define U_TRACE TRACE
#define U_TRACE(...)

Universe::Universe(AsteroidsSession& session, int width, int height)
	:
	Context({ static_cast<uint16_t>(width), static_cast<uint16_t>(height) }),
	m_rockField(*this, width, height),
	m_session(session),
	m_sessions(g_sessions)
{
	U_TRACE(__FUNCTION__ << ", Session count:" << m_sessions.get_count());
}

Universe::~Universe()
{
	U_TRACE(__FUNCTION__);
}

void Universe::ResizeEvent(int w, int h)
{
	Context::Resize(static_cast<uint16_t>(w), static_cast<uint16_t>(h));
}

void Universe::ClickEvent(uint16_t x, uint16_t y)
{
	auto randRange = 400;

	auto randX = rand() % randRange;
	auto randY = rand() % randRange;

	randX -= randRange / 2;
	randY -= randRange / 2;

	auto dx = static_cast<double>(randX) / static_cast<double>(FPS);
	auto dy = static_cast<double>(randY) / static_cast<double>(FPS);

	m_rockField.LaunchOne(static_cast<double>(x), static_cast<double>(y), dx, dy, 30);
}

void Universe::KeyEvent(int key, bool isDown)
{
	(void)key;
	(void)isDown;
	U_TRACE(__FUNCTION__);
}

void Universe::TickEvent(AsteroidsSession& session)
{
	//U_TRACE(__FUNCTION__ << ", session ID : " << session.SessionID());

	m_rockField.TickEvent(session);

	// initial AppBuffer: just the UniverseTickMessage header
	auto txBuff = std::make_unique<AppBuffer>(10, session.IsLittleEndian());

	txBuff->set_uint8(0xBB);
	txBuff->set_uint8(static_cast<uint8_t>(WebsockSession::MessageType_t::UniverseTickMessage));
	txBuff->set_uint32(session.SessionID());
	txBuff->set_uint32(session.GetTimerTick());

	// Update all AsteroidSession state for single/multiplayer modes.
	{
		auto numRocks = session.m_universe->m_rockField.m_rocks.size();
		size_t outsize = sizeof(int16_t) + (numRocks * (2 * sizeof(int16_t)));

		U_TRACE("numRocks: " << numRocks << ", outsize: " << outsize);
		auto txBuff2 = std::make_unique<AppBuffer>(*txBuff, outsize, session.IsLittleEndian());

		txBuff2->set_uint16(static_cast<uint16_t>(numRocks));

		for (auto rock : session.m_universe->m_rockField.m_rocks)
		{
			txBuff2->set_uint16(static_cast<int16_t>(rock->x));
			txBuff2->set_uint16(static_cast<int16_t>(rock->y));
		}

		txBuff = std::move(txBuff2);
	}

	bool doShips = true;
	if (doShips)
	{
		// the number of active sessions gives us the (possible) number of ships in the Universe
		// Possible because a session doesn't have a ship until the client side registers new session.
		size_t numShips = 0;

		// go through all sessions looking for those with valid Asteroids::Player
		// and hence a valid ship
		for (auto pair : m_sessions.get_map())
		{
			auto sessionID = pair.first;
			auto sessPtr = pair.second;

			// don't count ourself, and don't count unregistered sessions
			if (sessionID == session.SessionID() || !sessPtr->m_player)
				continue;

			numShips++;
		}

		auto count = numShips;
		size_t outsize = sizeof(int16_t) + (count * (3 * sizeof(int16_t)));

		// new AppBuffer with contents of initial AppBuffer (header), plus room for ships data
		auto txBuff2 = std::make_unique<AppBuffer>(*txBuff, outsize, session.IsLittleEndian());
		txBuff2->set_uint16(static_cast<uint16_t>(count));

		size_t totalBullets = 0;

		// Add all other ship's x,y,angle variables to tick message
		for (auto pair : m_sessions.get_map())
		{
			auto sessionID = pair.first;
			auto sessPtr = pair.second;

			// don't count ourself, and don't count unregistered sessions
			if (sessionID == session.SessionID() || !sessPtr->m_player)
				continue;

			totalBullets += sessPtr->m_player->m_ship.m_gun->m_bullets.size();

			int16_t shipX, shipY, shipAngle;

			sessPtr->m_player->m_ship.GetXY(shipX, shipY);
			sessPtr->m_player->m_ship.GetAngle(shipAngle);

			txBuff2->set_uint16(shipX);
			txBuff2->set_uint16(shipY);
			txBuff2->set_uint16(shipAngle);
		}

		// figure out how much room is needed for bullets, make new AppBuffer from "txBuffer" but with moreRoom
		auto bulletsSpace = sizeof(uint16_t) + (sizeof(uint16_t) + totalBullets * 2 * sizeof(int16_t));
		auto txBuff3 = std::make_unique<AppBuffer>(*txBuff2, bulletsSpace, session.IsLittleEndian());

		txBuff3->set_uint16(static_cast<uint16_t>(totalBullets));

		// add all other ship's bullets at end of all other ships
		if (totalBullets)
		{
			//U_TRACE(__FUNCTION__ << "writeOffset: " << txBuff2->bytesWritten() << ", size: " << txBuff2->size());

			for (auto pair : m_sessions.get_map())
			{
				auto sessionID = pair.first;
				auto sessPtr = pair.second;

				// don't count ourself, and don't count unregistered sessions
				if (sessionID == session.SessionID() || !sessPtr->m_player)
					continue;

				for (auto bullet : sessPtr->m_player->m_ship.m_gun->m_bullets)
				{
					txBuff3->set_uint16(static_cast<int16_t>(bullet->x));
					txBuff3->set_uint16(static_cast<int16_t>(bullet->y));
				}
			}
		}
		txBuff = std::move(txBuff3);
	}

	session.CommitTxBuffer(txBuff);
}
