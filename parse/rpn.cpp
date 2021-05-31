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
    SYM_COUNT
};

// order of precedence, lowest to highest
enum Type {
    TYPE_INT,
    TYPE_UINT,
    TYPE_FLOAT,
    TYPE_COUNT
};

union Number {
    Int i;
    Uint u;
    Float f;
};

struct Value {
    Number number;
    Type type;

    Value() noexcept;
    Value(Int number) noexcept;
    Value(Uint number) noexcept;
    Value(Float number) noexcept;

    void print() noexcept;
    enum Type coerce_chk(Value& other) noexcept; // greatest precedence
    void coerce_exec(enum Type type) noexcept; // cast to the type
    void coerce(Value& other) noexcept;

    Value add(Value& other) noexcept;
    Value sub(Value& other) noexcept;
    Value div(Value& other) noexcept;
    Value mul(Value& other) noexcept;
};

// assume This and Other are the same type
#define value_oneop(ThisP, Other, Op) \
    do { \
        switch (ThisP->type) { \
        case TYPE_FLOAT: return Value(this->number.f Op other.number.f); \
        case TYPE_INT:   return Value(this->number.i Op other.number.i); \
        case TYPE_UINT:  return Value(this->number.u Op other.number.u); \
        default: \
            assert(0); \
            break; \
        } \
    } while (0)

struct Node {
    virtual void exec(std::stack<Value>& stack) noexcept = 0;
};

typedef void (* SymOp)(std::stack<Value>& stack);

struct SymNode : public Node {
    SymOp op;

    SymNode(SymOp op) noexcept;
    void exec(std::stack<Value>& stack) noexcept override;
};

struct NumNode : public Node {
    Value value;

    NumNode(Value value) noexcept;
    void exec(std::stack<Value>& stack) noexcept override;
};

static Node *node_new(char *value) noexcept;
static void node_free(Node *self) noexcept;

static void op_none(std::stack<Value>& stack) noexcept;
static void op_add(std::stack<Value>& stack) noexcept;
static void op_sub(std::stack<Value>& stack) noexcept;
static void op_mul(std::stack<Value>& stack) noexcept;
static void op_div(std::stack<Value>& stack) noexcept;
static bool op_isunary(SymOp op) noexcept;
static bool op_isbinary(SymOp op) noexcept;

static SymOp binopTable[] = {
    op_none,
    op_add,
    op_sub,
    op_mul,
    op_div,
    NULL,
};

static SymOp unopTable[] = {
    NULL,
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
    value.print();
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
        case '+': return (Node *) new (std::nothrow) SymNode(op_add);
        case '-': return (Node *) new (std::nothrow) SymNode(op_sub);
        case '*': return (Node *) new (std::nothrow) SymNode(op_mul);
        case '/': return (Node *) new (std::nothrow) SymNode(op_div);
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
    return (Node *) new (std::nothrow) SymNode(op_none);
}

static void node_free(Node *self) noexcept {
    assert(self);
    delete self;
}

SymNode::SymNode(SymOp op) noexcept :
    op{op}
{
}

void SymNode::exec(std::stack<Value>& stack) noexcept {
    assert(stack.size() >= 1);

    if (op_isunary(this->op)) {

    }
    else {
        
    }

    this->op(stack);

    binopTable[this->value](stack);
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
    lhs.coerce(rhs);
    stack.push(lhs.add(rhs));
}

static void op_sub(std::stack<Value>& stack) noexcept {
    size_t size = stack.size();
    assert(size >= 2);

    Value rhs = stack.top();
    stack.pop();
    Value lhs = stack.top();
    stack.pop();
    lhs.coerce(rhs);
    stack.push(lhs.sub(rhs));
}

static void op_mul(std::stack<Value>& stack) noexcept {
    size_t size = stack.size();
    assert(size >= 2);

    Value rhs = stack.top();
    stack.pop();
    Value lhs = stack.top();
    stack.pop();
    lhs.coerce(rhs);
    stack.push(lhs.mul(rhs));
}

static void op_div(std::stack<Value>& stack) noexcept {
    size_t size = stack.size();
    assert(size >= 2);

    Value rhs = stack.top();
    stack.pop();
    Value lhs = stack.top();
    stack.pop();
    lhs.coerce(rhs);
    stack.push(lhs.div(rhs));
}

static bool op_isunary(SymOp op) noexcept {
    for (size_t i = 0; unopTable[i] != NULL; i++) {
        if (unopTable[i] == op) {
            return true;
        }
    }
    return false;
}

static bool op_isbinary(SymOp op) noexcept {
    return !op_isunary(op);
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

void Value::print() noexcept {
    switch (this->type) {
    case TYPE_FLOAT:
        std::cout << this->number.f << std::endl;
        break;
    case TYPE_INT:
        std::cout << this->number.u << std::endl;
        break;
    case TYPE_UINT:
        std::cout << this->number.i << std::endl;
        break;
    }
}

enum Type Value::coerce_chk(Value& other) noexcept {
    if (this->type < other.type) {
        return other.type;
    }
    return this->type;
}

void Value::coerce_exec(enum Type type) noexcept {
    if (this->type >= type) {
        return;
    }

    switch (type) {
    case TYPE_FLOAT: switch (this->type) {
        case TYPE_INT: this->number.f = (Float)this->number.i; break;
        case TYPE_UINT: this->number.f = (Float)this->number.u; break;
        }
        break;
    case TYPE_INT: switch (this->type) {
        case TYPE_FLOAT: this->number.i = (Int)this->number.f; break;
        case TYPE_UINT: this->number.i = (Int)this->number.u; break;
        }
        break;
    case TYPE_UINT: switch (this->type) {
        case TYPE_FLOAT: this->number.u = (Uint)this->number.f; break;
        case TYPE_INT: this->number.u = (Uint)this->number.i; break;
        }
        break;
    default:
        assert(0);
        break;
    }
    this->type = type;
}

void Value::coerce(Value& other) noexcept {
    this->coerce_exec(this->coerce_chk(other));
    other.coerce_exec(other.coerce_chk(*this));
}

Value Value::add(Value& other) noexcept {
    assert(this->type == other.type);
    value_oneop(this, other, +);
    assert(0);
    return Value();
}

Value Value::sub(Value& other) noexcept {
    assert(this->type == other.type);
    value_oneop(this, other, -);
    assert(0);
    return Value();
}

Value Value::mul(Value& other) noexcept {
    assert(this->type == other.type);
    value_oneop(this, other, *);
    assert(0);
    return Value();
}

Value Value::div(Value& other) noexcept {
    assert(this->type == other.type);
    value_oneop(this, other, /);
    assert(0);
    return Value();
}
