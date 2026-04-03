#include "types.h"
#include <array>
#include <stdlib.h>
using namespace chessbot;

piece* get_piece_at_coordinates(int x, int y, std::array<std::array<piece*, 8>, 8> board) {
    return board[y][x];
}

bool is_on_board(int x, int y) {
    return (x >= 0 && x <= 7 && y >= 0 && y <= 7);
}

static bool validate_piece_move(piece* chessPiece, int x, int y, std::array<std::array<piece*, 8>, 8>  board) {
    if (chessPiece == nullptr) {
        return false;
    }

    switch (chessPiece->piece_type) {
        case pieceType::PAWN:
            return validate_pawn_move(chessPiece, x, y, board);
        case pieceType::ROOK:
            return validate_rook_move(chessPiece, x, y, board);
        case pieceType::KNIGHT:
            return validate_knight_move(chessPiece, x, y, board);
        case pieceType::BISHOP:
            return validate_bishop_move(chessPiece, x, y, board);
        case pieceType::QUEEN:
            return validate_queen_move(chessPiece, x, y, board);
        case pieceType::KING:
            return validate_king_move(chessPiece, x, y, board);
        default:
            return false;
    }
}

static bool validate_pawn_move(piece* chessPiece, int x, int y, std::array<std::array<piece*, 8>, 8> board) {
    piece* target = get_piece_at_coordinates(x, y, board);
    if (target != nullptr && target->is_white == chessPiece->is_white) {
        return false; // Can't capture own piece
    }
    int direction = chessPiece->is_white ? 1 : -1; // White moves up, black moves down
    int startRow = chessPiece->is_white ? 1 : 6; // Starting
    if (x == chessPiece->x) {
        // Moving forward
        if (y == chessPiece->y + direction && target == nullptr) {
            return true; // Move one square forward
        }
        if (y == chessPiece->y + 2 * direction && chessPiece->y == startRow && target == nullptr) {
            return true; // Move two squares forward from starting position
        }
    } else if (abs(x - chessPiece->x) == 1 && y == chessPiece->y + direction && target != nullptr && target->is_white != chessPiece->is_white) {
        return true; // Capture diagonally
    }
    return false; // Invalid move
}
static bool validate_rook_move(piece* chessPiece, int x, int y, std::array<std::array<piece*, 8>, 8> board) {
    piece* target = get_piece_at_coordinates(x, y, board);
    if (target != nullptr && target->is_white == chessPiece->is_white) {
        return false; // Can't capture own piece
    }
    if (x != chessPiece->x && y != chessPiece->y) {
        return false; // Rook must move in a straight line
    }

    // Check for obstructions
    int xDirection = (x > chessPiece->x) ? 1 : (x < chessPiece->x) ? -1 : 0;
    int yDirection = (y > chessPiece->y) ? 1 : (y < chessPiece->y) ? -1 : 0;
    int currentX = chessPiece->x + xDirection;
    int currentY = chessPiece->y + yDirection;

    while (currentX != x || currentY != y) {
        if (get_piece_at_coordinates(currentX, currentY, board) != nullptr) {
            return false; // Path is obstructed
        }
        currentX += xDirection;
        currentY += yDirection;
    }
    return true; // Valid rook move
}
static bool validate_knight_move(piece* chessPiece, int x, int y, std::array<std::array<piece*, 8>, 8> board) {
    piece* target = get_piece_at_coordinates(x, y, board);
    if (target != nullptr && target->is_white == chessPiece->is_white) {
        return false; // Can't capture own piece
    }
    int dx = abs(x - chessPiece->x);
    int dy = abs(y - chessPiece->y);
    return (dx == 2 && dy == 1) || (dx == 1 && dy == 2); // Knight moves in an L shape
}
static bool validate_bishop_move(piece* chessPiece, int x, int y, std::array<std::array<piece*, 8>, 8> board) {
    piece* target = get_piece_at_coordinates(x, y, board);
    if (target != nullptr && target->is_white == chessPiece->is_white) {
        return false; // Can't capture own piece
    }

    int dx = abs(x - chessPiece->x);
    int dy = abs(y - chessPiece->y);

    if (dx != dy) {
        return false; // Bishop must move diagonally
    }

    // Check for obstructions
    int xDirection = (x > chessPiece->x) ? 1 : (x < chessPiece->x) ? -1 : 0;
    int yDirection = (y > chessPiece->y) ? 1 : (y < chessPiece->y) ? -1 : 0;
    int currentX = chessPiece->x + xDirection;
    int currentY = chessPiece->y + yDirection;

    while (currentX != x || currentY != y) {
        if (get_piece_at_coordinates(currentX, currentY, board) != nullptr) {
            return false; // Path is obstructed
        }
        currentX += xDirection;
        currentY += yDirection;
    }
    return true; // Valid bishop move
}
static bool validate_queen_move(piece* chessPiece, int x, int y, std::array<std::array<piece*, 8>, 8> board) {
    piece* target = get_piece_at_coordinates(x, y, board);
    if (target != nullptr && target->is_white == chessPiece->is_white) {
        return false; // Can't capture own piece
    }

    if (chessPiece->x == x || chessPiece->y == y) {
        return validate_rook_move(chessPiece, x, y, board); // Queen moves like a rook
    } else if (abs(x - chessPiece->x) == abs(y - chessPiece->y)) {
        return validate_bishop_move(chessPiece, x, y, board); // Queen moves like a bishop
    }
    return false; // Invalid queen move

}

static bool validate_king_move(piece* chessPiece, int x, int y, std::array<std::array<piece*, 8>, 8> board) { 
    piece* target = get_piece_at_coordinates(x, y, board);
    
    // Basic rule: Can't capture your own piece [cite: 156, 157]
    if (target != nullptr && target->is_white == chessPiece->is_white) {
        return false;
    }

    int dx = abs(x - chessPiece->x);
    int dy = abs(y - chessPiece->y);

    // Standard 1-square move [cite: 158, 159]
    if (dx <= 1 && dy <= 1) {
        return true;
    }

    // --- Castling Logic ---
    // King tries to move 2 squares horizontally on its starting row [cite: 64, 67]
    if (dy == 0 && dx == 2 && !chessPiece->has_moved) {
        int rookX = (x > chessPiece->x) ? 7 : 0; // King-side (7) or Queen-side (0) 
        piece* rook = board[chessPiece->y][rookX];

        // 1. Check if the Rook exists and hasn't moved 
        if (rook != nullptr && rook->piece_type == pieceType::ROOK && !rook->has_moved) {
            
            // 2. Check if the path between King and Rook is clear 
            int step = (x > chessPiece->x) ? 1 : -1;
            for (int checkX = chessPiece->x + step; checkX != rookX; checkX += step) {
                if (board[chessPiece->y][checkX] != nullptr) return false;
            }

            // 3. Check for Safety (The "No Check" rule) [cite: 35]
            // Note: is_square_attacked must be defined in your logic file
            bool enemyColor = !chessPiece->is_white;
            if (is_square_attacked(chessPiece->x, chessPiece->y, enemyColor, board)) return false; // Current sq
            if (is_square_attacked(chessPiece->x + step, chessPiece->y, enemyColor, board)) return false; // Passing sq
            // Target sq check is usually handled by the main move loop
            
            return true;
        }
    }
    
    return false;
}

// void execute_move(piece* p, int toX, int toY, std::array<std::array<piece*, 8>, 8>& board) {
//     // 1. Check for Castling (King moving 2 squares horizontally) 
//     if (p->piece_type == pieceType::KING && abs(toX - p->x) == 2) {
//         int rookFromX = (toX > p->x) ? 7 : 0; 
//         int rookToX = (toX > p->x) ? 5 : 3; 
        
//         piece* rook = board[p->y][rookFromX]; 
//         if (rook != nullptr) {
//             board[p->y][rookToX] = rook; 
//             board[p->y][rookFromX] = nullptr; 
//             rook->x = rookToX; 
//             rook->has_moved = true; 
//         }
//     }

//     // 2. Standard Move Execution 
//     board[p->y][p->x] = nullptr; 
//     board[toY][toX] = p; 
//     p->x = toX; 
//     p->y = toY; 
//     p->has_moved = true; 

//     // 3. Pawn Promotion Logic (New) 
//     if (p->piece_type == pieceType::PAWN) {
//         // White reaches top (row 7) or Black reaches bottom (row 0) [cite: 142]
//         if ((p->is_white && toY == 7) || (!p->is_white && toY == 0)) {
//             p->piece_type = pieceType::QUEEN; // Automatically promote to Queen [cite: 78, 108]
//         }
//     }
// }

bool is_square_attacked(int targetX, int targetY, bool whiteAttacker, std::array<std::array<piece*, 8>, 8> board) {
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            piece* p = board[y][x];
            // If there's a piece here and it belongs to the attacking side
            if (p != nullptr && p->is_white == whiteAttacker) {
                // Check if this piece can legally reach the target square
                if (validate_piece_move(p, targetX, targetY, board)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool is_move_legal(piece* p, int toX, int toY, std::array<std::array<piece*, 8>, 8> board) {
    //if (!is_on_board(toX, toY)) return false;
    
    if (!validate_piece_move(p, toX, toY, board)) return false; 

    // Create a "Pretend" board to test if the move is safe for the King
    auto hypotheticalBoard = board;
    hypotheticalBoard[p->y][p->x] = nullptr;
    hypotheticalBoard[toY][toX] = p;

    // Find your King's position
    int kX, kY;
    for(int y=0; y<8; y++) {
        for(int x=0; x<8; x++) {
            piece* temp = hypotheticalBoard[y][x];
            if(temp != nullptr && temp->piece_type == KING && temp->is_white == p->is_white) {
                kX = x; kY = y;
            }
        }
    }

    // If any enemy can attack your King's square on the new board, the move is illegal
    return !is_square_attacked(kX, kY, !p->is_white, hypotheticalBoard);
}

void reset_board(std::array<std::array<piece*, 8>, 8>& board) {
    // 1. Clear the board
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            board[y][x] = nullptr;
        }
    }

    // 2. Helper to place pieces
    auto place = [&](int x, int y, pieceType type, bool isWhite) {
        piece* p = new piece();
        p->x = x; p->y = y;
        p->piece_type = type;
        p->is_white = isWhite;
        p->has_moved = false; // Critical for Castling [cite: 177, 190]
        board[y][x] = p;
    };

    // 3. Place Pawns
    for (int i = 0; i < 8; i++) {
        place(i, 1, PAWN, true);  // White 
        place(i, 6, PAWN, false); // Black 
    }

    // 4. Place Rooks, Knights, Bishops, Queen, King
    pieceType backline[] = {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK};
    for (int i = 0; i < 8; i++) {
        place(i, 0, backline[i], true);  // White Backline
        place(i, 7, backline[i], false); // Black Backline
    }
}

/**
 * Determines if the king of a specific color is currently in check.
 */
bool is_in_check(bool isWhite, std::array<std::array<piece*, 8>, 8> board) {
    int kingX = -1, kingY = -1;

    // Find the king's position [cite: 69, 70]
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            piece* p = board[y][x];
            if (p != nullptr && p->piece_type == pieceType::KING && p->is_white == isWhite) {
                kingX = x; //[cite: 71]
                kingY = y; //[cite: 71]
                break;
            }
        }
    }

    // If king is not found (shouldn't happen in legal play), return false
    if (kingX == -1) return false;

    // Check if any enemy piece attacks the king's square [cite: 72]
    return is_square_attacked(kingX, kingY, !isWhite, board);
}

/**
 * Determines if the player of the specified color is in checkmate.
 */
bool is_checkmate(bool isWhite, std::array<std::array<piece*, 8>, 8> board) {
    // 1. If the king is not in check, it cannot be checkmate (might be stalemate)
    if (!is_in_check(isWhite, board)) {
        return false;
    }

    // 2. Iterate through every piece belonging to the player
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            piece* p = board[y][x];

            if (p != nullptr && p->is_white == isWhite) {
                // 3. Try moving this piece to every possible square on the board
                for (int targetY = 0; targetY < 8; targetY++) {
                    for (int targetX = 0; targetX < 8; targetX++) {
                        
                        // 4. Check if this specific move is legal [cite: 67]
                        // is_move_legal already simulates the move and checks for king safety 
                        if (is_move_legal(p, targetX, targetY, board)) {
                            // If at least one legal move exists, it's not checkmate
                            return false;
                        }
                    }
                }
            }
        }
    }

    // 5. No legal moves found while in check
    return true;
}

/**
 * Determines if the player of the specified color is in stalemate.
 * A stalemate occurs if the king is NOT in check, but there are no legal moves.
 */
bool is_stalemate(bool isWhite, std::array<std::array<piece*, 8>, 8> board) {
    // 1. If the king is currently in check, it's either checkmate or just "check"
    if (is_in_check(isWhite, board)) {
        return false;
    }

    // 2. Scan the board for all pieces belonging to the current player
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            piece* p = board[y][x];

            if (p != nullptr && p->is_white == isWhite) {
                // 3. Check every possible destination square on the 8x8 board
                for (int targetY = 0; targetY < 8; targetY++) {
                    for (int targetX = 0; targetX < 8; targetX++) {
                        
                        // 4. Use your existing logic to see if this move is valid and safe 
                        if (is_move_legal(p, targetX, targetY, board)) {
                            // If even one legal move exists, it's not a stalemate
                            return false;
                        }
                    }
                }
            }
        }
    }

    // 5. No legal moves were found, and the king is not in check
    return true; 
}