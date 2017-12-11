#include "expression.h"
#include "optimization.h"
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
  expr_setup_ht(10, 10);
  expression * e = new_binary(opmul,
                     new_binary(opmul, 
                       new_constant(3),
                       new_constant(5)),
                     new_binary(opadd,
                       new_constant(3),
                       new_variable("a")));
  expression * e2 = new_binary(opdiv, e, new_binary(opmul,
                                          new_binary(opadd,
                                            new_constant(1),
                                            new_constant(2))
                                          , new_binary(opadd,
                                            new_constant(1),
                                            new_constant(2))));

  expression * e3;

  do
  {
    expr_print(e2);
    printf("\n");
    e3 = e2;
    e2 = constant_propagation(e2);
    e2 = expression_distribution(e2);
  } while (e3 != e2);

  ht_print(expr_get_ht());
  expr_print(e2);
  printf("\n");

  expr_free_ht();
  return 0;
}
