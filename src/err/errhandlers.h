#pragma once

#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif


#define debug_print(f_,...) \
            do { if (DEBUG_TEST) printf(f_, ##__VA_ARGS__); } while (0)
/*
 ensures that the code acts like a statement (function call). 
 The unconditional use of the code ensures that the compiler always checks that your debug code is valid —
  but the optimizer will remove the code when DEBUG is 0.

*/       

void errmsgf(char *format,...);
void syserr(char *message);
