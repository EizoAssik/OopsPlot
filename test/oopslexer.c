#define __TEST_LEXER__
#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"
#include "lexer.h"
#include "error.h"

void lex(const char * filename) {
    size_t cur = 0;
    TOKEN * tk;
    FILE * source  = fopen(filename, "r");
    fseek(source, 0, SEEK_END);
    size_t len = ftell(source) + 1;
    fseek(source, 0, SEEK_SET);
    char * src = (char *) calloc(len + 1, sizeof(char));
    fread(src, len, 1, source);
    while (1) {
        tk = next_token(src, &cur);
        if (!tk) break;
        puts(dump_token_to_str(tk));
    }
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        lex(argv[1]);
    } else {
        puts("Usage: oopslexer filename\n");
    }
    return 0;
}