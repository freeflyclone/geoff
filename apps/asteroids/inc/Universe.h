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

        RockField& GetRockField() { return *m_rockField; }

        void TickEvent(Session&);

        std::unique_ptr<RockField> m_rockField;

    private:
        void PerSessionTickEvent(Session&);
        void CollisionDetection(Session&);
    };

    extern std::unique_ptr<Universe> g_universe;
    const Universe& GetUniverse();
}

#endif
