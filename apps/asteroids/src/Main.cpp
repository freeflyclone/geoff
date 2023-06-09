#include "geoff.h"

#include "Session.h"
#include "WebsockSessionManager.h"

namespace Websock
{
    WebsockSessionManager <asteroids::Session> gsm;
};
using namespace Websock;


int main(int argc, char* argv[])
{
    if (argc != 5)
    {
        std::cerr <<
            "Usage: asteroids <address> <port> <doc_root> <threads>\n" <<
            "Example:\n" <<
            "    asteroids 0.0.0.0 8080 . 1\n";
        return EXIT_FAILURE;
    }

    // if something bad happens deep in the bowels of
    // boost and/or std library stuff that throws 
    // an exception, catch it and maybe get a clue
    // what the cause is.
    try
    {
        Geoff(argv[1], argv[2], argv[3], argv[4]);
    }
    catch (std::exception& e)
    {
        TRACE("Exception: " << e.what());
    }
}