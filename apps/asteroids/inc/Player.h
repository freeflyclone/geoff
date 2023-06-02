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
        Player(Session& session, double width, double height);
        ~Player();

        Ship* GetShip() { return &m_ship; }
        Session* GetSession() { return &m_session; }

        std::unique_ptr<AppBuffer> MakeBuffer(Session&);

        void KeyEvent(int key, bool isDown);
        void ClickEvent(int x, int y);
        void ResizeEvent(int w, int h);
        void TickEvent(Session&);

    private:
        Session& m_session;
        Ship m_ship;

        int16_t m_deltaX;
        int16_t m_deltaY;
        uint32_t m_score;

        bool m_left_down;
        bool m_right_down;
        bool m_up_down;
        bool m_down_down;
        bool m_shift_down;
        bool m_manual_viewport;
    };
}

#endif
