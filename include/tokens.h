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
    // 终结符号
    SPACE,
    PLUS, MINUS, MUL, DIV, POWER,
    LP, RP, COMMA, SEMICOLON,
    // 字面值
    LITERAL,
    NUMBER, VAR, FUNC, POINT,
    // 保留字
    SYMBOL,
    IS,
    FOR,
    FROM,
    TO,
    STEP,
    DRAW,
    SCALE,
    ROT,
    ORIGIN
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

void * make_token_info (enum TOKEN_TYPE type, char * literal, ...);
extern char *REVERSED_LITERAL[];

TOKEN * make_symbol(char * literal, enum TOKEN_TYPE type);

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

TOKEN * solve_token(char * literal);
TOKEN * make_symbol(char * literal, enum TOKEN_TYPE type);
TOKEN * make_number(char * literal);
TOKEN * find_func(char * name);

// 用于调试的函数

char * dump_token_to_str(TOKEN * tk);

#endif
