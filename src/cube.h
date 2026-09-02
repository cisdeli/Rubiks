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

struct MoveTable {
    uint8_t cp[NUM_CORNERS]; // new.c_positions[i]    = old.c_positions[cp[i]]
    uint8_t co[NUM_CORNERS]; // new.c_orientations[i] = (old.c_orientations[cp[i]] + co[i]) % 3
    uint8_t ep[NUM_EDGES];
    uint8_t eo[NUM_EDGES];
};
static const MoveTable MOVE_TABLES[MOVE_COUNT] = {
    {// U
     {3, 0, 1, 2, 4, 5, 6, 7},
     {0, 0, 0, 0, 0, 0, 0, 0},
     {3, 0, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {// U'
     {1, 2, 3, 0, 4, 5, 6, 7},
     {0, 0, 0, 0, 0, 0, 0, 0},
     {1, 2, 3, 0, 4, 5, 6, 7, 8, 9, 10, 11},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {// U2
     {2, 3, 0, 1, 4, 5, 6, 7},
     {0, 0, 0, 0, 0, 0, 0, 0},
     {2, 3, 0, 1, 4, 5, 6, 7, 8, 9, 10, 11},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {// D
     {0, 1, 2, 3, 5, 6, 7, 4},
     {0, 0, 0, 0, 0, 0, 0, 0},
     {0, 1, 2, 3, 5, 6, 7, 4, 8, 9, 10, 11},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {// D'
     {0, 1, 2, 3, 7, 4, 5, 6},
     {0, 0, 0, 0, 0, 0, 0, 0},
     {0, 1, 2, 3, 7, 4, 5, 6, 8, 9, 10, 11},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {// D2
     {0, 1, 2, 3, 6, 7, 4, 5},
     {0, 0, 0, 0, 0, 0, 0, 0},
     {0, 1, 2, 3, 6, 7, 4, 5, 8, 9, 10, 11},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {// L
     {0, 2, 6, 3, 4, 1, 5, 7},
     {0, 1, 2, 0, 0, 2, 1, 0},
     {0, 1, 10, 3, 4, 5, 9, 7, 8, 2, 6, 11},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {// L'
     {0, 5, 1, 3, 4, 6, 2, 7},
     {0, 1, 2, 0, 0, 2, 1, 0},
     {0, 1, 9, 3, 4, 5, 10, 7, 8, 6, 2, 11},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {// L2
     {0, 6, 5, 3, 4, 2, 1, 7},
     {0, 0, 0, 0, 0, 0, 0, 0},
     {0, 1, 6, 3, 4, 5, 2, 7, 8, 10, 9, 11},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {// R
     {4, 1, 2, 0, 7, 5, 6, 3},
     {2, 0, 0, 1, 1, 0, 0, 2},
     {8, 1, 2, 3, 11, 5, 6, 7, 4, 9, 10, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {// R'
     {3, 1, 2, 7, 0, 5, 6, 4},
     {2, 0, 0, 1, 1, 0, 0, 2},
     {11, 1, 2, 3, 8, 5, 6, 7, 0, 9, 10, 4},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {// R2
     {7, 1, 2, 4, 3, 5, 6, 0},
     {0, 0, 0, 0, 0, 0, 0, 0},
     {4, 1, 2, 3, 0, 5, 6, 7, 11, 9, 10, 8},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {// F
     {1, 5, 2, 3, 0, 4, 6, 7},
     {1, 2, 0, 0, 2, 1, 0, 0},
     {0, 9, 2, 3, 4, 8, 6, 7, 1, 5, 10, 11},
     {0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0}},
    {// F'
     {4, 0, 2, 3, 5, 1, 6, 7},
     {1, 2, 0, 0, 2, 1, 0, 0},
     {0, 8, 2, 3, 4, 9, 6, 7, 5, 1, 10, 11},
     {0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0}},
    {// F2
     {5, 4, 2, 3, 1, 0, 6, 7},
     {0, 0, 0, 0, 0, 0, 0, 0},
     {0, 5, 2, 3, 4, 1, 6, 7, 9, 8, 10, 11},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {// B
     {0, 1, 3, 7, 4, 5, 2, 6},
     {0, 0, 1, 2, 0, 0, 2, 1},
     {0, 1, 2, 11, 4, 5, 6, 10, 8, 9, 3, 7},
     {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1}},
    {// B'
     {0, 1, 6, 2, 4, 5, 7, 3},
     {0, 0, 1, 2, 0, 0, 2, 1},
     {0, 1, 2, 10, 4, 5, 6, 11, 8, 9, 7, 3},
     {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1}},
    {// B2
     {0, 1, 7, 6, 4, 5, 3, 2},
     {0, 0, 0, 0, 0, 0, 0, 0},
     {0, 1, 2, 7, 4, 5, 6, 3, 8, 9, 11, 10},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
};
CUBE_HD inline Cube applyMove(const Cube &c, Move m) {
    const MoveTable &t = MOVE_TABLES[m];
    Cube n;
    for (int i = 0; i < NUM_CORNERS; i++) {
        n.c_positions[i] = c.c_positions[t.cp[i]];
        n.c_orientations[i] = (c.c_orientations[t.cp[i]] + t.co[i]) % 3;
    }
    for (int i = 0; i < NUM_EDGES; i++) {
        n.e_positions[i] = c.e_positions[t.ep[i]];
        n.e_orientations[i] = (c.e_orientations[t.ep[i]] + t.eo[i]) % 2;
    }
    return n;
}
