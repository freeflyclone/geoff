#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>

namespace as2
{
    // screen space dimensions, offset into virtual space of player's browser window
    struct Context
    {
        void Resize(uint16_t width, uint16_t height);
        void Move(uint16_t x, uint16_t y);

        uint16_t ctxW;
        uint16_t ctxH;

        // offset of upper left corner in Universe coordinates.
        uint16_t ctxOX;
        uint16_t ctxOY;
    };

    struct Size
    {
        double sizeW;
        double sizeH;
    };

    struct Position
    {
        double posX;
        double posY;
    };

    struct Velocity
    {
        double deltaX;
        double deltaY;
    };

    struct Orientation
    {
        double angle;
    };
}

#endif
