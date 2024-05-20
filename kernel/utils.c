#include "../lib/utils.h"
// other utils - timer, helper, ...


/* Functions to delay, set/wait timer */
// 1M = 1s 
void wait_msec(unsigned int n) {
    register unsigned long f, t, r, expiredTime;
    // Get the current counter frequency (Hz)
    asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));
    // Read the current counter value
    asm volatile ("mrs %0, cntpct_el0" : "=r"(t));
    // Calculate expire value for counter
    expiredTime = t + ( (f/1000)*n )/1000;
    do {
        asm volatile ("mrs %0, cntpct_el0" : "=r"(r));
    } while(r < expiredTime);
}


void set_wait_timer(int set, unsigned int msVal) {
    static unsigned long expiredTime = 0; //declare static to keep value
    register unsigned long r, f, t;
    if (set) { /* SET TIMER */
        // Get the current counter frequency (Hz)
        asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));

        // Read the current counter
        asm volatile ("mrs %0, cntpct_el0" : "=r"(t));

        // Calculate expired time:
        expiredTime = t + ( (f/1000)*msVal )/1000;
    } 
    else { /* WAIT FOR TIMER TO EXPIRE */
        do {
            asm volatile ("mrs %0, cntpct_el0" : "=r"(r));
        } while(r < expiredTime);
    }
}


uint64_t min_ul(const uint64_t a, const uint64_t b) {
    return (a < b) ? a : b;
}


uint64_t max_ul(const uint64_t a, const uint64_t b) {
    return (a > b) ? a : b;
}


long min_l(const long a, const long b) {
    return (a < b) ? a : b;
}


long max_l(const long a, const long b) {
    return (a > b) ? a : b;
}


float min_f(const float a, const float b) {
    return (a < b) ? a : b;
}


float max_f(const float a, const float b) {
    return (a > b) ? a : b;
}


uint32_t abs(const long n) {
    return n > 0 ? n : -n;
}