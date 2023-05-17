#ifndef BULLET_H
#define BULLET_H

#include "Structs.h"
#include "Session.h"

#define B_TRACE(...)

namespace asteroids
{
	class Bullet : public Position, public Velocity
	{
	public:
		Bullet(double x, double y, double dx, double dy);
		~Bullet();

		bool TickEvent(Session&);

	private:
		unsigned m_ticks_left;
	};
}
#endif
