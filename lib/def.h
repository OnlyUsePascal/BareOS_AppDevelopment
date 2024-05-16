#include "mbox.h"

#ifndef DEF_H
#define DEF_H

#define CMD_SZ 100
#define HIST_SZ 15
#define PROMPT "OS>"
#define FG_DEF "\033[37m"
#define BG_DEF "\033[40m"

#define DEBUG

extern const char *cmdList[];
extern const char *colorList[];
extern const char *bgColorList[];
extern const char *fgColorList[];
extern const int cmdListSz;
extern const int colorListSz;

enum Cmd {
    HELP,
    CLEAR,
    IMAGE,
    VIDEO,
    FONT,
    GAME,
#ifdef DEBUG
    TOGGLE_FOV
#endif
};

#define FEAT_IMG
// #define FEAT_VID
#define FEAT_FONT
#define FEAT_GAME

#endif