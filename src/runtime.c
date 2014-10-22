#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"
#include "parser.h"
#include "runtime.h"
#include "error.h"
#include "memory.h"

typedef double (*sfunc)(double);
typedef double (*dfunc)(double, double);

static inline const char * get_var_name(ExprNode * expr) {
    return expr->arg1.literal;
}

double eval(ExprNode * expr) {
    switch (expr->type) {
        case FUNC:
            if (expr->op->argc == 1) {
                double arg = eval(expr->arg1.node);
                return ((sfunc)(expr->op->func))(arg);
            } else {
                double arg1 = eval(expr->arg1.node);
                double arg2 = eval(expr->arg2.node);
                return ((dfunc)(expr->op->func))(arg1, arg2);
            }
            break;
        case VAR:
            return get_symbol(get_var_name(expr));
            break;
        case NUMBER:
            return expr->value;
            break;
        default:
            rte("not known type\n");
            return 0.0;
    }
}

void eval_for(ExprNode * var,
              ExprNode * from,
              ExprNode * to,
              ExprNode * step,
              ExprNode * x_expr,
              ExprNode * y_expr) {
    double sym, start, end, delta, x, y;
    start = eval(from);
      end = eval(to);
    delta = eval(step);
    const char * var_name = get_var_name(var);
    set_symbol(var_name, start);
    sym = start;
    while (sym < end) {
        x = eval(x_expr);
        y = eval(y_expr);
        //printf("(%lf, %lf)\n", x, y);
        sym += delta;
        set_symbol(var_name, sym);
    }
}

void eval_is(ExprNode * var, ExprNode * val) {
    // 改写var类型
    const char * var_name = var->arg1.tk->literal;
    if (has_symbol(var_name)){
        if(is_point(var_name)) {
            var->type = POINT;
        } else {
            var->type = NUMBER;
        }
    } else {
        var->type = val->type;
    }
    if (var->type != val->type)
        error(format("IS: %s<%s> excepted, got %s.\n",
                     var->arg1.tk->literal,
                     REVERSED_LITERAL[var->type],
                     REVERSED_LITERAL[val->type]));
    switch (var->type) {
        case NUMBER:
            set_symbol(get_var_name(var), eval(val));
            break;
        case POINT:
            set_symbol_point(get_var_name(var),
                             eval(val->arg1.node),
                             eval(val->arg2.node));
            break;
        default:
            error(format("IS: warn type <%s>.\n",
                         REVERSED_LITERAL[var->type]));
    }
}
