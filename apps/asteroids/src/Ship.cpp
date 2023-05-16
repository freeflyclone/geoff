#include "geoff.h"

#include "Consts.h"

#include "Ship.h"
#include "Gun.h"

using namespace asteroids;

#undef SH_TRACE
#define SH_TRACE TRACE

Ship::Ship(uint16_t cw, uint16_t ch, double x, double y, double angle)
	:
	Context({cw, ch}),
	Position({ x, y }),
	Orientation({ angle }),
	Radius({SHIP_SIZE}),
	m_gun(std::make_unique<Gun>())
{
	SH_TRACE(__FUNCTION__);
}

Ship::~Ship()
{
	SH_TRACE(__FUNCTION__);
}

void Ship::FireGuns()
{
	//SH_TRACE(__FUNCTION__);

	if(m_gun)
		m_gun->Fire(*this);
}

void Ship::KeyEvent(int key, bool isDown)
{
	(void)key;
	(void)isDown;

	//SH_TRACE(__FUNCTION__);

	switch (key)
	{
	case 37:
		m_rotation = (isDown) ? SHIP_RADS_PER_TICK : 0;
		break;

	case 39:
		m_rotation = (isDown) ? -SHIP_RADS_PER_TICK : 0;
		break;

	case 38:
		m_thrusting = isDown;
		break;

	case 32:
			if (isDown)
				FireGuns();
			break;
	}
}

void Ship::TickEvent(Session& session)
{
	//SH_TRACE(__FUNCTION__);
	MoveShip();

	if(m_gun)
		m_gun->TickEvent(session);
}

void Ship::MoveShip()
{
	SH_TRACE(__FUNCTION__);
}

