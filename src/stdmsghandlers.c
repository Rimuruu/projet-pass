#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "errhandlers.h"

void errmsgf(char *format,...) {
  va_list p;
  va_start(p,format);
  vfprintf(stderr,format,p);
  va_end(p);
}

void syserr(char *message) {
  perror(message);
}
