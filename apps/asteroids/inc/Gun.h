#ifndef GUN_H
#define GUN_H

#include "Structs.h"
#include "Session.h"
#include "Ship.h"
#include "Bullet.h"

#define GN_TRACE(...)

namespace asteroids
{
    class Ship;
    class Gun {
    public:

        typedef std::unique_ptr<Bullet> BulletPtr_t;
        typedef std::list<BulletPtr_t> BulletList_t;
        typedef BulletList_t::iterator BulletIterator;

        Gun();
        ~Gun();

        BulletList_t* GetBullets();

        void Fire(Ship&);

        void TickEvent(Session&);

        std::unique_ptr<AppBuffer> MakeBuffer(Session&);

    private:
        BulletList_t m_bullets;
    };

}
#endif
