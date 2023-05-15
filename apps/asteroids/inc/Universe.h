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

        void PerSessionTickEvent(Session&);
    };
}

#endif
