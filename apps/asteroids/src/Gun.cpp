#include "geoff.h"

#include "Consts.h"
#include "Gun.h"
#include "Ship.h"

using namespace asteroids;

//#undef GN_TRACE
//#define GN_TRACE TRACE

Gun::Gun()
{
	GN_TRACE(__FUNCTION__);
}

Gun::~Gun()
{
	GN_TRACE(__FUNCTION__);
}

void Gun::Fire(Ship& ship)
{
	// Calculate firing solution from ship position & angle
	double offX = static_cast<double>(ship.ctxOX);
	double offY = static_cast<double>(ship.ctxOY);
	double posX = ship.posX - offX;
	double posY = ship.posY - offY;

	auto px = (posX + ship.radius * cos(ship.angle)) + offX;
	auto py = (posY - ship.radius * sin(ship.angle)) + offY;

	auto mvx = (double)MUZZLE_VELOCITY * cos(ship.angle) / (double)FPS;
	auto mvy = (double)MUZZLE_VELOCITY * -sin(ship.angle) / (double)FPS;

	GN_TRACE(__FUNCTION__ << "Ship: " << ship.posX << "," << ship.posY << "," << ship.angle);

	m_bullets.emplace_back(std::make_unique<Bullet>(px, py, mvx, mvy));
}

std::unique_ptr<AppBuffer> Gun::MakeBulletsBuffer(Session& session)
{
	GN_TRACE(__FUNCTION__);

	return nullptr;
}

void Gun::TickEvent(Session& session)
{
	if (m_bullets.empty())
		return;

	GN_TRACE(__FUNCTION__ << ", there are " << m_bullets.size() << " bullet(s)");
	
	bool timedOut = false;

	for (auto& bullet : m_bullets)
	{
		if (bullet->TickEvent(session))
		{
			GN_TRACE(__FUNCTION__ << ", bullet timed out");
			timedOut = true;
		}
	}

	if (timedOut)
	{
		m_bullets.pop_front();
		if (m_bullets.size() == 0)
		{
			GN_TRACE(__FUNCTION__ << ", no more bullets");
		}
	}
}