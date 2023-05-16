#ifndef ROCK_H
#define ROCK_H

#include "Structs.h"
#include "Session.h"

#define RK_TRACE(...)

namespace as2
{
	class Rock : public Position, public Velocity
	{
	public:
		Rock(double x, double y, double dx, double dy, double radius);
		~Rock();

		void TickEvent(Session&);

	private:
		double m_radius;
	};
}

#endif
