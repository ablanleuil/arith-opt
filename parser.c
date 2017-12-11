#include "parser.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

enum ttype { tvariable, tconstant, toperator, tleftpar, trightpar };

typedef struct token
{
  int type;
  union
  {
    char * name;
    float value;
    char op;
  };
} token;

typedef struct token_list
{
  token tok;
  struct token_list * next;
} token_list;

token lex_variable(char ** strp)
{
  char * str = *strp;

  while (isalpha(*(++str)));

  token ret;
  ret.type = tvariable;
  ret.name = (char*)malloc(str - *strp + 1);
  strncpy(ret.name, *strp, str - *strp);
  ret.name[str - *strp] = 0;

  *strp = str;

  return ret;
}

token lex_constant(char ** strp)
{
  char * str = *strp;
  char * tmp;

  while (*str == '.' || isdigit(*str))
    ++str;

  tmp = (char*)malloc((str - *strp) + 1);
  strncpy(tmp, *strp, str - *strp);
  tmp[*strp - str] = 0;

  token ret;
  ret.type = tconstant;
  ret.value = atof(tmp);

  *strp = str;
  free(tmp);

  return ret;
}

void skip_spaces(char ** strp)
{
  char * str = *strp;
  while (isspace(*(++str)));
  *strp = str;
}

token_list * lex(char * str)
{
  token_list * ret = NULL;
  token_list * l = NULL;
  while (*str)
  {
    token_list * n = (token_list*)malloc(sizeof(token_list));
    n->next = NULL;
    if (isalpha(*str))
    {
      n->tok = lex_variable(&str);
    }
    else if (isdigit(*str))
    {
      n->tok = lex_constant(&str);
    }
    else if (*str == '(')
    {
      token tok; tok.type = tleftpar; n->tok = tok; ++str;
    }
    else if (*str == ')')
    {
      token tok; tok.type = trightpar; n->tok = tok; ++str;
    }
    else if (isspace(*str))
    {
      free(n); skip_spaces(&str);
    }
    else if (*str == '+' || *str == '-' || *str == '*' || *str == '/')
    {
      token tok; tok.type = toperator; tok.op = *(str++); n->tok = tok;
    }
    else
    {
      return l;
    }

    l = l ? (l->next = n) : (ret = n);
  }

  return ret;
}

expression * parse_E(token_list ** l);
expression * parse_T(token_list ** l);

expression * parse_F(token_list ** l)
{
  if (!(*l))
  {
    printf("Parse error\n");
    exit(-1);
  }

  switch ((*l)->tok.type)
  {
    case tleftpar:
    {
      *l = (*l)->next;
      expression * ret = parse_E(l);
      *l = (*l)->next;
      return ret;
    }
    case tvariable:
    {
      expression * ret = new_variable((*l)->tok.name);
      free((*l)->tok.name);
      *l = (*l)->next;
      return ret;
    }
    case tconstant:
    {
      float f = (*l)->tok.value;
      *l = (*l)->next;
      return new_constant(f);
    }
    default:
      printf("Parse error\n");
      exit(-1);
  }
}

expression * parse_T2(expression * left, token_list ** l)
{
  if ((*l) && (*l)->tok.type == toperator)
  {
    char op = (*l)->tok.op;
    switch (op)
    {
      case '*':
      case '/':
        *l = (*l)->next;
        return parse_T2(new_binary(op, left, parse_F(l)), l);
      default: return left;
    }
  }
  else
    return left;
}

expression * parse_T(token_list ** l)
{
  expression * ret = parse_T2(parse_F(l), l);
  return ret;
}

expression * parse_E2(expression * left, token_list ** l)
{
  if ((*l) && (*l)->tok.type == toperator)
  {
    char op = (*l)->tok.op;
    switch (op)
    {
      case '+':
      case '-':
        *l = (*l)->next;
        return parse_E2(new_binary(op, left, parse_T(l)), l);
      default: return left;
    }
  }
  else
    return left;
}

expression * parse_E(token_list ** l)
{
  expression * ret = parse_E2(parse_T(l), l);
  return ret;
}

expression * parse_(token_list * l)
{
  expression * ret = parse_E(&l);
  return ret;
}

expression * parse(const char * str)
{
  return parse_(lex((char*)str));
}

