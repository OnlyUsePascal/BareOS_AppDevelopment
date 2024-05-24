#ifndef GAME_BE_H
#define GAME_BE_H

#include "gpio.h"

#define MAZE_SZ_CELL 11 

typedef struct {
  int posX; // x in maze
  int posY; // y in maze
  int width; 
  int height; 
  unsigned long *bitmap;
} Asset; 


typedef struct {
  int posX;
  int posY;
} Position;


typedef enum {
  TRAP,
  BOMB,
  VISION,
  PORTAL,
} ItemId;


typedef struct {
  Asset *asset;
  Position *pos;
  ItemId id;
  int collided;
} Item;


typedef struct {
  int radius;
  int level; 
  int fadingSpeed; // ???
} Light;


typedef struct {
  Asset *asset;
  Position *pos;
} Player;


typedef enum {
  UP,
  LEFT,
  DOWN,
  RIGHT
} Direction;


typedef struct {
  int level;
  unsigned long pathColor;
  unsigned long *bitmap;
  Item **items;
  int itemsSz;
  Player *player;
} Maze;

extern const int xOffset[];
extern const int yOffset[];
extern const char directionKey[];
extern uint16_t currentRadius;

void game_enter();
int game_menu_enter();
void game_start(Maze *mz);
void game_continue();
void game_help();
void game_exit();
void clearScreen();

#ifdef DEBUG
void cli_toggle_fov();
#endif

Item* detect_collision(Position playerPos, Item *items[], int itemsSz);



#endif