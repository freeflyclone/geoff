#ifndef UNIVERSE_H
#define UNIVERSE_H

#include "Structs.h"
#include "Session.h"
#include "WebsockSessionManager.h"

#define UN_TRACE(...)

namespace as2
{
    class Universe : public Size
    {
    public:
        Universe(int width, int height);
        ~Universe();

        void TickEvent(Session&);

    private:
        //RockField m_rockField;

        void OtherSessionsTickEvent(Session&);
    };

    extern std::unique_ptr<Universe> g_universe;
    const Universe& Init(int w, int h);
}

#endif
