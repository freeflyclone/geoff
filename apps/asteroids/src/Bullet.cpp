#include "geoff.h"
#include "Bullet.h"

using namespace as2;

#undef B_TRACE
#define B_TRACE TRACE

Bullet2::Bullet2(double x, double y, double dx, double dy)
	: 
	Position({ x, y }),
	Velocity({ dx, dy })
{
	B_TRACE(__FUNCTION__);
}

Bullet2::~Bullet2()
{
	B_TRACE(__FUNCTION__);
}

bool Bullet2::TickEvent()
{
	B_TRACE(__FUNCTION__);

	return false;
}
