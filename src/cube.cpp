#include "cube.h"
/*
             ┌────────┐
             │ 0  1  2│
             │ 3  4  5│   U
             │ 6  7  8│
    ┌────────┼────────┼────────┬────────┐
    │36 37 38│18 19 20│ 9 10 11│45 46 47│
    │39 40 41│21 22 23│12 13 14│48 49 50│
    │42 43 44│24 25 26│15 16 17│51 52 53│
    │   L    │   F    │   R    │   B    │
    └────────┼────────┼────────┴────────┘
             │27 28 29│
             │30 31 32│   D
             │33 34 35│
             └────────┘
*/

// For each corner slot its 3 facelets, in the cubie's own rotational order.
static const uint8_t CORNER_FACELET[NUM_CORNERS][3] = {
    {8, 9, 20},   // URF : U9 R1 F3
    {6, 18, 38},  // UFL : U7 F1 L3
    {0, 36, 47},  // ULB : U1 L1 B3
    {2, 45, 11},  // UBR : U3 B1 R3
    {29, 26, 15}, // DFR : D3 F9 R7
    {27, 44, 24}, // DLF : D1 L9 F7
    {33, 53, 42}, // DBL : D7 B9 L7
    {35, 17, 51}, // DRB : D9 R9 B7
};

static const uint8_t EDGE_FACELET[NUM_EDGES][2] = {
    {5, 10},  // UR : U6 R2
    {7, 19},  // UF : U8 F2
    {3, 37},  // UL : U4 L2
    {1, 46},  // UB : U2 B2
    {32, 16}, // DR : D6 R8
    {28, 25}, // DF : D2 F8
    {30, 43}, // DL : D4 L8
    {34, 52}, // DB : D8 B8
    {23, 12}, // FR : F6 R4
    {21, 41}, // FL : F4 L6
    {50, 39}, // BL : B6 L4
    {48, 14}, // BR : B4 R6
};

void toFacelets(const Cube &c, uint8_t out[54]) {
    for (int f = 0; f < 6; f++)
        out[f * 9 + 4] = f; // centres never move

    for (int i = 0; i < NUM_CORNERS; i++) {
        int j = c.c_positions[i], ori = c.c_orientations[i];
        for (int n = 0; n < 3; n++)
            out[CORNER_FACELET[i][(n + ori) % 3]] = CORNER_FACELET[j][n] / 9;
    }
    for (int i = 0; i < NUM_EDGES; i++) {
        int j = c.e_positions[i], ori = c.e_orientations[i];
        for (int n = 0; n < 2; n++)
            out[EDGE_FACELET[i][(n + ori) % 2]] = EDGE_FACELET[j][n] / 9;
    }
}
