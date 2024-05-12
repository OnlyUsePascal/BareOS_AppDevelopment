#include "../lib/math.h"

uint64_t min_ul(const uint64_t a, const uint64_t b) {
        return (a < b) ? a : b;
}

long min_l(const long a, const long b) {
        return (a < b) ? a : b;
}

uint32_t abs(const long n) {
        return n > 0 ? n : -n;
}