#include <stdio.h>

#include "rpn.hpp"

namespace Rpn {

Value::Value() :
t{VAL_NONE},
v{nullptr}
{
}

Value::Value(ValueBinop binop) :
t{Value::VAL_BIN},
v{.binop=binop}
{
}

Value::Value(ValueUnop unop) :
t{Value::VAL_UN},
v{.unop=unop}
{
}

Value::Value(void *ptr) :
t{Value::VAL_LIT},
v{.ptr=ptr}
{
}

Calc::Calc()
{
}

void Calc::push(Value v)
{
    m_instructions.push_back(v);
}

int Calc::process()
{
    for (Value &value : m_instructions)
    {
        if (value.t == Value::VAL_BIN)
        {
            if (m_stack.size() < 2)
            {
                fprintf(stderr, "rpn: binop not enough values on the stack\n");
                return 1;
            }

            ValueBinop op = value.v.binop;
            Value rhs = m_stack.top();
            m_stack.pop();
            Value lhs = m_stack.top();
            m_stack.pop();

            Value result = op(lhs, rhs);
            if (result.t != Value::VAL_NONE)
            {
                m_stack.push(result);
            }
        }
        else if (value.t == Value::VAL_UN)
        {
            if (m_stack.size() < 1)
            {
                fprintf(stderr, "rpn: unop not enough values on the stack\n");
                return 1;
            }

            ValueUnop op = value.v.unop;
            Value v = m_stack.top();
            m_stack.pop();

            Value result = op(v);
            if (result.t != Value::VAL_NONE)
            {
                m_stack.push(result);
            }
        }
        else if (value.t == Value::VAL_LIT)
        {
            m_stack.push(value);
        }
        else // VAL_NONE
        {
            continue;
        }
    }

    return 0;
}

}
