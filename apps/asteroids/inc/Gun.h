#ifndef GUN_H
#define GUN_H

#include "Structs.h"
#include "Session.h"
#include "Ship.h"
#include "Bullet.h"

#define GN_TRACE(...)

namespace as2
{
    class Ship;
    class Gun {
    public:

        typedef std::unique_ptr<Bullet2> BulletPtr_t;
        typedef std::list<BulletPtr_t> BulletList_t;
        typedef BulletList_t::iterator BulletIterator;

        Gun(Ship2& s);
        ~Gun();

        void Fire();

        void TickEvent(Session&);
/*
        std::unique_ptr<AppBuffer> MakeBulletsPacket(bool isLittleEndian);
        Ship2& GetShip() { return m_ship; }
        BulletList_t m_bullets;
*/

    private:
        Ship2& m_ship;
    };

}
#endif
