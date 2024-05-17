// === FRONT-END ===
#include "../lib/game_fe.h"
#include "../lib/game_be.h"
#include "../lib/utils.h"
#include "../lib/def.h"
#include "../lib/framebf.h"
#include "../lib/data/game/maze.h"
#include "lib/data/game/fov.h"

#define STEP_AMOUNT 3

void clearScreen() {
    framebf_drawRect(0, 0, GAME_W, GAME_H, MENU_BACKGND, 1);
}


void drawMenu(int posX, int posY, int spacing, char *opts[], int optSz) {
    for (int i = 0; i < optSz; i++) {
        font_drawString(posX, posY + i * spacing, opts[i], MENU_FOREGND, 2);
    }
}


int getMenuOpt(int markPosX, int markPosY, int yOffset, int optSz) {
    int actionIdx = 0;
    font_drawChar(markPosX, markPosY, '>', MENU_FOREGND, 2);

    while (1) {
        char c = uart_getc();
        uart_sendc(c);
        uart_sendc('\n');
        if (c == 'w' || c == 'a') {
            font_drawChar(markPosX, markPosY + actionIdx * yOffset, '>', MENU_BACKGND, 2);
            actionIdx = (actionIdx - 1) % optSz;
            if (actionIdx < 0) actionIdx += optSz;
            font_drawChar(markPosX, markPosY + actionIdx * yOffset, '>', MENU_FOREGND, 2);
        } else if (c == 's' || c == 'd') {
            font_drawChar(markPosX, markPosY + actionIdx * yOffset, '>', MENU_BACKGND, 2);
            actionIdx = (actionIdx + 1) % optSz;
            font_drawChar(markPosX, markPosY + actionIdx * yOffset, '>', MENU_FOREGND, 2);
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

void removeFOV(const Position playerPos) {
    for (int y = playerPos.posY - currentRadius; y <= playerPos.posY + currentRadius; ++y) {
        for (int x = playerPos.posX - currentRadius; x <= playerPos.posX + currentRadius; ++x) {
            int dx = x - playerPos.posX, dy = y - playerPos.posY;
            if (dx * dx + dy * dy <= currentRadius * currentRadius) {
                if (x >= 0 && y >= 0 && x < MAZE_SZ && y < MAZE_SZ) {
                    framebf_drawPixel(x, y, MENU_BACKGND);
                }
            }
        }
    }
}

void drawMovement(Asset *asset, Direction dir) {
    // TODO: animation with frame
    int step = STEP_AMOUNT;
    int stepOffset = MAZE_SZ_CELL_PIXEL / step;
    int posXFinal = asset->posX + xOffset[dir] * MAZE_SZ_CELL_PIXEL;
    int posYFinal = asset->posY + yOffset[dir] * MAZE_SZ_CELL_PIXEL;

    // walk the middle
    for (int i = 0; i < step - 1; i++) {
        removeAsset(asset);
        updateAssetPos(asset, asset->posX + xOffset[dir] * stepOffset,
                       asset->posY + yOffset[dir] * stepOffset);

        drawAsset(asset);
        wait_msec(62500);
    }

    // walk the last step
    removeAsset(asset);
    updateAssetPos(asset, posXFinal, posYFinal);
    drawAsset(asset);
}

void drawFOVMovement(Position initialPlayerPosition, Direction dir) {
    // TODO: animation with frame
    int step = STEP_AMOUNT;
    int stepOffset = MAZE_SZ_CELL_PIXEL / step;
    int posXFinal = initialPlayerPosition.posX + xOffset[dir] * MAZE_SZ_CELL_PIXEL;
    int posYFinal = initialPlayerPosition.posY + yOffset[dir] * MAZE_SZ_CELL_PIXEL;

    // walk the middle
    for (int i = 0; i < step - 1; i++) {
        removeFOV(initialPlayerPosition);

        initialPlayerPosition.posX = initialPlayerPosition.posX + xOffset[dir] * stepOffset;
        initialPlayerPosition.posY = initialPlayerPosition.posY + yOffset[dir] * stepOffset;

        drawFOV(initialPlayerPosition);
        wait_msec(62500);
    }

    // walk the last step
    removeFOV(initialPlayerPosition);
    initialPlayerPosition.posX = posXFinal;
    initialPlayerPosition.posY = posYFinal;
    drawFOV(initialPlayerPosition);
}


void updateAssetPos(Asset *asset, int x, int y) {
    asset->posX = x;
    asset->posY = y;
}

void drawFOV(const Position playerPos) {
    for (int y = playerPos.posY - currentRadius; y <= playerPos.posY + currentRadius; ++y) {
        for (int x = playerPos.posX - currentRadius; x <= playerPos.posX + currentRadius; ++x) {
            int dx = x - playerPos.posX, dy = y - playerPos.posY;
            if (dx * dx + dy * dy <= currentRadius * currentRadius) {
                if (x >= 0 && y >= 0 && x < MAZE_SZ && y < MAZE_SZ) {
                    framebf_drawPixel(x, y, bitmap_maze[y * MAZE_SZ + x]);
                }
            }
        }
    }
}



