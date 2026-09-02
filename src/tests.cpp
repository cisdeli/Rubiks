#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "cube.h"
#include <doctest.h>

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
    initializeCube(c); c.c_orientations[0] = 1;             // single twist
    CHECK_FALSE(isValid(c));
    initializeCube(c); c.e_orientations[0] = 1;             // single flip
    CHECK_FALSE(isValid(c));
    initializeCube(c); c.c_positions[0]=1; c.c_positions[1]=0;  // single swap
    CHECK_FALSE(isValid(c));
}

