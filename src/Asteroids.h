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

	class Ship
	{
	public:
		Ship(int windowWidth, int windowHeight, int x, int y, float angle);

		~Ship();

        void MoveShip();
        void SetPosition(int x, int y);

		void KeyEvent(int key, bool isDown);
		void TickEvent();

	private:
        int m_width, m_height;
        int m_x, m_y;
        float m_angle;
        float m_radius;

        bool m_canShoot;
        bool m_dead;
        int m_explode_time;
        std::vector<Bullet> m_bullets;
        float m_rotation;
        bool m_thrusting;
        struct Thrust
        {
            int x;
            int y;
        } m_thrust;
	};
};

#endif // ASTEROIDS