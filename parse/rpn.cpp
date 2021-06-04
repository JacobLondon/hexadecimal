#include <stack>
#include <string>
#include <iostream>
#include <new>
#include <vector>
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "rpn.hpp"
#include "math.hpp"

#ifdef USE_32BITS
typedef float Float;
typedef int32_t Int;
typedef uint32_t Uint;
#define FMT_FLOAT "%f"
#define FMT_INT "%i"
#define FMT_UINT "%u"
#define FMT_HEX "%X"
#define FMT_OCT "%o"
#define FLOAT_MOD(...) fmodf(__VA_ARGS__)
#define FLOAT_POW(...) powf(__VA_ARGS__)
#define FLOAT_SQRT(...) fsqrtf(__VA_ARGS__)
#define FLOAT_ZERO 0.0f
#else
typedef double Float;
typedef int64_t Int;
typedef uint64_t Uint;
#define FMT_FLOAT "%lf"
#define FMT_INT "%li"
#define FMT_UINT "%lu"
#define FMT_HEX "%lX"
#define FMT_OCT "%lo"
#define FLOAT_MOD(...) fmod(__VA_ARGS__)
#define FLOAT_POW(...) pow(__VA_ARGS__)
#define FLOAT_SQRT(...) sqrtf(__VA_ARGS__)
#define FLOAT_ZERO 0.0
#endif
#define FMT_STRING "%s"

// order of precedence, lowest to highest
enum Type {
    TYPE_INT,
    TYPE_UINT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_COUNT
};

static const char *typeTable[] = {
    "Int",
    "Uint",
    "Float",
    "String",
    "",
};

enum Format {
    FORMAT_DEC,
    FORMAT_HEX,
    FORMAT_OCT,
    FORMAT_BIN,
    //FORMAT_BIG,
    //FORMAT_LITTLE,
    FORMAT_COUNT
};

static const char *formatTable[] = {
    "Dec",
    "Hex",
    "Oct",
    "Bin",
    //"Big", // endianness
    //"Little",
    "",
};

union Number {
    Int i;
    Uint u;
    Float f;
    const char *s;
};

struct Value {
    Number number;
    Type type;
    Format fmt;

    Value() noexcept;
    Value(Int number) noexcept;
    Value(Uint number) noexcept;
    Value(Float number) noexcept;
    Value(const char *value) noexcept;

    void print() noexcept;
    void println() noexcept;
    enum Type coerce_chk(Value& other) noexcept; // greatest precedence
    void coerce_exec(enum Type type) noexcept; // cast to the type
    void coerce(Value& other) noexcept;
    void pun(enum Type type) noexcept; // pun to the type
    void format(enum Format format) noexcept; // change the fmt
    Value unexpected_type(void) noexcept;
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

static bool is_signed(const char *value) noexcept;
static bool is_unsigned(const char *value) noexcept;
static bool conve_binary(const char *value, Uint *out) noexcept;
static void print_binary(Uint value) noexcept;

static Node *node_new(char *value) noexcept;
static void node_free(Node *self) noexcept;

static Value binop_none(Value& lhs, Value& rhs) noexcept;
static Value binop_add(Value& lhs, Value& rhs) noexcept;
static Value binop_sub(Value& lhs, Value& rhs) noexcept;
static Value binop_mul(Value& lhs, Value& rhs) noexcept;
static Value binop_div(Value& lhs, Value& rhs) noexcept;
static Value binop_mod(Value& lhs, Value& rhs) noexcept;
static Value binop_pow(Value& lhs, Value& rhs) noexcept;
static Value binop_xor(Value& lhs, Value& rhs) noexcept;
static Value binop_bitand(Value& lhs, Value& rhs) noexcept;
static Value binop_bitor(Value& lhs, Value& rhs) noexcept;
static Value binop_and(Value& lhs, Value& rhs) noexcept;
static Value binop_or(Value& lhs, Value& rhs) noexcept;
static Value binop_shl(Value& lhs, Value& rhs) noexcept;
static Value binop_shr(Value& lhs, Value& rhs) noexcept;
static Value binop_equ(Value& lhs, Value& rhs) noexcept;
static Value binop_neq(Value& lhs, Value& rhs) noexcept;
static Value binop_gt(Value& lhs, Value& rhs) noexcept;
static Value binop_gte(Value& lhs, Value& rhs) noexcept;
static Value binop_lt(Value& lhs, Value& rhs) noexcept;
static Value binop_lte(Value& lhs, Value& rhs) noexcept;
static Value binop_cast(Value& lhs, Value& rhs) noexcept;
static Value binop_pun(Value& lhs, Value& rhs) noexcept;

static Value unop_not(Value& lhs) noexcept;
static Value unop_inv(Value& lhs) noexcept;
static Value unop_end(Value& lhs) noexcept;
static Value unop_sep(Value& lhs) noexcept;
static bool op_isunary(SymOp op) noexcept;
static bool op_isbinary(SymOp op) noexcept;

static SymBinop binopTable[] = {
    binop_none,
    binop_add,
    binop_sub,
    binop_mul,
    binop_div,
    binop_mod,
    binop_pow,
    binop_xor,
    binop_bitand,
    binop_bitor,
    binop_and,
    binop_or,
    binop_shl,
    binop_shr,
    binop_equ,
    binop_neq,
    binop_gt,
    binop_gte,
    binop_lt,
    binop_lte,
    binop_cast,
    binop_pun,
    NULL,
};

static SymUnop unopTable[] = {
    unop_not,
    unop_inv,
    unop_end,
    unop_sep,
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
    if (self->stack.size() < 1) {
        fprintf(stderr, "Stack empty\n");
        exit(1);
    }

    Value& value = self->stack.top();
    value.println();
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
    }

    #define XENTRY(Name, Op) {Name, (SymOp)Op}
    static struct {
        const char *name; SymOp op;
    } lookup[] = {
        XENTRY("+", binop_add),
        XENTRY("add", binop_add),
        XENTRY("-", binop_sub),
        XENTRY("sub", binop_sub),
        XENTRY("*", binop_mul),
        XENTRY("mul", binop_mul),
        XENTRY("/", binop_div),
        XENTRY("div", binop_div),
        XENTRY("%", binop_mod),
        XENTRY("mod", binop_mod),
        XENTRY("^", binop_xor),
        XENTRY("xor", binop_xor),
        XENTRY("&", binop_bitand),
        XENTRY("bitand", binop_bitand),
        XENTRY("|", binop_bitor),
        XENTRY("bitor", binop_bitor),
        XENTRY("&~", NULL),//binop_bitandinv), // a & ~b
        XENTRY("bitandinv", NULL),//binop_bitandinv), // a & ~b
        XENTRY("!", unop_not),
        XENTRY("not", unop_not),
        XENTRY("~", unop_inv),
        XENTRY("inv", unop_inv),
        XENTRY("&&", binop_and),
        XENTRY("and", binop_and),
        XENTRY("||", binop_or),
        XENTRY("or", binop_or),
        XENTRY("**", binop_pow),
        XENTRY("pow", binop_pow),
        XENTRY("<<", binop_shl),
        XENTRY("shl", binop_shl),
        XENTRY(">>", binop_shr),
        XENTRY("shr", binop_shr),
        XENTRY("==", binop_equ),
        XENTRY("eq", binop_equ),
        XENTRY("!=", binop_neq),
        XENTRY("neq", binop_neq),
        XENTRY(">", binop_gt),
        XENTRY("gt", binop_gt),
        XENTRY(">=", binop_gte),
        XENTRY("gte", binop_gte),
        XENTRY("<", binop_lt),
        XENTRY("lt", binop_lt),
        XENTRY("<=", binop_lte),
        XENTRY("lte", binop_lte),
        XENTRY(";", unop_end), // these two ops actually put an unused value on the stack
        XENTRY("end", unop_end),
        XENTRY(",", unop_sep), // but this is fine
        XENTRY("sep", unop_sep),
        XENTRY("to", binop_cast),
        XENTRY("as", binop_pun),
        XENTRY("sqrt", NULL),
        XENTRY("gcd", NULL),
        XENTRY("lcm", NULL),
        XENTRY("sin", NULL),
        XENTRY("cos", NULL),
        XENTRY("tan", NULL),
        XENTRY("abs", NULL), // to +
        XENTRY("neg", NULL), // to opposite
        XENTRY("sgn", NULL),
        XENTRY("floor", NULL),
        XENTRY("round", NULL),
        XENTRY(NULL, NULL),
    };
    #undef XENTRY

    for (size_t i = 0; lookup[i].name != NULL; i++) {
        if (strcmp(lookup[i].name, value) == 0) {
            if (lookup[i].op == NULL) {
                fprintf(stderr, "Operation '%s' not implemented\n", lookup[i].name);
                exit(1);
            }
            return (Node *) new (std::nothrow) SymNode(lookup[i].op);
        }
    }

    // signed
    if (is_signed((const char *)value)) {
        Int number;
        if (sscanf(value, FMT_INT, &number) == 1) {
            Value tmp = Value(number);
            tmp.format(FORMAT_HEX);
            return (Node *) new (std::nothrow) NumNode(tmp);
        }
    }
    // unsigned
    else if (is_unsigned((const char *)value)) {
        Uint number;
        if (sscanf(value, FMT_UINT, &number) == 1) {
            Value tmp = Value(number);
            tmp.format(FORMAT_HEX);
            return (Node *) new (std::nothrow) NumNode(tmp);
        }
    }

    if (value[1] != 0 && value[0] == '0') {
        switch (value[1]) {
        case 'x': // fallthrough
        case 'X': {
            Uint number;
            if (sscanf(&value[2], FMT_HEX, &number) == 1) {
                Value tmp = Value(number);
                return (Node *) new (std::nothrow) NumNode(tmp);
            }
            break;
        }
        case 'O': // fallthrough
        case 'o': {
            Uint number;
            if (sscanf(&value[2], FMT_OCT, &number) == 1) {
                Value tmp = Value(number);
                return (Node *) new (std::nothrow) NumNode(tmp);
            }
            break;
        }
        case 'b': // fallthrough
        case 'B': {
            Uint number;
            if (conve_binary(&value[2], &number)) {
                Value tmp = Value(number);
                return (Node *) new (std::nothrow) NumNode(tmp);
            }
            break;
        }
        default:
            break;
        }
    }

    // must be a word operation
    return (Node *) new (std::nothrow) NumNode(Value((const char *)value));
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
    if (stack.size() < 1) {
        fprintf(stderr, "Stack empty\n");
        exit(1);
    }

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
    assert(lhs.type == rhs.type);
    switch (lhs.type) {
    case TYPE_FLOAT: return Value(lhs.number.f + rhs.number.f);
    case TYPE_INT:   return Value(lhs.number.i + rhs.number.i);
    case TYPE_UINT:  return Value(lhs.number.u + rhs.number.u);
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_sub(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    assert(lhs.type == rhs.type);
    switch (lhs.type) {
    case TYPE_FLOAT: return Value(lhs.number.f - rhs.number.f);
    case TYPE_INT:   return Value(lhs.number.i - rhs.number.i);
    case TYPE_UINT:  return Value(lhs.number.u - rhs.number.u);
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_mul(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    assert(lhs.type == rhs.type);
    switch (lhs.type) {
    case TYPE_FLOAT: return Value(lhs.number.f * rhs.number.f);
    case TYPE_INT:   return Value(lhs.number.i * rhs.number.i);
    case TYPE_UINT:  return Value(lhs.number.u * rhs.number.u);
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_div(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    assert(lhs.type == rhs.type);
    switch (lhs.type) {
    case TYPE_FLOAT: return Value(lhs.number.f / rhs.number.f);
    case TYPE_INT:   return Value(lhs.number.i / rhs.number.i);
    case TYPE_UINT:  return Value(lhs.number.u / rhs.number.u);
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_mod(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    assert(lhs.type == rhs.type);
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)FLOAT_MOD(lhs.number.f, rhs.number.f));
    case TYPE_INT:   return Value((Int)(lhs.number.i % rhs.number.i));
    case TYPE_UINT:  return Value((Uint)(lhs.number.u % rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_pow(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    assert(lhs.type == rhs.type);
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)FLOAT_POW(lhs.number.f, rhs.number.f));
    case TYPE_INT:   return Value((Int)int_pow((unsigned long)lhs.number.i, (unsigned long)rhs.number.i));
    case TYPE_UINT:  return Value((Uint)int_pow((unsigned long)lhs.number.u, (unsigned long)rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_xor(Value& lhs, Value& rhs) noexcept {
    switch (lhs.type) {
    case TYPE_FLOAT: {
        Value tmp = Value((Uint)(lhs.number.u ^ rhs.number.u));
        tmp.pun(TYPE_FLOAT);
        return tmp;
    }
    case TYPE_INT:   return Value((Int)(lhs.number.i ^ rhs.number.i));
    case TYPE_UINT:  return Value((Uint)(lhs.number.u ^ rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_bitand(Value& lhs, Value& rhs) noexcept {
    switch (lhs.type) {
    case TYPE_FLOAT: {
        Value tmp = Value((Uint)(lhs.number.u & rhs.number.u));
        tmp.pun(TYPE_FLOAT);
        return tmp;
    }
    case TYPE_INT:   return Value((Int)(lhs.number.i & rhs.number.i));
    case TYPE_UINT:  return Value((Uint)(lhs.number.u & rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_bitor(Value& lhs, Value& rhs) noexcept {
    switch (lhs.type) {
    case TYPE_FLOAT: {
        Value tmp = Value((Uint)(lhs.number.u | rhs.number.u));
        tmp.pun(TYPE_FLOAT);
        return tmp;
    }
    case TYPE_INT:   return Value((Int)(lhs.number.i | rhs.number.i));
    case TYPE_UINT:  return Value((Uint)(lhs.number.u | rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_and(Value& lhs, Value& rhs) noexcept {
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)((lhs.number.f != FLOAT_ZERO) && (rhs.number.u != FLOAT_ZERO)));
    case TYPE_INT:   return Value((Int)(lhs.number.i && rhs.number.i));
    case TYPE_UINT:  return Value((Uint)(lhs.number.u && rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_or(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    assert(lhs.type == rhs.type);
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)((lhs.number.f != FLOAT_ZERO) || (rhs.number.f != FLOAT_ZERO)));
    case TYPE_INT:   return Value((Int)(lhs.number.i || rhs.number.i));
    case TYPE_UINT:  return Value((Uint)(lhs.number.u || rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_shl(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    assert(lhs.type == rhs.type);
    switch (lhs.type) {
    case TYPE_FLOAT: {
        Value tmp = Value((Uint)(lhs.number.u << (Uint)round(rhs.number.f)));
        tmp.pun(TYPE_FLOAT);
        return tmp;
    }
    case TYPE_INT:   return Value((Int)(lhs.number.i << rhs.number.i));
    case TYPE_UINT:  return Value((Uint)(lhs.number.u << rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_shr(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    assert(lhs.type == rhs.type);
    switch (lhs.type) {
    case TYPE_FLOAT: {
        Value tmp = Value((Uint)(lhs.number.u >> (Uint)round(rhs.number.f)));
        tmp.pun(TYPE_FLOAT);
        return tmp;
    }
    case TYPE_INT:   return Value((Int)(lhs.number.i >> rhs.number.i));
    case TYPE_UINT:  return Value((Uint)(lhs.number.u >> rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_equ(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    assert(lhs.type == rhs.type);
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)(lhs.number.f == rhs.number.f));
    case TYPE_INT:   return Value((Int)(lhs.number.i == rhs.number.i));
    case TYPE_UINT:  return Value((Uint)(lhs.number.u == rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_neq(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    assert(lhs.type == rhs.type);
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)(lhs.number.f != rhs.number.f));
    case TYPE_INT:   return Value((Int)(lhs.number.i != rhs.number.i));
    case TYPE_UINT:  return Value((Uint)(lhs.number.u != rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_gt(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    assert(lhs.type == rhs.type);
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)(lhs.number.f > rhs.number.f));
    case TYPE_INT:   return Value((Int)(lhs.number.i > rhs.number.i));
    case TYPE_UINT:  return Value((Uint)(lhs.number.u > rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_gte(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    assert(lhs.type == rhs.type);
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)(lhs.number.f >= rhs.number.f));
    case TYPE_INT:   return Value((Int)(lhs.number.i >= rhs.number.i));
    case TYPE_UINT:  return Value((Uint)(lhs.number.u >= rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_lt(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    assert(lhs.type == rhs.type);
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)(lhs.number.f < rhs.number.f));
    case TYPE_INT:   return Value((Int)(lhs.number.i < rhs.number.i));
    case TYPE_UINT:  return Value((Uint)(lhs.number.u < rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_lte(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    assert(lhs.type == rhs.type);
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)(lhs.number.f <= rhs.number.f));
    case TYPE_INT:   return Value((Int)(lhs.number.i <= rhs.number.i));
    case TYPE_UINT:  return Value((Uint)(lhs.number.u <= rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_cast(Value& lhs, Value& rhs) noexcept {
    if (rhs.type != TYPE_STRING) {
        return rhs.unexpected_type();
    }

    for (size_t i = 0; i < TYPE_COUNT; i++) {
        if (strcasecmp(rhs.number.s, typeTable[i]) == 0) {
            switch (lhs.type) {
            case TYPE_FLOAT: {
                Value tmp = Value(lhs.number.f);
                tmp.coerce_exec((Type)i);
                return tmp;
            }
            case TYPE_INT: {
                Value tmp = Value(lhs.number.i);
                tmp.coerce_exec((Type)i);
                return tmp;
            }
            case TYPE_UINT: {
                Value tmp = Value(lhs.number.u);
                tmp.coerce_exec((Type)i);
                return tmp;
            }
            default:
                return lhs.unexpected_type();
            }
        }
    }

    return rhs.unexpected_type();
}

static Value binop_pun(Value& lhs, Value& rhs) noexcept {
    if (rhs.type != TYPE_STRING) {
        return rhs.unexpected_type();
    }

    for (size_t i = 0; i < TYPE_COUNT; i++) {
        if (strcasecmp(rhs.number.s, typeTable[i]) == 0) {
            switch (lhs.type) {
            case TYPE_FLOAT: {
                Value tmp = Value(lhs.number.f);
                tmp.pun((Type)i);
                return tmp;
            }
            case TYPE_INT: {
                Value tmp = Value(lhs.number.i);
                tmp.pun((Type)i);
                return tmp;
            }
            case TYPE_UINT: {
                Value tmp = Value(lhs.number.u);
                tmp.pun((Type)i);
                return tmp;
            }
            default:
                return lhs.unexpected_type();
            }
        }
    }

    for (size_t i = 0; i < FORMAT_COUNT; i++) {
        if (strcasecmp(rhs.number.s, formatTable[i]) == 0) {
            switch (lhs.type) {
            case TYPE_FLOAT: {
                Value tmp = Value(lhs.number.f);
                tmp.format((Format)i);
                return tmp;
            }
            case TYPE_INT: {
                Value tmp = Value(lhs.number.i);
                tmp.format((Format)i);
                return tmp;
            }
            case TYPE_UINT: {
                Value tmp = Value(lhs.number.u);
                tmp.format((Format)i);
                return tmp;
            }
            default:
                return lhs.unexpected_type();
            }
        }
    }

    return rhs.unexpected_type();
}

static Value unop_not(Value& lhs) noexcept {
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)!lhs.number.u);
    case TYPE_INT:   return Value((Int)!lhs.number.i);
    case TYPE_UINT:  return Value((Uint)!lhs.number.u);
    default: break;
    }
    return lhs.unexpected_type();
}

static Value unop_inv(Value& lhs) noexcept {
    switch (lhs.type) {
    case TYPE_FLOAT: {
        Value tmp = Value((Uint)~lhs.number.u);
        tmp.pun(TYPE_FLOAT);
        return tmp;
    }
    case TYPE_INT:  return Value((Int)~lhs.number.i);
    case TYPE_UINT: return Value((Uint)~lhs.number.u);
    default: break;
    }
    return lhs.unexpected_type();
}

static Value unop_end(Value& lhs) noexcept {
    lhs.println();
    return Value();
}

static Value unop_sep(Value& lhs) noexcept {
    lhs.print();
    return Value();
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
    type{TYPE_UINT},
    fmt{FORMAT_DEC}
{
}

Value::Value(const char *value) noexcept :
    number{0},
    type{TYPE_STRING},
    fmt{FORMAT_DEC}
{
    number.s = value;
}

Value::Value(Float f) noexcept :
    number{0},
    type{TYPE_FLOAT},
    fmt{FORMAT_DEC}
{
    number.f = f;
}

Value::Value(Int i) noexcept :
    number{0},
    type{TYPE_INT},
    fmt{FORMAT_DEC}
{
    number.i = i;
}

Value::Value(Uint u) noexcept :
    number{0},
    type{TYPE_UINT},
    fmt{FORMAT_DEC}
{
    number.u = u;
}

void Value::print() noexcept {
    switch (this->type) {
    case TYPE_FLOAT:
        std::cout << this->number.f << " ";
        break;
    case TYPE_INT:
        std::cout << this->number.u << " ";
        break;
    case TYPE_UINT:
        std::cout << this->number.i << " ";
        break;
    }
    fflush(stdout);
}

void Value::println() noexcept {
    switch (this->fmt) {
    case FORMAT_DEC:
        break;
    case FORMAT_HEX:
        fprintf(stdout, "0x" FMT_HEX "\n", this->number.u);
        return;
    case FORMAT_OCT:
        fprintf(stdout, "0o" FMT_OCT "\n", this->number.u);
        return;
    case FORMAT_BIN:
        print_binary(this->number.u);
        return;
    default:
        assert(0);
        break;
    }

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

static bool is_signed(const char *value) noexcept {
    assert(value);
    if (value[0] != 0 && (value[0] == '+' || value[0] == '-')) {
        return is_unsigned(&value[1]);
    }
    return false;
}

static bool is_unsigned(const char *value) noexcept {
    assert(value);
    for (size_t i = 0; value[i] != 0; i++) {
        if (!isdigit(value[i])) {
            return false;
        }
    }
    return true;
}

static void print_binary(Uint value) noexcept {
    int size = (int)sizeof(Uint) * 8 - 1;

    fprintf(stdout, "0b");
    fflush(stdout);
    for (int i = size; i >= 0; i--) {
        if ((Uint)(((Uint)1) << (Uint)i) > value) {
            continue;
        }
        fprintf(stdout, "%u", (unsigned)((value >> i) & 1));
        fflush(stdout);
    }
    fprintf(stdout, "\n");
}

static bool conve_binary(const char *value, Uint *out) noexcept {
    Uint builder = 0;

    assert(value);
    assert(out);

    Uint i = strlen(value) - 1;
    for (const char *p = value; *p != 0; p++, i--) {
        if (*p == '1') {
            builder |= (((Uint)1) << i);
        }
        else if (*p == '0') {
            ;
        }
        else {
            return false;
        }
    }
    *out = builder;
    return true;
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
        // not applicable
        return;
    }
    this->type = type;
}

void Value::coerce(Value& other) noexcept {
    this->coerce_exec(this->coerce_chk(other));
    other.coerce_exec(other.coerce_chk(*this));
}

void Value::pun(enum Type type) noexcept {
    this->type = type;
}

void Value::format(enum Format format) noexcept {
    this->fmt = format;
}

Value Value::unexpected_type(void) noexcept {
    const char *name = typeTable[this->type];
    switch (this->type) {
    case TYPE_FLOAT:  fprintf(stderr, "Unexpected %s: '" FMT_FLOAT "'\n", name, this->number.f); break;
    case TYPE_INT:    fprintf(stderr, "Unexpected %s: '" FMT_INT "'\n", name, this->number.i); break;
    case TYPE_UINT:   fprintf(stderr, "Unexpected %s: '" FMT_UINT "'\n", name, this->number.u); break;
    case TYPE_STRING: fprintf(stderr, "Unexpected %s: '" FMT_STRING "'\n", name, this->number.s); break;
    default:          fprintf(stderr, "Unexpected unknown error\n"); break;
    }
    exit(1);
    return Value();
}

