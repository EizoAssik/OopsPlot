#ifndef __OopsPlot__runtime__
#define __OopsPlot__runtime__

#include <stdio.h>
#include "parser.h"

void rte(const char * why);

typedef struct {
    const char * literal;
    double       value;
} Symbol;

double get_symbol(const char * literal);
double set_symbol(const char * literal, double value);

double eval(ExprNode * expr);

void eval_for(ExprNode * var,
              ExprNode * from,
              ExprNode * to,
              ExprNode * step,
              ExprNode * x_expr,
              ExprNode * y_expr);

void eval_is(ExprNode * var, ExprNode * val);

#endif /* defined(__OopsPlot__runtime__) */
