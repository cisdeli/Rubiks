#include "cube.h"
#include <vector>

#ifdef __CUDACC__
  #define CUBE_HD __host__ __device__
#else
  #define CUBE_HD
#endif

/* CUBE_HD inline Cube applyMove(Cube c, Move m) { ... } */

