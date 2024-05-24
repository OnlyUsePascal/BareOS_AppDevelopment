// === FRONT-END ===
#include "../lib/game_fe.h"
#include "../lib/utils.h"
#include "../lib/def.h"
#include "../lib/framebf.h"
#include "../lib/data/game/maze.h"
#include "../lib/data/data_font.h"
#include "../lib/data/game/player_movement.h"

#define STEP_AMOUNT 3
#define RECT_BORDER 10
#define DIALOG_HEIGHT 90
#define DIALOG_EXIT_MSG "Press Enter to continue"
#define DIALOG_EXIT_MSG_SIZE 23

float cur_darken = 1.0f; 
const float darken_factor = 0.8f; 
float cur_lighten = 1.0f; 
const float lighten_factor = 1.25f;
static uint16_t dialog_width = 0;

void clearScreen(){
    framebf_drawRect(0, 0, GAME_W, GAME_H, MENU_BACKGND, 1);
}


void drawMenu(int posX, int posY, int spacing, char *opts[], int optSz){
    for (int i = 0; i < optSz; i++){
        font_drawString(posX, posY + i * spacing, opts[i], MENU_FOREGND, 2, 1);
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
    for (int y = asset->posY - currentRadius+asset->height/2; 
            y <= asset->posY + currentRadius+asset->height/2; ++y) {
        for (int x = asset->posX - currentRadius+asset->height/2; 
                x <= asset->posX + currentRadius+asset->height/2; ++x) {
            int dx = x - asset->posX - asset->height/2, dy = y - asset->posY- asset->height/2;
            if (dx * dx + dy * dy <= currentRadius * currentRadius) {
                if (x >= 0 && y >= 0 && x < MAZE_SZ && y < MAZE_SZ) {
                    framebf_drawPixel(x, y, darkenPixel(maze->bitmap[y * MAZE_SZ + x], cur_darken));
                }
            }
        }
    }
}


void removeFOV(const Asset *asset) {
    for (int y = asset->posY - currentRadius+asset->height/2; 
            y <= asset->posY + currentRadius+asset->height/2; ++y) {
        for (int x = asset->posX - currentRadius+asset->height/2; 
                x <= asset->posX + currentRadius+asset->height/2; ++x) {
            int dx = x - asset->posX- asset->height/2, dy = y - asset->posY- asset->height/2;
            if (x >= 0 && y >= 0 && x < MAZE_SZ && y < MAZE_SZ) {
                framebf_drawPixel(x,y, MENU_BACKGND);
            }
        }
    }
}


void drawMovement(Maze *maze, Asset *playerAsset, Direction dir, Item *collidedItem){
    // TODO: animation with frame
    int stepOffset = MAZE_SZ_CELL_PIXEL / STEP_AMOUNT;
    int posXFinal = playerAsset->posX + xOffset[dir] * MAZE_SZ_CELL_PIXEL;
    int posYFinal = playerAsset->posY + yOffset[dir] * MAZE_SZ_CELL_PIXEL;
    
    // walk the middle
    for (int i = 0 ; i < STEP_AMOUNT - 1; i++){
        // removeAsset(playerAsset);
        removeFOV(playerAsset);
        updateAssetPos(playerAsset, playerAsset->posX + xOffset[dir] * stepOffset, 
                            playerAsset->posY + yOffset[dir] * stepOffset);
        drawFOV(maze, playerAsset);
        drawMoveAnimation(playerAsset, dir, i);
        wait_msec(250000);
    }
    
    // walk the last step
    removeFOV(playerAsset);
    if (collidedItem != NULL) {
        //replace embeded item with background
        embedAsset(maze, collidedItem->asset, false); 
    }
    updateAssetPos(playerAsset, posXFinal, posYFinal);
    drawFOV(maze, playerAsset);
    drawMoveAnimation(playerAsset, dir, STEP_AMOUNT - 1);
}

void drawMoveAnimation(Asset *playerAsset, Direction dir ,int order){
    switch (dir) {
    case RIGHT:
        if(order == 0){
            playerAsset->bitmap = bitmap_figure_go_right_1;
        } else if(order == 1){
            playerAsset->bitmap = bitmap_figure_go_right_3;
        }
        else{
            playerAsset->bitmap = bitmap_figure_go_right_2;
        }
        break;
    case LEFT:
        if(order == 0){
            playerAsset->bitmap = bitmap_figure_go_left_1;
        } else if(order == 1){
            playerAsset->bitmap = bitmap_figure_go_left_3;
        }
        else{
            playerAsset->bitmap = bitmap_figure_go_left_2;
        }
        break;
    case UP:
        if(order == 0){
            playerAsset->bitmap = bitmap_figure_go_up_1;
        } else if(order == 1){
            playerAsset->bitmap = bitmap_figure_go_up_3;
        }
        else{
            playerAsset->bitmap = bitmap_figure_go_up_2;
        }
        break;
    case DOWN:
        if(order == 0){
            playerAsset->bitmap = bitmap_figure_go_down_1;
        } else if(order == 1){
            playerAsset->bitmap = bitmap_figure_go_down_3;
        }
        else{
            playerAsset->bitmap = bitmap_figure_go_down_2;
        }
        break;
    default:
        break;
    }
    drawAsset(playerAsset);
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

void drawDialog(const char *title, const char *text) {
    uint16_t title_width = font_string_width(str_len(title), FONT_WIDTH);
    uint16_t text_width = font_string_width(str_len(text), FONT_WIDTH);
    uint16_t esc_msg_width = font_string_width(str_len(DIALOG_EXIT_MSG), FONT_WIDTH);

    dialog_width = max_l(text_width, esc_msg_width);

    // str_debug_num(text_width);

    framebf_drawRect(
        GAME_W / 2 - dialog_width / 2 - RECT_BORDER,
        GAME_H / 2 - DIALOG_HEIGHT / 2,
        GAME_W / 2 + dialog_width / 2 + RECT_BORDER,
        GAME_H / 2 + DIALOG_HEIGHT / 2,
        0xD58A3E,
        1
    );

    font_drawString(
        GAME_W / 2 - title_width / 2,
        GAME_H / 2 - DIALOG_HEIGHT / 2 + 5,
        title,
        0xFFFFFF,
        2,
        2
    );

    font_drawString(
        GAME_W / 2 - text_width / 2,
        GAME_H / 2 - DIALOG_HEIGHT / 6 + 5,
        text,
        0xE7E1DA,
        2,
        2
    );

    font_drawString(
        GAME_W / 2 - esc_msg_width / 2,
        GAME_H / 2 + DIALOG_HEIGHT / 6 + 5,
        DIALOG_EXIT_MSG,
        0xE7E1DA,
        2,
        2
    );
}

void removeDialog(const Position *pos) {//replace with maze pixel
    // TODO: move to storage files
    Asset maze = {0, 0, MAZE_SZ_CELL * MAZE_SZ_CELL_PIXEL,
                  MAZE_SZ_CELL * MAZE_SZ_CELL_PIXEL, bitmap_maze};

    uint16_t posX = pos->posX * MAZE_SZ_CELL_PIXEL + (MAZE_SZ_CELL_PIXEL - ITEM_SZ) / 2;
    uint16_t posY = pos->posY * MAZE_SZ_CELL_PIXEL + (MAZE_SZ_CELL_PIXEL - ITEM_SZ) / 2;

    for (int i = GAME_W / 2 - dialog_width / 2 - RECT_BORDER; i <= GAME_W / 2 + dialog_width / 2 + RECT_BORDER; i++) {
        for (int j = GAME_H / 2 - DIALOG_HEIGHT / 2; j <= GAME_H / 2 + DIALOG_HEIGHT / 2; j++) {
            int dx = i - posX, dy = j - posY;
            if (dx * dx + dy * dy <= currentRadius * currentRadius) {
                framebf_drawPixel(i, j, maze.bitmap[i + j * maze.width]);
            } else {
                framebf_drawPixel(i, j, MENU_BACKGND);
            }
        }
    }
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