#ifndef __OopsPlot__memory__
#define __OopsPlot__memory__

#include <stdio.h>
#include "parser.h"

typedef struct {
    const char * literal;
    double       value;
    double       optional;
    int          point;
} Symbol;

typedef struct {
    double x;
    double y;
} Point;

// for optimizer to alloc memory
void    new_symbol(const char * literal, double init);
double  get_symbol(const char * literal);
void    set_symbol(const char * literal, double value);
void    set_symbol_point(const char * literal, double x, double y);
int  find_symbol(const char * litral);

// for runtime
double  memaccess(size_t index);
Point * pointaccess(size_t index);
void    setvar(size_t index, double value);
void    setpoint(size_t index, double x, double y);

// for old code

int has_symbol(const char * literal);
int is_point(const char * literal);


#endif /* defined(__OopsPlot__memory__) */
