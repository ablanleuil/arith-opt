#include "expression.h"
#include "hash_table.h"
#include <stdlib.h>
#include <stdio.h>

hash_table * static_table = NULL;

void expr_setup_ht(int expr_size, int name_size)
{
  if (static_table)
    return;

  static_table = ht_create(expr_size, name_size);
}

void expr_free_ht()
{
  if (!static_table)
    return;

  ht_destroy(static_table);
  static_table = NULL;
}

hash_table * expr_get_ht()
{
  return static_table;
}

expression * new_variable(const char * name)
{
  if (!static_table)
    return NULL;

  return ht_add_variable(static_table, name);
}

expression * new_constant(float value)
{
  if (!static_table)
    return NULL;

  return ht_add_constant(static_table, value);
}

expression * new_binary(char op, expression * left, expression * right)
{
  if (!(static_table && left && right))
    return NULL;

  return ht_add_binary(static_table, op, left, right);
}

void expr_print(expression * expr)
{
  switch (expr->type)
  {
    case variable:
      printf(expr->name);
      break;
    case constant:
      printf("%f", expr->value);
      break;
    case binary:
      printf("(");
      expr_print(expr->op.left);
      printf("%c", expr->op.op);
      expr_print(expr->op.right);
      printf(")");
      break;
    default:
      break;
  }
}

int expression_compare(expression * l, expression * r)
{
  if (l == r)
    return 0;

  if (l->type != r->type)
    return l->type - r->type;

  int ret1, ret2;
  switch (l->type)
  {
    case variable: return strcmp(l->name, r->name);
    case constant: return (int)(l->value - r->value);
    case binary:
      if (!(ret1 = expression_compare(l->op.left, r->op.left)))
        if (!(ret2 = expression_compare(l->op.right, r->op.right)))
          return l->op.op - r->op.op;
        else
          return ret2;
      else
        return ret1;
  }
}

