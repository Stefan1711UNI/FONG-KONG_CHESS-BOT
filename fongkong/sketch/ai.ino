#include "types.h"
#include<array>
#include <cstring>
using namespace std;
using namespace chessbot;

/// @brief This function will return the ai move in the format of "e2e4" which means move piece from e2 to e4
/// @param board current chess board state
/// @param result output buffer (at least 5 chars) to store the move string
/// @param side 0 for white, 1 for black
static void get_ai_move(array<array<piece, 8>, 8> board, char* result, int side) {
    strcpy(result, "");
};