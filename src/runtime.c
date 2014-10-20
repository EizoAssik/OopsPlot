#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"
#include "parser.h"
#include "runtime.h"
#include "error.h"

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

void new_symbol_point(const char * literal, ExprNode * init) {
    if (symcount < symbol_limit) {
        symtable[symcount].literal = literal;
        symtable[symcount].point = 1;
        symtable[symcount].value    = eval(init->arg1.node);
        symtable[symcount].optional = eval(init->arg2.node);
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

int is_point(const char * literal) {
    for(int i=0; i<symcount; ++i) {
        if (strcmp(symtable[i].literal, literal) == 0)
            return symtable[i].point;
    }
    return 0;
}

int has_symbol(const char * literal) {
    for(int i=0; i<symcount; ++i) {
            if (strcmp(symtable[i].literal, literal) == 0)
                return 1;
        }
    return 0;
}


void set_symbol(const char * literal, double value) {
    for(int i=0; i<symcount; ++i) {
        if (strcmp(symtable[i].literal, literal) == 0) {
            symtable[i].value = value;
            return;
        }
    }
    new_symbol(literal, value);
}

void set_symbol_point(const char * literal, ExprNode * value) {
    for(int i=0; i<symcount; ++i) {
        if ((strcmp(symtable[i].literal, literal) == 0) &&
            (symtable[i].point != 0)) {
            symtable[i].value    = eval(value->arg1.node);
            symtable[i].optional = eval(value->arg2.node);
            return;
        }
    }
    new_symbol_point(literal, value);
}

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
        printf("(%lf, %lf)\n", x, y);
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
        error(format("RTE during IS: %s<%s> excepted, got %s.\n",
                     var->arg1.tk->literal,
                     REVERSED_LITERAL[var->type],
                     REVERSED_LITERAL[val->type]));
    switch (var->type) {
        case NUMBER:
            set_symbol(get_var_name(var), eval(val));
            break;
        case POINT:
            set_symbol_point(get_var_name(var), val);
            break;
        default:
            error(format("RTE during IS: warn type <%s>.\n",
                         REVERSED_LITERAL[var->type]));
    }
}

void rte(const char * why) {
    error(why);
}
