#include <stack>
#include <string>
#include <iostream>
#include <new>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "rpn.hpp"

#ifdef USE_32BITS
typedef float Float;
typedef int32_t Int;
typedef uint32_t Uint;
#define FMT_FLOAT "%f"
#define FMT_INT "%i"
#define FMT_UINT "%u"
#else
typedef double Float;
typedef int64_t Int;
typedef uint64_t Uint;
#define FMT_FLOAT "%lf"
#define FMT_INT "%zi"
#define FMT_UINT "%zu"
#endif

enum Symbol {
    SYM_NONE,
    SYM_ADD,
    SYM_SUB,
    SYM_MULT,
    SYM_DIV,
    SYM_COUNT,
};

enum Type {
    TYPE_FLOAT,
    TYPE_UINT,
    TYPE_INT,
};

union Number {
    Float f;
    Int i;
    Uint u;
};

//struct Number {
//    virtual void add(Number& other) = 0;
//    virtual void add(Number& other) = 0;
//    virtual void add(Number& other) = 0;
//
//    virtual void sub(Number& other) = 0;
//    virtual void div(Number& other) = 0;
//    virtual void mul(Number& other) = 0;
//};
//
//struct NumberFloat : public Number {
//    Float value;
//    NumberFloat add(Float value) override;
//    NumberFloat add(Int value) override;
//    NumberFloat add(Uint value) override;
//
//    void sub(Number& other) override;
//    void div(Number& other) override;
//    void mul(Number& other) override;
//};

struct Num {

};

struct Value {
    Number number;
    Type type;

    Value() noexcept;
    Value(Float number) noexcept;
    Value(Int number) noexcept;
    Value(Uint number) noexcept;

    Value add(Value& other) noexcept;
    Value sub(Value& other) noexcept;
    Value div(Value& other) noexcept;
    Value mul(Value& other) noexcept;
};

struct Node {
    virtual void exec(std::stack<Value>& stack) noexcept = 0;
};

struct SymNode : public Node {
    Symbol value;

    SymNode(Symbol value) noexcept;
    void exec(std::stack<Value>& stack) noexcept override;
};

struct NumNode : public Node {
    Value value;

    NumNode(Value value) noexcept;
    void exec(std::stack<Value>& stack) noexcept override;
};

static Node *node_new(char *value) noexcept;
static void node_free(Node *self) noexcept;

typedef void (* SymOp)(std::stack<Value>& stack);
static void op_none(std::stack<Value>& stack) noexcept;
static void op_add(std::stack<Value>& stack) noexcept;
static void op_sub(std::stack<Value>& stack) noexcept;
static void op_mult(std::stack<Value>& stack) noexcept;
static void op_div(std::stack<Value>& stack) noexcept;

static SymOp opTable[SYM_COUNT] = {
    op_none,
    op_add,
    op_sub,
    op_mult,
    op_div,
};


struct Rpn {
    std::stack<Value> stack;
    std::vector<Node *> nodes;

    Rpn() noexcept;
    ~Rpn() noexcept;
    void exec() noexcept;
    void push(char *value);
};

Rpn::Rpn() noexcept :
    stack{},
    nodes{}
{
}

Rpn::~Rpn() noexcept {
    for (Node *n : this->nodes) {
        node_free(n);
    }
}

void Rpn::exec() noexcept {
    for (Node *n : this->nodes) {
        n->exec(this->stack);
    }
}

void Rpn::push(char *value) {
    Node *n = node_new(value);
    assert(n);
    this->nodes.push_back(n);
}



Rpn *rpn_new() noexcept {
    Rpn *self = new (std::nothrow) Rpn{};
    assert(self);
    return self;
}

void rpn_exec(Rpn *self) noexcept {
    assert(self);
    self->exec();
}

void rpn_push(Rpn *self, char *value) noexcept {
    assert(self);
    assert(value);
    self->push(value);
}

void rpn_print(Rpn *self) noexcept {
    assert(self);
    assert(self->stack.size() >= 1);

    Value& value = self->stack.top();

    switch (value.type) {
    case TYPE_FLOAT:
        std::cout << value.number.f << std::endl;
        break;
    case TYPE_INT:
        std::cout << value.number.u << std::endl;
        break;
    case TYPE_UINT:
        std::cout << value.number.i << std::endl;
        break;
    }
}

void rpn_free(Rpn *self) noexcept {
    assert(self);
    delete self;
}



static Node *node_new(char *value) noexcept {
    assert(value);
    assert(value[0] != 0);

    // floating point
    if (strchr(value, '.')) {
        Float number;
        if (sscanf(value, FMT_FLOAT, &number) == 1) {
            return (Node *) new (std::nothrow) NumNode(Value(number));
        }
        else {
            assert(0);
        }
    }

    // one char ops
    else if (value[1] == 0) {
        switch (value[0]) {
        case '+': return (Node *) new (std::nothrow) SymNode(SYM_ADD);
        case '-': return (Node *) new (std::nothrow) SymNode(SYM_SUB);
        case '*': return (Node *) new (std::nothrow) SymNode(SYM_MULT);
        case '/': return (Node *) new (std::nothrow) SymNode(SYM_DIV);
        }
        assert(0);
    }

    // two char ops
    else if (value[2] == 0) {
        // ...
    }

    // signed
    if (strchr(value, '-') || strchr(value, '+')) {
        Int number;
        if (sscanf(value, FMT_INT, &number) == 1) {
            return (Node *) new (std::nothrow) NumNode(Value(number));
        }
        else {
            assert(0);
        }
    }
    // unsigned
    else {
        Uint number;
        if (sscanf(value, FMT_UINT, &number) == 1) {
            return (Node *) new (std::nothrow) NumNode(Value(number));
        }
        else {
            assert(0);
        }
    }

    assert(0);
    return (Node *) new (std::nothrow) SymNode(SYM_NONE);
}

static void node_free(Node *self) noexcept {
    assert(self);
    delete self;
}

SymNode::SymNode(Symbol value) noexcept :
    value{value}
{
}

void SymNode::exec(std::stack<Value>& stack) noexcept {
    assert(stack.size() >= 1);
    opTable[this->value](stack);
}

NumNode::NumNode(Value value) noexcept :
    value{value}
{
}

void NumNode::exec(std::stack<Value>& stack) noexcept {
    stack.push(this->value);
}

static void op_none(std::stack<Value>& stack) noexcept {
    return;
}

static void op_add(std::stack<Value>& stack) noexcept {
    size_t size = stack.size();
    assert(size >= 2);

    Value rhs = stack.top();
    stack.pop();
    Value lhs = stack.top();
    stack.pop();
    stack.push(lhs.add(rhs));
}

static void op_sub(std::stack<Value>& stack) noexcept {
    size_t size = stack.size();
    assert(size >= 2);

    Value rhs = stack.top();
    stack.pop();
    Value lhs = stack.top();
    stack.pop();
    stack.push(lhs.sub(rhs));
}

static void op_mult(std::stack<Value>& stack) noexcept {
    assert(stack.size() >= 1);
}

static void op_div(std::stack<Value>& stack) noexcept {
    assert(stack.size() >= 2);
}


Value::Value() noexcept :
    number{0},
    type{TYPE_UINT}
{
}

Value::Value(Float f) noexcept :
    number{0},
    type{TYPE_FLOAT}
{
    number.f = f;
}

Value::Value(Int i) noexcept :
    number{0},
    type{TYPE_INT}
{
    number.i = i;
}

Value::Value(Uint u) noexcept :
    number{0},
    type{TYPE_UINT}
{
    number.u = u;
}

Value Value::add(Value& other) noexcept {
    switch (this->type) {
    case TYPE_FLOAT: switch (other.type) {
        case TYPE_FLOAT: return Value(this->number.f + other.number.f);
        case TYPE_INT:   return Value(this->number.f + (Float)other.number.i);
        case TYPE_UINT:  return Value(this->number.f + (Float)other.number.u);
    }
    assert(0);
    break;
    case TYPE_INT: switch (other.type) {
        case TYPE_FLOAT: return Value(this->number.f + other.number.f);
        case TYPE_INT:   return Value(this->number.i + other.number.i);
        case TYPE_UINT:  return Value(this->number.u + (Int)other.number.u);
    }
    assert(0);
    break;
    case TYPE_UINT: switch (other.type) {
        case TYPE_FLOAT: return Value(this->number.f + (Float)other.number.f);
        case TYPE_INT:   return Value(this->number.u + (Uint)other.number.i);
        case TYPE_UINT:  return Value(this->number.u + other.number.u);
    }
    assert(0);
    break;
    }
    assert(0);
    return Value();
}

Value Value::sub(Value& other) noexcept {
    switch (this->type) {
    case TYPE_FLOAT: switch (other.type) {
        case TYPE_FLOAT: return Value(this->number.f - other.number.f);
        case TYPE_INT:   return Value(this->number.f - (Float)other.number.i);
        case TYPE_UINT:  return Value(this->number.f - (Float)other.number.u);
    }
    assert(0);
    break;
    case TYPE_INT: switch (other.type) {
        case TYPE_FLOAT: return Value(this->number.f - other.number.f);
        case TYPE_INT:   return Value(this->number.i - other.number.i);
        case TYPE_UINT:  return Value(this->number.u - (Int)other.number.u);
    }
    assert(0);
    break;
    case TYPE_UINT: switch (other.type) {
        case TYPE_FLOAT: return Value(this->number.f - (Float)other.number.f);
        case TYPE_INT:   return Value(this->number.u - (Uint)other.number.i);
        case TYPE_UINT:  return Value(this->number.u - other.number.u);
    }
    assert(0);
    break;
    }
    assert(0);
    return Value();
}

Value Value::mul(Value& other) noexcept {

}

Value Value::div(Value& other) noexcept {

}
