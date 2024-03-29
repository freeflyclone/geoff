#ifndef SHIP_H
#define SHIP_H

#include "Structs.h"
#include "Session.h"

#define SH_TRACE(...)

#include "Consts.h"

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
		void Explode();
		bool IsExploding() { return m_exploding; }
		bool Vulnerable() { return !m_invulnerable; }
		bool Dead() { return m_dead; }
		void Resurrect();

		std::unique_ptr<AppBuffer> MakeBuffer(Session&);

		void KeyEvent(int key, bool isDown);
		void ResizeEvent(int w, int h);
		void TickEvent(Session&);

	private:
		void MoveShip();
		void NewLife();

		std::unique_ptr<Gun> m_gun;
		double m_rotation;
		double m_max_delta_v;
		double m_viewport_margin;
		double m_explosion_duration;
		double m_inv_duration;
		double m_blink_time;
		int m_blink_num;
		int m_lives_left;
		bool m_thrusting;
		bool m_left;
		bool m_right;
		bool m_slide_viewport;
		bool m_wrap_viewport;
		bool m_enforce_boundaries;
		bool m_exploding;
		bool m_visible;
		bool m_dead;
		bool m_invulnerable;
	};
}
#endif
