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
        uint32_t GetTicks() { return m_ticks;}

        void TickEvent(Session&, uint32_t tickCount);

        std::unique_ptr<RockField> m_rockField;
        std::unique_ptr<Timer> m_timer;

    private:
        void PerSessionTickEvent(Session&);
        void CollisionDetection(Session&);

        uint32_t m_ticks;
    };

    extern std::unique_ptr<Universe> g_universe;
    const Universe& GetUniverse();
}

#endif
