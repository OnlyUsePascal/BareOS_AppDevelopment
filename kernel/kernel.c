// -----------------------------------main.c -------------------------------------
#include "../lib/kernel.h"
#include "../lib/def.h"
#include "../lib/helper.h"
#include "../lib/utils.h"
#include "../lib/game_be.h"
#include "../lib/font.h"
// #include "../lib/data/data_font.h"
#include "../lib/data/data_vid.h"
#include "../lib/image.h"
#include "../lib/data/data_image.h"


void cli() {
  char cmdBuf[CMD_SZ];	
  int cmdBufIdx = 0;
  
  //autocomplete
  int atcOrder = -1;  
  char atcBuf[CMD_SZ]; 
  
  //history
  char histList[HIST_SZ][CMD_SZ]; 
  int histSz = 0; 
  char *histBuf[CMD_SZ]; 
  int histIdx = -1; 

  // keep tracing keypress & execute cmd forever.
  while (1){
    char c = uart_getc();
    // AUTO COMPLETE
    if (c == '\t') {
      int atcOrderNew = -1; 
      int temp = (atcOrder + 1) % cmdListSz;

      // find cmd
      int i = temp;
      if (atcOrder == -1) str_cpy(cmdBuf, atcBuf); //auto-comp first time
      do {
        if (str_is_prefix(atcBuf, cmdList[i])) {
          atcOrderNew = i; break;
        }
        i = (i + 1) % cmdListSz;
      } while (i != temp);
      
      // copy content backend
      if (atcOrderNew == -1) continue;
      atcOrder = atcOrderNew;
      str_cpy(cmdList[atcOrder], cmdBuf);
      
      // frontend - delete and refill
      while (cmdBufIdx > str_len(atcBuf)) {
        uart_sendc('\b');
        uart_sendc(' ');
        uart_sendc('\b');
        cmdBufIdx--;
      }
      while (cmdBufIdx < str_len(cmdBuf)){
        uart_sendc(cmdBuf[cmdBufIdx]);
        cmdBufIdx++;
      }    

      continue;
    } 
    
    // HISTORY NAVIGATION BASED ON PREFIX
    if (c == '_' || c == '+') {
      if (histSz == 0) continue;
      if (histIdx == -1) str_cpy(cmdBuf, histBuf); //history first time
      
      int histIdxNew = -1;
      if (c == '_') { 
        // UP = later history
        for (int i = histIdx + 1; i < histSz; i++){
          if (str_is_prefix(histBuf, histList[i])){
            histIdxNew = i; break;
          }
        }
        
        // backend
        if (histIdxNew == -1) continue;
        histIdx = histIdxNew;
        str_cpy(histList[histIdx], cmdBuf);
        
      } else {
        // DOWN = more recent history
        for (int i = histIdx - 1; i >= 0; i--){
          if (str_is_prefix(histBuf, histList[i])){
            histIdxNew = i; break;
          }
        }
        
        // backend
        histIdx = histIdxNew;
        str_cpy((histIdxNew == -1) ? histBuf : histList[histIdx], cmdBuf);
      }
      
      // front end - delete and refill
      while (cmdBufIdx > str_len(histBuf)) {
        uart_sendc('\b');
        uart_sendc(' ');
        uart_sendc('\b');
        cmdBufIdx--;
      }
      while (cmdBufIdx < str_len(cmdBuf)){
        uart_sendc(cmdBuf[cmdBufIdx]);
        cmdBufIdx++;
      }    

      continue; 
    }    
    
    // EXECUTE COMMAND
    if (c == '\n') {
      uart_puts("\n");
      cmdBuf[cmdBufIdx] = '\0';
      
      //processing
      cli_processCmd(cmdBuf, cmdBufIdx);
      
      // update history by shifting all cmds from i -> i+1
      if (str_len(cmdBuf) > 0){
        histSz = (histSz + 1 > HIST_SZ) ? HIST_SZ : (histSz + 1);
        for (int i = histSz-2; i >= 0; i--){
          str_cpy(histList[i], histList[i+1]);
          histList[i][CMD_SZ-1] = '\0';
        }
        str_cpy(cmdBuf, histList[0]);
      }
      
      // reprompting
      uart_sendc('\n');
      uart_puts(PROMPT);
      cmdBufIdx = 0;
      cmdBuf[cmdBufIdx] = '\0';
    } 
    
    // BACKSPACE 
    else if (c == '\b') {
      if (cmdBufIdx <= 0) continue;
      uart_sendc('\b');
      uart_sendc(' ');
      uart_sendc('\b');
      cmdBufIdx--;
      cmdBuf[cmdBufIdx] = '\0';
    
    } 
    
    // NORMAL CHARACTER
    else {
      uart_sendc(c);
      cmdBuf[cmdBufIdx] = c;
      cmdBuf[cmdBufIdx+1] = '\0';
      cmdBufIdx++;
    }
    
    // reset autocomplete, history status
    atcOrder = -1;
    histIdx = -1;
  }
}



void cli_processCmd(const char *cmd, int cmdSz){
  // trim space
  char *cmdLastChar = cmd + cmdSz;
  while (*cmd == ' ') cmd++; //trim first spaces
  if (cmdLastChar > cmd){ //trim last spaces
    while (*(cmdLastChar-1) == ' ') cmdLastChar--;
    *cmdLastChar = '\0';
  }
  
  // retrieve base command (first word)
  char cmdBase[CMD_SZ];
  str_find_nth_word(cmd, cmdBase, 0);
  enum Cmd cmdIdx = cli_findCmd(cmdBase);
  
  switch (cmdIdx) {
    case HELP:
      cli_help(cmd);
      break;

    case CLEAR:
      cli_clrscr();
      break;
    
    case IMAGE:
      cli_img();

      break;
      
    case VIDEO:
      cli_vid();
      break;
    
    case FONT: 
      cli_font();
      break;
    
    case GAME:
      cli_game();
      break;
      
    default:
      uart_puts("Command not found!\n");
      break;
  }
}



// find index of given cmd in avalablie cmd list
enum Cmd cli_findCmd(const char *cmd){
  enum Cmd cmdIdx = -1; 
  for (int i = 0; i < cmdListSz; i++){
    if (str_cmp(cmd, cmdList[i]) == 0) {
      cmdIdx = i;
      break;    
    }
  }
  
  return cmdIdx;
}



// cmd help
void cli_help(const char *cmd){
  uart_puts("-- Help --\n");
  // retrieve command name
  char cmdBase[CMD_SZ];
  str_find_nth_word(cmd, cmdBase, 1); 

  // generic help
  if (str_len(cmdBase) == 0){ 
    helper_generic();
    return;
  }
  
  helper_specific(cli_findCmd(cmdBase));  
}



void cli_clrscr() {
  uart_puts("\033[2J\033[H");
}



void cli_img(){
  uart_puts("--> cli image!!\n");
  #ifdef FEAT_IMG
  draw_picture(moved_image, MOVED_IMAGE_WIDTH, MOVED_IMAGE_HEIGHT);

  char ch = uart_getc();
  while (true) {
          if (ch != IMG_UP && ch != IMG_DOWN && ch != IMG_LEFT && ch != IMG_RIGHT) {
                  break;
          }

          move_picture(ch);
          ch = uart_getc();
  }
  #endif
}



void cli_vid(){
  #ifdef FEAT_VID
  uart_puts("--> cli video!!\n");
  
  framebf_init(VID_W, VID_H, VID_W, VID_H);
  // play video 3 times
  for (int i = 0 ; i < 2 ; i++) {
    for (int frame = 0; frame < epd_bitmap_allArray_LEN; frame++){
      framebf_drawImg(0,0,VID_W, VID_H, epd_bitmap_allArray[frame]);
      wait_msec(36250);
    }
    wait_msec(1000000);
  }
  #endif
}



void cli_font(){
  uart_puts("--> cli font!!\n");
  
  const int fontW = 900, fontH = 600;
  const int OFFSET = 250;
  framebf_init(fontW, fontH, fontW, fontH);
  
  // font_drawString(fontW / 2 - OFFSET, 100, "GROUP: NARUTO", 0xFFFF00, 5);
  // font_drawString(fontW / 2 - OFFSET + 20, 200, "NGUYEN DANG HUAN      - S3927467", 0x00FFFF, 2);
  // font_drawString(fontW / 2 - OFFSET + 20, 250, "PHAM XUAN DAT         - S3927188", 0x000FFA, 2);
  // font_drawString(fontW / 2 - OFFSET + 20, 300, "TRUONG VO THIEN NHAN  - S3929215", 0xE4D00A, 2);
  // font_drawString(fontW / 2 - OFFSET + 20, 350, "PAVEL POTEMKIN        - S3963284", 0x7FFFD4, 2);
  
  font2_drawString(fontW / 2 - OFFSET, 100, "GROUP: NARUTO", 0xFFFF00, 5);
  font2_drawString(fontW / 2 - OFFSET + 20, 200, "NGUYEN DANG HUAN      - S3927467", 0x00FFFF, 2);
  font2_drawString(fontW / 2 - OFFSET + 20, 250, "PHAM XUAN DAT         - S3927188", 0x000FFA, 2);
  font2_drawString(fontW / 2 - OFFSET + 20, 300, "TRUONG VO THIEN NHAN  - S3929215", 0xE4D00A, 2);
  font2_drawString(fontW / 2 - OFFSET + 20, 350, "PAVEL POTEMKIN        - S3963284", 0x7FFFD4, 2);
  
  // font2_drawChar(100, 100, 'N', 0xFFFF00, 50);
  wait_msec(1000000);
}



void cli_game(){
  uart_puts("Entering Game Mode...\n");
  game_enter();
}



void main(){
  // intitialize UART
  uart_init();
  helper_welcome();
  uart_puts(PROMPT);
  while(1) {
    cli();
  }
}

