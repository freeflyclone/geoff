#ifndef GUN_H
#define GUN_H

#include "Structs.h"
#include "Session.h"
#include "Ship.h"
#include "Bullet.h"

#define GN_TRACE(...)

namespace as2
{
    class Ship2;
    class Gun {
    public:

        typedef std::unique_ptr<Bullet2> BulletPtr_t;
        typedef std::list<BulletPtr_t> BulletList_t;
        typedef BulletList_t::iterator BulletIterator;

        Gun();
        ~Gun();

        void Fire(Ship2&);

        void TickEvent(Session&);

        std::unique_ptr<AppBuffer> MakeBulletsBuffer(Session&);

    private:
        BulletList_t m_bullets;
    };

}
#endif
