#ifndef ASTEROIDS_H
#define ASTEROIDS_H

#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

#include "AppBuffer.h"
#include "GameSession.h"

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

    class Bullet : public Position, public Velocity
    {
    public:
        Bullet(GameSession& g, double x, double y, double dx, double dy);
        ~Bullet();

        bool TickTock();

    private:
        GameSession& gs;
        int ticksLeft;
    };

    class Gun {
    public:
        Gun::Gun(GameSession& g) : gs(g) {}
        Gun::~Gun() {}

        void Fire(double x, double y, double dx, double dy);
        void TickTock();

    private:
        GameSession& gs;
        std::list<std::shared_ptr<Bullet>> bullets;
    };

	class Ship : public Context, public Position, public Velocity
	{
	public:
		Ship(int windowWidth, int windowHeight, double x, double y, double angle, GameSession &gs);

		~Ship();

        void MoveShip();
        void FireShot();
        void GetXY(int16_t& x, int16_t& y);
        void GetAngle(int16_t& angle);
		void KeyEvent(int key, bool isDown);
		void TickEvent();

	private:
        GameSession& m_gs;
        std::unique_ptr<Gun> m_gun;

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
