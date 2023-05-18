#include "geoff.h"
#include "Consts.h"

#include "RockField.h"
#include "Rock.h"

#include "Player.h"
#include "Gun.h"

#include "Universe.h"
#include "Session.h"

#include "WebsockSessionManager.h"
#include "AppSession.h"

using namespace asteroids;
using namespace Websock;

namespace asteroids
{
	// we want just one of these, but we want to control when it gets created.
	std::unique_ptr<Universe> g_universe;

	Universe* GetUniverse()
	{
		if (!g_universe)
			g_universe = std::make_unique<Universe>(8192, 8192);

		return g_universe.get();
	}
};

//#undef UN_TRACE
//#define UN_TRACE TRACE

Universe::Universe(int width, int height)
	: Size({ static_cast<double>(width), static_cast<double>(height) }),
	m_rockField(std::make_unique<RockField>(width, height)),
	m_timer(std::make_unique<Timer>(1000000 / FPS)),
	m_ticks(0)
{
	UN_TRACE(__FUNCTION__);
}

Universe::~Universe()
{
	UN_TRACE(__FUNCTION__);
}

void Universe::NewPlayer(Session& session, int w, int h)
{
	m_players[session.SessionID()] = std::make_shared<Player>(session, w, h);

	TRACE(__FUNCTION__);
}
std::shared_ptr<Player> Universe::GetPlayerById(uint32_t sessionID)
{
	return m_players[sessionID];
}

void Universe::TickEvent(uint32_t tickCount)
{
	UN_TRACE(__FUNCTION__);

	m_ticks = tickCount;

	if (m_rockField)
		m_rockField->TickEvent();

	CollisionDetection();

	for (auto pair : g_sessions.get_map())
	{
		auto sess = pair.second;

		if (!sess->GetPlayer())
		{
			TRACE("found session with no Player yet.");
			continue;
		}

		PerSessionTickEvent(*sess);
	}
}

void Universe::CollisionDetection()
{
	std::list<RockField::RockIterator> collidedRocks;
	RockField::RockIterator rockIter;

	std::list<Gun::BulletIterator> collidedBullets;
	Gun::BulletIterator bulletIter;

	for (auto pair : m_players)
	{
		auto player = pair.second;
		auto& rocks = m_rockField->GetRocks();

		for (rockIter = rocks.begin(); rockIter != rocks.end(); rockIter++)
		{
			auto bullets = player->GetShip()->GetGun()->GetBullets();
			for (bulletIter = bullets->begin(); bulletIter != bullets->end(); bulletIter++)
			{
				auto bullet = bulletIter->get();
				auto rock = rockIter->get();

				if (DistanceBetweenPoints(*rock, *bullet) < rock->Radius())
				{
					UN_TRACE("Bullet Hit");
					collidedBullets.push_back(bulletIter);
					collidedRocks.push_back(rockIter);
				}
			}
			for (auto& bullet : collidedBullets)
				bullets->erase(bullet);
			collidedBullets.clear();
		}

		for (auto& rock : collidedRocks)
 			m_rockField->DestroyRock(rock);
		collidedRocks.clear();
	}
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
	txBuff->set_uint32(g_universe->GetTicks());
	txBuff->set_uint16(static_cast<uint16_t>(sizeW));
	txBuff->set_uint16(static_cast<uint16_t>(sizeH));

	// Update all Session state for single/multiplayer modes.

	bool doRocks = true;
	if (doRocks)
	{
		auto& rocks = g_universe->GetRockField()->GetRocks();
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

	bool doShips = true;
	if (doShips)
	{
		// the number of active sessions gives us the (possible) number of ships in the Universe
		// Possible because a session doesn't have a ship until the client side registers new session.
		size_t numShips = 0;

		// go through all sessions looking for those with valid Asteroids::Player
		// and hence a valid ship
		for (auto pair : g_sessions.get_map())
		{
			auto sessionID = pair.first;
			auto sessPtr = pair.second;

			// skip ourself, and don't count unregistered sessions
			if (sessionID == session.SessionID() || !sessPtr->GetPlayer())
			{
				if (!sessPtr->GetPlayer())
					TRACE("Found session with no Player yet.");
				continue;
			}

			numShips++;
		}

		if (numShips)
		{
			size_t totalBullets = 0;
			size_t outsize = sizeof(int16_t) + (numShips * (3 * sizeof(int16_t)));

			// new AppBuffer with contents of initial AppBuffer (header), plus room for ships data
			auto txBuff2 = std::make_unique<AppBuffer>(*txBuff, outsize, session.IsLittleEndian());
			txBuff2->set_uint16(static_cast<uint16_t>(numShips));

			//TRACE("there are " << numShips << " other ship(s)");
			for (auto pair : g_sessions.get_map())
			{
				auto sessionID = pair.first;
				auto sessPtr = pair.second;
				auto player = sessPtr->GetPlayer();

				if (sessionID == session.SessionID() || !player)
				{
					if (!player)
						TRACE("Found a session with no with no player yet, where we shouldn't!");

					continue;
				}
				
				auto ship = sessPtr->GetPlayer()->GetShip();

				txBuff2->set_uint16(static_cast<uint16_t>(ship->posX));
				txBuff2->set_uint16(static_cast<uint16_t>(ship->posY));
				txBuff2->set_uint16(static_cast<uint16_t>(ship->angle * FP_4_12));

				totalBullets += ship->GetGun()->GetBullets()->size();
			}

			// at this point, we're done with txBuff2 so we can reuse the identifier
			txBuff = std::move(txBuff2);

			if (totalBullets)
			{
				//TRACE(totalBullets << " other bullets.");
				auto bulletsSize = sizeof(int16_t) + (totalBullets * 2 * sizeof(int16_t));
				txBuff2 = std::make_unique<AppBuffer>(*txBuff, bulletsSize, session.IsLittleEndian());

				txBuff2->set_uint16(static_cast<uint16_t>(totalBullets));

				for (auto pair : g_sessions.get_map())
				{
					auto sessionID = pair.first;
					auto sessPtr = pair.second;

					if (sessionID == session.SessionID() || !sessPtr->GetPlayer())
					{
						continue;
					}

					auto ship = sessPtr->GetPlayer()->GetShip();
					auto bullets = ship->GetGun()->GetBullets();

					for (auto& bullet : *bullets)
					{
						txBuff2->set_uint16(static_cast<int16_t>(bullet->posX));
						txBuff2->set_uint16(static_cast<int16_t>(bullet->posY));
					}
				}

				txBuff = std::move(txBuff2);
			}
		}
	}

	// Send all of it to the client side
	session.CommitTxBuffer(txBuff);
}
