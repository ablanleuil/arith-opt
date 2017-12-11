#include "expression.h"
#include "optimization.h"
#include "parser.h"
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
  expr_setup_ht(10, 10);
  expression * e1, * e2;
  char tmp[1024];
  scanf("%1023[^\n]", tmp);
  e1 = parse(tmp);
  do
  {
    expr_print(e1);
    printf("\n");
    e2 = e1;
    e1 = constant_propagation(e1);
    e1 = expression_distribution(e1);
  } while (e1 != e2);

  expr_free_ht();
  return 0;
}
