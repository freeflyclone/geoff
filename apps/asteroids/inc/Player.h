#ifndef PLAYER_H
#define PLAYER_H

#include "Structs.h"
#include "Session.h"
#include "Ship.h"

#define PL_TRACE(...)

namespace as2
{
    class Player : public Size, public Position, public Context
    {
    public:
        Player(Session& session, double width, double height);
        ~Player();

        void KeyEvent(int key, bool isDown);
        void ClickEvent(int x, int y);
        void ResizeEvent(int w, int h);
        void TickEvent(Session&);

        Session& m_session;
        Ship2 m_ship;
    };
}

#endif
