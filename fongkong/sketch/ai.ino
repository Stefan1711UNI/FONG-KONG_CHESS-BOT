#include "types.h"
#include <array>
#include <cstring>

// --- Core Engine Constants ---
#define W while          // Shorthand for loops used throughout the search 
#define M 0x88           // The "0x88" board mask used to detect off-board moves 
#define S 128            // The full size of the internal board array (including padding) [cite: 3, 9]
#define I 8000           // The value represents "Infinity" (used for Checkmate/King value) [cite: 3, 17]
#define MYRAND_MAX 65535 // Limits the 16-bit pseudo-random generator for move variability


using namespace chessbot;
using namespace std;
/// @brief This function will return the ai move in the format of "e2e4" which means move piece from e2 to e4
/// @param board current chess board state
/// @param result output buffer (at least 5 chars) to store the move string
/// @param side 0 for white, 1 for black
//k
long  N, T;                  /* N=evaluated positions+S, T=recursion limit */
short Q, O, K, R, k=16;      /* k=moving side */
char *p, c[5], Z;            /* p=pointer to c, c=user input, computer output, Z=recursion counter */

char L,
w[]={0,2,2,7,-1,8,12,23},                             /* relative piece values    */
o[]={-16,-15,-17,0,1,16,0,1,16,15,17,0,14,18,31,33,0, /* step-vector lists */
     7,-1,11,6,8,3,6,                                 /* 1st dir. in o[] per piece*/
     6,3,5,7,4,5,3,6};                                /* initial piece setup      */
/* board is left part, center-pts table is right part, and dummy */  

char b[]={     
  22, 19, 21, 23, 20, 21, 19, 22, 28, 21, 16, 13, 12, 13, 16, 21,
  18, 18, 18, 18, 18, 18, 18, 18, 22, 15, 10,  7,  6,  7, 10, 15,
   0,  0,  0,  0,  0,  0,  0,  0, 18, 11,  6,  3,  2,  3,  6, 11,
   0,  0,  0,  0,  0,  0,  0,  0, 16,  9,  4,  1,  0,  1,  4,  9,
   0,  0,  0,  0,  0,  0,  0,  0, 16,  9,  4,  1,  0,  1,  4,  9,
   0,  0,  0,  0,  0,  0,  0,  0, 18, 11,  6,  3,  2,  3,  6, 11,
   9,  9,  9,  9,  9,  9,  9,  9, 22, 15, 10,  7,  6,  7, 10, 15,
  14, 11, 13, 15, 12, 13, 11, 14, 28, 21, 16, 13, 12, 13, 16, 21, 0
};

char bk[16*8+1];

unsigned int seed=0;
uint32_t  byteBoard[8];

char sym[17] = {".?pnkbrq?P?NKBRQ"};
int mn=1;
char lastH[5], lastM[5];

unsigned short myrand(void) {
  unsigned short r = (unsigned short)(seed % MYRAND_MAX);
  return r = ((r << 11) + (r << 7) + r) >> 1;
}
/* recursive minimax search */
/* (q,l)=window, e=current eval. score, */
/* E=e.p. sqr.z=prev.dest, n=depth; return score */
short D(short q, short l, short e, unsigned char E, unsigned char z, unsigned char n) {
  short m, v, i, P, V, s;
  unsigned char t, p, u, x, y, X, Y, H, B, j, d, h, F, G, C;
  signed char r;
  if (++Z > 30) {                                   /* stack underrun check */
    --Z; return e;
  }
  q--;                                          /* adj. window: delay bonus */
  k ^= 24;                                      /* change sides             */
  d = Y = 0;                                    /* start iter. from scratch */
  X = myrand() & ~M;                            /* start at random field    */
  W(d++ < n || d < 3 ||                         /* iterative deepening loop */
    z & K == I && (N < T & d < 98 ||            /* root: deepen upto time   */
                   (K = X, L = Y & ~M, d = 3)))                /* time's up: go do best    */
  { x = B = X;                                   /* start scan at prev. best */
    h = Y & S;                                   /* request try noncastl. 1st*/
    P = d < 3 ? I : D(-l, 1 - l, -e, S, 0, d - 3); /* Search null move         */
    m = -P < l | R > 35 ? d > 2 ? -I : e : -P;   /* Prune or stand-pat       */
    ++N;                                         /* node count (for timing)  */
    do {
      u = b[x];                                   /* scan board looking for   */
      if (u & k) {                                /*  own piece (inefficient!)*/
        r = p = u & 7;                             /* p = piece type (set r>0) */
        j = o[p + 16];                             /* first step vector f.piece*/
        W(r = p > 2 & r < 0 ? -r : -o[++j])        /* loop over directions o[] */
        { A:                                        /* resume normal after best */
          y = x; F = G = S;                         /* (x,y)=move, (F,G)=castl.R*/
          do {                                      /* y traverses ray, or:     */
            H = y = h ? Y ^ h : y + r;               /* sneak in prev. best move */
            if (y & M)break;                         /* board edge hit           */
            m = E - S & b[E] && y - E < 2 & E - y < 2 ? I : m; /* bad castling             */
            if (p < 3 & y == E)H ^= 16;              /* shift capt.sqr. H if e.p.*/
            t = b[H]; if (t & k | p < 3 & !(y - x & 7) - !t)break; /* capt. own, bad pawn mode */
            i = 37 * w[t & 7] + (t & 192);           /* value of capt. piece t   */
            m = i < 0 ? I : m;                       /* K capture                */
            if (m >= l & d > 1)goto C;               /* abort on fail high       */
            v = d - 1 ? e : i - p;                   /* MVV/LVA scoring          */
            if (d - !t > 1)                          /* remaining depth          */
            { v = p < 6 ? b[x + 8] - b[y + 8] : 0;    /* center positional pts.   */
              b[G] = b[H] = b[x] = 0; b[y] = u | 32;  /* do move, set non-virgin  */
              if (!(G & M))b[F] = k + 6, v += 50;     /* castling: put R & score  */
              v -= p - 4 | R > 29 ? 0 : 20;           /* penalize mid-game K move */
              if (p < 3)                              /* pawns:                   */
              { v -= 9 * ((x - 2 & M || b[x - 2] - u) + /* structure, undefended    */
                          (x + 2 & M || b[x + 2] - u) - 1  /*        squares plus bias */
                          + (b[x ^ 16] == k + 36))          /* kling to non-virgin King */
                     - (R >> 2);                       /* end-game Pawn-push bonus */
                V = y + r + 1 & S ? 647 - p : 2 * (u & y + 16 & 32); /* promotion or 6/7th bonus */
                b[y] += V; i += V;                     /* change piece, add score  */
              }
              v += e + i; V = m > q ? m : q;          /* new eval and alpha       */
              C = d - 1 - (d > 5 & p > 2 & !t & !h);
              C = R > 29 | d < 3 | P - I ? C : d;     /* extend 1 ply if in check */
              do
                s = C > 2 | v > V ? -D(-l, -V, -v,     /* recursive eval. of reply */
                                       F, 0, C) : v;    /* or fail low if futile    */
              W(s > q&++C < d); v = s;
              if (z && K - I && v + I && x == K & y == L) /* move pending & in root:  */
              { Q = -e - i; O = F;                     /*   exit if legal & found  */
                R += i >> 7; --Z; return l;            /* captured non-P material  */
              }
              b[G] = k + 6; b[F] = b[y] = 0; b[x] = u; b[H] = t; /* undo move,G can be dummy */
            }
            if (v > m)                               /* new best, update max,best*/
              m = v, X = x, Y = y | S & F;            /* mark double move with S  */
            if (h) {
              h = 0;  /* redo after doing old best*/
              goto A;
            }
            if (x + r - y | u & 32 |                 /* not 1st step,moved before*/
                p > 2 & (p - 4 | j - 7 ||             /* no P & no lateral K move,*/
                         b[G = x + 3 ^ r >> 1 & 7] - k - 6     /* no virgin R in corner G, */
                         || b[G ^ 1] | b[G ^ 2])               /* no 2 empty sq. next to R */
               )t += p < 5;                           /* fake capt. for nonsliding*/
            else F = y;                              /* enable e.p.              */
          } W(!t);                                  /* if not capt. continue ray*/
        }
      }
    } W((x = x + 9 & ~M) - B);                 /* next sqr. of board, wrap */
C: if (m > I - M | m < M - I)d = 98;           /* mate holds to any depth  */
    m = m + I | P == I ? m : 0;                  /* best loses K: (stale)mate*/
    if (z && d > 2)
    { *c = 'a' + (X & 7); c[1] = '8' - (X >> 4); c[2] = 'a' + (Y & 7); c[3] = '8' - (Y >> 4 & 7); c[4] = 0;
      char buff[150];
    }
  }                                             /*    encoded in X S,8 bits */
  k ^= 24;                                      /* change sides back        */
  --Z; return m += m < e;                       /* delayed-loss bonus       */
}
void gameOver() {
  for (;;);
}
void bkp() {
  for (int i = 0; i < 16 * 8 + 1; i++) {
    bk[i] = b[i];
  }
}


char translate_to_engine_bits(piece* p) {
    // If piece_type is empty or a dot (common for empty squares)
    
    char type = 0;
    // Engine mapping: P=1, N=2, K=3, B=4, R=5, Q=6 [cite: 6]
    switch(p->piece_type) {
        case pieceType::PAWN: type = 1; break;
        case pieceType::KNIGHT: type = 2; break;
        case pieceType::KING: type = 3; break;
        case pieceType::BISHOP: type = 4; break;
        case pieceType::ROOK: type = 5; break;
        case pieceType::QUEEN: type = 6; break;
        default:  type = 0; break;
    }

    if (type == 0) return 0;

    // Side bits: White (Uppercase) = 16, Black (Lowercase) = 8 [cite: 4, 101]
    // Note: Adjust the logic below if your 'side' param in get_ai_move handles color differently
    return type | (p->is_white ? 16 : 8);
}


static void get_ai_move(std::array<std::array<piece*, 8>, 8> board, char* result, int side) {
    // 1. Reset Engine Globals for a fresh search
    N = 0;             // Reset node count [cite: 16]
    Z = 0;             // Reset recursion depth [cite: 33]
    Q = 0;             // Reset evaluation score
    O = S;             // Reset en-passant/castling flags to default [cite: 31]
    T = 0x3F;          // Set search time/depth limit 
    k = (side == 0) ? 16 : 8; // Set moving side (0=White/16, 1=Black/8) [cite: 4]
    K = I;             // Set K to 'Infinity' to signal search mode [cite: 21]
    
    // 2. Sync pieces while PRESERVING positional tables (indices 8-15 in each row)
    for(int i=0; i<8; i++) {
        for(int j=0; j<8; j++) {
            // b[row * 16 + col] preserves the 0x88 board structure [cite: 7, 89]
            b[i*16 + j] = translate_to_engine_bits(board[i][j]); 
        }
    }

    // 3. Perform Search
    // D(alpha, beta, score, ep_square, prev_dest, depth) [cite: 18, 31]
    D(-I, I, Q, O, 1, 3); 

    // 4. Copy the encoded move string (e.g., "e2e4") to result [cite: 93, 94]
    if (c[0] != '\0') {
        strncpy(result, c, 5); 
    } else {
        strcpy(result, "none"); // Safety fallback
    }
}
     
void sync_board_to_engine(std::array<std::array<piece*, 8>, 8> board) {
    // 1. Clear the engine's internal board [cite: 99]
    for (int i = 0; i < 128; i++) b[i] = 0;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            piece* p = board[y][x]; //[cite: 100]
            if (p != nullptr) {
                // 2. Convert (x,y) to 0x88 index: (row << 4) + col [cite: 101]
                int engineIndex = (y << 4) + x; 
                
                // 3. Convert your piece struct into the engine's bit format [cite: 103]
                b[engineIndex] = translate_to_engine_bits(p);
            }
        }
    }
}
