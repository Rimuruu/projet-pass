#pragma once

#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif


#define debug_print(f_,...) \
            do { if (DEBUG_TEST) printf(f_, ##__VA_ARGS__); } while (0)
    

void errmsgf(char *format,...);
void syserr(char *message);
