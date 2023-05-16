#include "geoff.h"
#include "Consts.h"

#include "RockField.h"
#include "Rock.h"

#include "Player.h"
#include "Gun.h"

#include "Universe.h"
#include "Session.h"

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

	const Universe& GetUniverse()
	{
		if (!g_universe)
			g_universe = std::make_unique<Universe>(8192, 8192);

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

void Universe::TickEvent(Session& session)
{
	UN_TRACE(__FUNCTION__);

	if (m_rockField)
		m_rockField->TickEvent(session);

	// Fire all TickEvent handlers on each session.
	for (auto pair : g_sessions.get_map())
	{
		//auto sessionID = pair.first;
		auto sessPtr = pair.second;

		sessPtr->TickEvent();

		PerSessionTickEvent(*sessPtr);
	}

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

void Universe::PerSessionTickEvent(Session& session)
{
	UN_TRACE(__FUNCTION__);

	if (!g_universe)
		return;

	// initial AppBuffer: just the UniverseTickMessage header
	auto txBuff = std::make_unique<AppBuffer>(14, session.IsLittleEndian());

	txBuff->set_uint8(0xBB);
	txBuff->set_uint8(static_cast<uint8_t>(WebsockSession::MessageType_t::UniverseTickMessage));
	txBuff->set_uint32(session.SessionID());
	txBuff->set_uint32(session.GetTimer().GetTick());
	txBuff->set_uint16(static_cast<uint16_t>(sizeW));
	txBuff->set_uint16(static_cast<uint16_t>(sizeH));

	bool doRocks = true;
	if (doRocks)
	{
		auto& rocks = g_universe->GetRockField().GetRocks();
		auto numRocks = rocks.size();

		size_t outsize = sizeof(int16_t) + (numRocks * (3 * sizeof(int16_t)));

		//U_TRACE("numRocks: " << numRocks << ", outsize: " << outsize);
		auto txBuff2 = std::make_unique<AppBuffer>(*txBuff, outsize, session.IsLittleEndian());

		txBuff2->set_uint16(static_cast<uint16_t>(numRocks));

		for (auto& rock : rocks)
		{
			txBuff2->set_uint16(static_cast<int16_t>(rock->posX));
			txBuff2->set_uint16(static_cast<int16_t>(rock->posY));
			txBuff2->set_uint16(static_cast<uint16_t>(rock->Radius()));
		}

		txBuff = std::move(txBuff2);
	}

	// Update all Session state for single/multiplayer modes.
	// Send all of it to the client side
	session.CommitTxBuffer(txBuff);
}