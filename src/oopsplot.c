#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

const char * USAGE = "usage: oopsplot [file]\n";
const char * EF_TPL = " is not a avaliable source file.\n";

static void error(const char * why) {
    puts(why);
    exit(-1);
}

int main(int argc, char * argv[]) {
    if (argc != 2)
        error(USAGE);
    FILE * source = fopen(argv[1], "r");
    if (!source) {
        puts(argv[1]);
        error(EF_TPL);
    }
    fclose(source);
    parse(argv[1]);
    return 0;
}
