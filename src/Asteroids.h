#ifndef ASTEROIDS_H
#define ASTEROIDS_H

#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

#include "AppBuffer.h"
#include "GameSession.h"
#include "WebsockSessionManager.h"

class AsteroidsSession;

namespace Asteroids
{
    // Take from asteroids.js
    const int FPS = 60;
    const float FRICTION = 0.7f;
    const float SHIP_BLINK_DUR = 0.2f;       // duration in seconds of a single blink during ship's invisibility
    const float SHIP_EXPLODE_DUR = 0.5f;     // duration of the ship's explosion in seconds
    const float SHIP_INV_DUR = 3.0f;         // duration of the ship's invisibility in seconds
    const int SHIP_SIZE = 20;                // ship height in pixels
    const int SHIP_THRUST = 10;              // acceleration of the ship in pixels per second per second
    const int SHIP_TURN_SPEED = 360;         // turn speed in degrees per second
    const float MUZZLE_VELOCITY = 500;		 // pixels per second

    class Gun;
    class Ship;
    class RockField;

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
        Bullet(Gun& g, double x, double y, double dx, double dy);
        ~Bullet();

        bool TickTock();

        Gun& GetGun() { return m_gun; }

    private:
        Gun& m_gun;
        int ticksLeft;
    };

    class Gun {
    public:
        Gun(Ship& s) : m_ship(s) {}
        ~Gun() {}

        void Fire(double x, double y, double dx, double dy);
        void TickTock();
        std::unique_ptr<AppBuffer> MakeBulletsPacket(bool isLittleEndian);
        Ship& GetShip() { return m_ship; }

        std::list<std::shared_ptr<Bullet>> m_bullets;

    private:
        Ship& m_ship;
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

    class Rock : public Position, public Velocity
    {
    public:
        Rock(RockField& field, double x, double y, double dx, double dy, double radius);
        ~Rock();

        bool TickTock();

        RockField& GetRockField() { return m_field; };

    private:
        RockField& m_field;

        double m_radius;
    };

    class RockField : public Context
    {
    public:
        RockField(int w, int h);
        ~RockField();

        void LaunchOne(double x, double y, double dx, double dy, double radius);

        void ResizeEvent(int w, int h);
        void TickEvent(AsteroidsSession&);

        std::list<std::shared_ptr<Rock>>& m_rocks;
    };

    class Player : public Context
    {
    public:
        Player(int width, int height);
        ~Player();

        void KeyEvent(int key, bool isDown);
        void ResizeEvent(int w, int h);
        void TickEvent(AsteroidsSession&);
    
        Ship m_ship;
    };

	// For multiplayer support, objects from ALL AsteroidsSessions
	// need to be conveyed to each client.
	// We'll use a separate packet to the client for the Universe update "tick"
    class Universe : public Context
    {
    public:
        Universe(int width, int height);
        ~Universe();

        void ClickEvent(uint16_t x, uint16_t y);
        void KeyEvent(int key, bool isDown);
        void ResizeEvent(int w, int h);
        void TickEvent(AsteroidsSession &);

        RockField m_rocks;

    private:
        WebsockSessionManager<AsteroidsSession>& m_sessions;
    };

};

#endif // ASTEROIDS
