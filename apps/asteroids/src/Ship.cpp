#include "geoff.h"

#include "Ship.h"

using namespace as2;

#undef SH_TRACE
#define SH_TRACE TRACE

Ship2::Ship2(uint16_t cw, uint16_t ch, double x, double y, double angle)
	:
	Context({cw, ch}),
	Position({ x, y }),
	Orientation({ angle })
{
	SH_TRACE(__FUNCTION__);
}

Ship2::~Ship2()
{
	SH_TRACE(__FUNCTION__);
}

void Ship2::GetXYA(double& x, double& y, double& a)
{
	x = Position::posX;
	y = Position::posY;
	a = Orientation::angle;
	SH_TRACE(__FUNCTION__);
}

void Ship2::MoveShip()
{
	SH_TRACE(__FUNCTION__);
}

void Ship2::FireShot()
{
	SH_TRACE(__FUNCTION__);
}

void Ship2::KeyEvent(int key, bool isDown)
{
	(void)key;
	(void)isDown;
	SH_TRACE(__FUNCTION__);
}

void Ship2::TickEvent(Session& session)
{
	SH_TRACE(__FUNCTION__);
}
