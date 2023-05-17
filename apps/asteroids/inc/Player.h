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

        void KeyEvent(int key, bool isDown);
        void ClickEvent(int x, int y);
        void ResizeEvent(int w, int h);
        void TickEvent(Session&);

    private:
        Session& m_session;
        Ship m_ship;
    };
}

#endif
