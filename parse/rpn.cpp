#include <stack>
#include <string>
#include <iostream>
#include <new>
#include <vector>
#include <assert.h>
#include <math.h>
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
    Value mod(Value& other) noexcept;
};

struct Node {
    virtual void exec(std::stack<Value>& stack) noexcept = 0;
};

typedef Value (* SymOp)(void);
typedef Value (* SymBinop)(Value& lhs, Value& rhs);
typedef Value (*SymUnop)(Value& lhs);

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

static Value binop_none(Value& lhs, Value& rhs) noexcept;
static Value binop_add(Value& lhs, Value& rhs) noexcept;
static Value binop_sub(Value& lhs, Value& rhs) noexcept;
static Value binop_mul(Value& lhs, Value& rhs) noexcept;
static Value binop_div(Value& lhs, Value& rhs) noexcept;
static Value binop_mod(Value& lhs, Value& rhs) noexcept;
static bool op_isunary(SymOp op) noexcept;
static bool op_isbinary(SymOp op) noexcept;

static SymBinop binopTable[] = {
    binop_none,
    binop_add,
    binop_sub,
    binop_mul,
    binop_div,
    binop_mod,
    NULL,
};

static SymUnop unopTable[] = {
    NULL,
};


struct Rpn {
    std::stack<Value> stack;
    std::vector<Node *> nodes;

    Rpn() noexcept;
    ~Rpn() noexcept;
    void exec() noexcept;
    void push(char *value) noexcept;
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

void Rpn::push(char *value) noexcept {
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
        case '+': return (Node *) new (std::nothrow) SymNode((SymOp)binop_add);
        case '-': return (Node *) new (std::nothrow) SymNode((SymOp)binop_sub);
        case '*': return (Node *) new (std::nothrow) SymNode((SymOp)binop_mul);
        case '/': return (Node *) new (std::nothrow) SymNode((SymOp)binop_div);
        case '%': return (Node *) new (std::nothrow) SymNode((SymOp)binop_mod);
        }
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
    return (Node *) new (std::nothrow) SymNode((SymOp)binop_none);
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
        size_t size = stack.size();
        assert(size >= 1);
        Value lhs = stack.top();
        stack.pop();
        SymUnop op = (SymUnop)this->op;
        Value res = op(lhs);
        stack.push(res);
    }
    else {
        size_t size = stack.size();
        assert(size >= 2);
        Value rhs = stack.top();
        stack.pop();
        Value lhs = stack.top();
        stack.pop();
        SymBinop op = (SymBinop)this->op;
        Value res = op(lhs, rhs);
        stack.push(res);
    }
}

NumNode::NumNode(Value value) noexcept :
    value{value}
{
}

void NumNode::exec(std::stack<Value>& stack) noexcept {
    stack.push(this->value);
}

static Value binop_none(Value& lhs, Value& rhs) noexcept {
    return Value();
}

static Value binop_add(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    return lhs.add(rhs);
}

static Value binop_sub(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    return lhs.sub(rhs);
}

static Value binop_mul(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    return lhs.mul(rhs);
}

static Value binop_div(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    return lhs.div(rhs);
}

static Value binop_mod(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    return lhs.mod(rhs);
}

static bool op_isunary(SymOp op) noexcept {
    for (size_t i = 0; unopTable[i] != NULL; i++) {
        if (unopTable[i] == (SymUnop)op) {
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
    if (this->type == type) {
        return;
    }

    switch (type) {
    case TYPE_FLOAT: switch (this->type) {
        case TYPE_INT: // up convert
            this->number.f = (Float)this->number.i;
            break;
        case TYPE_UINT: // up convert
            this->number.f = (Float)this->number.u;
            break;
        }
        break;
    case TYPE_INT: switch (this->type) {
        case TYPE_FLOAT: // down convert
            this->number.i = (Int)this->number.f;
            break;
        case TYPE_UINT: // down convert
            this->number.i = (Int)this->number.u;
            break;
       }
       break;
    case TYPE_UINT: switch (this->type) {
        case TYPE_FLOAT: // down convert
            this->number.u = (Uint)this->number.f;
            break;
        case TYPE_INT: // up convert
            this->number.u = (Uint)this->number.i;
            break;
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
    switch (this->type) {
    case TYPE_FLOAT: return Value(this->number.f + other.number.f);
    case TYPE_INT:   return Value(this->number.i + other.number.i);
    case TYPE_UINT:  return Value(this->number.u + other.number.u);
    default: break;
    }
    assert(0);
    return Value();
}

Value Value::sub(Value& other) noexcept {
    assert(this->type == other.type);
    switch (this->type) {
    case TYPE_FLOAT: return Value(this->number.f - other.number.f);
    case TYPE_INT:   return Value(this->number.i - other.number.i);
    case TYPE_UINT:  return Value(this->number.u - other.number.u);
    default: break;
    }
    assert(0);
    return Value();
}

Value Value::mul(Value& other) noexcept {
    assert(this->type == other.type);
    switch (this->type) {
    case TYPE_FLOAT: return Value(this->number.f * other.number.f);
    case TYPE_INT:   return Value(this->number.i * other.number.i);
    case TYPE_UINT:  return Value(this->number.u * other.number.u);
    default: break;
    }
    assert(0);
    return Value();
}

Value Value::div(Value& other) noexcept {
    assert(this->type == other.type);
    switch (this->type) {
    case TYPE_FLOAT: return Value(this->number.f / other.number.f);
    case TYPE_INT:   return Value(this->number.i / other.number.i);
    case TYPE_UINT:  return Value(this->number.u / other.number.u);
    default: break;
    }
    assert(0);
    return Value();
}

Value Value::mod(Value& other) noexcept {
    assert(this->type == other.type);
    switch (this->type) {
    case TYPE_FLOAT: return Value((Float)fmod(this->number.f, other.number.f));
    case TYPE_INT:   return Value(this->number.i % other.number.i);
    case TYPE_UINT:  return Value(this->number.u % other.number.u);
    default: break;
    }
    assert(0);
    return Value();
}

