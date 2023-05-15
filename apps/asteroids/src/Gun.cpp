#include "geoff.h"

#include "Gun.h"
#include "Ship.h"

using namespace as2;

#undef GN_TRACE
#define GN_TRACE TRACE

Gun::Gun(Ship2& s)
	:
	m_ship(s)
{
	GN_TRACE(__FUNCTION__);
}

Gun::~Gun()
{
	GN_TRACE(__FUNCTION__);
}

void Gun::Fire()
{
	// TODO: calculate firing solution from ship position & angle
	double sx, sy, sa;
	m_ship.GetXYA(sx, sy, sa);

	GN_TRACE(__FUNCTION__ << "Ship: " << sx << "," << sy << "," << sa);

	m_bullets.emplace_back(std::make_unique<Bullet2>(0,0,0,0));
}

std::unique_ptr<AppBuffer> Gun::MakeBulletsBuffer(Session& session)
{
	GN_TRACE(__FUNCTION__);

	return nullptr;
}

void Gun::TickEvent(Session& session)
{
	GN_TRACE(__FUNCTION__ << ", there are " << m_bullets.size() << " bullet(s)");

	for (auto& bullet : m_bullets)
	{
		bullet->TickEvent(session);
	}
}