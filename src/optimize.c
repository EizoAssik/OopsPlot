#include "optimize.h"
#include "runtime.h"
#include "parser.h"
#include "memory.h"


int is_mutable(ExprNode * expr) {
    int mutable = 0;
    switch (expr->type) {
        // 当前层次FOR的所有读取都是值不变的
        case VAR:
        case NUMBER:
            return 0;
        case FUNC:
            optimize(expr->arg1.node);
            mutable = is_mutable(expr->arg1.node);
            if (expr->op->argc == 2) {
                optimize(expr->arg2.node);
                mutable += is_mutable(expr->arg2.node);
            }
            break;
        default:
            return 1;
    }
    return mutable;
}

ExprNode * optimize(ExprNode * expr) {
    const char * literal;
    switch (expr->type) {
        case VAR:
            literal = expr->arg1.literal;
            if (has_symbol(literal)) {
                expr->arg2.index = find_symbol(literal);
            } else {
                new_symbol(literal, 0);
                expr->arg2.index = find_symbol(literal);
            }
            expr->type = DMA;
            break;
            
        case FUNC:
            if (!is_mutable(expr)) {
                expr->value = eval(expr);
                expr->type = NUMBER;
            }
        default:
            break;
    }
    return expr;
}