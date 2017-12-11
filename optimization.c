#include "optimization.h"

expression * constant_propagation(expression * e)
{
  switch (e->type)
  {
    case variable:
    case constant:
      return e;
    case binary:
    {
      expression * l = constant_propagation(e->op.left);
      expression * r = constant_propagation(e->op.right);

      // the constant propagation yielded 2 constants
      if (l->type == constant && r->type == constant)
      {
        switch (e->op.op)
        {
          case opadd: return new_constant(l->value + r->value);
          case opsub: return new_constant(l->value - r->value);
          case opmul: return new_constant(l->value * r->value);
          case opdiv: return new_constant(l->value / r->value);
        }
      }

      // pattern: (op1 (op2 X constant) constant) works with + * -
      if (l->type == binary && l->op.right->type == constant
          && r->type == constant)
      {
        switch (e->op.op)
        {
          case opadd:
          {
            switch (l->op.op)
            {
              // (+ (+ _ _) _)
              case opadd: return new_binary(opadd, l->op.left,
                            new_constant(l->op.right->value + r->value));
              // (+ (- _ _) _)
              case opsub: return new_binary(opadd, l->op.left,
                            new_constant(r->value - l->op.right->value));
              default: break;
            }
            break;
          }
          case opsub:
          {
            switch (l->op.op)
            {
              // (- (+ _ _) _)
              case opadd: return new_binary(opadd, l->op.left,
                            new_constant(l->op.right->value - r->value));
              // (- (- _ _) _)
              case opsub: return new_binary(opsub, l->op.left,
                            new_constant(r->value + l->op.right->value));
              default: break;
            }
            break;
          }
          case opmul:
          {
            switch (l->op.op)
            {
              // (* (* _ _) _)
              case opmul: return new_binary(opmul, l->op.left,
                            new_constant(l->op.right->value * r->value));
              // (* (/ _ _) _)
              case opdiv: return new_binary(opmul, l->op.left,
                            new_constant(r->value / l->op.right->value));
              default: break;
            }
            break;
          }
          case opdiv:
           {
            switch (l->op.op)
            {
              // (/ (* _ _) _)
              case opmul: return new_binary(opmul, l->op.left,
                            new_constant(l->op.right->value / r->value));
              // (/ (/ _ _) _)
              case opdiv: return new_binary(opdiv, l->op.left,
                            new_constant(r->value * l->op.right->value));
              default: break;
            }
            break;
          }
          
          return new_binary(opdiv, l->op.left,
                          new_constant(l->op.right->value * r->value));
        }
      }

      return new_binary(e->op.op, l, r);
    }
  }
}

expression * expression_distribution(expression * e)
{
  switch (e->type)
  {
    case variable:
    case constant:
      return e;
    case binary:
    {
      expression * l = expression_distribution(e->op.left);
      expression * r = expression_distribution(e->op.right);
      if (l->type == binary && (l->op.op == opadd || l->op.op == opsub)
          && r->type == constant)
      {
          if (e->op.op == opmul || e->op.op == opdiv)
            return new_binary(l->op.op, new_binary(e->op.op, l->op.left, r),
                new_binary(e->op.op, l->op.right, r));
      }

      return new_binary(e->op.op, l, r);
    }
  }
}

