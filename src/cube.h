#pragma once
#include <cstdint>
#include <sys/types.h>

struct Cube {
    uint8_t cp[8];
    uint8_t co[8];
    uint8_t ep[12];
    uint8_t eo[12];
}; // 40 bytes

/*
 * This makes:
 *  amount = m / 3
 *  face = m % 3
 * It allows pruning of the search tree by avoiding consecutive moves on the same face.
 * Table of moves:
 *  index | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 |
 *  --------------------------------------------------------------------------------------
 *  move  | U | U'| U2| D | D'| D2| L | L'| L2| R | R' | R2 | F  | F' | F2 | B  | B' | B2 |
 *
 *  Ref: https://jperm.net/3x3/moves
*/
enum Move {
    U, U_PRIME, U2,
    D, D_PRIME, D2,
    L, L_PRIME, L2,
    R, R_PRIME, R2,
    F, F_PRIME, F2,
    B, B_PRIME, B2
};

Cube identity();
Cube applyMove(Cube c, Move m);
bool isSolved(const Cube& c);
void toFacelets(const Cube& c, uint8_t out[54]);   // renderer's view
const char* moveName(Move m);
bool parseMoves(const char* s, std::vector<Move>& out);
