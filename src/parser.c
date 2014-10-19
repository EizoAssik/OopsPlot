#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "tokens.h"
#include "parser.h"
#include "runtime.h"

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
    if (!has_lookback(src)) {
        src->current = next_token(src->buffer, &(src->cur));
    } 
    return src->current;
}

void error(TOKEN * tk) {
    printf("ERROR & %s\n", dump_token_to_str(tk));
    exit(1);
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
        error(src.current);
    printf("√");
}

void match(enum TOKEN_TYPE t) {
    if (has_lookback(&src)) {
        clear_lookback(&src); 
        return match_current(t);
    }
    TOKEN * tk = lookforward(&src);
    if (tk->type != t)
        error(tk);
    printf("√");
}

void pares_stmt(enum TOKEN_TYPE production[]) {
    for (int p_pos = 1; production[p_pos]!=SEMICOLON; ++p_pos) {
        if (is_terminal(production[p_pos])) {
            // TODO 继续处理非终结符号
        } else {
            // TODO 检查终结符号
            match(production[p_pos]);
        }
    }
    TOKEN * tk = lookforward(&src);
    if (tk->type != SEMICOLON)
        error(tk);
}

ExprNode * stmt_for() {
    ExprNode * var, * start,  * end, * step;
    ExprNode * x_expr, * y_expr;
    var = new_node();
    var->type = VAR;
    match(VAR);
    var->arg1 = (ExprNode *) src.current;
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
    match(VAR);
    desc->arg1 = (ExprNode *)src.current;
    match(IS);
    // 试探以避免回溯
    if (test(LP)) {
        // 赋值为PONIT
    } else {
        match(NUMBER);
        value->value = src.current->info.value;
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
        en->type = EXPR;
    } else if (test_current(FUNC)) { // FUNCALL
        en = (ExprNode *) malloc(sizeof(ExprNode));
        en->op   = src.current;
        match(LP); 
        en->arg1 = stmt_expr();
        match(RP); 
        en->type = FUNC;
    } else if (test_current(VAR)) { // VAR
        en = (ExprNode *) malloc(sizeof(ExprNode));
        en = (ExprNode *) calloc(1, sizeof(ExprNode));
        en->type = VAR;
        en->arg1 = (ExprNode *)src.current;
    } else if (test_current(NUMBER)){
        en = new_node();
        en->type = NUMBER;
        en->value = src.current->info.value;
    } else {
        error(src.current);
    }
    return en;
}

ExprNode * stmt_component() {
    ExprNode * atom = stmt_atom();
    ExprNode * comp;
    ExprNode * en;
    if (test(POWER)) {
        comp = stmt_component();
          en = new_node();
          en->type = FUNC;
          en->op = find_func("**");
          en->arg1 = atom;
          en->arg2 = comp;
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
            en = new_node();
            en->type = src.current->type;
            en->arg1 = stmt_factor();
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
        term->type = src.current->type;
        ExprNode * second = stmt_factor();
        term->arg1 = first;
        term->arg2 = second;
        first = term;
        term = new_node();
        lookforward(&src);
    }
    set_lookback(&src);
    return first;
}

ExprNode * stmt_expr() {
    ExprNode * term = new_node();
    ExprNode * first = stmt_factor();
    lookforward(&src);
    while ( test_current(PLUS) || test_current(MINUS) ) {
        term->type = src.current->type;
        ExprNode * second = stmt_factor();
        term->arg1 = first;
        term->arg2 = second;
        first = term;
        term = new_node();
        lookforward(&src);
    }
    set_lookback(&src);
    return first;
}

int main(int argc, char * argv[]) {
    init_source_info("example.gp", &src);
    printf("FILE CONTAINS: \n %s \n", src.buffer);
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
                error(tk);
                break;
        }
    }
    return 0;
}
