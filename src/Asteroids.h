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
        
        uint16_t width;
        uint16_t height;
    };

    struct Position
    {
        double x, y;
    };

    struct Velocity
    {
        double dx, dy;
    };

    class Bullet : public Position, public Velocity
    {
    public:
        Bullet(double x, double y, double dx, double dy);
        ~Bullet();
    };

	class Ship : public Context, public Position, public Velocity
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
        std::vector<std::unique_ptr<Bullet>> m_bullets;

        double m_angle;
        double m_radius;

        bool m_canShoot;
        bool m_dead;
        int m_explode_time;
        double m_rotation;
        bool m_thrusting;
        bool m_show_position;
	};
};

#endif // ASTEROIDS