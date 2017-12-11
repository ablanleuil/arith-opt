#include "hash_table.h"
#include "expression.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct elist
{
  expression expr;
  struct elist * next;
} elist;

typedef struct slist
{
  const char * string;
  struct slist * next;
} slist;

struct hash_table
{
  int expr_size;
  int name_size;
  elist ** expr_table;
  slist ** name_table;
};

int hash_expression(expression expr)
{
  int to_ret = 0;
  switch (expr.type)
  {
    case variable:
      to_ret = (long int)expr.name;
      break;
    case constant:
      to_ret = expr.value;
      break;
    case binary:
      to_ret = (int)expr.op.op + (long int)expr.op.left + (long int)expr.op.right;
      break;
    default:
      to_ret = 0;
  };

  return to_ret;
}

int hash_name(const char * name)
{
  int to_ret = 0;
  while (*name)
    to_ret += *(name++);

  return to_ret;
}

hash_table * ht_create(int expr_size, int name_size)
{
  int i;
  hash_table * ret = (hash_table*)malloc(sizeof(hash_table));
  ret->expr_size = expr_size;
  ret->name_size = name_size;

  ret->expr_table = (elist**)malloc(expr_size * sizeof(elist*));
  for (i = 0; i < expr_size; ++i)
    ret->expr_table[i] = NULL;

  ret->name_table = (slist**)malloc(name_size * sizeof(slist*));
  for (i = 0; i < name_size; ++i)
    ret->name_table[i] = NULL;

  return ret;
}

void elist_free(elist * l)
{
  if (l == NULL)
    return;

  elist_free(l->next);
  free(l->next);
}

void slist_free(slist * l)
{
  if (l == NULL)
    return;

  slist_free(l->next);
  free(l->string);
  free(l->next);
}

void ht_destroy(hash_table * table)
{
  int i;
  for (i = 0; i < table->expr_size; ++i)
    elist_free(table->expr_table[i]);
  for (i = 0; i < table->name_size; ++i)
    slist_free(table->name_table[i]);

  free(table->expr_table);
  free(table->name_table);

  table->expr_size = table->name_size = 0;
  table->expr_table = NULL;
  table->name_table = NULL;
}

const char * ht_add_name(hash_table * table, const char * name)
{
  int name_h = hash_name(name);
  slist * l = table->name_table[name_h % table->name_size];

  if (!l)
  {
    slist * added = (slist*)malloc(sizeof(slist));
    added->next = NULL;
    added->string = strdup(name);
    table->name_table[name_h % table->name_size] = added;

    return added->string;
  }

  slist * prev = l;
  do
  {
    l = prev;
    if (!strcmp(name, l->string))
      return l->string;

    prev = l->next;
  } while (prev != NULL);

  slist * added = (slist*)malloc(sizeof(slist));
  l->next = added;
  added->next = NULL;
  added->string = strdup(name);

  return added->string;
}

expression * ht_add_variable(hash_table * table, const char * name)
{
  expression expr;
  expr.type = variable;
  expr.name = ht_add_name(table, name);
  unsigned int expr_h = hash_expression(expr);
  elist * l = table->expr_table[expr_h % table->expr_size];

  if (!l)
  {
    elist * added = (elist*)malloc(sizeof(elist));
    table->expr_table[expr_h % table->expr_size] = added;
    added->next = NULL;
    added->expr = expr;
    return &(added->expr);
  }

  elist * prev = l;
  do
  {
    l = prev;
    if (l->expr.type == variable && l->expr.name == expr.name)
      return &(l->expr);

    prev = l->next;
  } while (prev != NULL);

  elist * added = (elist*)malloc(sizeof(elist));
  l->next = added;
  added->next = NULL;
  added->expr = expr;

  return &(added->expr);
}

expression * ht_add_constant(hash_table * table, float value)
{
  expression expr;
  expr.type = constant;
  expr.value = value;

  unsigned int expr_h = hash_expression(expr);
  elist * l = table->expr_table[expr_h % table->expr_size];

  if (!l)
  {
    elist * added = (elist*)malloc(sizeof(elist));
    table->expr_table[expr_h % table->expr_size] = added;
    added->next = NULL;
    added->expr = expr;

    return &(added->expr);
  }

  elist * prev = l;
  do
  {
    l = prev;
    if (l->expr.type == constant && l->expr.value == expr.value)
      return &(l->expr);

    prev = l->next;
  } while (prev != NULL);

  elist * added = (elist*)malloc(sizeof(elist));
  l->next = added;
  added->next = NULL;
  added->expr = expr;

  return &(added->expr);
}

expression * ht_add_binary
  ( hash_table * table
  , char op
  , expression * left
  , expression * right)
{
  expression expr;
  expr.type = binary;
  expr.op.op = op;

  int comp = op == opsub || op == opdiv ? -1 : expression_compare(left, right);

  expr.op.left = ht_add_expression(table, comp < 0 ? left : right);
  expr.op.right = ht_add_expression(table, comp < 0 ? right : left);

  unsigned int expr_h = hash_expression(expr);
  elist * l = table->expr_table[expr_h % table->expr_size];

  if (!l)
  {
    elist * added = (elist*)malloc(sizeof(elist));
    table->expr_table[expr_h % table->expr_size] = added;
    added->next = NULL;
    added->expr = expr;

    return &(added->expr);
  }

  elist * prev = l;
  do
  {
    l = prev;
    if (l->expr.type == binary &&
        l->expr.op.op == expr.op.op &&
        l->expr.op.left == expr.op.left &&
        l->expr.op.right == expr.op.right)
      return &(l->expr);

    prev = l->next;
  } while (prev != NULL);

  elist * added = (elist*)malloc(sizeof(elist));
  l->next = added;
  added->next = NULL;
  added->expr = expr;

  return &(added->expr);
}

expression * ht_add_expression(hash_table * table, expression * expr)
{
  switch (expr->type)
  {
    case variable:
      return ht_add_variable(table, expr->name);
    case constant:
      return ht_add_constant(table, expr->value);
    case binary:
      return ht_add_binary(table, expr->op.op, expr->op.left, expr->op.right);
    default:
      return NULL;
  }
}

void elist_print(elist * l)
{
  if (!l)
    printf("[]\n");
  else
  {
    printf("%p:", l);
    elist_print(l->next);
  }
}

void slist_print(slist * l)
{
  if (!l)
    printf("[]\n");
  else
  {
    printf("%p:", l);
    slist_print(l->next);
  }
}

void ht_print(hash_table * table)
{
  if (!table)
    return;

  int i;
  printf("hash_table->expr_size = %d\n", table->expr_size);
  printf("hash_table->name_size = %d\n", table->name_size);

  for (i = 0; i < table->expr_size; ++i)
  {
    printf("hash_table->expr_table[%d] = ", i);
    elist_print(table->expr_table[i]);
  }

  for (i = 0; i < table->name_size; ++i)
  {
    printf("hash_table->name_table[%d] = ", i);
    slist_print(table->name_table[i]);
  }
}

