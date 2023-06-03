#ifndef PLAYER_H
#define PLAYER_H

#include "Structs.h"
#include "Session.h"
#include "Ship.h"

#define PL_TRACE(...)

namespace asteroids
{
    class Player : public Context, public Size
    {
    public:
        Player(double width, double height);
        ~Player();

        Ship* GetShip() { return &m_ship; }

        void AddToScore(uint32_t increment);
        
        std::unique_ptr<AppBuffer> MakeBuffer(Session&);

        void KeyEvent(int key, bool isDown);
        void ClickEvent(int x, int y);
        void ResizeEvent(int w, int h);
        void TickEvent(Session&);

    private:
        Ship m_ship;

        int16_t m_deltaX;
        int16_t m_deltaY;
        uint32_t m_score;
        uint16_t m_phase;
    };
}

#endif
