#ifndef ASTEROIDS_H
#define ASTEROIDS_H

#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

namespace Asteroids
{
    struct Context
    {
        void Resize(uint16_t width, uint16_t height);
        
        uint16_t m_width;
        uint16_t m_height;
    };

    class Bullet
    {
    public:
        Bullet(int x, int y, int dx, int dy);
        ~Bullet();

    private:
        int m_x, m_y;
        int m_dx, m_dy;
    };

	class Ship : public Context
	{
	public:
		Ship(int windowWidth, int windowHeight, double x, double y, double angle);

		~Ship();

        void MoveShip();
        void GetXY(int16_t& x, int16_t& y);
        void GetAngle(int16_t& angle);
		void KeyEvent(int key, bool isDown);
		void TickEvent();

	private:
        double m_x, m_y;
        double m_angle;
        double m_radius;

        bool m_canShoot;
        bool m_dead;
        int m_explode_time;
        std::vector<Bullet> m_bullets;
        double m_rotation;
        bool m_thrusting;
        struct Thrust
        {
            double x;
            double y;
        } m_thrust;
        bool m_show_position;
	};
};

#endif // ASTEROIDS