// -----------------------------------main.c -------------------------------------
// #include "../lib/uart0.h"
#include "../lib/cli.h"

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


void main(){
  // intitialize UART
  uart_init();
  helper_welcome();
  uart_puts(PROMPT);
  while(1) {
    cli();
  }
}
