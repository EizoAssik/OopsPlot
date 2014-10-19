#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include "lexer.h"
#include "tokens.h"
#include "function.h"

#define BUFFER_SIZE 32


static inline char curr_char(const char* src, size_t* cur) { return src[*cur]; }
static inline char next_char(const char* src, size_t* cur) { return src[*cur + 1]; }

static inline void pass_to(const char * src, size_t * cur, char c) {
    while(src[*cur]&&(src[*cur]!=(c))) *cur += 1;
}

#define has_buffer() (tok_buff_pos+ltr_buff_pos > 0)

static inline void clear_buffer(char *buffer, size_t limit) {
    for (size_t i = 0; i != limit; ++i)
        buffer[i] = 0;
}

static TOKEN * buffer_or_this(char * literal_buffer, char * token_buffer,
                                     size_t * tok_buff_pos,
                                     size_t * ltr_buff_pos,
                                     size_t * cur,
                                     TOKEN * this) {
    TOKEN * next = NULL;
    if (*tok_buff_pos) {
        next = solve_token(token_buffer);
        clear_buffer(token_buffer, *tok_buff_pos);
        *tok_buff_pos = 0;
    } else if (*ltr_buff_pos) {
        next = make_number(literal_buffer);
        clear_buffer(literal_buffer, *ltr_buff_pos);
        *ltr_buff_pos = 0;
    } else {
        (*cur) += 1;      
        next = this;
    }
    return next;
}

#define BUFFER_OR_THIS(this) \
buffer_or_this(literal_buffer, \
               token_buffer, \
               &tok_buff_pos, \
               &ltr_buff_pos, \
               cur, \
               this)


// 其实一个如此简单的语言是完全没必要大动干戈的
TOKEN * next_token(const char * src, size_t * cur) {
    TOKEN * _tk = NULL;
    char token_buffer[BUFFER_SIZE] = {};
    char literal_buffer[BUFFER_SIZE] = {};
    size_t tok_buff_pos = 0;
    size_t ltr_buff_pos = 0;
    char current;
    current = toupper(curr_char(src, cur));
    while (current != '\0') {
        switch (current) {
            // 掠过空格
            case ' ': case '\t': case '\n': case '\r':
                _tk = BUFFER_OR_THIS(NULL);
                if (_tk) return _tk;
                break;
                
            // 处理结构性符号
            case '(':
                return BUFFER_OR_THIS(&TK_LP);
                break;
            case ')':
                return BUFFER_OR_THIS(&TK_RP);
                break;
            case ',':
                return BUFFER_OR_THIS(&TK_COMMA);
                break;
            case ';':
                return BUFFER_OR_THIS(&TK_SEMICOLON);
                break;
            case '+':
                return BUFFER_OR_THIS(&TK_PLUS);
                break;
                
            case '*':
                if (next_char(src, cur) == '*') {
                    TOKEN * _exp = BUFFER_OR_THIS(&TK_POWER);
                    if (_exp == &TK_POWER){
                        *cur += 1;
                    }
                    return _exp;
                } else {
                    return BUFFER_OR_THIS(&TK_MUL);
                }
                break;
                
            case '-':
                if (next_char(src, cur) == '-') {
                    pass_to(src, cur, '\n');
                } else {
                    return BUFFER_OR_THIS(&TK_MINUS);
                }
                break;
            case '/':
                if (next_char(src, cur) == '/') {
                    pass_to(src, cur, '\n');
                } else {
                    return BUFFER_OR_THIS(&TK_DIV);
                }
                break;
                
            // 这里真的需要fall-throgh
            case '.':
                if (!ltr_buff_pos) {
                    return NULL;
                }
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                literal_buffer[ltr_buff_pos++] = current;
                (*cur)++;
                break;
                
            // 处理名字
            default:
                token_buffer[tok_buff_pos++] = current;
                (*cur)++;
                break;
        }
        current = toupper(curr_char(src, cur));
    }
    return BUFFER_OR_THIS(NULL);
}
# ifdef __TEST_LEXER__
int main(int argc, char * argv[]) {
    argv[1] = "example.gp";
    FILE * src_file = fopen(argv[1], "r");
    fseek(src_file, 0, SEEK_END);
    size_t len = ftell(src_file) + 1;
    fseek(src_file, 0, SEEK_SET);
    char * src = (char *) calloc(1, len);
    fread(src, 1, len, src_file);
    printf("LEXER @ %s - %ld bytes\n", argv[1], len);
    printf("FILE CONTAINS: \n %s \n", src);
    size_t pos = 0;
    TOKEN * tk = NULL;
    while (1) {
        tk = next_token(src, &pos);
        if (tk==NULL)
            break;
        printf("%s\n", dump_token_to_str(tk));
    }
    return 0;
}
# endif