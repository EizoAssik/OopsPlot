#ifndef __gayplot__function__
#define __gayplot__function__

#include <stdio.h>
#include <math.h>
#include "tokens.h"

double add(double a, double b);
double sub(double a, double b);
double mul(double a, double b);
double divsion(double a, double b);

double plus(double x);
double minus(double x);

extern FUNC_INFO BUILTIN_FUNC[];

#endif /* defined(__gayplot__function__) */
