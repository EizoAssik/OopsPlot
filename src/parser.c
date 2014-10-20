#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "tokens.h"
#include "parser.h"
#include "runtime.h"
#include "function.h"
#include "error.h"

SOURCE_INFO src;

void init_source_info(const char * filename, SOURCE_INFO * si) {
    si->file = fopen(filename, "r");
    fseek(si->file, 0, SEEK_END);
    size_t len = ftell(si->file) + 1;
    fseek(si->file, 0, SEEK_SET);
    si->buffer = (char *) calloc(1, len);
    fread(si->buffer, 1, len, si->file);
    si->length = len;
}

static inline ExprNode * new_node() {
    return (ExprNode *) calloc(1, sizeof(ExprNode));
}

static inline void   set_lookback(SOURCE_INFO *si) {
    si->back_counter = 1;
}

static inline void clear_lookback(SOURCE_INFO *si) {
    si->back_counter = 0;
}

static inline size_t has_lookback(SOURCE_INFO *si) {
    return si->back_counter;
}

TOKEN * lookforward(SOURCE_INFO * src) {
    if (has_lookback(src)) {
        clear_lookback(src);
    } else {
        src->current = next_token(src->buffer, &(src->cur));
    }
    return src->current;
}

static void lexer_error(TOKEN * tk) {
    error(format("ERROR @ %s\n", dump_token_to_str(tk)));
}

static inline _Bool test_current(enum TOKEN_TYPE t) {
    return src.current->type == t;
}

static inline _Bool test(enum TOKEN_TYPE t) {
    if (has_lookback(&src)) {
        clear_lookback(&src); 
        return test_current(t);
    }
    TOKEN * tk = lookforward(&src);
    return tk->type == t;
}

void match_current(enum TOKEN_TYPE t) {
    if (src.current->type != t)
        lexer_error(src.current);
}

void match(enum TOKEN_TYPE t) {
    if (has_lookback(&src)) {
        clear_lookback(&src); 
        return match_current(t);
    }
    TOKEN * tk = lookforward(&src);
    if (tk->type != t)
        lexer_error(tk);
}

ExprNode * stmt_for() {
    ExprNode * var, * start,  * end, * step;
    ExprNode * x_expr, * y_expr;
    var = new_node();
    var->type = VAR;
    match(VAR);
    var->arg1.tk = src.current;
    match(FROM);
    start    = stmt_expr();
    match(TO);
    end      = stmt_expr();
    match(STEP);
    step     = stmt_expr();
    match(DRAW); match(LP);
    x_expr   = stmt_expr();
    match(COMMA);
    y_expr   = stmt_expr();
    match(RP);   match(SEMICOLON);
    eval_for(var, start, end, step, x_expr, y_expr);
    return NULL;
}

ExprNode * stmt_is() {
    ExprNode * desc  = new_node();
    ExprNode * value = new_node();
    desc->arg1.tk = src.current;
    desc->type = VAR;
    match(IS);
    // 试探以避免回溯
    if (test(LP)) {
        ExprNode * x, * y;
        x = new_node();
        y = new_node();
        x->type = y->type = NUMBER;
        match(NUMBER);
        x->value = src.current->info.value;
        match(COMMA);
        match(NUMBER);
        y->value = src.current->info.value;
        match(RP);
        value->type = POINT;
        value->arg1.node = x;
        value->arg2.node = y;
    } else {
        set_lookback(&src);
        match(NUMBER);
        value->value = src.current->info.value;
        value->type = NUMBER;
    }
    match(SEMICOLON);
    eval_is(desc, value);
    return NULL;
}

ExprNode * stmt_atom() {
    ExprNode * en;
    if (test(LP)) { // (expr)
        en = stmt_expr(); 
        match(RP);
    } else if (test_current(FUNC)) { // FUNCALL
        en     = new_node(); 
        en->op = find_func(src.current->literal);
        match(LP); 
        en->arg1.node = stmt_expr();
        match(RP); 
        en->type = FUNC;
    } else if (test_current(VAR)) { // VAR
        en = new_node();
        en->type = VAR;
        en->arg1.literal = src.current->literal;
    } else if (test_current(NUMBER)){
        en = new_node();
        en->type = NUMBER;
        en->value = src.current->info.value;
    } else {
        lexer_error(src.current);
    }
    return en;
}

ExprNode * stmt_component() {
    ExprNode * atom = stmt_atom();
    ExprNode * comp;
    ExprNode * en;
    if (test(POWER)) {
          en = new_node();
          en->type = FUNC;
          en->op   = find_func(src.current->literal);
          comp = stmt_component();
          en->arg1.node = atom;
          en->arg2.node = comp;
    } else {
        set_lookback(&src);
        en = atom;
    }
    return en;
}

ExprNode * stmt_factor() {
    lookforward(&src);
    ExprNode * en;
    switch (src.current->type) {
        case PLUS: case MINUS:
            en       = new_node();
            en->type = FUNC;
            en->op   = find_func(src.current->type == PLUS ? "(+)" : "(-)");
            en->arg1.node = stmt_factor();
            break;
        default:
            set_lookback(&src);
            en = stmt_component();
            break;
    }
    return en;
}

ExprNode * stmt_term() {
    ExprNode * term = new_node();
    ExprNode * first = stmt_factor();
    lookforward(&src);
    while ( test_current(MUL) || test_current(DIV) ) {
        term->type = FUNC;
        term->op   = find_func(src.current->literal);
        ExprNode * second = stmt_factor();
        term->arg1.node = first;
        term->arg2.node = second;
        first = term;
        term  = new_node();
        lookforward(&src);
    }
    set_lookback(&src);
    free(term);
    return first;
}

ExprNode * stmt_expr() {
    ExprNode * expr  = new_node();
    ExprNode * first = stmt_term();
    lookforward(&src);
    while ( test_current(PLUS) || test_current(MINUS) ) {
        expr->type = FUNC;
        expr->op   = find_func(src.current->literal);
        ExprNode * second = stmt_term();
        expr->arg1.node = first;
        expr->arg2.node = second;
        first = expr;
        expr  = new_node();
        lookforward(&src);
    }
    set_lookback(&src);
    free(expr);
    return first;
}

void parse(const char * filename) {
    init_source_info(filename, &src);
    TOKEN * tk;
    while (1) {
        tk = lookforward(&src);
        if (!tk) break;
        switch (tk->type) {
            case FOR:
                stmt_for();
                break;
            case VAR:
                stmt_is();
                break;
            default:
                lexer_error(tk);
                break;
        }
    }
}
