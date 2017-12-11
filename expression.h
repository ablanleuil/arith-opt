#ifndef EXPRESSION_H
#define EXPRESSION_H

typedef struct hash_table hash_table;

enum { opadd = '+', opsub = '-', opmul = '*', opdiv = '/' };
enum { variable = 0, binary = 1, constant = 2 };
typedef struct expression expression;
typedef struct binop
{
  char op;
  expression * left;
  expression * right;
} binop;

struct expression
{
  char type;
  union
  {
    char * name;
    float value;
    binop op;
  };
};

void expr_setup_ht(int expr_size, int name_size);
void expr_free_ht();
hash_table * expr_get_ht();
expression * new_variable(const char * name);
expression * new_constant(float value);
expression * new_binary(char op, expression * left, expression * right);
int expression_compare(expression * l, expression * r);
void expr_print(expression * expr);

#endif // EXPRESSION_H
