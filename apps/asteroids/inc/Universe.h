#ifndef UNIVERSE_H
#define UNIVERSE_H

#include "Structs.h"
#include "Session.h"

#define UN_TRACE(...)

namespace asteroids
{
    class RockField;

    class Universe : public Size
    {
    public:
        Universe(int width, int height);
        ~Universe();

        RockField& GetRockField();

        void TickEvent(Session&);

    private:
        void OtherSessionsTickEvent(Session&);
        void CollisionDetection(Session&);

        std::unique_ptr<RockField> m_rockField;
    };

    extern std::unique_ptr<Universe> g_universe;
    const Universe& Init(int w, int h);
}

#endif
