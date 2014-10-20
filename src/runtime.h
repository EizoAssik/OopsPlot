#ifndef __OopsPlot__runtime__
#define __OopsPlot__runtime__

#include <stdio.h>
#include "parser.h"

void rte(const char * why);

typedef struct {
    const char * literal;
    double       value;
    double       optional; 
    int          point;
} Symbol;

double get_symbol(const char * literal);
void   set_symbol(const char * literal, double value);
void   set_symbol_point(const char * literal, ExprNode * value);

double eval(ExprNode * expr);

void eval_for(ExprNode * var,
              ExprNode * from,
              ExprNode * to,
              ExprNode * step,
              ExprNode * x_expr,
              ExprNode * y_expr);

void eval_is(ExprNode * var, ExprNode * val);

#endif /* defined(__OopsPlot__runtime__) */
