#pragma once
#include <cstdint>

#ifdef __CUDACC__
#define CUBE_HD __host__ __device__
#else
#define CUBE_HD
#endif

/*
 * c_positions[i] = j can mean:
 *   - slot i currently has cubie j
 *   - cubie i is currently at slot j
 * This implementation follow the first!
 *
 * Corners:  0=URF  1=UFL  2=ULB  3=UBR  4=DFR  5=DLF  6=DBL  7=DRB
 * Edges:    0=UR   1=UF   2=UL   3=UB   4=DR   5=DF
 *           6=DL   7=DB 8=FR   9=FL  10=BL  11=BR
 */
constexpr int NUM_CORNERS = 8;
constexpr int NUM_EDGES = 12;
struct Cube {
    uint8_t c_positions[NUM_CORNERS];    // corner positions
    uint8_t c_orientations[NUM_CORNERS]; // corner orientations
    uint8_t e_positions[NUM_EDGES];      // edge positions
    uint8_t e_orientations[NUM_EDGES];   // edge orientations
}; // 40 bytes

/*
 * This makes:
 *  face = m / 3 -> 0 = U, 1 = D, 2 = L, 3 = R, 4 = F, 5 = B
 *  amount = m % 3 -> 0 = quarter, 1 = prime, 2 = half
 * It allows pruning of the search tree by avoiding consecutive moves on the same face.
 * Table of moves:
 *  index | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 |
 *  --------------------------------------------------------------------------------------
 *  move  | U | U'| U2| D | D'| D2| L | L'| L2| R | R' | R2 | F  | F' | F2 | B  | B' | B2 |
 *
 *  Ref: https://jperm.net/3x3/moves
 */
/* clang-format off */
enum Move {
    U, U_PRIME, U2,
    D, D_PRIME, D2,
    L, L_PRIME, L2,
    R, R_PRIME, R2,
    F, F_PRIME, F2,
    B, B_PRIME, B2,
    MOVE_COUNT
};
/* clang-format on */

/*
 * Solved State:
 * Corner Positions -> [0, 1, 2, 3, 4, 5, 6, 7]
 * Corner Orientations -> [0, 0, 0, 0, 0, 0, 0, 0]
 * Edge Positions -> [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
 * Edge Orientations -> [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
 */
CUBE_HD inline void initializeCube(Cube &c) {
    for (int i = 0; i < NUM_CORNERS; i++) {
        c.c_positions[i] = i;
        c.c_orientations[i] = 0;
    }
    for (int i = 0; i < NUM_EDGES; i++) {
        c.e_positions[i] = i;
        c.e_orientations[i] = 0;
    }
}

CUBE_HD inline int permutationParity(const uint8_t *pos, int n) {
    uint8_t visited[NUM_EDGES] = {0};
    int swaps = 0;

    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            int cycle_size = 0;
            int x = i;
            while (!visited[x]) {
                visited[x] = 1;
                x = pos[x];
                cycle_size++;
            }

            if (cycle_size > 1)
                swaps += (cycle_size - 1);
        }
    }
    return swaps % 2;
}

CUBE_HD inline int isValid(const Cube &c) {
    int sum_c = 0;
    for (int i = 0; i < NUM_CORNERS; i++)
        sum_c += c.c_orientations[i];
    if (sum_c % 3 == 0) { // check corners if ok then
        int sum_e = 0;
        for (int i = 0; i < NUM_EDGES; i++)
            sum_e += c.e_orientations[i];
        if (sum_e % 2 == 0) { // check edges if ok then
            int c_parity = permutationParity(c.c_positions, 8);
            int e_parity = permutationParity(c.e_positions, 12);
            if (c_parity == e_parity) // check parity if ok then
                return 1;
            else
                return 0;
        } else
            return 0;
    } else
        return 0;
}

/* Cube identity(); */
/* Cube applyMove(Cube c, Move m); */
/* CUBE_HD inline Cube applyMove(Cube c, Move m) { ... } */
/* bool isSolved(const Cube& c); */
/* void toFacelets(const Cube& c, uint8_t out[54]);   // renderer's view */
/* const char* moveName(Move m); */
/* bool parseMoves(const char* s, std::vector<Move>& out); */
