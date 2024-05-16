// === BACK-END ===
#include "../lib/game_be.h"
#include "../lib/game_fe.h"
#include "../lib/framebf.h"
#include "../lib/font.h"
#include "../lib/def.h"
#include "../lib/util_str.h"
#include "../lib/data/game/maze.h"
#include "../lib/data/game/maze_state.h"
#include "../lib/data/game/player.h"
#include "lib/data/game/fov.h"
#include "lib/gpio.h"

// ===== BACK-END =====
const char directionKey[] = {'w', 'a', 's', 'd'};
const int yOffset[] = {-1, 0, 1, 0};
const int xOffset[] = {0, -1, 0, 1};

void game_enter() {
    // init
    framebf_init(GAME_W, GAME_H, GAME_W, GAME_H);

    // background + headline
    clearScreen();

    // menu
    int menuPosX = 150, menuPosY = 200, yOffset = 50;
    char *opts[] = {"Start", "Continue", "How To Play?", "Exit"};
    int optSz = sizeof(opts) / sizeof(opts[0]);

    while (1) {
        drawMenu(menuPosX, menuPosY, yOffset, opts, optSz);
        int optIdx = getMenuOpt(menuPosX - 50, menuPosY, yOffset, optSz);

        switch (optIdx) {
            case 0: //start
                game_start();
                break;

            case 1: //continue
                game_continue();
                break;

            case 2: //help
                game_help();
                break;

            case 3: //exit
                game_exit();
                return;
        }

    }
}

void render_scene(const Asset *fovAsset, const Asset *playerAsset, const bool isFOVShown) {
    if (!isFOVShown) {
        framebf_drawImg(0,0, MAZE_SZ, MAZE_SZ, bitmap_maze);
    } else {
        clearScreen();
        drawFOV( fovAsset);
    }

    drawAsset(playerAsset);
}

void game_start() {
    uart_puts("Starting Game...\n");
    Asset playerAsset = {ASSET_HIDDEN, ASSET_HIDDEN, PLAYER_SZ, PLAYER_SZ, bitmap_player};
    Asset fovAsset = {ASSET_HIDDEN, ASSET_HIDDEN, FOV_SZ, FOV_SZ, bitmap_fov};
    Position playerPos = {0, 5};

#ifdef DEBUG
    bool isFOVShown = false;
#endif

    updateAssetPos(&playerAsset, (MAZE_SZ_CELL_PIXEL - PLAYER_SZ) / 2,
                   MAZE_SZ_CELL_PIXEL * (MAZE_SZ_CELL / 2) + (MAZE_SZ_CELL_PIXEL - PLAYER_SZ) / 2);
    updateAssetPos(&fovAsset, (MAZE_SZ_CELL_PIXEL - PLAYER_SZ) / 2,
                   MAZE_SZ_CELL_PIXEL * (MAZE_SZ_CELL / 2) + (MAZE_SZ_CELL_PIXEL - PLAYER_SZ) / 2);
    clearScreen();

#ifdef DEBUG
    render_scene((const Asset *) &fovAsset, (const Asset *) &playerAsset, isFOVShown);
#else
    render_scene((const Asset *) &fovAsset, (const Asset *) &playerAsset, true);
#endif

    // movement
    while (1) {
        uart_puts("---\n");
        char c = uart_getc();
        debugPos(playerPos);

        if (c == 27) {
            //TODO: temporary escape to menu
            clearScreen();
            break;
        }
#ifdef DEBUG
        else if (c == 'k') {
            render_scene(
                    (const Asset *) &fovAsset,
                    (const Asset *) &playerAsset,
                    (isFOVShown = !isFOVShown)
            );
        }
#endif
        else {
            //TODO: movement
            Direction dir = -1;
            for (int i = 0; i < 4; i++) {
                if (directionKey[i] == c) {
                    dir = i;
                    break;
                }
            }

            if (dir == -1)
                continue;
            Position posTmp = {playerPos.posX, playerPos.posY};
            updatePos(&posTmp, dir);
            uart_puts("> ");
            debugPos(posTmp);

            if (posTmp.posX < 0 || posTmp.posY < 0)
                continue;
            //TODO: get map state base on its level
            int mazeState = bitmap_mazeState[MAZE_SZ_CELL * posTmp.posY + posTmp.posX];
            str_debug_num(mazeState);
            if (mazeState == 0) {
                str_debug("hit wall!");
                continue;
            }

            updatePos(&playerPos, dir);
#ifdef DEBUG
            if (isFOVShown) {
                drawFOVMovement(&fovAsset, dir);
            }
#else
            drawFOVMovement(&fovAsset, dir);
#endif
            drawMovement(&playerAsset, dir);
        }

    }

}


void game_continue() {
    uart_puts("Continueing Game...\n");

}


void game_help() {
    uart_puts("Game Instruction...\n");


}


void game_exit() {
    clearScreen();
    uart_puts("Exiting game...\n");
    font_drawString(150, 150, "Ta reng Ta reng Ta reng", MENU_FOREGND, 2);
}

// ===== MAIN AREA =====
void updatePos(Position *des, Direction dir) {
    des->posX += xOffset[dir];
    des->posY += yOffset[dir];
}


void debugPos(Position pos) {
    uart_puts("[");
    uart_dec(pos.posX);
    uart_puts(",");
    uart_dec(pos.posY);
    uart_puts("]\n");
}