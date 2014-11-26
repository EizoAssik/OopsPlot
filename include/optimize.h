#ifndef __OopsPlot__optimize__
#define __OopsPlot__optimize__

#include <stdio.h>
#include "parser.h"

// 用于对FOR-DRAW进行优化的优化器
// 分配变量，改写VAR为静态索引，计算值不变表达式

int is_mutable(ExprNode * expr);
ExprNode * optimize(ExprNode * expr);

#endif /* defined(__OopsPlot__optimize__) */
