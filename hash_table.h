#ifndef HASH_TABLE_H
#define HASH_TABLE_H

typedef struct expression expression;
typedef struct hash_table hash_table;

hash_table * ht_create(int expr_size, int name_size);
void ht_destroy(hash_table * table);
expression * ht_add_variable(hash_table * table, const char * name);
expression * ht_add_constant(hash_table * table, float value);
expression * ht_add_binary
  ( hash_table * table
  , char op
  , expression * left
  , expression * right);
expression * ht_add_expression(hash_table * table, expression * expr);

void ht_print(hash_table * table);

#endif // HASH_TABLE_H
