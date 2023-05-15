#ifndef SHIP_H
#define SHIP_H

#include "Structs.h"
#include "Session.h"

#define SH_TRACE(...)

namespace as2
{
	class Gun;

	class Ship2 : public Context, public Position, public Orientation, public Velocity
	{
	public:
		Ship2(uint16_t cw, uint16_t ch, double dx, double dy, double angle);
		~Ship2();

		void GetXYA(double& x, double& y, double& a);

		void MoveShip();
		void FireGuns();

		void KeyEvent(int key, bool isDown);
		void TickEvent(Session&);

	private:
		std::unique_ptr<Gun> m_gun;
	};
}
#endif
