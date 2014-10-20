#ifndef __OopsPlot__error__
#define __OopsPlot__error__

#include <stdio.h>
#include <stdarg.h>

void   error(const char * des);
void warning(const char * des);

char * format(const char * fmt, ...);

#endif /* defined(__OopsPlot__error__) */
