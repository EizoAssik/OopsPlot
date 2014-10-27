#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"
#include "parser.h"
#include "runtime.h"
#include "error.h"
#include "memory.h"
#include "optimize.h"

typedef double (*sfunc)(double);
typedef double (*dfunc)(double, double);

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
            return get_symbol(expr->arg1.literal);
            break;
        case DMA:
            return memaccess(expr->arg2.index);
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
    var = optimize(var);
    size_t var_pos = var->arg2.index;
    setvar(var_pos, start);
    sym = start;
    x_expr = optimize(x_expr);
    y_expr = optimize(y_expr);
    while (sym < end) {
        x = eval(x_expr);
        y = eval(y_expr);
        printf("(%lf, %lf)\n", x, y);
        sym += delta;
        setvar(var_pos, sym);
    }
}

void eval_is(ExprNode * var, ExprNode * val) {
    // 改写var类型
    const char * var_name = var->arg1.tk->literal;
    if (has_symbol(var_name)) {
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
            set_symbol(var_name, eval(val));
            break;
        case POINT:
            set_symbol_point(var_name,
                             eval(val->arg1.node),
                             eval(val->arg2.node));
            break;
        default:
            error(format("IS: warn type <%s>.\n",
                         REVERSED_LITERAL[var->type]));
    }
}
