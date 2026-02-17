//#include "validation/validation.h"
#include <stdint.h>
#include "types.h"
#include<array>
#include <cstring>
using namespace std;
using namespace chessbot;

static void validate_piece_move(piece piece, int x, int y);
static chessbot::move translate_move_to_coordinates(const char* stringMove);
static void get_ai_move(array<array<piece, 8>, 8> board, char* result);
void detect_player_move(char* result);


   

std::array<std::array<piece, 8>, 8>  board; 


void setup() {

}


void loop() {
    // while game is running
    while (check_game_state())
    {
        // get player move
        char player_move[5];
        detect_player_move(player_move);
        // validate player move

        // update board state
        // get ai move
        // validate ai move
        // update board state
    }
    

     
}


static chessbot::move translate_move_to_coordinates(const char* stringMove) {
    // translate move from format "e2e4" to coordinates (4, 1) to (4, 3)
    //  return move in the format of move struct
    chessbot::move move;
    return move;
}

static void cofigure() {

}

bool check_game_state() {
    return true;
}

