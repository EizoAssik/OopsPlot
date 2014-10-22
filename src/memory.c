#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "parser.h"
#include "error.h"

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

void new_symbol_point(const char * literal, double x, double y) {
    if (symcount < symbol_limit) {
        symtable[symcount].literal = literal;
        symtable[symcount].point = 1;
        symtable[symcount].value    = x;
        symtable[symcount].optional = y;
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

void set_symbol_point(const char * literal, double x, double y) {
    for(int i=0; i<symcount; ++i) {
        if ((strcmp(symtable[i].literal, literal) == 0) &&
            (symtable[i].point != 0)) {
            symtable[i].value    = x;
            symtable[i].optional = y;
            return;
        }
    }
    new_symbol_point(literal, x, y);
}

inline int find_symbol(const char * literal) {
    for (int i=0; i<symcount; ++i)
        if (strcmp(literal, symtable[i].literal)) {
            return i;
        }
    return -1;
}

inline double memaccess(size_t index) {
    return symtable[index].value;
}

inline Point * pointaccess(size_t index) {
    Symbol * point = &symtable[index];
    Point * ret = (Point *) malloc(sizeof(Point));
    ret->x = point->value;
    ret->y = point->optional;
    return ret;
}

inline void setvar(size_t index, double value) {
    symtable[index].value = value;
}

inline void setpoint(size_t index, double x, double y) {
    symtable[index].value = x;
    symtable[index].optional = y;
}
