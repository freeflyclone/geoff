#ifndef BULLET_H
#define BULLET_H

#include "Structs.h"
#include "Session.h"

#define B_TRACE(...)

namespace as2
{
	class Gun2;

	class Bullet2 : public Position, public Velocity
	{
	public:
		Bullet2(double x, double y, double dx, double dy);
		~Bullet2();

		bool TickEvent(Session&);

	private:
		unsigned m_ticks_left;
	};
}
#endif
