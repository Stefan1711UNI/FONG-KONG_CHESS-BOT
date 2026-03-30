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
    if (target != nullptr && target->is_white == chessPiece->is_white) {
        return false; // Can't capture own piece
    }

    int dx = abs(x - chessPiece->x);
    int dy = abs(y - chessPiece->y);
    return (dx <= 1 && dy <= 1); // King moves one square in any direction
    
}

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
    if (!is_on_board(toX, toY)) return false; [cite: 112]
    if (!validate_piece_move(p, toX, toY, board)) return false; [cite: 113]

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
