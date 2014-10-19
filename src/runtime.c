#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"
#include "parser.h"
#include "runtime.h"

typedef double (*sfunc)(double);
typedef double (*dfunc)(double, double);

static const size_t symbol_limit = 128;
static Symbol symtable[symbol_limit];
static size_t symcount = 0;

void new_symbol(const char * literal, double init) {
    if (symcount < symbol_limit) {
        symtable[symcount].literal = literal;
        symtable[symcount].value = init;
        symcount += 1;
    } else {
        rte("variables out of limit.\n");
    }
}

double get_symbol(const char * literal) {
    for(int i=0; i<symcount; ++i) {
        if (strcmp(symtable[i].literal, literal) == 0)
            return symtable[i].value;
    }
    new_symbol(literal, 0.0);
    return 0.0;
//    rte(sprintf("variable \"%s\" not defined.\n", literal));
}

double set_symbol(const char * literal, double value) {
    for(int i=0; i<symcount; ++i) {
        if (strcmp(symtable[i].literal, literal) == 0)
            symtable[i].value = value;
    }
    new_symbol(literal, value);
    return value;
}

static inline const char * get_var_name(ExprNode * expr) {
    return ((TOKEN *)(expr->arg1))->literal;
}

double eval(ExprNode * expr) {
    FUNC_INFO * op;
    switch (expr->type) {
        case FUNC:
            op = expr->op->info.ptr;
            if (op->argc == 1) {
                return ((sfunc)(op->func))(eval(expr->arg1));
            } else {
                return ((dfunc)(op->func))(eval(expr->arg1), eval(expr->arg2));
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
    double sym, start, end, delta;
    start = eval(from);
      end = eval(to);
    delta = eval(step);
    const char * var_name = get_var_name(var);
    set_symbol(var_name, start);
    sym = start;
    while (sym < end) {
        printf("(%lf, %lf)\n", eval(x_expr), eval(y_expr));
        sym += delta;
        set_symbol(var_name, sym);
    }
}

void eval_is(ExprNode * var, ExprNode * val) {
    set_symbol(get_var_name(var), eval(val));
}

void rte(const char * why) {
    printf("%s", why);
    exit(-1);
}
