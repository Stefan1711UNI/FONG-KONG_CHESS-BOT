#include <array>
#include "types.h"
#include "CoreXYController.h"
#include <string.h>

using namespace chessbot;


static bool try_move_piece(char* from,char* to, std::array<std::array<piece*, 8>, 8>  board) {
    chessbot::move player_move = translate_move_to_coordinates(from);
    chessbot::move player_move_to = translate_move_to_coordinates(to);
    piece* chessPiece = get_piece_at_coordinates(player_move.from_x, player_move.from_y);
    if (chessPiece == nullptr) {
        return false;
    }
    piece* targetPiece = get_piece_at_coordinates(player_move_to.to_x, player_move_to.to_y);
    if (targetPiece != nullptr && targetPiece->is_white != chessPiece->is_white) {
        CoreXYController::capturePiece(to, targetPiece->y * 8 + targetPiece->x); 
        CoreXYController::movePiece(from, to);
        return true;
    }
 
    if (chessPiece->piece_type == pieceType::KNIGHT) {
        return CoreXYController::moveKnightPiece(from, to);
    } else {
        return CoreXYController::movePiece(from, to);
    }
} 