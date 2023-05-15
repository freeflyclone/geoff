#include "geoff.h"
#include "Bullet.h"

#undef B_TRACE
#define B_TRACE TRACE

Bullet2::Bullet2(Gun2& gun, double x, double y, double dx, double dy)
	: 
	Position({ x, y }),
	Velocity({ dx, dy }),
	m_gun(gun)
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

Gun2& Bullet2::GetGun()
{
	return m_gun;
}