// === FRONT-END ===
#include "../lib/game_fe.h"
#include "../lib/utils.h"
#include "../lib/def.h"
#include "../lib/framebf.h"
#include "../lib/data/game/maze.h"

#define STEP_AMOUNT 3

float cur_darken = 1.0f; 
const float darken_factor = 0.8f; 
float cur_lighten = 1.0f; 
const float lighten_factor = 1.25f;


void clearScreen(){
    framebf_drawRect(0, 0, GAME_W, GAME_H, MENU_BACKGND, 1);
}


void drawMenu(int posX, int posY, int spacing, char *opts[], int optSz){
    for (int i = 0; i < optSz; i++){
        font_drawString(posX, posY + i * spacing, opts[i], MENU_FOREGND, 2);
    }
}


int getMenuOpt(int markPosX, int markPosY, int yOffset, int optSz, const unsigned int foregnd, const unsigned int backgnd) {
    int actionIdx = 0;
    font_drawChar(markPosX, markPosY, '>', foregnd, 2);

    while (1) {
        char c = uart_getc();
        uart_sendc(c);
        uart_sendc('\n');
        if (c == 'w' || c == 'a') {
            font_drawChar(markPosX, markPosY + actionIdx * yOffset, '>', backgnd, 2);
            actionIdx = (actionIdx - 1) % optSz;
            if (actionIdx < 0) actionIdx += optSz;
            font_drawChar(markPosX, markPosY + actionIdx * yOffset, '>', foregnd, 2);
        } else if (c == 's' || c == 'd') {
            font_drawChar(markPosX, markPosY + actionIdx * yOffset, '>', backgnd, 2);
            actionIdx = (actionIdx + 1) % optSz;
            font_drawChar(markPosX, markPosY + actionIdx * yOffset, '>', foregnd, 2);
        } else if (c == '\n') {
            break;
        }
    }
    return actionIdx;
}


void drawAsset(const Asset *asset) {
    for (int i = asset->posX, posX = 0; posX < asset->width; i++, posX++) {
        for (int j = asset->posY, posY = 0; posY < asset->height; j++, posY++) {
            if (asset->bitmap[posX + posY * asset->width] != 0)
                framebf_drawPixel(i, j, asset->bitmap[posX + posY * asset->width]);
        }
    }
}


void removeAsset(const Asset *asset) {
    //replace with maze pixel
    // TODO: move to storage files
    Asset maze = {0, 0, MAZE_SZ_CELL * MAZE_SZ_CELL_PIXEL,
                  MAZE_SZ_CELL * MAZE_SZ_CELL_PIXEL, bitmap_maze};

    for (int i = asset->posX, posX = 0; posX < asset->width; i++, posX++) {
        for (int j = asset->posY, posY = 0; posY < asset->height; j++, posY++) {
            framebf_drawPixel(i, j, maze.bitmap[i + j * maze.width]);
        }
    }
}


void embedAsset(const Maze *maze, const Asset *asset, bool fill){
    for (int i = asset->posX, posX = 0; posX < asset->width; i++, posX++) {
        for (int j = asset->posY, posY = 0; posY < asset->height; j++, posY++) {
            if (asset->bitmap[posX + posY * asset->width] != 0)
                maze->bitmap[i + j * MAZE_SZ] = (fill) ? 
                                                asset->bitmap[posX + posY * asset->width] 
                                                : maze->pathColor;
        }
    }
}


void drawFOV(const Maze *maze, const Asset *asset) {
    for (int y = asset->posY - currentRadius; y <= asset->posY + currentRadius; ++y) {
        for (int x = asset->posX - currentRadius; x <= asset->posX + currentRadius; ++x) {
            int dx = x - asset->posX, dy = y - asset->posY;
            if (dx * dx + dy * dy <= currentRadius * currentRadius) {
                if (x >= 0 && y >= 0 && x < MAZE_SZ && y < MAZE_SZ) {
                    framebf_drawPixel(x, y, darkenPixel(maze->bitmap[y * MAZE_SZ + x], cur_darken));
                }
            }
        }
    }
}


void removeFOV(const Asset *asset) {
    for (int y = asset->posY - currentRadius; y <= asset->posY + currentRadius; ++y) {
        for (int x = asset->posX - currentRadius; x <= asset->posX + currentRadius; ++x) {
            int dx = x - asset->posX, dy = y - asset->posY;
            if (dx * dx + dy * dy <= currentRadius * currentRadius) {
                if (x >= 0 && y >= 0 && x < MAZE_SZ && y < MAZE_SZ) {
                    framebf_drawPixel(x, y, MENU_BACKGND);
                }
            }
        }
    }
}


void drawMovement(Maze *maze, Asset *asset, Direction dir, Item *collidedItem){
    // TODO: animation with frame
    int stepOffset = MAZE_SZ_CELL_PIXEL / STEP_AMOUNT;
    int posXFinal = asset->posX + xOffset[dir] * MAZE_SZ_CELL_PIXEL;
    int posYFinal = asset->posY + yOffset[dir] * MAZE_SZ_CELL_PIXEL;
    
    // walk the middle
    for (int i = 0 ; i < STEP_AMOUNT - 1; i++){
        // removeAsset(playerAsset);
        removeFOV(asset);
        updateAssetPos(asset, asset->posX + xOffset[dir] * stepOffset, 
                            asset->posY + yOffset[dir] * stepOffset);
        drawFOV(maze, asset);
        drawAsset(asset);
        wait_msec(62500);
    }
    
    // walk the last step
    removeFOV(asset);
    if (collidedItem != NULL) {
        //replace embeded item with background
        embedAsset(maze, collidedItem->asset, false); 
    }
    updateAssetPos(asset, posXFinal, posYFinal);
    drawFOV(maze, asset);
    drawAsset(asset);
}


void adjustBrightness(const Maze *maze, const Asset *asset, bool darken) {
    cur_darken = (darken) ? max_f(cur_darken * darken_factor, 0) 
                        : min_f(cur_darken / darken_factor, 1);
    drawFOV(maze, asset);
    drawAsset(asset);
}


void resetScreenDarkness() {
    for (int i = 0; i < MAZE_SZ; i++){
        for (int j = 0; j < MAZE_SZ; j++){
            framebf_drawPixel(
                i, j, darkenPixel(bitmap_maze[i + j * MAZE_SZ], cur_lighten));
        }
    }
}


uint64_t darkenPixel(uint64_t color, const float factor) {
    unsigned long r = (color & 0xFF0000) >> 16;
    unsigned long g = (color & 0x00FF00) >> 8;
    unsigned long b = (color & 0x0000FF);

    // Darken the RGB values       
    r = (unsigned long)(r * factor);
    g = (unsigned long)(g * factor);
    b = (unsigned long)(b * factor);

    // Ensure the values are within the valid range
    if (r < 0) r = 0;
    if (g < 0) g = 0;
    if (b < 0) b = 0;

    // Combine the RGB values back into a single color value
    return ((r << 16) | (g << 8) | b);
}


void updateAssetPos(Asset *asset, int x, int y) {
    asset->posX = x;
    asset->posY = y;
}


void posBeToFe(Position *pos, Asset *asset){
    asset->posX = pos->posX * MAZE_SZ_CELL_PIXEL + (MAZE_SZ_CELL_PIXEL - asset->width) / 2;
    asset->posY = pos->posY * MAZE_SZ_CELL_PIXEL + (MAZE_SZ_CELL_PIXEL - asset->height) / 2;
}


void debugAsset(Asset asset){
    uart_puts("["); uart_dec(asset.posX); uart_puts(","); uart_dec(asset.posY); uart_puts("]\n");
}


void getMazePathColor(Maze *maze){
    int x = 0 * MAZE_SZ_CELL_PIXEL + (MAZE_SZ_CELL_PIXEL - PLAYER_SZ) / 2;
    int y = 5 * MAZE_SZ_CELL_PIXEL + (MAZE_SZ_CELL_PIXEL - PLAYER_SZ) / 2;
    maze->pathColor = maze->bitmap[x + y * MAZE_SZ];
}