#ifndef RPN_INCLUDE
#include "rpn.hh"
#endif

#ifdef RPN_32BITS
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
#define FLOAT_SQRT(...) sqrtf(__VA_ARGS__)
#define FLOAT_SIN(...) sinf(__VA_ARGS__)
#define FLOAT_COS(...) cosf(__VA_ARGS__)
#define FLOAT_TAN(...) tanf(__VA_ARGS__)
#define FLOAT_FLOOR(...) floorf(__VA_ARGS__)
#define FLOAT_ROUND(...) roundf(__VA_ARGS__)
#define FLOAT_LOG10(...) log10f(__VA_ARGS__)
#define FLOAT_LOG(...) logf(__VA_ARGS__)
#define FLOAT_ZERO 0.0f
union FloatInfo { // on a Big Endian machine will need to convert to LE first
    Float f;
    struct {
        Uint mantissa : 23;
        Uint exponent : 8;
        Uint sign : 1;
    } parts;
};
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
#define FLOAT_SQRT(...) sqrt(__VA_ARGS__)
#define FLOAT_SIN(...) sin(__VA_ARGS__)
#define FLOAT_COS(...) cos(__VA_ARGS__)
#define FLOAT_TAN(...) tan(__VA_ARGS__)
#define FLOAT_FLOOR(...) floor(__VA_ARGS__)
#define FLOAT_ROUND(...) round(__VA_ARGS__)
#define FLOAT_LOG10(...) log10(__VA_ARGS__)
#define FLOAT_LOG(...) log(__VA_ARGS__)
#define FLOAT_ZERO 0.0
union FloatInfo {
    Float f;
    struct {
        Uint mantissa : 52;
        Uint exponent : 11;
        Uint sign : 1;
    } parts;
};
#endif
#define FMT_STRING "%s"

#define EPRINT(...) \
    do { \
        if (_verbose) { \
            fprintf(stderr, __VA_ARGS__); \
        } \
    } while (0)

// order of precedence, lowest to highest
enum Type {
    TYPE_INT,
    TYPE_UINT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_COUNT
};

static const char *typeTable[] = {
    "int",
    "uint",
    "float",
    "string",
    "",
};

enum Format {
    FORMAT_DEC,
    FORMAT_HEX,
    FORMAT_OCT,
    FORMAT_BIN,
    FORMAT_BIG,
    FORMAT_LITTLE,
    FORMAT_COUNT
};

static const char *formatTable[] = {
    "dec",
    "hex",
    "oct",
    "bin",
    "big", // endianness
    "little",
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
    virtual ~Node() = default;
};

typedef Value (* SymOp)(void);
typedef Value (* SymBinop)(Value& lhs, Value& rhs);
typedef Value (* SymUnop)(Value& lhs);

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

static bool conve_binary(const char *value, Uint *out) noexcept;
static void print_binary(Uint value) noexcept;
static void print_reversed(Uint value) noexcept;

static Node *node_new(char *value) noexcept;
static void node_free(Node *self) noexcept;

//static Value binop_none(Value& lhs, Value& rhs) noexcept;
static Value binop_add(Value& lhs, Value& rhs) noexcept;
static Value binop_sub(Value& lhs, Value& rhs) noexcept;
static Value binop_mul(Value& lhs, Value& rhs) noexcept;
static Value binop_div(Value& lhs, Value& rhs) noexcept;
static Value binop_mod(Value& lhs, Value& rhs) noexcept;
static Value binop_pow(Value& lhs, Value& rhs) noexcept;
static Value binop_xor(Value& lhs, Value& rhs) noexcept;
static Value binop_bitand(Value& lhs, Value& rhs) noexcept;
static Value binop_bitor(Value& lhs, Value& rhs) noexcept;
static Value binop_bitandinv(Value& lhs, Value& rhs) noexcept;
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
static Value binop_gcd(Value& lhs, Value& rhs) noexcept;
static Value binop_lcm(Value& lhs, Value& rhs) noexcept;

static Value unop_not(Value& lhs) noexcept;
static Value unop_inv(Value& lhs) noexcept;
static Value unop_end(Value& lhs) noexcept;
static Value unop_sep(Value& lhs) noexcept;
static Value unop_sqrt(Value& lhs) noexcept;
static Value unop_sin(Value& lhs) noexcept;
static Value unop_cos(Value& lhs) noexcept;
static Value unop_tan(Value& lhs) noexcept;
static Value unop_abs(Value& lhs) noexcept;
static Value unop_sgn(Value& lhs) noexcept;
static Value unop_floor(Value& lhs) noexcept;
static Value unop_round(Value& lhs) noexcept;
static Value unop_ord(Value &lhs) noexcept;
//static Value unop_chr(Value &lhs) noexcept;
static Value unop_ln(Value &lhs) noexcept;
static Value unop_log(Value &lhs) noexcept;
static Value unop_info(Value &lhs) noexcept;
static Value unop_fsgn(Value &lhs) noexcept;
static Value unop_fexp(Value &lhs) noexcept;
static Value unop_fmant(Value &lhs) noexcept;
static bool op_isunary(SymOp op) noexcept;
//static bool op_isbinary(SymOp op) noexcept;

#if 0
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
    binop_bitandinv,
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
    binop_gcd,
    binop_lcm,
    NULL,
};
#endif

static SymUnop unopTable[] = {
    unop_not,
    unop_inv,
    unop_end,
    unop_sep,
    unop_sqrt,
    unop_sin,
    unop_cos,
    unop_tan,
    unop_abs,
    unop_sgn,
    unop_floor,
    unop_round,
    unop_ord,
    //unop_chr,
    unop_ln,
    unop_log,
    unop_info,
    unop_fsgn,
    unop_fexp,
    unop_fmant,
    NULL,
};

#define XENTRY(Name, Op) {Name, (SymOp)Op}
static struct {
    const char *name; SymOp op;
} opLookup[] = {
    XENTRY(REG_OP_ADD_SYM, binop_add),
    XENTRY(REG_OP_ADD, binop_add),
    XENTRY(REG_OP_SUB_SYM, binop_sub),
    XENTRY(REG_OP_SUB, binop_sub),
    XENTRY(REG_OP_MUL_SYM, binop_mul),
    XENTRY(REG_OP_MUL, binop_mul),
    XENTRY(REG_OP_DIV_SYM, binop_div),
    XENTRY(REG_OP_DIV, binop_div),
    XENTRY(REG_OP_MOD_SYM, binop_mod),
    XENTRY(REG_OP_MOD, binop_mod),
    XENTRY(REG_OP_XOR_SYM, binop_xor),
    XENTRY(REG_OP_XOR, binop_xor),
    XENTRY(REG_OP_BITAND_SYM, binop_bitand),
    XENTRY(REG_OP_BITAND, binop_bitand),
    XENTRY(REG_OP_BITOR_SYM, binop_bitor),
    XENTRY(REG_OP_BITOR, binop_bitor),
    XENTRY(REG_OP_BITANDINV_SYM, binop_bitandinv), // a & ~b
    XENTRY(REG_OP_BITANDINV, binop_bitandinv),
    XENTRY(REG_OP_NOT_SYM, unop_not),
    XENTRY(REG_OP_NOT, unop_not),
    XENTRY(REG_OP_INV_SYM, unop_inv),
    XENTRY(REG_OP_INV, unop_inv),
    XENTRY(REG_OP_AND_SYM, binop_and),
    XENTRY(REG_OP_AND, binop_and),
    XENTRY(REG_OP_OR_SYM, binop_or),
    XENTRY(REG_OP_OR, binop_or),
    XENTRY(REG_OP_POW_SYM, binop_pow),
    XENTRY(REG_OP_POW, binop_pow),
    XENTRY(REG_OP_SHL_SYM, binop_shl),
    XENTRY(REG_OP_SHL, binop_shl),
    XENTRY(REG_OP_SHR_SYM, binop_shr),
    XENTRY(REG_OP_SHR, binop_shr),
    XENTRY(REG_OP_EQ_SYM, binop_equ),
    XENTRY(REG_OP_EQ, binop_equ),
    XENTRY(REG_OP_NEQ_SYM, binop_neq),
    XENTRY(REG_OP_NEQ, binop_neq),
    XENTRY(REG_OP_GT_SYM, binop_gt),
    XENTRY(REG_OP_GT, binop_gt),
    XENTRY(REG_OP_GTE_SYM, binop_gte),
    XENTRY(REG_OP_GTE, binop_gte),
    XENTRY(REG_OP_LT_SYM, binop_lt),
    XENTRY(REG_OP_LT, binop_lt),
    XENTRY(REG_OP_LTE_SYM, binop_lte),
    XENTRY(REG_OP_LTE, binop_lte),
    XENTRY(REG_OP_END_SYM, unop_end),
    XENTRY(REG_OP_END, unop_end),
    XENTRY(REG_OP_SEP_SYM, unop_sep),
    XENTRY(REG_OP_SEP, unop_sep),
    XENTRY(REG_OP_CAST, binop_cast),
    XENTRY(REG_OP_AS, binop_pun),
    XENTRY(REG_OP_SQRT, unop_sqrt),
    XENTRY(REG_OP_GCD, binop_gcd),
    XENTRY(REG_OP_LCM, binop_lcm),
    XENTRY(REG_OP_SIN, unop_sin),
    XENTRY(REG_OP_COS, unop_cos),
    XENTRY(REG_OP_TAN, unop_tan),
    XENTRY(REG_OP_ABS, unop_abs), // to +
    XENTRY(REG_OP_SGN, unop_sgn),
    XENTRY(REG_OP_FLOOR, unop_floor),
    XENTRY(REG_OP_ROUND, unop_round),
    XENTRY(REG_OP_ORD, unop_ord),
    //XENTRY(REG_OP_CHR, unop_chr),
    XENTRY(REG_OP_LN, unop_ln),
    XENTRY(REG_OP_LOG, unop_log),
    XENTRY(REG_OP_INFO, unop_info),
    XENTRY(REG_OP_FSGN, unop_fsgn),
    XENTRY(REG_OP_FEXP, unop_fexp),
    XENTRY(REG_OP_FMANT, unop_fmant),
    XENTRY(NULL, NULL),
};
#undef XENTRY

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
    if (!n) {
        EPRINT("Out of memory\n");
        exit(ENOMEM);
    }
    this->nodes.push_back(n);
    //rpn_print(this);
}

Rpn *rpn_create() noexcept {
    Rpn *self = new (std::nothrow) Rpn{};
    if (!self) {
        EPRINT("Out of memory\n");
        exit(ENOMEM);
    }
    return self;
}

void rpn_exec(Rpn *self) noexcept {
    assert(self);
    self->exec();
}

void rpn_push(Rpn*self, char *value) noexcept {
    assert(self);
    assert(value);
    self->push(value);
}

void rpn_print(Rpn *self) noexcept {
    assert(self);
    if (self->stack.size() < 1) {
        EPRINT("print: Stack empty\n");
        exit(1);
    }

    Value& value = self->stack.top();
    value.println();
}

void rpn_destroy(Rpn *self) noexcept {
    assert(self);
    delete self;
}

void rpn_help() noexcept {
    EPRINT("Operations can be binary or unary, following C-style convention\n");
    EPRINT("Special operations are 'end' or 'sep' which print a newline or space\n");
    EPRINT("Format is space-seperated RPN (Reverse Polish Notation)\n");
    for (int i = 0; opLookup[i].name != NULL; i++) {
        if (i % 14 == 0) {
            EPRINT("\n\t");
        }
        EPRINT("%s ", opLookup[i].name);
        fflush(stderr);
    }
    EPRINT("\n\n");

    EPRINT("Types can be used as the rhs operand of 'as' or 'to' operations\n\n\t");
    for (int i = 0; i < TYPE_COUNT; i++) {
        EPRINT("%s ", typeTable[i]);
        fflush(stderr);
    }
    EPRINT("\n\n");

    EPRINT("Formats must be the rhs operand of 'as' operations\n\n\t");
    for (int i = 0; i < FORMAT_COUNT; i++) {
        EPRINT("%s ", formatTable[i]);
        fflush(stderr);
    }
    EPRINT("\n");
    fflush(stderr);
}

static Node *node_new(char *value) noexcept {
    assert(value);

    // this isn't a memory leak, it will be collected
    static std::regex *uReg = NULL;
    static std::regex *iReg = NULL;
    static std::regex *fReg = NULL;
    static std::regex *bReg = NULL;
    static std::regex *oReg = NULL;
    static std::regex *hReg = NULL;

    #define REG_INIT(RegP, Regexp) do { \
        if (!RegP) { \
            RegP = new (std::nothrow) std::regex(Regexp); \
            if (!RegP) { \
                EPRINT("Out of memory\n"); \
                exit(ENOMEM); \
            } \
        } \
    } while (0)

    REG_INIT(uReg, REG_UNSIGNED);
    REG_INIT(iReg, REG_SIGNED);
    REG_INIT(fReg, REG_FLOAT);
    REG_INIT(bReg, REG_BIN);
    REG_INIT(oReg, REG_OCT);
    REG_INIT(hReg, REG_HEX);

    if (value[0] == 0) {
        EPRINT("operation: <empty> does not exist\n");
        exit(1);
    }

    static struct {
        const char *name;
        Value value;
    } constants[] = {
        {"pi", Value((Float)M_PI)},
        {"e", Value((Float)M_E)},
        {"inf", Value((Float)INFINITY)},
        {"nan", Value((Float)NAN)},
        {NULL, Value()}
    };

    for (int i = 0; constants[i].name; i++) {
        if (strcasecmp(value, constants[i].name) == 0) {
            return (Node *) new (std::nothrow) NumNode(constants[i].value);
        }
    }

    // floating point
    if (std::regex_match(value, *fReg)) {
        Float number;
        if (sscanf(value, FMT_FLOAT, &number) == 1) {
            return (Node *) new (std::nothrow) NumNode(Value(number));
        }
    }

    // signed
    else if (std::regex_match(value, *iReg)) {
        Int number;
        if (sscanf(value, FMT_INT, &number) == 1) {
            Value tmp = Value(number);
            tmp.format(FORMAT_HEX);
            return (Node *) new (std::nothrow) NumNode(tmp);
        }
    }
    // unsigned
    else if (std::regex_match(value, *uReg)) {
        Uint number;
        if (sscanf(value, FMT_UINT, &number) == 1) {
            Value tmp = Value(number);
            tmp.format(FORMAT_HEX);
            return (Node *) new (std::nothrow) NumNode(tmp);
        }
    }
    // hexadecimal
    else if (std::regex_match(value, *hReg)) {
        Uint number;
        if (sscanf(&value[2], FMT_HEX, &number) == 1) {
            Value tmp = Value(number);
            return (Node *) new (std::nothrow) NumNode(tmp);
        }
    }
    // octal
    else if (std::regex_match(value, *oReg)) {
        Uint number;
        if (sscanf(&value[2], FMT_OCT, &number) == 1) {
            Value tmp = Value(number);
            return (Node *) new (std::nothrow) NumNode(tmp);
        }
    }
    // binary
    else if (std::regex_match(value, *bReg)) {
        Uint number;
        if (conve_binary(&value[2], &number)) {
            Value tmp = Value(number);
            return (Node *) new (std::nothrow) NumNode(tmp);
        }
    }

    for (size_t i = 0; opLookup[i].name != NULL; i++) {
        if (strcmp(opLookup[i].name, value) == 0) {
            if (opLookup[i].op == NULL) {
                EPRINT("operation: '%s' not implemented\n", opLookup[i].name);
                exit(1);
            }
            return (Node *) new (std::nothrow) SymNode(opLookup[i].op);
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
        EPRINT("Stack empty\n");
        exit(1);
    }

    if (op_isunary(this->op)) {
        size_t size = stack.size();
        if (size < 1) {
            EPRINT("unary op: Invalid stack\n");
            exit(1);
        }
        Value lhs = stack.top();
        stack.pop();
        SymUnop op = (SymUnop)this->op;
        Value res = op(lhs);
        if (op != unop_end && op != unop_sep) {
            stack.push(res);
        }
    }
    else {
        size_t size = stack.size();
        if (size < 2) {
            EPRINT("binary op: Invalid stack\n");
            exit(1);
        }
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

#if 0
static Value binop_none(Value& lhs, Value& rhs) noexcept {
    (void)lhs;
    (void)rhs;
    return Value();
}
#endif

static Value binop_add(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
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
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)FLOAT_POW(lhs.number.f, rhs.number.f));
    case TYPE_INT:   return Value((Int)int_pow((unsigned long)lhs.number.i, (unsigned long)rhs.number.i));
    case TYPE_UINT:  return Value((Uint)int_pow((unsigned long)lhs.number.u, (unsigned long)rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_xor(Value& lhs, Value& rhs) noexcept {
    if (rhs.type == TYPE_STRING) {
        rhs.unexpected_type();
    }
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
    if (rhs.type == TYPE_STRING) {
        rhs.unexpected_type();
    }
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
    if (rhs.type == TYPE_STRING) {
        rhs.unexpected_type();
    }
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

static Value binop_bitandinv(Value& lhs, Value& rhs) noexcept {
    if (rhs.type == TYPE_STRING) {
        rhs.unexpected_type();
    }
    switch (lhs.type) {
    case TYPE_FLOAT: {
        Value tmp = Value((Uint)(lhs.number.u & (~rhs.number.u)));
        tmp.pun(TYPE_FLOAT);
        return tmp;
    }
    case TYPE_INT:   return Value((Int)(lhs.number.i & (~rhs.number.i)));
    case TYPE_UINT:  return Value((Uint)(lhs.number.u & (~rhs.number.u)));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_and(Value& lhs, Value& rhs) noexcept {
    if (rhs.type == TYPE_STRING) {
        rhs.unexpected_type();
    }
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

static Value unop_sqrt(Value& lhs) noexcept {
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)FLOAT_SQRT(lhs.number.f));
    case TYPE_INT:   return Value((Float)FLOAT_SQRT((Float)lhs.number.i));
    case TYPE_UINT:  return Value((Float)FLOAT_SQRT((Float)lhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_gcd(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)gcd((unsigned long)lhs.number.f, (unsigned long)rhs.number.f));
    case TYPE_INT:   return Value((Int)gcd((unsigned long)lhs.number.i, (unsigned long)rhs.number.i));
    case TYPE_UINT:  return Value((Uint)gcd((unsigned long)lhs.number.u, (unsigned long)rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_lcm(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)lcm((unsigned long)lhs.number.f, (unsigned long)rhs.number.f));
    case TYPE_INT:   return Value((Int)lcm((unsigned long)lhs.number.i, (unsigned long)rhs.number.i));
    case TYPE_UINT:  return Value((Uint)lcm((unsigned long)lhs.number.u, (unsigned long)rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
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

static Value unop_sin(Value& lhs) noexcept {
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)FLOAT_SIN(lhs.number.f));
    case TYPE_INT:   return Value((Float)FLOAT_SIN((Float)lhs.number.i));
    case TYPE_UINT:  return Value((Float)FLOAT_SIN((Float)lhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value unop_cos(Value& lhs) noexcept {
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)FLOAT_COS(lhs.number.f));
    case TYPE_INT:   return Value((Float)FLOAT_COS((Float)lhs.number.i));
    case TYPE_UINT:  return Value((Float)FLOAT_COS((Float)lhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value unop_tan(Value& lhs) noexcept {
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)FLOAT_TAN(lhs.number.f));
    case TYPE_INT:   return Value((Float)FLOAT_TAN((Float)lhs.number.i));
    case TYPE_UINT:  return Value((Float)FLOAT_TAN((Float)lhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value unop_abs(Value& lhs) noexcept {
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)(lhs.number.f < FLOAT_ZERO ? (((Float)-1) * lhs.number.f) : lhs.number.f));
    case TYPE_INT:   return Value((Int)(lhs.number.i < ((Int)0) ? (((Int)-1) * lhs.number.i) : lhs.number.i));
    case TYPE_UINT:  return Value(lhs.number.u);
    default: break;
    }
    return lhs.unexpected_type();
}

static Value unop_sgn(Value& lhs) noexcept {
    switch (lhs.type) {
    case TYPE_FLOAT:
        if (lhs.number.f > (Float)0) {
            return Value((Float)1);
        }
        else if (lhs.number.f == (Float)0) {
            return Value((Float)0);
        }
        else {
            return Value((Float)-1);
        }
    case TYPE_INT:
        if (lhs.number.i > (Int)0) {
            return Value((Int)1);
        }
        else if (lhs.number.i == (Int)0) {
            return Value((Int)0);
        }
        else {
            return Value((Int)-1);
        }
    case TYPE_UINT:
        if (lhs.number.u > (Uint)0) {
            return Value((Uint)1);
        }
        else {
            return Value((Uint)0);
        }
    default: break;
    }
    return lhs.unexpected_type();
}

static Value unop_floor(Value& lhs) noexcept {
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)FLOAT_FLOOR(lhs.number.f));
    case TYPE_INT:   return Value(lhs.number.i);
    case TYPE_UINT:  return Value(lhs.number.u);
    default: break;
    }
    return lhs.unexpected_type();
}

static Value unop_round(Value& lhs) noexcept {
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)FLOAT_ROUND(lhs.number.f));
    case TYPE_INT:   return Value(lhs.number.i);
    case TYPE_UINT:  return Value(lhs.number.u);
    default: break;
    }
    return lhs.unexpected_type();
}

static Value unop_ord(Value &lhs) noexcept {
    switch (lhs.type) {
    case TYPE_FLOAT: return lhs.unexpected_type();
    case TYPE_INT:   return lhs.unexpected_type();
    case TYPE_UINT:  return lhs.unexpected_type();
    case TYPE_STRING: return Value((Int)lhs.number.s[0]); // ascii only :/
    default: break;
    }
    return lhs.unexpected_type();
}

static Value unop_ln(Value &lhs) noexcept {
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)FLOAT_LOG(lhs.number.f));
    case TYPE_INT:   return Value((Float)FLOAT_LOG(lhs.number.i));
    case TYPE_UINT:  return Value((Float)FLOAT_LOG(lhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value unop_log(Value &lhs) noexcept {
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)FLOAT_LOG10(lhs.number.f));
    case TYPE_INT:   return Value((Float)FLOAT_LOG10(lhs.number.i));
    case TYPE_UINT:  return Value((Float)FLOAT_LOG10(lhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value unop_info(Value &lhs) noexcept {
    switch (lhs.type) {
    case TYPE_FLOAT: {
        FloatInfo fi;
        fi.f = lhs.number.f;
        // Note the sizing of these parts is weird. This works:
        fprintf(stdout, "%u, %u, " FMT_UINT "\n",
            fi.parts.sign, fi.parts.exponent, fi.parts.mantissa);
        fprintf(stdout, "%d, 0x%X, 0x" FMT_HEX "\n",
            fi.parts.sign, fi.parts.exponent, fi.parts.mantissa);
        fflush(stdout);
        return lhs;
    }
    case TYPE_INT: {
        lhs.println();
        return lhs;
    }
    case TYPE_UINT: {
        lhs.println();
        return lhs;
    }
    case TYPE_STRING:
        fprintf(stdout, "%s\n", lhs.number.s);
        fflush(stdout);
        return lhs;
    default:
        break;
    }
    return lhs.unexpected_type();
}

static Value unop_fsgn(Value &lhs) noexcept {
    FloatInfo fi;
    fi.f = lhs.number.f;
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)(fi.parts.sign));
    case TYPE_INT:   return Value((Int)(fi.parts.sign));
    case TYPE_UINT:  return Value((Uint)(fi.parts.sign));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value unop_fexp(Value &lhs) noexcept {
    FloatInfo fi;
    fi.f = lhs.number.f;
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)(fi.parts.exponent));
    case TYPE_INT:   return Value((Int)(fi.parts.exponent));
    case TYPE_UINT:  return Value((Uint)(fi.parts.exponent));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value unop_fmant(Value &lhs) noexcept {
    FloatInfo fi;
    fi.f = lhs.number.f;
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)(fi.parts.mantissa));
    case TYPE_INT:   return Value((Int)(fi.parts.mantissa));
    case TYPE_UINT:  return Value((Uint)(fi.parts.mantissa));
    default: break;
    }
    return lhs.unexpected_type();
}

static bool op_isunary(SymOp op) noexcept {
    for (size_t i = 0; unopTable[i] != NULL; i++) {
        if (unopTable[i] == (SymUnop)op) {
            return true;
        }
    }
    return false;
}

#if 0
static bool op_isbinary(SymOp op) noexcept {
    return !op_isunary(op);
}
#endif

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
    switch (this->fmt) {
    case FORMAT_DEC:
        break;
    case FORMAT_HEX:
    hex:
        fprintf(stdout, "0x" FMT_HEX " ", this->number.u);
        return;
    case FORMAT_OCT:
        fprintf(stdout, "0o" FMT_OCT " ", this->number.u);
        return;
    case FORMAT_BIN:
        print_binary(this->number.u);
        fprintf(stdout, " ");
        return;
    case FORMAT_BIG:
        if (is_little_endian()) {
            print_reversed(this->number.u);
            fprintf(stdout, " ");
            return;
        }
        else {
            goto hex;
        }
    case FORMAT_LITTLE:
        if (is_big_endian()) {
            print_reversed(this->number.u);
            fprintf(stdout, " ");
            return;
        }
        else {
            goto hex;
        }
    default:
        assert(0);
        break;
    }

    switch (this->type) {
    case TYPE_FLOAT:
        std::cout << this->number.f << " ";
        break;
    case TYPE_INT:
        std::cout << this->number.i << " ";
        break;
    case TYPE_UINT:
        std::cout << this->number.u << " ";
        break;
    }
    fflush(stdout);
}

void Value::println() noexcept {
    switch (this->fmt) {
    case FORMAT_DEC:
        break;
    case FORMAT_HEX:
    hex:
        fprintf(stdout, "0x" FMT_HEX "\n", this->number.u);
        return;
    case FORMAT_OCT:
        fprintf(stdout, "0o" FMT_OCT "\n", this->number.u);
        return;
    case FORMAT_BIN:
        print_binary(this->number.u);
        fprintf(stdout, "\n");
        return;
    case FORMAT_BIG:
        if (is_little_endian()) {
            print_reversed(this->number.u);
            fprintf(stdout, "\n");
            return;
        }
        else {
            goto hex;
        }
    case FORMAT_LITTLE:
        if (is_big_endian()) {
            print_reversed(this->number.u);
            fprintf(stdout, "\n");
            return;
        }
        else {
            goto hex;
        }
    default:
        assert(0);
        break;
    }

    switch (this->type) {
    case TYPE_FLOAT:
        std::cout << this->number.f << std::endl;
        break;
    case TYPE_INT:
        std::cout << this->number.i << std::endl;
        break;
    case TYPE_UINT:
        std::cout << this->number.u << std::endl;
        break;
    }
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
}

static void print_reversed(Uint value) noexcept {
    Uint reversed = 0;
    int size = (int)sizeof(Uint);
    uint8_t *forward = (uint8_t *)&value;
    uint8_t *backward = (uint8_t *)&reversed;

    for (int i = 0; i < size; i++) {
        backward[i] = forward[size - i - 1];
    }

    fprintf(stdout, "0x" FMT_HEX, reversed);
    fflush(stdout);
}

static bool conve_binary(const char *value, Uint *out) noexcept {
    assert(value);
    assert(out);

    Uint builder = 0;
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
    if (this->type == TYPE_INT && other.type == TYPE_UINT) {
        if (this->number.i < (Int)0) {
            return this->type;
        }
    }
    else if (this->type == TYPE_UINT && other.type == TYPE_INT) {
        if (other.number.i < (Int)0) {
            return other.type;
        }
    }

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
        EPRINT("coerce: %s cannot convert to %s\n",
            typeTable[this->type], typeTable[type]);
        exit(1);
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
    case TYPE_FLOAT:  EPRINT("Unexpected %s: '" FMT_FLOAT "'\n", name, this->number.f); break;
    case TYPE_INT:    EPRINT("Unexpected %s: '" FMT_INT "'\n", name, this->number.i); break;
    case TYPE_UINT:   EPRINT("Unexpected %s: '" FMT_UINT "'\n", name, this->number.u); break;
    case TYPE_STRING: EPRINT("Unexpected %s: '" FMT_STRING "'\n", name, this->number.s); break;
    default:          EPRINT("Unexpected unknown error\n"); break;
    }
    exit(1);
    return Value();
}

