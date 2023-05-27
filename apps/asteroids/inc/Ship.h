#ifndef SHIP_H
#define SHIP_H

#include "Structs.h"
#include "Session.h"

#define SH_TRACE(...)

namespace asteroids
{
	class Gun;

	class Ship : public Context, public Position, public Velocity, public Orientation, public Radius
	{
	public:
		Ship(uint16_t cw, uint16_t ch, double dx, double dy, double angle);
		~Ship();

		Gun* GetGun() { return m_gun.get(); }

		void FireGuns();

		void KeyEvent(int key, bool isDown);
		void ResizeEvent(int w, int h);
		void TickEvent(Session&);

	private:
		void MoveShip();

		std::unique_ptr<Gun> m_gun;
		double m_rotation;
		bool m_thrusting;
		bool m_left;
		bool m_right;
	};
}
#endif
