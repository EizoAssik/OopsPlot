#ifndef __gayplot__tokens__
#define __gayplot__tokens__

#include <stdio.h>
#include <math.h>

#define TOKEN_LEN 256
#define VAR_POINT 1
#define VAR_VALUE 0

//
enum TOKEN_TYPE {
    VOID = 0,
    // 保留字
    IS, FOR, FROM, TO, STEP, DRAW,
#ifdef __USE_RAW__
    // 保留以兼容旧语法
    T, SCALE, ROT, ORIGIN,
#endif
    // 终结符号
    PLUS, MINUS, MUL, DIV, POWER,
    LP, RP, COMMA, SEMICOLON,
    NUMBER, VAR, FUNC, DMA,
    // 非终结符号
    NONTER, POINT, EXPR,
    // 长度标记位
    TOKEN_TYPE_SIZE
};

// 词法符号
typedef struct {
    enum         TOKEN_TYPE type;
    const char * literal;
    union {
        void   * ptr;
        double   value;
    } info;
} TOKEN;

// 函数信息
typedef struct {
    int    argc;
    void * func;
    char * name;
} FUNC_INFO;

// 点的信息
typedef struct {
    double x, y;
} POINT_INFO;

extern char *REVERSED_LITERAL[];

extern TOKEN RESERVED_CONSTANT[];
extern TOKEN RESERVED_VARIABLE[];
extern TOKEN RESERVED_KEYWORDS[];

extern TOKEN TK_LP;
extern TOKEN TK_RP;
extern TOKEN TK_PLUS;
extern TOKEN TK_MINUS;
extern TOKEN TK_MUL;
extern TOKEN TK_DIV;
extern TOKEN TK_POWER;
extern TOKEN TK_SEMICOLON;
extern TOKEN TK_COMMA;

// 用于查找/处理TOKENs的函数

TOKEN     * solve_token(const char * literal);
TOKEN     * make_symbol(const char * literal, enum TOKEN_TYPE type);
TOKEN     * make_number(const char * literal);
FUNC_INFO *   find_func(const char * name);

// 用于鉴别TOKENs的函数
_Bool is_terminal(enum TOKEN_TYPE tk_t);

// 用于调试的函数
#ifdef __TEST_LEXER__
char * dump_token_to_str(TOKEN * tk);
#endif

#endif