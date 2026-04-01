#include <array>
#include "types.h"
#include "CoreXYController.h"
#include <Arduino.h>
// #include <string.h>

using namespace chessbot;

CoreXYController chessBoard(45.0, 40.0);
#define X_STEP_PIN 2
#define X_DIR_PIN 5
#define Y_STEP_PIN 3
#define Y_DIR_PIN 6

void boardSetup()
{
    chessBoard.setUp(X_STEP_PIN, X_DIR_PIN, Y_STEP_PIN, Y_DIR_PIN, 12, 9, 10, 8);
    chessBoard.calibrate();
}

static bool try_move_piece(char *from, char *to, std::array<std::array<piece *, 8>, 8> board)
{
    chessbot::move player_move = translate_move_to_coordinates(from);
    chessbot::move player_move_to = translate_move_to_coordinates(to);

    arduino::String strTo = arduino::String(to);
    arduino::String strFrom = arduino::String(from);

    piece *chessPiece = get_piece_at_coordinates(player_move.from_x, player_move.from_y);
    if (chessPiece == nullptr)
    {
        return false;
    }
    piece *targetPiece = get_piece_at_coordinates(player_move_to.to_x, player_move_to.to_y);
    if (targetPiece != nullptr && targetPiece->is_white != chessPiece->is_white)
    {

        chessBoard.capturePiece(strTo);
        chessBoard.movePiece(strFrom, strTo);
        return true;
    }

    if (chessPiece->piece_type == pieceType::KNIGHT)
    {
        return chessBoard.moveKnightPiece(strFrom, strTo);
    }
    else
    {
        return chessBoard.movePiece(strFrom, strTo);
    }
}