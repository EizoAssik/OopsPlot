#include "optimize.h"
#include "parser.h"
#include "memory.h"


int is_mutable(ExprNode * expr) {
    switch (expr->type) {
        case VAR:
            return 1;
        case NUMBER:
            return 0;
        case FUNC:
            if (expr->op->argc == 1) {
                return is_mutable(expr->arg1.node);
            } else {
                return is_mutable(expr->arg1.node)
                + is_mutable(expr->arg2.node);
            }
        default:
            return 1;
    }
}

ExprNode * optimize(ExprNode * expr) {
    return expr;
}