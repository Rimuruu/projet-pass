#pragma once

#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif

#define debug_print(f_, ...)           \
    do                                 \
    {                                  \
        if (DEBUG_TEST)                \
        {                              \
            printf("\033[0;34m");      \
            printf(f_, ##__VA_ARGS__); \
        }                              \
    } while (0)

#define message_print(f_, ...) \
    printf("\033[0;32m");      \
    printf(f_, ##__VA_ARGS__);

void errmsgf(char *format, ...);
void syserr(char *message);
