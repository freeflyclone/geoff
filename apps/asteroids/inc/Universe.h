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

        Timer* GetTimer() { return m_timer.get(); }

        RockField* GetRockField() { return m_rockField.get(); }
        uint32_t GetTicks() { return m_ticks;}

        void TickEvent(uint32_t tickCount);

    private:
        void CollisionDetection();
        void PerSessionTickEvent(Session&);

        std::unique_ptr<RockField> m_rockField;
        std::unique_ptr<Timer> m_timer;
        uint32_t m_ticks;
    };

    extern std::unique_ptr<Universe> g_universe;
    Universe* GetUniverse();
}

#endif
