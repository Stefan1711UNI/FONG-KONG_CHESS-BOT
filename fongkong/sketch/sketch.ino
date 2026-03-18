//#include "validation/validation.h"
#include <stdint.h>
#include "types.h"
#include<array>
#include <cstring>
using namespace std;
using namespace chessbot;

static bool validate_piece_move(piece* chessPiece, int x, int y, std::array<std::array<piece*, 8>, 8>  board);
static chessbot::move translate_move_to_coordinates(const char* stringMove);
static void get_ai_move(array<array<piece*, 8>, 8> board, char* result, int side);
piece* get_piece_at_coordinates(int x, int y);
char* detect_player_move();
static bool try_move_piece(int old_x, int old_y, int new_x, int new_y);
void configure();
void initBoard(std::array<std::array<piece*, 8>, 8> board);
void showTurn(bool playerTurn);
void pieceCaptured();



   

std::array<std::array<piece*, 8>, 8>  board; 
bool player_white = true;


void setup() {
    configure();
    initBoard(board);

}


void loop() {
    // while game is running
    while (check_game_state())
    {
        // get player move
        showTurn(true);
        char* player_move = detect_player_move();
        
        // validate player move
        chessbot::move player_chessbot_move = translate_move_to_coordinates(player_move);
        piece* player_piece = get_piece_at_coordinates(player_chessbot_move.from_x, player_chessbot_move.from_y);
        bool valid = validate_piece_move(player_piece, player_chessbot_move.to_x, player_chessbot_move.to_y, board);
        if (get_piece_at_coordinates(player_chessbot_move.to_x, player_chessbot_move.to_y) != nullptr) {
            pieceCaptured();
        }

        // update board state
        if (valid) {
            board[player_chessbot_move.to_y][player_chessbot_move.to_x] = player_piece;
            board[player_chessbot_move.from_y][player_chessbot_move.from_x] = nullptr;
        }
        // get ai move
        char ai_move[5];
        get_ai_move(board, ai_move, player_white ? 1 : 0);
        showTurn(false);

        chessbot::move ai_chessbot_move = translate_move_to_coordinates(ai_move);
        piece* ai_piece = get_piece_at_coordinates(ai_chessbot_move.from_x, ai_chessbot_move.from_y);
        if (get_piece_at_coordinates(ai_chessbot_move.to_x, ai_chessbot_move.to_y) != nullptr) {
            pieceCaptured();
        }
        bool ai_valid = validate_piece_move(ai_piece, ai_chessbot_move.to_x, ai_chessbot_move.to_y, board);

        // update board state
        if (ai_valid) {
            bool piece_moved = try_move_piece(ai_chessbot_move.from_x, ai_chessbot_move.from_y, ai_chessbot_move.to_x, ai_chessbot_move.to_y);
            if (piece_moved) {
                board[ai_chessbot_move.to_y][ai_chessbot_move.to_x] = ai_piece;
                board[ai_chessbot_move.from_y][ai_chessbot_move.from_x] = nullptr;
            }
        }
    }
    
}

    
static chessbot::move translate_move_to_coordinates(const char* stringMove) {
    // translate move from format "e2e4" to coordinates (4, 1) to (4, 3)
    //  return move in the format of move struct
    // assuming stringMove is always in the correct format and valid and encoded in ASCII
    chessbot::move move;
    move.from_x = stringMove[0] - 'a';
    move.from_y = stringMove[1] - '1';
    move.to_x = stringMove[2] - 'a';
    move.to_y = stringMove[3] - '1';
    return move;
}

void initBoard(std::array<std::array<piece*, 8>, 8> board) {
    board.fill({
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    });

    board[0] = {
        new piece { 0, 0, pieceType::ROOK, true },
        new piece { 1, 0, pieceType::KNIGHT, true },
        new piece { 2, 0, pieceType::BISHOP, true },
        new piece { 3, 0, pieceType::QUEEN, true },
        new piece { 4, 0, pieceType::KING, true },
        new piece { 5, 0, pieceType::BISHOP, true },
        new piece { 6, 0, pieceType::KNIGHT, true },
        new piece { 7, 0, pieceType::ROOK, true }
    };

    board[1] = {
        new piece { 0, 1, pieceType::PAWN, true },
        new piece { 1, 1, pieceType::PAWN, true },
        new piece { 2, 1, pieceType::PAWN, true },
        new piece { 3, 1, pieceType::PAWN, true },
        new piece { 4, 1, pieceType::PAWN, true },
        new piece { 5, 1, pieceType::PAWN, true },
        new piece { 6, 1, pieceType::PAWN, true },
        new piece { 7, 1, pieceType::PAWN, true }
    };


    board[6] = {
        new piece { 0, 6, pieceType::PAWN, false },
        new piece { 1, 6, pieceType::PAWN, false },
        new piece { 2, 6, pieceType::PAWN, false },
        new piece { 3, 6, pieceType::PAWN, false },
        new piece { 4, 6, pieceType::PAWN, false },
        new piece { 5, 6, pieceType::PAWN, false },
        new piece { 6, 6, pieceType::PAWN, false },
        new piece { 7, 6, pieceType::PAWN, false }
    };

    board[7] = {
        new piece { 0, 7, pieceType::ROOK, false },
        new piece { 1, 7, pieceType::KNIGHT, false },
        new piece { 2, 7, pieceType::BISHOP, false },
        new piece { 3, 7, pieceType::QUEEN, false },
        new piece { 4, 7, pieceType::KING, false },
        new piece { 5, 7, pieceType::BISHOP, false },
        new piece { 6, 7, pieceType::KNIGHT, false },
        new piece { 7, 7, pieceType::ROOK, false }
    };
}

void configure() {

}

bool check_game_state() {
    return true;
}

piece* get_piece_at_coordinates(uint8_t x, uint8_t y) {
    return board[y][x];
}