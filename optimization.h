#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include "expression.h"

expression * constant_propagation(expression * e);
expression * expression_distribution(expression * e);

#endif // OPTIMIZATION_H
