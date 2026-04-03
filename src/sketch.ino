//#include "validation/validation.h"
#include <stdint.h>
#include "types.h"
#include <array>
#include <cstring>
#include <Arduino.h>
//#include <Arduino_RouterBridge.h>


using namespace chessbot;

#define LEFT_BUTTON_PIN 4
#define RIGHT_BUTTON_PIN 7
#define PAGE_BUTTON_PIN 11

static bool validate_piece_move(piece* chessPiece, int x, int y, std::array<std::array<piece*, 8>, 8>  board);
static chessbot::move translate_move_to_coordinates(const char* stringMove);
static void get_ai_move(std::array<std::array<piece*, 8>, 8> board, char* result, int side);
piece* get_piece_at_coordinates(int x, int y);
char* detect_player_move(bool is_first_turn, volatile bool* playerEndedTurn);
static bool try_move_piece(char* from, char* to, std::array<std::array<piece*, 8>, 8>  board);
void configure();
void initBoard(std::array<std::array<piece*, 8>, 8>& board);
void showTurn(bool playerTurn);
void pieceCaptured();
void boardSetup();
void sensorSetup();
//void lcd_confirmMove(const char* move);
void lcdSetup();
bool winning_move(piece* chessPiece, int x, int y, std::array<std::array<piece*, 8>, 8>  board);
void wait_for_pawns();
void lcd_aiMove();
void lcd_playerMove();
void lcd_wrongMove();
void lcd_moveRejected();
void lcd_confirmMove(const char* move, bool is_ai_move);
bool is_move_legal(piece* p, int toX, int toY, std::array<std::array<piece*, 8>, 8> board);
bool is_checkmate(bool isWhite, std::array<std::array<piece*, 8>, 8> board);
void pieceCaptured();
void lcd_check();
void lcd_checkMatePlayer();
void lcd_checkMateAI();
bool is_in_check(bool isWhite, std::array<std::array<piece*, 8>, 8> board);

   

std::array<std::array<piece*, 8>, 8>  board; 
bool player_white = true;
bool game_won = false;
bool volatile player_confirm = false;
int turn = 0;

bool left_button_pressed = false;
bool right_button_pressed = false;
bool page_button_pressed = false;

const unsigned long debounce_delay = 250;

volatile unsigned long last_left_press = 0;
void buttonLeftPressed() {
    unsigned long current_time = millis();
    if (current_time - last_left_press > debounce_delay) {
        left_button_pressed = true;
        last_left_press = current_time;
    }
}

volatile unsigned long last_right_press = 0;
void buttonRightPressed() {
    unsigned long current_time = millis();
    if (current_time - last_right_press > debounce_delay) {
        right_button_pressed = true;
        last_right_press = current_time;
    }
}

volatile unsigned long last_page_press = 0;

void buttonPagePressed() {
    unsigned long current_time = millis();
    if (current_time - last_page_press > debounce_delay) {
       player_confirm = true;
       last_page_press = current_time;
    }
}

void print_internal_board() {
    Serial.println("\n=== Internal Software Board ===");
    // Loop backwards (7 down to 0) so Black is at the top, White at the bottom
    for (int row = 7; row >= 0; row--) {
        Serial.print(row + 1); // Print the rank number (8 down to 1)
        Serial.print(" | ");
        
        for (int col = 0; col < 8; col++) {
            piece* p = board[row][col]; // Read from your global board array
            
            if (p == nullptr) {
                Serial.print(". "); // Empty square
            } else {
                char c = '?';
                switch(p->piece_type) {
                    case pieceType::PAWN:   c = 'p'; break;
                    case pieceType::KNIGHT: c = 'n'; break;
                    case pieceType::BISHOP: c = 'b'; break;
                    case pieceType::ROOK:   c = 'r'; break;
                    case pieceType::QUEEN:  c = 'q'; break;
                    case pieceType::KING:   c = 'k'; break;
                }
                // Make White pieces UPPERCASE by shifting ASCII value
                if (p->is_white) {
                    c = c - 32; 
                }
                Serial.print(c);
                Serial.print(" ");
            }
        }
        Serial.println();
    }
    Serial.println("    ---------------");
    Serial.println("    a b c d e f g h");
    Serial.println("===============================\n");
}


void setup() {

    pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);
    pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);
    pinMode(PAGE_BUTTON_PIN, INPUT_PULLUP);

    //Bridge.begin();
    Serial.begin(9600);
    Serial.println("Initializing...");
    configure();
    initBoard(board);
    //sensorSetup();
    lcdSetup();
    boardSetup();
    attachInterrupt(digitalPinToInterrupt(LEFT_BUTTON_PIN), buttonLeftPressed, FALLING);
    attachInterrupt(digitalPinToInterrupt(RIGHT_BUTTON_PIN), buttonRightPressed, FALLING);
    attachInterrupt(digitalPinToInterrupt(PAGE_BUTTON_PIN), buttonPagePressed, FALLING);
}


void loop() {
    // while game is running
    while (check_game_state())
    {

        // get player move
        //showTurn(true);
        Serial.println("New loop");

        char* player_move = nullptr;

        lcd_playerMove();

        if (turn == 0) {
            Serial.println("First loop");
            wait_for_pawns();
            player_confirm = false; 
            player_move = detect_player_move(true, &player_confirm);
        } else {
            player_confirm = false;
            player_move = detect_player_move(false, &player_confirm);
        }
        player_confirm = false;

        if (player_move == nullptr) {
            Serial.println("Error: Move was incomplete. Waiting for a valid move...");
            continue; // Go back to the top of the while loop and ask the player again!
        }

        Serial.println("Player move");
        Serial.println(player_move);
        // validate player move
        chessbot::move player_chessbot_move = translate_move_to_coordinates(player_move);
        piece* player_piece = get_piece_at_coordinates(player_chessbot_move.from_x, player_chessbot_move.from_y);
        Serial.println("Player piece:" + String(player_piece->piece_type));
        Serial.println("At coordinates:" + String(player_piece->x) + "," + String(player_piece->y));
        
        bool valid = is_move_legal(player_piece, player_chessbot_move.to_x, player_chessbot_move.to_y, board);
        Serial.println(valid);

        if (!valid) {
            lcd_moveRejected();
            Serial.println("Move rejected. Try again.");
            continue; // Go back to the top and let the player try again
        }
        
        lcd_confirmMove(player_move, false);
        
        if (get_piece_at_coordinates(player_chessbot_move.to_x, player_chessbot_move.to_y) != nullptr) {
            //pieceCaptured();
        }

        // update board state
        
            board[player_chessbot_move.to_y][player_chessbot_move.to_x] = player_piece;
            player_piece->x = player_chessbot_move.to_x;
            player_piece->y = player_chessbot_move.to_y;
            board[player_chessbot_move.from_y][player_chessbot_move.from_x] = nullptr;
            // if (winning_move(player_piece, player_chessbot_move.to_x, player_chessbot_move.to_y, board)) {
            //     game_won = true;
            //     break;
            // }
            if (is_checkmate(!player_white, board)) {
            lcd_checkMatePlayer();
            game_won = true;
            break;
            }
            // Check if AI is in check
            if (is_in_check(!player_white, board)) {
                lcd_check();
                delay(1000);
            }

        //here
        print_internal_board();

        // get ai move
        Serial.println("AI move");
        char ai_move[5];
        lcd_aiMove();
        get_ai_move(board, ai_move, player_white ? 1 : 0);
        //showTurn(false);
        Serial.println(ai_move);
        lcd_confirmMove(ai_move, true);
        chessbot::move ai_chessbot_move = translate_move_to_coordinates(ai_move);
        piece* ai_piece = get_piece_at_coordinates(ai_chessbot_move.from_x, ai_chessbot_move.from_y);

        
        bool ai_valid = is_move_legal(ai_piece, ai_chessbot_move.to_x, ai_chessbot_move.to_y, board);

        // update board state
        if (ai_valid) {
            char from[2];
            char to[2];
            strncpy(from, ai_move, 2);
            strncpy(to, ai_move + 2, 2);
            bool piece_moved = try_move_piece(from,to, board);

            player_confirm = false; // Reset it before the wait!
            Serial.println("Waiting for player to confirm AI move completion...");

            while (player_confirm == false) {
                delay(10); 
            }
            player_confirm = false;
            
            if (piece_moved) {
                board[ai_chessbot_move.to_y][ai_chessbot_move.to_x] = ai_piece;
                ai_piece->x = ai_chessbot_move.to_x; 
                ai_piece->y = ai_chessbot_move.to_y;
                board[ai_chessbot_move.from_y][ai_chessbot_move.from_x] = nullptr;
                // if (winning_move(ai_piece, ai_chessbot_move.to_x, ai_chessbot_move.to_y, board)) {
                //     game_won = true;
                //     break;
                // }
                // Check if player is in check
            if (is_checkmate(player_white, board)) {
            lcd_checkMateAI();
            game_won = true;
            break;
            }
            if (is_in_check(player_white, board)) {
            lcd_check();
            delay(1000);
            }
            }
        } else {
            Serial.println("AI FAILED");
        }
        
        // Serial.println("board after AI move");
        // print_internal_board();

        turn++;
    }
    
}


void handleButtons(int button, int current_page) {
    // 1. Cycle Page Button
//     if (digitalRead(buttonCyclePage) == LOW) {
//         currentPage = (currentPage + 1) % totalPages;
//         updateDisplay();
//         delay(300); // Debounce
//     }

//     // 2. Restart Game Button
// if (digitalRead(buttonRestart) == LOW) {
//     lcd.clear();
//     lcd.print("Resetting Board");
    
//     // Call the reset logic
//     reset_board(board);
    
//     // Reset AI state 
//     playerTurn = true;
//     currentPage = 0;
    
//     updateDisplay();
//     //delay(500); // Prevent accidental double-reset
// }

//     // 3. End Turn Button [cite: 22, 23]
//     if (digitalRead(buttonEndTurn) == LOW && playerTurn) {
//         playerTurn = false;
//         updateDisplay();
//         // Trigger AI Move Logic here
//         delay(300); 
//     }
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

void initBoard(std::array<std::array<piece*, 8>, 8>& board) {
    
    // 1. Memory Cleanup: Delete existing pieces if the board is being reset
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (board[row][col] != nullptr) {
                delete board[row][col];
            }
        }
    }

    // 2. Clear the board with nullptrs
    std::array<piece*, 8> empty_row = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    board.fill(empty_row);

    // 3. Setup White Major Pieces (Row 0)
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

    // 4. Setup White Pawns (Row 1)
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

    // 5. Setup Black Pawns (Row 6)
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

    // 6. Setup Black Major Pieces (Row 7)
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
    bool white_won = is_checkmate(true, board);
    bool black_won = is_checkmate(false, board);
    return !white_won || black_won;
}

piece* get_piece_at_coordinates(uint8_t x, uint8_t y) {
    return board[y][x];
}