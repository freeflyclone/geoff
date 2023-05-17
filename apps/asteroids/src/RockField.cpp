#include "geoff.h"

#include "Consts.h"
#include "RockField.h"
#include "Rock.h"

using namespace asteroids;

//#undef RF_TRACE
//#define RF_TRACE TRACE

RockField::RockField(int w, int h)
	:
	Context({  (uint16_t)w, (uint16_t)h })
{
	RF_TRACE(__FUNCTION__);
}

RockField::~RockField()
{
	RF_TRACE(__FUNCTION__);
}

RockField::RockList_t& RockField::GetRocks()
{
	return m_rocks;
}

void RockField::LaunchOne(double x, double y, double r)
{
	RF_TRACE(__FUNCTION__);

	auto random = []() -> double {
		return (double)rand() / (double)RAND_MAX;
	};

	double dx = random() * ROCK_SPEED / FPS * (random() < 0.5 ? 1 : -1);
	double dy = random() * ROCK_SPEED / FPS * (random() < 0.5 ? 1 : -1);

	RockPtr_t rock(new Rock(x, y, dx, dy, r));

	m_rocks.push_back(std::move(rock));
}

void RockField::DestroyRock(RockIterator rockIt) 
{
	RF_TRACE(__FUNCTION__);
	Rock rock = *rockIt->get();

	m_rocks.erase(rockIt);

	double xPos = rock.posX;
	double yPos = rock.posY;
	double dx = rock.deltaX;
	double dy = rock.deltaY;
	double radius = rock.Radius();

	RF_TRACE("Destroy Rock: X: " << rock->x << ", Y: " << rock->y << ", radius: " << rock->Radius());

	if (radius >= ROCK_RADIUS)
	{
		LaunchOne(xPos, yPos, radius / 2);
		LaunchOne(xPos, yPos, radius / 2);
	}
	else if (radius >= ROCK_RADIUS / 2)
	{
		LaunchOne(xPos, yPos, radius / 2);
		LaunchOne(xPos, yPos, radius / 2);
	}
}

void RockField::TickEvent(Session& session)
{
	RF_TRACE(__FUNCTION__);
	for (auto& rock : m_rocks)
	{
		rock->TickEvent(session);
	}
}
