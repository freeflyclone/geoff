#ifndef ROCKFIELD_H
#define ROCKFIELD_H

#include "Structs.h"
#include "Session.h"
#include "Rock.h"
#include "Universe.h"

#define RF_TRACE(...)

namespace asteroids
{
    class RockField : public Context
    {
    public:
        typedef std::unique_ptr<Rock> RockPtr_t;
        typedef std::list<RockPtr_t> RockList_t;
        typedef RockList_t::iterator RockIterator;

        RockField(int w, int h);
        ~RockField();

        RockList_t& GetRocks();

        void LaunchOne(double x, double y, double r);
        void DestroyRock(RockIterator rock);

        void TickEvent(Session&);

    private:
        RockList_t m_rocks;
    };
}

#endif