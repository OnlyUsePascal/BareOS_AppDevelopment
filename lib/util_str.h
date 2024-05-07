#include "def.h"

int str_cmp(const char *str1, const char *str2);
void str_find_nth_word(const char *src, char *des, int n);
int str_word_idx(const char *src, const char *word);
void str_find_word_after(const char *src, char *des, const char *word);
void str_cat(const char *src, char *des);
void str_cpy(const char *src, char *des);
void str_debug(const char *str);
void str_debug_num(unsigned int num);
void str_debug_arr(char arr[][CMD_SZ], int sz);
void str_debug_float(float f);
int str_len(const char *str);
int str_is_prefix(const char *str1, const char *str2);