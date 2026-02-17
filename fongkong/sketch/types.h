#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

namespace chessbot
{
    struct piece
    {
        uint8_t x;
        uint8_t y;
        char piece_type[2];
    };

    struct move
    {
        uint8_t from_x;
        uint8_t from_y;
        uint8_t to_x;
        uint8_t to_y;
    };
}

#endif