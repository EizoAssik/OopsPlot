#ifndef __OopsPlot__parser__
#define __OopsPlot__parser__

#include <stdio.h>
#include "tokens.h"

typedef struct {
    char   * buffer;
    FILE   * file;
    TOKEN  * current;
    size_t   back_counter;
    size_t   length;
    size_t   lineno;
    size_t   colno;
    size_t   cur;
} SOURCE_INFO;

TOKEN * lookforward(SOURCE_INFO * src);

typedef struct _ExprNode ExprNode;
struct _ExprNode {
    enum TOKEN_TYPE   type;
    FUNC_INFO       * op;
    union {
        ExprNode    * node;
        TOKEN       * tk;
        const char  * literal;
    }                 arg1;
    union {
        ExprNode    * node;
        TOKEN       * tk;
        size_t        index;
    }                 arg2;
    double            value;
};

ExprNode * stmt_for();
ExprNode * stmt_is();
ExprNode * stmt_atom();
ExprNode * stmt_component();
ExprNode * stmt_factor();
ExprNode * stmt_term();
ExprNode * stmt_expr();

void parse(const char * filename);
#endif
