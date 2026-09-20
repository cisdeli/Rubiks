#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "cube.h"
#include <cstring>
#include <doctest.h>

static const char *MOVE_NAMES[MOVE_COUNT] = {
    "U", "U'", "U2", "D", "D'", "D2", "L", "L'", "L2",
    "R", "R'", "R2", "F", "F'", "F2", "B", "B'", "B2"};

// How many times must a sequence of moves seq have to be repeated
// before the cube returns to solved.
// Returns -1 if it has not come back within the limit of tries.
static int orderOf(const Move *seq, int len, int limit = 2000) {
    Cube c;
    initializeCube(c);
    for (int n = 1; n <= limit; n++) {
        for (int i = 0; i < len; i++)
            c = applyMove(c, seq[i]);
        if (isSolved(c))
            return n;
    }
    return -1;
}

TEST_CASE("is cube solved") {
    Cube c;
    initializeCube(c);

    for (int i = 0; i < NUM_CORNERS; i++) {
        CHECK(c.c_positions[i] == i);
        CHECK(c.c_orientations[i] == 0);
    }

    for (int i = 0; i < NUM_EDGES; i++) {
        CHECK(c.e_positions[i] == i);
        CHECK(c.e_orientations[i] == 0);
    }
}

TEST_CASE("current state is valid") {
    Cube c;
    initializeCube(c);
    REQUIRE(isValid(c));
}

TEST_CASE("isValid rejects impossible states") {
    Cube c;
    initializeCube(c);
    c.c_orientations[0] = 1; // single twist
    CHECK_FALSE(isValid(c));
    initializeCube(c);
    c.e_orientations[0] = 1; // single flip
    CHECK_FALSE(isValid(c));
    initializeCube(c);
    c.c_positions[0] = 1;
    c.c_positions[1] = 0; // single swap
    CHECK_FALSE(isValid(c));
}

TEST_CASE("R U has order 105") {
    // (R U) cycles 3, 7 and 15 elements, so it returns after lcm(3,7,15) = 105.
    Move seq[2] = {R, U};
    CHECK(orderOf(seq, 2) == 105);
}

TEST_CASE("R U R' U' has order 6") {
    Move seq[4] = {R, U, R_PRIME, U_PRIME};
    CHECK(orderOf(seq, 4) == 6);
}

TEST_CASE("every move has the right order") {
    // A quarter turn repeats 4 times before coming back; a half turn twice.
    // amount = m % 3 -> 0 = quarter, 1 = prime (also a quarter), 2 = half.
    for (int m = 0; m < MOVE_COUNT; m++) {
        Move seq[1] = {(Move)m};
        int expected = (m % 3 == 2) ? 2 : 4;
        CAPTURE(MOVE_NAMES[m]);
        CHECK(orderOf(seq, 1) == expected);
    }
}

TEST_CASE("a move followed by its inverse is the identity") {
    for (int face = 0; face < 6; face++) {
        Move quarter = (Move)(face * 3);   // X
        Move prime = (Move)(face * 3 + 1); // X'
        Move half = (Move)(face * 3 + 2);  // X2

        CAPTURE(MOVE_NAMES[quarter]);

        Cube c;
        initializeCube(c);
        c = applyMove(c, quarter);
        c = applyMove(c, prime);
        CHECK(isSolved(c)); // X X' == identity

        initializeCube(c);
        c = applyMove(c, half);
        c = applyMove(c, half);
        CHECK(isSolved(c)); // X2 X2 == identity

        initializeCube(c);
        c = applyMove(c, quarter);
        c = applyMove(c, quarter);
        Cube d;
        initializeCube(d);
        d = applyMove(d, half);
        CHECK(memcmp(&c, &d, sizeof(Cube)) == 0); // X X == X2
    }
}

TEST_CASE("any sequence of moves leaves the cube in a valid state") {
    Cube c;
    initializeCube(c);
    uint32_t seed = 12345; // for debugging
    for (int i = 0; i < 100000; i++) {
        seed = seed * 1103515245u + 12345u;
        Move m = (Move)((seed >> 16) % MOVE_COUNT);
        c = applyMove(c, m);
        if (!isValid(c)) {
            CAPTURE(i);
            CAPTURE(MOVE_NAMES[m]);
            REQUIRE(false);
        }
    }
    CHECK(isValid(c));
}
