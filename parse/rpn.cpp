#include <stack>
#include <string>
#include <iostream>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "rpn.hpp"

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
    double f;
    int64_t i;
    uint64_t u;
};

struct Value {
    Number number;
    Type type;
};

static Value ValueNew(void) noexcept;
static Value ValueNewDouble(double f) noexcept;
static Value ValueNewInt(int64_t i) noexcept;
static Value ValueNewUint(uint64_t u) noexcept;
static Value ValueAdd(Value& self, Value& other) noexcept;
static Value ValueSub(Value& self, Value& other) noexcept;
static Value ValueDiv(Value& self, Value& other) noexcept;
static Value ValueMult(Value& self, Value& other) noexcept;

struct SymNode {
    Node parent;

    Symbol value;
};

struct NumNode {
    Node parent;

    Value value;
};

static SymNode *SymNodeNew(Symbol value) noexcept;
static void SymNodeExec(SymNode *self) noexcept;
static NumNode *NumNodeNew(Value value) noexcept;
static void NumNodeExec(NumNode *self) noexcept;

typedef void (* SymOp)();
static void op_none() noexcept;
static void op_add() noexcept;
static void op_sub() noexcept;
static void op_mult() noexcept;
static void op_div() noexcept;

static std::stack<Value> stack;
static SymOp opTable[SYM_COUNT] = {
    op_none,
    op_add,
    op_sub,
    op_mult,
    op_div,
};

Node *rpn_new(char *value) noexcept {
    assert(value);
    assert(value[0] != 0);

    // floating point
    if (strchr(value, '.')) {
        double number;
        if (sscanf(value, "%lf", &number) == 1) {
            return (Node *)NumNodeNew(ValueNewDouble(number));
        }
        else {
            assert(0);
        }
    }

    // one char ops
    else if (value[1] == 0) {
        switch (value[0]) {
        case '+': return (Node *)SymNodeNew(SYM_ADD);
        case '-': return (Node *)SymNodeNew(SYM_SUB);
        case '*': return (Node *)SymNodeNew(SYM_MULT);
        case '/': return (Node *)SymNodeNew(SYM_DIV);
        }
        assert(0);
    }

    // two char ops
    else if (value[2] == 0) {
        // ...
    }

    // signed
    if (strchr(value, '-') || strchr(value, '+')) {
        int64_t number;
        if (sscanf(value, "%zi", &number) == 1) {
            return (Node *)NumNodeNew(ValueNewInt(number));
        }
        else {
            assert(0);
        }
    }
    // unsigned
    else {
        uint64_t number;
        if (sscanf(value, "%zu", &number) == 1) {
            return (Node *)NumNodeNew(ValueNewUint(number));
        }
        else {
            assert(0);
        }
    }

    assert(0);
    return (Node *)SymNodeNew(SYM_NONE);
}

void rpn_free(Node *self) noexcept {
    assert(self);
    free(self);
}

void rpn_exec(void) noexcept {
    // TODO: add to a vector, manage all here...
}

void rpn_print() noexcept {
    assert(stack.size() >= 1);
    Value& value = stack.top();
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

SymNode *SymNodeNew(Symbol value) noexcept {
    SymNode *self = (SymNode *)malloc(sizeof(SymNode));
    assert(self);
    self->value = value;
    self->parent.exec = (void (*)(Node *))SymNodeExec;
    return self;
}

void SymNodeExec(SymNode *self) noexcept {
    assert(stack.size() >= 1);
    opTable[self->value]();
}

NumNode *NumNodeNew(Value value) noexcept {
    NumNode *self = (NumNode *)malloc(sizeof(NumNode));
    assert(self);
    self->value = value;
    self->parent.exec = (void (*)(Node *))NumNodeExec;
    return self;
}

void NumNodeExec(NumNode *self) noexcept {
    stack.push(self->value);
}

static void op_none() noexcept {
    return;
}

static void op_add() noexcept {
    size_t size = stack.size();
    assert(size >= 2);

    Value rhs = stack.top();
    stack.pop();
    Value lhs = stack.top();
    stack.pop();
    stack.push(ValueAdd(lhs, rhs));
}

static void op_sub() noexcept {
    size_t size = stack.size();
    assert(size >= 2);

    Value rhs = stack.top();
    stack.pop();
    Value lhs = stack.top();
    stack.pop();
    stack.push(ValueSub(lhs, rhs));
}

static void op_mult() noexcept {
    assert(stack.size() >= 1);
}

static void op_div() noexcept {
    assert(stack.size() >= 2);
}


static Value ValueNew() noexcept {
    Value self;
    self.number.i = 0;
    self.type = TYPE_UINT;
    return self;
}

static Value ValueNewDouble(double f) noexcept {
    Value self;
    self.number.f = f;
    self.type = TYPE_FLOAT;
    return self;
}

static Value ValueNewInt(int64_t i) noexcept {
    Value self;
    self.number.i = i;
    self.type = TYPE_INT;
    return self;
}

static Value ValueNewUint(uint64_t u) noexcept {
    Value self;
    self.number.u = u;
    self.type = TYPE_UINT;
    return self;
}

static Value ValueAdd(Value& self, Value& other) noexcept {
    switch (self.type) {
    case TYPE_FLOAT: switch (other.type) {
        case TYPE_FLOAT: return ValueNewDouble(self.number.f + other.number.f);
        case TYPE_INT:   return ValueNewDouble(self.number.f + (double)other.number.i);
        case TYPE_UINT:  return ValueNewDouble(self.number.f + (double)other.number.u);
    }
    assert(0);
    break;
    case TYPE_INT: switch (other.type) {
        case TYPE_FLOAT: return ValueNewInt(self.number.i + (int64_t)other.number.f);
        case TYPE_INT:   return ValueNewInt(self.number.i + other.number.i);
        case TYPE_UINT:  return ValueNewInt(self.number.i + (int64_t)other.number.u);
    }
    assert(0);
    break;
    case TYPE_UINT: switch (other.type) {
        case TYPE_FLOAT: return ValueNewUint(self.number.u + (uint64_t)other.number.f);
        case TYPE_INT:   return ValueNewUint(self.number.u + (uint64_t)other.number.i);
        case TYPE_UINT:  return ValueNewUint(self.number.u + other.number.u);
    }
    assert(0);
    break;
    }
    assert(0);
    return ValueNew();
}

static Value ValueSub(Value& self, Value& other) noexcept {
    switch (self.type) {
    case TYPE_FLOAT: switch (other.type) {
        case TYPE_FLOAT: return ValueNewDouble(self.number.f - other.number.f);
        case TYPE_INT:   return ValueNewDouble(self.number.f - (double)other.number.i);
        case TYPE_UINT:  return ValueNewDouble(self.number.f - (double)other.number.u);
    }
    assert(0);
    break;
    case TYPE_INT: switch (other.type) {
        case TYPE_FLOAT: return ValueNewInt(self.number.i - (int64_t)other.number.f);
        case TYPE_INT:   return ValueNewInt(self.number.i - other.number.i);
        case TYPE_UINT:  return ValueNewInt(self.number.i - (int64_t)other.number.u);
    }
    assert(0);
    break;
    case TYPE_UINT: switch (other.type) {
        case TYPE_FLOAT: return ValueNewUint(self.number.u - (uint64_t)other.number.f);
        case TYPE_INT:   return ValueNewUint(self.number.u - (uint64_t)other.number.i);
        case TYPE_UINT:  return ValueNewUint(self.number.u - other.number.u);
    }
    assert(0);
    break;
    }
    assert(0);
    return ValueNew();
}

static Value ValueMult(Value& self, Value& other) noexcept {

}

static Value ValueDiv(Value& self, Value& other) noexcept {

}
