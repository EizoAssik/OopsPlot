#include "tokens.h"
#include "function.h"
#include "usrfunc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

char *REVERSED_LITERAL[] = {
    "VOID",
    // 保留字
    "IS", "FOR", "FROM", "TO", "STEP", "DRAW",
#ifdef __USE_RAW__
    // 保留以兼容旧语法
    "T", "SCALE", "ROT", "ORIGIN"
#endif
    // 终结符号
    "PLUS", "MINUS", "MUL", "DIV", "POWER",
    "LP", "RP", "COMMA", "SEMICOLON",
    "NUMBER", "VAR", "FUNC",
    // 非终结符号
    "NONTER", "POINT", "EXPR",
};

TOKEN RESERVED_CONSTANT[] = {
    { .type = NUMBER, .literal = "E",  .info = { .value = M_E  } },
    { .type = NUMBER, .literal = "PI", .info = { .value = M_PI } },
    { .type = VOID },
};

#ifdef __USE_RAW__
TOKEN RESERVED_VARIABLE[] = {
    { .type = T,      .literal = "T"      },
    { .type = ROT,    .literal = "ROT"    },
    { .type = SCALE,  .literal = "SCALE"  },
    { .type = ORIGIN, .literal = "ORIGIN" },
    { .type = VOID },
};
#endif

TOKEN RESERVED_KEYWORDS[] = {
    { .type = IS,   .literal = "IS"   },
    { .type = FOR,  .literal = "FOR"  },
    { .type = FROM, .literal = "FROM" },
    { .type = TO,   .literal = "TO"   },
    { .type = STEP, .literal = "STEP" },
    { .type = DRAW, .literal = "DRAW" },
    { .type = VOID },
};

TOKEN TK_LP        = { .type = LP,        .literal = "("  };
TOKEN TK_RP        = { .type = RP,        .literal = ")"  };
TOKEN TK_PLUS      = { .type = PLUS,      .literal = "+"  };
TOKEN TK_MINUS     = { .type = MINUS,     .literal = "-"  };
TOKEN TK_MUL       = { .type = MUL,       .literal = "*"  };
TOKEN TK_DIV       = { .type = DIV,       .literal = "/"  };
TOKEN TK_POWER     = { .type = POWER,     .literal = "**" };
TOKEN TK_SEMICOLON = { .type = SEMICOLON, .literal = ";"  };
TOKEN TK_COMMA     = { .type = COMMA,     .literal = ","  };

static inline FUNC_INFO * search_func(FUNC_INFO funcs[], char* literal) {
    for (int i = 0 ; funcs[i].name != NULL ; i++)
        if (strcmp(literal, funcs[i].name) == 0)
            return &funcs[i];
    return NULL;
}

static inline TOKEN * search_token(TOKEN tks[], char* literal) {
    for (int i = 0 ; tks[i].literal != NULL ; i++)
        if (strcmp(literal, tks[i].literal) == 0)
            return &tks[i];
    return NULL;
}

TOKEN * make_symbol(char * literal, enum TOKEN_TYPE type) {
    TOKEN * symbol = (TOKEN *) malloc(sizeof(TOKEN));
    symbol->type = type;
    symbol->literal = literal;
    return symbol;
}

// 搜索内建或创建一个SYMBOL
TOKEN * solve_token(char * literal) {
    TOKEN * tk = NULL;
    // 如果名字是一个常量
    tk = search_token(RESERVED_CONSTANT, literal);
    if (tk) return tk;
    // 如果名字是一个关键字
    tk = search_token(RESERVED_KEYWORDS, literal);
    if (tk) return tk;
#ifdef __USE_RAW__
    // 如果名字在原始语法的保留变量空间
    tk = search_token(RESERVED_VARIABLE, literal);
    if (tk) return tk;
#endif
    // 如果名字能够被解析成函数
    tk = find_func(literal);
    if (tk) return tk;
    // 否则构造为一个变量名
    return make_symbol(strdup(literal), VAR);
}

TOKEN * make_number (char * literal) {
    // TODO 写一个健壮的实w现
    char ** endptr;
    double val = strtod(literal, NULL);
    TOKEN * tk = (TOKEN *) malloc(sizeof(TOKEN));
    tk->type = NUMBER;
    tk->literal = strdup(literal);
    tk->info.value = val;
    return tk;
}

TOKEN * find_func (char * name) {
    TOKEN * tk = NULL;
    FUNC_INFO * fi = NULL;
    // 如果名字是一个用户自定义函数
    fi = search_func(USER_FUNC, name);
    if (!fi) // 如果名字是一个内建函数
        fi = search_func(BUILTIN_FUNC, name);
    if (!fi) // 如果搜索失败
        return NULL;
    tk = (TOKEN *) calloc(1, sizeof(TOKEN));
    tk->type = FUNC;
    tk->info.ptr = fi;
    tk->literal = fi->name;
    return tk;
}

void * make_token_info (enum TOKEN_TYPE type, char * literal, ...) {
    switch (type) {
        case FUNC:
            break;
        
        case NUMBER:
            break;
            
        case POINT:
            break;
        default:
            break;
    }
    return NULL;
}

inline _Bool is_terminal(enum TOKEN_TYPE tk_t) {
    return tk_t > NONTER;
}

char * dump_token_to_str(TOKEN * tk) {
    char * desc = malloc(64);
    switch (tk->type) {
        case NUMBER:
            sprintf(desc, "<TOKEN %-8s \"%s\" as %lf>",
                    REVERSED_LITERAL[tk->type],
                    tk->literal,
                    tk->info.value);
            break;
        case POINT:
            sprintf(desc, "<TOKEN %-8s \"%s\" as (%lf, %lf)>",
                    REVERSED_LITERAL[tk->type],
                    tk->literal,
                    ((POINT_INFO*)(tk->info.ptr))->x,
                    ((POINT_INFO*)(tk->info.ptr))->y);
            break;
            
        default:
            sprintf(desc, "<TOKEN %-8s \"%s\">", REVERSED_LITERAL[tk->type], tk->literal);
            break;
    }
    return desc;
}
