#include "errhandlers.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "net.h"
#include <sys/select.h>
#include "enumvalue.h"

int main()
{
    char test[32];
    uint8_t test2[32];
    strcpy(test, "test");
    memcpy(test2, test, 32);
    printf("%s test1\n", (char *)test2);
    printf("%d test1\n", test2[0]);
    return 0;
}