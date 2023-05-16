#ifndef ROCKFIELD_H
#define ROCKFIELD_H

#include "Structs.h"
#include "Session.h"
#include "Universe.h"
#include "Rock.h"

#define UN_TRACE(...)

namespace as2
{
    class Rock;

    class RockField : public Context
    {
    public:
        typedef std::unique_ptr<Rock> RockPtr_t;
        typedef std::list<RockPtr_t> RockList_t;
        typedef RockList_t::iterator RockIterator;

        RockField(int w, int h);
        ~RockField();

        void LaunchOne(double x, double y, double r);
        void DestroyRock(RockIterator rock);

        RockList_t& GetRocks();
        void EraseRocks();

        void TickEvent();

    private:
        RockList_t m_rocks;
    };
}

#endif