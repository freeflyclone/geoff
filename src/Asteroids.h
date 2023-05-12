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
    const double ROCK_RADIUS = 32.0;         // Rock radius in pixels.
    const int ROCK_SPEED = 50;               // Rock speed in pixels/second;

    class Gun;
    class Ship;
    class RockField;
    class Player;
    class Universe;

    struct Context
    {
        void Resize(uint16_t width, uint16_t height);
        void Move(uint16_t x, uint16_t y);
        
        uint16_t width;
        uint16_t height;

        uint16_t offsetX;
        uint16_t offsetY;
    };

    struct Size
    {
        double w, h;
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
        typedef std::list<std::shared_ptr<Bullet>> BulletsList_t;

        Gun(Ship& s) : m_ship(s) {}
        ~Gun() {}

        void Fire(double x, double y, double dx, double dy);
        void TickTock();
        std::unique_ptr<AppBuffer> MakeBulletsPacket(bool isLittleEndian);
        Ship& GetShip() { return m_ship; }

        BulletsList_t m_bullets;

    private:
        Ship& m_ship;
    };

	class Ship : public Context, public Position, public Velocity
	{
	public:
		Ship(Player& player, int windowWidth, int windowHeight, double x, double y, double angle);

		~Ship();

        void MoveShip();
        void FireShot();
        void GetXY(int16_t& x, int16_t& y);
        void GetAngle(int16_t& angle);
		void KeyEvent(int key, bool isDown);
		void TickEvent();

        Player& m_player;
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
        double Radius() { return m_radius;  }

    private:
        RockField& m_field;

        double m_radius;
    };

    class RockField : public Context
    {
    public:
        typedef std::list<std::shared_ptr<Rock>> RocksList_t;

        RockField(Universe& universe, int w, int h);
        ~RockField();

        void LaunchOne(double x, double y, double r);
        void DestroyRock(std::shared_ptr<Rock>);

        void TickEvent();

        Universe& m_universe;
        RocksList_t m_rocks;
    };

    class Player : public Size, public Position, public Context
    {
    public:
        Player(AsteroidsSession& session, int width, int height);
        ~Player();

        void KeyEvent(int key, bool isDown);
        void ClickEvent(int x, int y);
        void ResizeEvent(int w, int h);
        void TickEvent(AsteroidsSession&);
    
        AsteroidsSession& m_session;
        Ship m_ship;
    };

	// For multiplayer support, objects from ALL AsteroidsSessions
	// need to be conveyed to each client.
    // 
	// We'll use a separate packet to the client for the Universe update "tick"
    class Universe : public Size, public Context
    {
    public:
        Universe(int width, int height, uint32_t interval);
        ~Universe();

        void PerSessionTickEvent(AsteroidsSession &);

        // borrow from GameSession
        void TickEvent();
        void TimerTicker();
        uint32_t GetTimerTick() { return m_timer_tick; }

        RockField m_rockField;

    private:
        WebsockSessionManager<AsteroidsSession>& m_sessions;

        std::unique_ptr<net::deadline_timer> m_timer;
        uint32_t m_tick_interval_in_us;
        bool m_timer_complete;
        bool m_run_timer;
        uint32_t m_timer_tick;
    };

    extern std::unique_ptr<Universe> g_universe;
    const Universe& Init(int w, int h);
};

#endif // ASTEROIDS
