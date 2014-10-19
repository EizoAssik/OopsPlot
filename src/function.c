#include "function.h"

double add(double a, double b) {
    return a + b;
}

double sub(double a, double b) {
    return a - b;
}

double mul(double a, double b) {
    return a * b;
}

double divsion(double a, double b) {
    return a / b;
}

double plus(double x) {
    return x;
}

double minus(double x) {
    return -x;
}

FUNC_INFO BUILTIN_FUNC[] = {
    {.name = "+",   .argc = 1, .func = plus},
    {.name = "-",   .argc = 1, .func = minus},
    {.name = "SIN", .argc = 1, .func = sin},
    {.name = "COS", .argc = 1, .func = cos},
    {.name = "TAN", .argc = 1, .func = tan},
    {.name = "SART",.argc = 1, .func = sqrt},
    {.name = "+",   .argc = 2, .func = add},
    {.name = "-",   .argc = 2, .func = sub},
    {.name = "*",   .argc = 2, .func = mul},
    {.name = "/",   .argc = 2, .func = divsion},
    {.name = "**",  .argc = 2, .func = pow},
    // VOID
    {.name = NULL,  .argc = 0, .func = NULL}
};
