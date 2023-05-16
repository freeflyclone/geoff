#include "geoff.h"

#include "Universe.h"
#include "Session.h"
#include "RockField.h"
#include "Gun.h"
#include "Player.h"

#include "WebsockSessionManager.h"

using namespace asteroids;

namespace Websock
{
	WebsockSessionManager<Session> g_sessions;
};
using namespace Websock;

namespace asteroids
{
	// we want just one of these, but we want to control when it gets created.
	std::unique_ptr<Universe> g_universe;

	const Universe& Init(int w, int h)
	{
		if (!g_universe)
			g_universe = std::make_unique<Universe>(w, h);

		return *g_universe;
	}
};

//#undef UN_TRACE
//#define UN_TRACE TRACE

Universe::Universe(int width, int height)
	: Size({ static_cast<double>(width), static_cast<double>(height) }),
	m_rockField(std::make_unique<RockField>(width, height))
{
	UN_TRACE(__FUNCTION__);
}

Universe::~Universe()
{
	UN_TRACE(__FUNCTION__);
}

RockField& Universe::GetRockField()
{
	return *m_rockField;
}

void Universe::TickEvent(Session& session)
{
	UN_TRACE(__FUNCTION__);

	if (m_rockField)
		m_rockField->TickEvent(session);

	CollisionDetection(session);

	// TODO: output stuff to client
}

void Universe::CollisionDetection(Session& session)
{
	std::list<RockField::RockIterator> collidedRocks;
	RockField::RockIterator rockIter;

	std::list<Gun::BulletIterator> collidedBullets;
	Gun::BulletIterator bulletIter;

	auto& rocks = m_rockField->GetRocks();

	for (rockIter = rocks.begin(); rockIter != rocks.end(); rockIter++)
	{
		for (auto pair : g_sessions.get_map())
		{
			//auto sessionID = pair.first;
			auto sess = pair.second;

			auto& bullets = sess->GetPlayer().GetShip().GetGun().GetBullets();

			for (bulletIter = bullets.begin(); bulletIter != bullets.end(); bulletIter++)
			{
				auto rock = rockIter->get();
				auto bullet = bulletIter->get();

				if (session.DistanceBetweenPoints(*rock, *bullet) < rock->Radius())
				{
					//TRACE("Bullet Hit");
					collidedRocks.push_back(rockIter);
					collidedBullets.push_back(bulletIter);
				}
			}

			for (auto bullet : collidedBullets)
				bullets.erase(bullet);

			// Once the all of the collidedBullets have been erased,
			// clear the collidedBullets list, else mayhem on next rock in
			// the outer loop.
			collidedBullets.clear();
		}
	}

	for (auto it : collidedRocks)
		m_rockField->DestroyRock(it);

	collidedRocks.clear();
}

void Universe::OtherSessionsTickEvent(Session& session)
{
	UN_TRACE(__FUNCTION__);
}