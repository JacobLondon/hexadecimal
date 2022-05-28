#pragma once

namespace Rpn {

#include <stack>
#include <vector>

struct Value {
    enum {
        VAL_NONE = 0x00,
        VAL_BIN  = 0x01, // Value(Value, Value)
        VAL_UN   = 0x03,  // Value(Value)
        VAL_LIT  = 0x80, // Value
    } t;

    typedef Value (* ValueBinop)(Value, Value);
    typedef Value (* ValueUnop)(Value);

    union {
        ValueBinop binop;
        ValueUnop unop;
        void *ptr;
    } v;

    Value();
    Value(ValueBinop binop);
    Value(ValueUnop unop);
    Value(void *ptr);
};


struct Calc {
public:
    Calc();

    void push(Value v);
    int process();

private:
    std::stack<Value> m_stack;
    std::vector<Value> m_instructions;
};

}
