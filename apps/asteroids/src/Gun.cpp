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

Gun::BulletList_t& Gun::GetBullets()
{
	return m_bullets;
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
	if (m_bullets.size() == 0)
		return std::move(nullptr);

	auto bulletDataSize = 2 * sizeof(int16_t);

	auto appBufferSize = 2 + (m_bullets.size() * bulletDataSize);
	auto txBuff = std::make_unique<AppBuffer>(appBufferSize, session.IsLittleEndian());

	txBuff->set_uint16(static_cast<uint16_t>(m_bullets.size()));

	size_t i = 0;
	for (auto& bullet : m_bullets)
	{
		auto cx = static_cast<uint16_t>(bullet->posX);
		auto cy = static_cast<uint16_t>(bullet->posY);

		GN_TRACE("bullet[" << i << "]: " << bullet->posX << "(" << cx << ")," << bullet->posY << "(" << cy << ")");

		txBuff->set_uint16(cx);
		txBuff->set_uint16(cy);
		i++;
	}

	return txBuff;
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