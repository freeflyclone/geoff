#ifndef UNIVERSE_H
#define UNIVERSE_H

#include "Structs.h"
#include "Player.h"
#include "Session.h"

#define UN_TRACE(...)

namespace asteroids
{
    class RockField;

    class Universe : public Size
    {
    public:
        typedef std::vector<Position> StarField_t;

        Universe(int width, int height);
        ~Universe();

        void NewPlayer(Session&, int w, int h);

        Timer* GetTimer() { return m_timer.get(); }
        std::shared_ptr<Player> GetPlayerById(uint32_t sessionID);

        RockField* GetRockField() { return m_rockField.get(); }
        StarField_t* GetStarField() { return &m_stars; }
        uint32_t GetTicks() { return m_ticks;}

        void TickEvent(uint32_t tickCount);

    private:
        void CollisionDetection();
        void PerSessionTickEvent(Session&);

        std::unique_ptr<RockField> m_rockField;
        std::map<uint32_t, std::shared_ptr<Player>> m_players;
        std::unique_ptr<Timer> m_timer;
        uint32_t m_ticks;
        StarField_t m_stars;
    };

    extern std::unique_ptr<Universe> g_universe;
    Universe* GetUniverse();
}

#endif
