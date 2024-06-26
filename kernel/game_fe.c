// === FRONT-END ===
#include "../lib/game_fe.h"
#include "../lib/utils.h"
#include "../lib/def.h"
#include "../lib/framebf.h"
#include "../lib/data/game/maze.h"
#include "../lib/data/data_font.h"
#include "../lib/data/game/player_movement.h"
#include "../lib/font.h"

#define STEP_AMOUNT 3
#define RECT_BORDER 10
#define DIALOG_HEIGHT 90
#define DIALOG_EXIT_MSG "Press Enter to continue..."
#define DIALOG_EXIT_MSG_SIZE 26

float curDarken = 1.0f; 
const float darkenFactor = 0.8f; 
static uint16_t dialog_width = 0;


void clearScreen(){
    framebf_drawRect(0, 0, GAME_W, GAME_H, MENU_BACKGND, 1);
}


void drawMenu(int posX, int posY, int spacing, char *opts[], 
                int optSz, uint32_t foreGnd, uint32_t backGnd, bool fill){
    for (int i = 0; i < optSz; i++){
        font_drawString(posX, posY + i * spacing, 
                        opts[i], (fill) ? foreGnd : backGnd, 2, 1);
    }
}


int getMenuOpt(int markPosX, int markPosY, int yOffset, int optSz, 
                const unsigned int foregnd, const unsigned int backgnd) {
    int actionIdx = 0;
    font_drawChar(markPosX, markPosY, '>', foregnd, 2, 1);

    while (1) {
        char c = uart_getc();
        uart_sendc(c); uart_sendc('\n');
        if (c == 'w' || c == 'a') {
            font_drawChar(markPosX, markPosY + actionIdx * yOffset, '>', backgnd, 2, 1);
            actionIdx = (actionIdx - 1) % optSz;
            if (actionIdx < 0) actionIdx += optSz;
            font_drawChar(markPosX, markPosY + actionIdx * yOffset, '>', foregnd, 2, 1);
        } else if (c == 's' || c == 'd') {
            font_drawChar(markPosX, markPosY + actionIdx * yOffset, '>', backgnd, 2, 1);
            actionIdx = (actionIdx + 1) % optSz;
            font_drawChar(markPosX, markPosY + actionIdx * yOffset, '>', foregnd, 2, 1);
        } else if (c == '\n') {
            break;
        }
    }
    
    // remove selector
    font_drawChar(markPosX, markPosY + actionIdx * yOffset, '>', backgnd, 2, 1);
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


void drawAssetPartial(const ItemMeta *item, const Asset *pl) {
    Asset *asset = item->asset;
    int fovLowerX = pl->posX - currentRadius + pl->height/2,
        fovUpperX = pl->posX + currentRadius + pl->height/2,
        fovLowerY = pl->posY - currentRadius + pl->height/2,
        fovUpperY = pl->posY + currentRadius + pl->height/2,
        itemLowerX = asset->posX,
        itemUpperX = asset->posX + asset->width,
        itemLowerY = asset->posY,
        itemUpperY = asset->posY + asset->height;
    
    // skip collided or out-range item
    if (item->collided || !(max_l(fovLowerX, itemLowerX) <= min_l(fovUpperX, itemUpperX) 
                            && max_l(fovLowerY, itemLowerY) <= min_l(fovUpperY, itemUpperY))) 
                            return;
    
    for (int i = asset->posX, posX = 0; posX < asset->width; i++, posX++) {
        for (int j = asset->posY, posY = 0; posY < asset->height; j++, posY++) {
            int dx = i - pl->posX - pl->height/2, 
                dy = j - pl->posY - pl->height/2;
            if (!(dx * dx + dy * dy <= currentRadius * currentRadius)) continue;
            if (asset->bitmap[posX + posY * asset->width] != 0)
                framebf_drawPixel(i, j, asset->bitmap[posX + posY * asset->width]);
        }
    }   
}


void drawFOV(const Maze *maze, const Asset *asset) {
    for (int y = asset->posY - currentRadius+asset->height/2; 
            y <= asset->posY + currentRadius+asset->height/2; ++y) {
        for (int x = asset->posX - currentRadius+asset->height/2; 
                x <= asset->posX + currentRadius+asset->height/2; ++x) {
            int dx = x - asset->posX - asset->height/2, dy = y - asset->posY- asset->height/2;
            if (dx * dx + dy * dy <= currentRadius * currentRadius 
                    && x >= 0 && y >= 0 && x < MAZE_SZ && y < MAZE_SZ) {
                framebf_drawPixel(x, y, darkenPixel(maze->bitmap[y * MAZE_SZ + x], curDarken));
            }
        }
    }
}


void drawFOVWeakWall(const Maze *mz, const Asset *asset, const Asset *weakWall){
    // high light weak wall
    int wallLowerX = weakWall->posX * MAZE_SZ_CELL_PIXEL, 
        wallUpperX = (weakWall->posX + 1) * MAZE_SZ_CELL_PIXEL,
        wallLowerY = weakWall->posY * MAZE_SZ_CELL_PIXEL,
        wallUpperY = (weakWall->posY + 1) * MAZE_SZ_CELL_PIXEL,
        fovLowerX = asset->posX - currentRadius + asset->height/2,
        fovUpperX = asset->posX + currentRadius + asset->height/2,
        fovLowerY = asset->posY - currentRadius + asset->height/2,
        fovUpperY = asset->posY + currentRadius + asset->height/2;
    
    for (int x = fovLowerX; x < fovUpperX; x++) {
        for (int y = fovLowerY; y < fovUpperY; y++) {
            if (!(x >= 0 && y >= 0 && x < MAZE_SZ && y < MAZE_SZ)) continue;
            
            int dx = x - asset->posX - asset->height/2, 
                dy = y - asset->posY - asset->height/2;
            if (!(dx * dx + dy * dy <= currentRadius * currentRadius)) continue;
            float darkenLevel = (x >= wallLowerX && x < wallUpperX 
                                && y >= wallLowerY && y < wallUpperY) 
                                ? (curDarken / darkenFactor) : curDarken;
            framebf_drawPixel(x, y, darkenPixel(mz->bitmap[y * MAZE_SZ + x], darkenLevel));
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


void drawMovement(Maze *mz, Asset *plAsset, Direction dir, ItemMeta *collidedItem){
    int stepOffset = MAZE_SZ_CELL_PIXEL / STEP_AMOUNT;
    int posXFinal = plAsset->posX + xOffset[dir] * MAZE_SZ_CELL_PIXEL;
    int posYFinal = plAsset->posY + yOffset[dir] * MAZE_SZ_CELL_PIXEL;
    
    // walk the middle
    for (int i = 0 ; i < STEP_AMOUNT - 1; i++){
        // removeAsset(playerAsset);
        removeFOV(plAsset);
        updateAssetPos(plAsset, plAsset->posX + xOffset[dir] * stepOffset, 
                            plAsset->posY + yOffset[dir] * stepOffset);
        drawFOV(mz, plAsset);
        for (int i = 0 ; i < mz->itemMetasSz; i++){
            ItemMeta *item = mz->itemMetas[i];
            drawAssetPartial(item, plAsset);
        }
        drawMovementFrame(plAsset, dir, i);
        wait_msec(125000);
    }
    
    // walk the last step
    removeFOV(plAsset);
    updateAssetPos(plAsset, posXFinal, posYFinal);
    drawFOV(mz, plAsset);
    // still draw item asset, but if collidediTem = curItem then dont draw
    for (int i = 0 ; i < mz->itemMetasSz; i++){
        ItemMeta *item = mz->itemMetas[i];
        if (item != collidedItem) drawAssetPartial(item, plAsset);
    }
    drawMovementFrame(plAsset, dir, STEP_AMOUNT - 1);
}


void drawMovementFrame(Asset *playerAsset, Direction dir ,int order){
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


void adjustBrightness(const Maze *mz, const Asset *plAsset, bool darken) {
    curDarken = (darken) ? max_f(curDarken * darkenFactor , 0) 
                        : min_f(curDarken / darkenFactor , 1);
    uart_puts("dark level: "); str_debug_float(curDarken);
    drawFOV(mz, plAsset);
    for (int i = 0 ; i < mz->itemMetasSz; i++){
        ItemMeta *item = mz->itemMetas[i];
        drawAssetPartial(item, plAsset);
    }
    drawAsset(plAsset);
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

void drawLevelTransitionText(const uint8_t levelNum) {
    uint64_t color = 0xE7E1DA;
    if (levelNum != 3) {
        uint16_t msg_level_width = font_string_width(6, FONT_WIDTH);
        uint16_t msg_number_width = font_string_width(1, FONT_WIDTH);
        uint16_t full_msg_width = font_string_width(16, FONT_WIDTH);
        uint16_t esc_msg_width = font_string_width(str_len(DIALOG_EXIT_MSG), FONT_WIDTH);

        char levelLabel[2];
        levelLabel[0] = levelNum + '0';
        levelLabel[1] = '\0';

        font_drawString(
                GAME_W / 2 - full_msg_width / 2,
                GAME_H / 2 - DIALOG_HEIGHT / 2,
                "Level ",
                color,
                2,
                1
        );

        font_drawString(
                GAME_W / 2 - full_msg_width / 2 + msg_level_width,
                GAME_H / 2 - DIALOG_HEIGHT / 2,
                levelLabel,
                color,
                2,
                1
        );

        font_drawString(
                GAME_W / 2 - full_msg_width / 2 + msg_level_width + msg_number_width,
                GAME_H / 2 - DIALOG_HEIGHT / 2,
                " cleared!",
                color,
                2,
                1
        );

        font_drawString(
                GAME_W / 2 - esc_msg_width / 2,
                GAME_H / 2 + DIALOG_HEIGHT / 2,
                DIALOG_EXIT_MSG,
                color,
                2,
                1
        );
    } else {
        uint16_t msg_level_width = font_string_width(8, FONT_WIDTH);
        uint16_t esc_msg_width = font_string_width(32, FONT_WIDTH);

        font_drawString(
                GAME_W / 2 - msg_level_width / 2,
                GAME_H / 2 - DIALOG_HEIGHT / 2,
                "You win!",
                color,
                2,
                1
        );

        font_drawString(
                GAME_W / 2 - esc_msg_width / 2,
                GAME_H / 2 + DIALOG_HEIGHT / 2,
                "Press Enter to return to menu...",
                color,
                2,
                1
        );
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