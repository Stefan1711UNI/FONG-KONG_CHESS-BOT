#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

namespace chessbot
{
    struct move
    {
        uint8_t from_x;
        uint8_t from_y;
        uint8_t to_x;
        uint8_t to_y;
    };

    enum pieceType {
        PAWN,
        ROOK,
        KNIGHT,
        BISHOP,
        QUEEN,
        KING
    };

    struct piece
    {
        uint8_t x;
        uint8_t y;
        pieceType piece_type;
        bool is_white;
        bool has_moved;
    };
}

#endif
