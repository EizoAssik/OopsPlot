#include <stdio.h>
#include <stdlib.h>
#include "error.h"

static const char * RTE = "RTE: ";
char * format(const char * fmt, ...) {
    char * ret;
    va_list args;
    va_start(args, fmt);
    vasprintf(&ret, fmt, args);
    return ret;
}

void  error(const char * des) {
    fputs(des, stderr);
    exit(-1);
}

void waring(const char * des) {
    puts(des);
}

void rte(const char * des) {
    puts(RTE);
    error(des);
}
