#ifndef UTILS_H
#define UTILS_H

#include "def.h"
#include "util_str.h"

void wait_msec(unsigned int n);
void set_wait_timer(int set, unsigned int msVal);

uint64_t min_ul(const uint64_t a, const uint64_t b);
uint64_t max_ul(const uint64_t a, const uint64_t b);
long min_l(const long a, const long b);
long max_l(const long a, const long b);
float min_f(const float a, const float b);
float max_f(const float a, const float b);

uint32_t abs(const long num);


#endif