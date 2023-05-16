#include "geoff.h"

#include "Consts.h"
#include "RockField.h"
#include "Rock.h"

using namespace as2;

#undef RF_TRACE
#define RF_TRACE TRACE

RockField::RockField(int w, int h)
	:
	Context(
		{
			(uint16_t)w, 
			(uint16_t)h, 
			(uint16_t)(g_universe->sizeW / 2 - w/2), 
			(uint16_t)(g_universe->sizeH / 2 - h / 2)
		}
	)
{
	RF_TRACE(__FUNCTION__);
}

RockField::~RockField()
{
	RF_TRACE(__FUNCTION__);
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

void RockField::DestroyRock(RockIterator rock) 
{
	RF_TRACE(__FUNCTION__);
}

RockField::RockList_t& RockField::GetRocks()
{
	return m_rocks;
}

void RockField::EraseRocks()
{
	m_rocks.clear();
}

void TickEvent()
{
	RF_TRACE(__FUNCTION__);
}
