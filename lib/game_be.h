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
  bool collided;
} ItemMeta;

typedef struct {
  // inherit ItemMeta
  ItemMeta *itemMeta;
  Position *des;
} Portal;

typedef struct {
  ItemMeta *itemMeta;
  Position *weakWall;
  bool used;
} Bomb;

typedef struct {
  Asset *asset;
  Position *pos;
  uint32_t step;
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
  int *bitmapState;
  ItemMeta *itemMetas[10];
  int itemMetasSz;
  Player *player;
  Portal *portal;
  Bomb *bomb;
} Maze;

extern const int xOffset[];
extern const int yOffset[];
extern const char directionKey[];
extern uint16_t currentRadius;

void game_enter();
int game_menu_enter();
void game_start(Maze *mz, int *_optIdx);
void game_continue();
void game_help();
void game_exit();
void clearScreen();
ItemMeta* detect_collision(Position playerPos, ItemMeta *itemMetas[], int itemMetasSz);
void debug_item(ItemMeta itemMeta);

#ifdef DEBUG
void cli_toggle_fov();
#endif

#endif