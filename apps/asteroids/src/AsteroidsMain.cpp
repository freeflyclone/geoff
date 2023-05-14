#include "geoff.h"

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

    Geoff(argv[1], argv[2], argv[3], argv[4]);
}