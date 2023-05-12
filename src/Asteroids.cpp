#include "geoff.h"

#include "Asteroids.h"
#include "AsteroidsSession.h"
#include "WebsockSessionManager.h"
#include "WebsockServer.h"

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
	// we want just one of these, but we want to control when it gets created.
	std::unique_ptr<Universe> g_universe;

	const Universe& Init(int w, int h)
	{
		if (!g_universe)
			g_universe = std::make_unique<Universe>(w, h, 1000000 / FPS);

		return *g_universe;
	}
};

#define CTX_TRACE(...)
//#define CTX_TRACE TRACE

void Context::Resize(uint16_t w, uint16_t h)
{
	width = w;
	height = h;

	CTX_TRACE(__FUNCTION__ << "width: " << width << ", height: " << height);
}

void Context::Move(uint16_t x, uint16_t y)
{
	offsetX = x;
	offsetY = y;

	CTX_TRACE(__FUNCTION__ << "offsetX: " << offsetX << ", offsetY: " << offsetY);
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
		auto ctx = ship;

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
	auto ctx = (Asteroids::Context)rockField;

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
	Context({ 
		static_cast<uint16_t>(w), 
		static_cast<uint16_t>(h), 
		static_cast<uint16_t>(w/2),
		static_cast<uint16_t>(h/2)
	}),
	m_universe(universe)
{

}

RockField::~RockField()
{
}

void RockField::LaunchOne(double x, double y, double r)
{
	auto random = []() -> double{
		return (double)rand() / (double)RAND_MAX;
	};

	double dx = random() * ROCK_SPEED / FPS * (random() < 0.5 ? 1 : -1);
	double dy = random() * ROCK_SPEED / FPS * (random() < 0.5 ? 1 : -1);

	m_rocks.emplace_back(std::make_unique<Rock>(*this, x, y, dx, dy, r));

	//TRACE(__FUNCTION__ << "x: " << x << ", y: " << y); //  << ", radius: " << radius << ", " << m_rocks.size() << " rock(s) exist.");
}

void RockField::DestroyRock(std::shared_ptr<Rock> rock)
{
	m_rocks.remove(rock);

	//TRACE("Destroy Rock: X: " << rock->x << ", Y: " << rock->y << ", radius: " << rock->Radius());

	if (rock->Radius() >= ROCK_RADIUS)
	{
		LaunchOne(rock->x, rock->y, rock->Radius() / 2);
		LaunchOne(rock->x, rock->y, rock->Radius() / 2);
	}
	else if (rock->Radius() >= ROCK_RADIUS / 2)
	{
		LaunchOne(rock->x, rock->y, rock->Radius() / 2);
		LaunchOne(rock->x, rock->y, rock->Radius() / 2);
	}
}

void RockField::TickEvent() 
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

	for (auto bullet : m_bullets)
	{
		if (bullet->TickTock())
		{
			G_TRACE(__FUNCTION__ << "bulletDone, m_bullets.size(): " << m_bullets.size());
			bulletDone = true;
		}
	}

	if (bulletDone)
	{
		m_bullets.pop_front();
		if (m_bullets.size() == 0)
		{
			G_TRACE(__FUNCTION__ << "bulletDone, no more m_bullets");
		}
	}
}

#define SH_TRACE TRACE
//#define SH_TRACE(...)

Ship::Ship(Player& player, int w, int h, double x, double y, double angle) :
	Context({
		static_cast<uint16_t>(w),
		static_cast<uint16_t>(h),
		static_cast<uint16_t>(w / 2),
		static_cast<uint16_t>(h / 2)
		}),
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

Player::Player(AsteroidsSession& session, int w, int h)
	:
	Context({
		static_cast<uint16_t>(w),
		static_cast<uint16_t>(h),
		static_cast<uint16_t>(w / 2),
		static_cast<uint16_t>(h / 2)
		}),
	m_session(session),
	m_ship(*this, w, h, w / 2, h / 2, static_cast<float>(M_PI / 2.0f))
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

void Player::ClickEvent(int clickX, int clickY)
{
	P_TRACE(__FUNCTION__);

	g_universe->m_rockField.LaunchOne(clickX, clickY, ROCK_RADIUS);
}

void Player::ResizeEvent(int eventW, int eventH)
{
	Context::Resize(static_cast<uint16_t>(eventW), static_cast<uint16_t>(eventH));
	
	// Any Context derived child objects should also have their
	// Context::Resize() methods called here.
	m_ship.Resize(static_cast<uint16_t>(eventW), static_cast<uint16_t>(eventH));
}

void Player::TickEvent(AsteroidsSession& session)
{
	P_TRACE(__FUNCTION__ << ", sessionID: " << session.SessionID());

	// update all Player specfic state here. (objects with TickEvent() handler)
	m_ship.TickEvent();

	// dump all player-specific state to the client
	int16_t shipX, shipY, shipA;

	m_ship.GetXY(shipX, shipY);
	m_ship.GetAngle(shipA);

	size_t outSize = 26;

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
	txBuff->set_uint32(g_universe->GetTimerTick());

	txBuff->set_uint16(Context::width);
	txBuff->set_uint16(Context::height);
	txBuff->set_uint16(Context::offsetX);
	txBuff->set_uint16(Context::offsetY);

	txBuff->set_uint16(shipX);
	txBuff->set_uint16(shipY);
	txBuff->set_uint16(shipA);

	// default bullet count to 0
	txBuff->set_uint16(0);

	if (outSize > 26)
	{
		auto offset = txBuff->allocate(static_cast<int>(bulletsBuffer->size()));
		txBuff->set_uint16(24, bulletsBuffer->get_uint16(0));
		memcpy(txBuff->data() + offset, bulletsBuffer->data() + 2, bulletsBuffer->size() - 2);
	}

	session.CommitTxBuffer(txBuff);
}

#define U_TRACE TRACE
//#define U_TRACE(...)

Universe::Universe(int w, int h, uint32_t interval)
	:
	Context({
		static_cast<uint16_t>(w),
		static_cast<uint16_t>(h),
		static_cast<uint16_t>(w / 2),
		static_cast<uint16_t>(h / 2)
		}),
	m_rockField(*this, w, h),
	m_sessions(g_sessions),
	m_tick_interval_in_us(interval)
{
	U_TRACE(__FUNCTION__ << ", width: " << w << ", height: " << h << ", interval: " << interval << ", Session count : " << m_sessions.get_count());

	m_timer = std::make_unique<net::deadline_timer>(*WebsockServer::GetInstance().IoContext(), boost::posix_time::microseconds(m_tick_interval_in_us));
	m_run_timer = true;
	TimerTicker();
}

Universe::~Universe()
{
	U_TRACE(__FUNCTION__);

	m_run_timer = false;
	m_timer_complete = false;
	int retries = 50;
	while (!m_timer_complete && retries)
	{
		m_run_timer = false;
		std::this_thread::sleep_for(std::chrono::duration(std::chrono::milliseconds(10)));
		retries--;
	}
}

void Universe::TickEvent()
{
	//U_TRACE(__FUNCTION__);

	// update all Universe specfic state here.  (objects with TickEvent() handler)
	m_rockField.TickEvent();

	// Collision detection
	// Rocks and/or Bullets to be destroyed AFTER all collision checks are complete.
	// (deleting these in the collision detection loops causes mayhem)
	RockField::RocksList_t collidedRocks;
	Gun::BulletsList_t collidedBullets;

	// Bullets vs Rocks collisions. (brute force, no optimization)
	for (auto rock : m_rockField.m_rocks)
	{
		for (auto pair : m_sessions.get_map())
		{
			auto sessionID = pair.first;
			auto session = pair.second;

			auto bullets = session->m_player->m_ship.m_gun->m_bullets;

			for (auto bullet : bullets)
			{
				if (session->DistanceBetweenPoints(*rock, *bullet) < rock->Radius())
				{
					collidedRocks.push_back(rock);
					collidedBullets.push_back(bullet);
					break;
				}
			}

			for (auto bullet : collidedBullets)
				bullets.remove(bullet);
		}
	}

	for (auto rock : collidedRocks)
		m_rockField.DestroyRock(rock);

	// Fire all TickEvent handlers on each session.
	for (auto pair : m_sessions.get_map())
	{
		auto sessionID = pair.first;
		auto session = pair.second;

		session->HandleTimerTick();
		PerSessionTickEvent(*session);
	}

	m_timer_tick++;
}

void Universe::TimerTicker()
{
	TickEvent();

	boost::system::error_code ec;

	if (!m_timer)
	{
		TRACE("")
		m_timer_complete = true;
		return;
	}

	if (!m_run_timer)
	{
		//TRACE("")
		m_timer_complete = true;
		return;
	}

	m_timer->expires_from_now(boost::posix_time::microseconds(m_tick_interval_in_us), ec);
	if (ec)
	{
		TRACE(ec);
		return;
	}

	m_timer->async_wait([this](const boost::system::error_code& e) {
		(void)e;
		TimerTicker();
	});
}

void Universe::PerSessionTickEvent(AsteroidsSession& session)
{
	//U_TRACE(__FUNCTION__ << ", session ID : " << session.SessionID());

	// initial AppBuffer: just the UniverseTickMessage header
	auto txBuff = std::make_unique<AppBuffer>(14, session.IsLittleEndian());

	txBuff->set_uint8(0xBB);
	txBuff->set_uint8(static_cast<uint8_t>(WebsockSession::MessageType_t::UniverseTickMessage));
	txBuff->set_uint32(session.SessionID());
	txBuff->set_uint32(g_universe->GetTimerTick());
	txBuff->set_uint16(Context::width);
	txBuff->set_uint16(Context::height);

	// Update all AsteroidSession state for single/multiplayer modes.
	{
		auto numRocks = g_universe->m_rockField.m_rocks.size();
		size_t outsize = sizeof(int16_t) + (numRocks * (3 * sizeof(int16_t)));

		//U_TRACE("numRocks: " << numRocks << ", outsize: " << outsize);
		auto txBuff2 = std::make_unique<AppBuffer>(*txBuff, outsize, session.IsLittleEndian());

		txBuff2->set_uint16(static_cast<uint16_t>(numRocks));

		for (auto rock : g_universe->m_rockField.m_rocks)
		{
			txBuff2->set_uint16(static_cast<int16_t>(rock->x));
			txBuff2->set_uint16(static_cast<int16_t>(rock->y));
			txBuff2->set_uint16(static_cast<uint16_t>(rock->Radius()));
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
