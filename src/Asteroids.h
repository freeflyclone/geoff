#ifndef ASTEROIDS_H
#define ASTEROIDS_H

#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

#include "AppBuffer.h"
#include "GameSession.h"

class CustomSession;

namespace Asteroids
{
    struct Context
    {
        void Resize(uint16_t width, uint16_t height);
        
        uint16_t width;
        uint16_t height;

        GameSession& gs;
    };

    struct Position
    {
        double x, y;
    };

    struct Velocity
    {
        double dx, dy;
    };

    class Gun;
    class Bullet : public Position, public Velocity
    {
    public:
        Bullet(Gun& g, double x, double y, double dx, double dy);
        ~Bullet();

        bool TickTock();

        Gun& GetGun() { return m_gun; }

    private:
        Gun& m_gun;
        int ticksLeft;
    };

    class Ship;
    class Gun {
    public:
        Gun(Ship& s) : m_ship(s) {}
        ~Gun() {}

        void Fire(double x, double y, double dx, double dy);
        void TickTock();
        std::unique_ptr<AppBuffer> MakeBulletsPacket(bool isLittleEndian);
        Ship& GetShip() { return m_ship; }

    private:
        Ship& m_ship;
        std::list<std::shared_ptr<Bullet>> bullets;
    };

	class Ship : public Context, public Position, public Velocity
	{
	public:
		Ship(int windowWidth, int windowHeight, double x, double y, double angle);

		~Ship();

        void MoveShip();
        void FireShot();
        void GetXY(int16_t& x, int16_t& y);
        void GetAngle(int16_t& angle);
		void KeyEvent(int key, bool isDown);
		void TickEvent();

        std::shared_ptr<Gun> m_gun;

    private:
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
