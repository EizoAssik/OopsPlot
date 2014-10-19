#ifndef __OopsPlot__grammer__
#define __OopsPlot__grammer__

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
    TOKEN           * op;
    ExprNode        * arg1;
    ExprNode        * arg2;
    double            value;
};

ExprNode * stmt_for();
ExprNode * stmt_is();
ExprNode * stmt_atom();
ExprNode * stmt_component();
ExprNode * stmt_factor();
ExprNode * stmt_term();
ExprNode * stmt_expr();

#endif
