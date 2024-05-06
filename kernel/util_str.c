#include "../lib/util_str.h"


int str_cmp(const char *str1, const char *str2) {
  while (*str1 && (*str1 == *str2)) {
    str1++;
    str2++;
  }
  
  // return difference
  return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}


int str_is_prefix(const char *sub, const char *str) { //0 or 1
  if (str_len(sub) > str_len(str)) return 0;
  
  while (*sub && (*sub == *str)) {
    sub++;
    str++;
  }
  return (*sub == '\0');
}


void str_cat(const char *src, char *des) {
  
}


void str_cpy(const char *src, char *des) {
  while (src && *src) {
    *des = *src;
    src++;
    des++;
  }
  *des = '\0';
}


void str_find_nth_word(const char *src, char *des, int n){
  // skip left words
  for (int i = 0 ; i < n ; i++){
    while (*src != ' ' && *src != '\0') src++; //skip letter
    while (*src == ' ') src++; //skip space
  }
  
  // get nth word 
  while (*src != ' ' && *src != '\0'){
    *des = *src; 
    des++;
    src++;
  }
  *des = '\0';
}


int str_word_idx(const char *src, const char *word) {
  char *word1[CMD_SZ];
  int word1Idx = -1;
  for (int i = 1 ; i <= 10 ; i++) {
    str_find_nth_word(src, word1, i); 
    if (str_cmp(word1, word) == 0) {
      word1Idx = i;
      break;
    }
  }
  return word1Idx;
}


void str_find_word_after(const char *src, char *des, const char *word){
  // find the word itself
  
  // find the word itself, assuming the cmd to have max 10 words
  char *word1[CMD_SZ];
  int word1Idx = -1;
  for (int i = 1 ; i <= 10 ; i++) {
    str_find_nth_word(src, word1, i); 
    if (str_cmp(word1, word) == 0) {
      word1Idx = i;
      break;
    }
  }
  if (word1Idx == -1) {
    *des = '\0';
    return;
  }
  
  // then find the word with order n+1th
  str_find_nth_word(src, des, word1Idx+1);
}


int str_len(const char *str) {
  int sz = 0;
  while (str[sz]) sz++; //increase size until "\0" is met
  return sz;
}

// --- DEBUG AREA ---
void str_debug(const char *str) {
  uart_puts("["); 
  uart_puts(str); 
  uart_puts("]\n");
}


void str_debug_num(unsigned int num) {
  uart_puts("["); 
  uart_dec(num); uart_puts(","); uart_hex(num); 
  uart_puts("]\n");
}


void str_debug_arr(char arr[][CMD_SZ], int sz){
  if (sz == -1) sz = sizeof(arr) / sizeof(arr[0]); //print all
  uart_puts("[");
  for (int i = 0 ; i < sz; i++) {
      uart_puts(arr[i]);
      uart_puts(",");  
    }
  uart_puts("]\n");
      
}


void str_debug_float(float fl) {
  uart_puts("[");
  int flInt = (int) fl;
  uart_dec(flInt); uart_sendc('.');
  
  float flDec = (float) fl - flInt; 
  for (int i = 0 ; i < 4 ; i++){ //precision = 4
    flDec *= 10;
    int flDecInt = (int) flDec; 
    uart_dec(flDecInt);
    flDec -= flDecInt;
  }
  
  uart_puts("]\n");
}