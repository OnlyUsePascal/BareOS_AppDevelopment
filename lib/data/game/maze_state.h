#ifndef MAZE_STATE_H
#define MAZE_STATE_H

#include "../../game_be.h"

const int bitmap_mazeState[MAZE_SZ_CELL * MAZE_SZ_CELL] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
  0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0,
  0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
  0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0,
  0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

const int *bitmap_mazeStateAll[] = {
  bitmap_mazeState
};

#endif