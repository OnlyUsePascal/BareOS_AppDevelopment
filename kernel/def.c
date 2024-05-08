#include "../lib/def.h"

const char *cmdList[] = {"help", "clear" , "image", "video", "font", "game"};
const char *colorList[] = {"black", "red", "green", "yellow", "blue", "magenta", "cyan", "white"};
const char *fgColorList[] = {"\033[30m","\033[31m","\033[32m","\033[33m","\033[34m","\033[35m","\033[36m","\033[37m",};
const char *bgColorList[] = {"\033[40m","\033[41m","\033[42m","\033[43m","\033[44m","\033[45m","\033[46m","\033[47m",};

const int cmdListSz = sizeof(cmdList) / sizeof(cmdList[0]);
const int colorListSz = sizeof(colorListSz) / sizeof(colorList[0]);

