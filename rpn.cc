#ifndef RPN_INCLUDE
#include "rpn.hh"
#endif

#if defined(RPN_8BITS)
// https://en.wikipedia.org/wiki/Minifloat
#define NO_FLOAT
#define Float Int
typedef int8_t Int;
typedef uint8_t Uint;
// windows complains but whatever it still works
#define FMT_FLOAT "%hhi"
#define FMT_INT "%hhi"
#define FMT_UINT "%hhu"
#define FMT_HEX "%hhX"
#define FMT_OCT "%hho"
#define FMT_LONG_HEX "%02hhX"
#define FMT_LONG_OCT "%03hho"

#define FLOAT_MOD(a, b) (a % b)
#define FLOAT_POW(a, b) (Float)int_pow(a, b)
#define FLOAT_SQRT(...) (Float)sqrtf(__VA_ARGS__)
#define FLOAT_SIN(...) (Float)sinf(__VA_ARGS__)
#define FLOAT_COS(...) (Float)cosf(__VA_ARGS__)
#define FLOAT_TAN(...) (Float)tanf(__VA_ARGS__)
#define FLOAT_FLOOR(...) (Float)floorf(__VA_ARGS__)
#define FLOAT_ROUND(...) (Float)roundf(__VA_ARGS__)
#define FLOAT_CEIL(...) (Float)ceilf(__VA_ARGS__)
#define FLOAT_TRUNC(...) (Float)truncf(__VA_ARGS__)
#define FLOAT_LOG10(...) (Float)log10f(__VA_ARGS__)
#define FLOAT_LOG(...) (Float)logf(__VA_ARGS__)
#define FLOAT_ZERO 0
#define MY_INTMAX INT8_MAX
#define MY_UINTMAX UINT8_MAX
#define MY_FLOATMAX INT8_MAX
#define MY_FLOATMIN UINT8_MAX
#define MY_BITMAX 7
#define MY_FMANTMASK 0x7
#define MY_FEXPMASK 0xF
#define MY_FEXPBIT 3
union FloatInfo { // on a Big Endian machine will need to convert to LE first
    Float f;
    struct {
        Uint mantissa : 3;
        Uint exponent : 4;
        Uint sign : 1;
    } parts;
};

#elif defined(RPN_16BITS)
// https://en.wikipedia.org/wiki/Half-precision_floating-point_format
#define NO_FLOAT
#define Float Int
typedef int16_t Int;
typedef uint16_t Uint;
#define FMT_FLOAT "%hi"
#define FMT_INT "%hi"
#define FMT_UINT "%hu"
#define FMT_HEX "%hX"
#define FMT_OCT "%ho"
#define FMT_LONG_HEX "%04hX"
#define FMT_LONG_OCT "%06ho"
#define FLOAT_MOD(a, b) (a % b)
#define FLOAT_POW(a, b) (Float)int_pow(a, b)
#define FLOAT_SQRT(...) (Float)sqrtf(__VA_ARGS__)
#define FLOAT_SIN(...) (Float)sinf(__VA_ARGS__)
#define FLOAT_COS(...) (Float)cosf(__VA_ARGS__)
#define FLOAT_TAN(...) (Float)tanf(__VA_ARGS__)
#define FLOAT_FLOOR(...) (Float)floorf(__VA_ARGS__)
#define FLOAT_ROUND(...) (Float)roundf(__VA_ARGS__)
#define FLOAT_CEIL(...) (Float)ceilf(__VA_ARGS__)
#define FLOAT_TRUNC(...) (Float)truncf(__VA_ARGS__)
#define FLOAT_LOG10(...) (Float)log10f(__VA_ARGS__)
#define FLOAT_LOG(...) (Float)logf(__VA_ARGS__)
#define FLOAT_ZERO 0
#define MY_INTMAX INT16_MAX
#define MY_UINTMAX UINT16_MAX
#define MY_FLOATMAX INT16_MAX
#define MY_FLOATMIN UINT16_MAX
#define MY_BITMAX 15
#define MY_FMANTMASK 0x3FF
#define MY_FEXPMASK 0x1F
#define MY_FEXPBIT 10
union FloatInfo { // on a Big Endian machine will need to convert to LE first
    Float f;
    struct {
        Uint mantissa : 10;
        Uint exponent : 5;
        Uint sign : 1;
    } parts;
};

#elif defined(RPN_32BITS)
typedef float Float;
typedef int32_t Int;
typedef uint32_t Uint;
#define FMT_FLOAT "%f"
#define FMT_INT "%i"
#define FMT_UINT "%u"
#define FMT_HEX "%X"
#define FMT_OCT "%o"
#define FMT_LONG_FLOAT "%.10f"
#define FMT_LONG_HEX "%08X"
#define FMT_LONG_OCT "%011o"
#define FLOAT_MOD(...) fmodf(__VA_ARGS__)
#define FLOAT_POW(...) powf(__VA_ARGS__)
#define FLOAT_SQRT(...) sqrtf(__VA_ARGS__)
#define FLOAT_SIN(...) sinf(__VA_ARGS__)
#define FLOAT_COS(...) cosf(__VA_ARGS__)
#define FLOAT_TAN(...) tanf(__VA_ARGS__)
#define FLOAT_FLOOR(...) floorf(__VA_ARGS__)
#define FLOAT_CEIL(...) (Float)ceilf(__VA_ARGS__)
#define FLOAT_TRUNC(...) (Float)truncf(__VA_ARGS__)
#define FLOAT_ROUND(...) roundf(__VA_ARGS__)
#define FLOAT_LOG10(...) log10f(__VA_ARGS__)
#define FLOAT_LOG(...) logf(__VA_ARGS__)
#define FLOAT_ZERO 0.0f
#define MY_INTMAX INT32_MAX
#define MY_UINTMAX UINT32_MAX
#define MY_FLOATMAX __FLT_MAX__
#define MY_FLOATMIN __FLT_MIN__
#define MY_BITMAX 31
#define MY_FMANTMASK 0x7FFFFF
#define MY_FEXPMASK 0xFF
#define MY_FEXPBIT 23
union FloatInfo { // on a Big Endian machine will need to convert to LE first
    Float f;
    struct {
        Uint mantissa : 23;
        Uint exponent : 8;
        Uint sign : 1;
    } parts;
};

#elif defined(RPN_64BITS)
typedef double Float;
typedef int64_t Int;
typedef uint64_t Uint;
#ifdef __APPLE__
#  define FMT_FLOAT "%lf"
#  define FMT_INT "%lli"
#  define FMT_UINT "%llu"
#  define FMT_HEX "%llX"
#  define FMT_OCT "%llo"
#  define FMT_LONG_FLOAT "%.20lf"
#  define FMT_LONG_HEX "%016llX"
#  define FMT_LONG_OCT "%022llo"
#elif defined(WIN32) || defined(_WIN32)
#  define FMT_FLOAT "%lf"
#  define FMT_INT "%I64i"
#  define FMT_UINT "%I64u"
#  define FMT_HEX "%I64X"
#  define FMT_OCT "%I64o"
#  define FMT_LONG_FLOAT "%.20lf"
#  define FMT_LONG_HEX "%016I64X"
#  define FMT_LONG_OCT "%022I64o"
#else
#  define FMT_FLOAT "%lf"
#  define FMT_INT "%li"
#  define FMT_UINT "%lu"
#  define FMT_HEX "%lX"
#  define FMT_OCT "%lo"
#  define FMT_LONG_FLOAT "%.20lf"
#  define FMT_LONG_HEX "%016lX"
#  define FMT_LONG_OCT "%022lo"
#endif
#define FLOAT_MOD(...) fmod(__VA_ARGS__)
#define FLOAT_POW(...) pow(__VA_ARGS__)
#define FLOAT_SQRT(...) sqrt(__VA_ARGS__)
#define FLOAT_SIN(...) sin(__VA_ARGS__)
#define FLOAT_COS(...) cos(__VA_ARGS__)
#define FLOAT_TAN(...) tan(__VA_ARGS__)
#define FLOAT_FLOOR(...) floor(__VA_ARGS__)
#define FLOAT_CEIL(...) (Float)ceil(__VA_ARGS__)
#define FLOAT_TRUNC(...) (Float)trunc(__VA_ARGS__)
#define FLOAT_ROUND(...) round(__VA_ARGS__)
#define FLOAT_LOG10(...) log10(__VA_ARGS__)
#define FLOAT_LOG(...) log(__VA_ARGS__)
#define FLOAT_ZERO 0.0
#define MY_INTMAX INT64_MAX
#define MY_UINTMAX UINT64_MAX
#define MY_FLOATMAX __DBL_MAX__
#define MY_FLOATMIN __DBL_MIN__
#define MY_BITMAX 63
#define MY_FMANTMASK 0xFFFFFFFFFFFFF
#define MY_FEXPMASK 0x7FF
#define MY_FEXPBIT 52
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
    FORMAT_CHAR,
    FORMAT_TYPE,
    FORMAT_COUNT
};

static const char *formatTable[] = {
    "dec",
    "hex",
    "oct",
    "bin",
    "big", // endianness
    "little",
    "chr",
    "type",
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
#ifndef NO_FLOAT
    Value(Float number) noexcept;
#endif
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
static Value binop_ror(Value& lhs, Value& rhs) noexcept;
static Value binop_rol(Value& lhs, Value& rhs) noexcept;
static Value binop_ncr(Value& lhs, Value& rhs) noexcept;
static Value binop_npr(Value& lhs, Value& rhs) noexcept;
static Value binop_save(Value& val, Value& name) noexcept;

static Value unop_not(Value& lhs) noexcept;
static Value unop_inv(Value& lhs) noexcept;
static Value unop_end(Value& lhs) noexcept;
static Value unop_sep(Value& lhs) noexcept;
static Value unop_quiet(Value& lhs) noexcept;
static Value unop_sqrt(Value& lhs) noexcept;
static Value unop_sin(Value& lhs) noexcept;
static Value unop_cos(Value& lhs) noexcept;
static Value unop_tan(Value& lhs) noexcept;
static Value unop_abs(Value& lhs) noexcept;
static Value unop_sgn(Value& lhs) noexcept;
static Value unop_floor(Value& lhs) noexcept;
static Value unop_round(Value& lhs) noexcept;
static Value unop_ceil(Value& lhs) noexcept;
static Value unop_trunc(Value& lhs) noexcept;
static Value unop_ord(Value &lhs) noexcept;
//static Value unop_chr(Value &lhs) noexcept;
static Value unop_ln(Value &lhs) noexcept;
static Value unop_log(Value &lhs) noexcept;
static Value unop_info(Value &lhs) noexcept;
static Value unop_fsgn(Value &lhs) noexcept;
static Value unop_fexp(Value &lhs) noexcept;
static Value unop_fmant(Value &lhs) noexcept;
static Value unop_factorial(Value &lhs) noexcept;
static Value unop_inverse(Value &lhs) noexcept;

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
    binop_ror,
    binop_rol,
    NULL,
};
#endif

static SymUnop unopTable[] = {
    unop_not,
    unop_inv,
    unop_end,
    unop_sep,
    unop_quiet,
    unop_sqrt,
    unop_sin,
    unop_cos,
    unop_tan,
    unop_abs,
    unop_sgn,
    unop_floor,
    unop_round,
    unop_ceil,
    unop_trunc,
    unop_ord,
    //unop_chr,
    unop_ln,
    unop_log,
    unop_info,
    unop_fsgn,
    unop_fexp,
    unop_fmant,
    unop_factorial,
    unop_inverse,
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
    XENTRY(REG_OP_LSH, binop_shl),
    XENTRY(REG_OP_SHR_SYM, binop_shr),
    XENTRY(REG_OP_SHR, binop_shr),
    XENTRY(REG_OP_RSH, binop_shr),
    XENTRY(REG_OP_EQ_SYM, binop_equ),
    XENTRY(REG_OP_EQ, binop_equ),
    XENTRY(REG_OP_EQU, binop_equ),
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
    XENTRY(REG_OP_QUIET, unop_quiet),
    XENTRY(REG_OP_CAST, binop_cast),
    XENTRY(REG_OP_AS, binop_pun),
    XENTRY(REG_OP_SQRT, unop_sqrt),
    XENTRY(REG_OP_GCD, binop_gcd),
    XENTRY(REG_OP_LCM, binop_lcm),
    XENTRY(REG_OP_ROR, binop_ror),
    XENTRY(REG_OP_ROL, binop_rol),
    XENTRY(REG_OP_SIN, unop_sin),
    XENTRY(REG_OP_COS, unop_cos),
    XENTRY(REG_OP_TAN, unop_tan),
    XENTRY(REG_OP_ABS, unop_abs), // to +
    XENTRY(REG_OP_SGN, unop_sgn),
    XENTRY(REG_OP_FLOOR, unop_floor),
    XENTRY(REG_OP_ROUND, unop_round),
    XENTRY(REG_OP_CEIL, unop_ceil),
    XENTRY(REG_OP_TRUNC, unop_trunc),
    XENTRY(REG_OP_ORD, unop_ord),
    //XENTRY(REG_OP_CHR, unop_chr),
    XENTRY(REG_OP_LN, unop_ln),
    XENTRY(REG_OP_LOG, unop_log),
    XENTRY(REG_OP_INFO, unop_info),
    XENTRY(REG_OP_FSGN, unop_fsgn),
    XENTRY(REG_OP_FEXP, unop_fexp),
    XENTRY(REG_OP_FMANT, unop_fmant),
    XENTRY(REG_OP_FACTORIAL, unop_factorial),
    XENTRY(REG_OP_INVERSE, unop_inverse),
    XENTRY(REG_OP_NCR, binop_ncr),
    XENTRY(REG_OP_NPR, binop_npr),
    XENTRY(REG_OP_SAVE, binop_save),
    XENTRY(NULL, NULL),
};
#undef XENTRY

struct Variable {
    const char *name;
    Value value;
};

static std::vector<Variable> constants = {
    {"pi", Value((Float)M_PI)},
    {"%e", Value((Float)M_E)},
    {"inf", Value((Float)INFINITY)},
    {"-inf", Value((Float)-INFINITY)},
    {"nan", Value((Float)NAN)},
    {"true", Value((Int)1)},
    {"false", Value((Int)0)},
    {"intmax", Value((Int)MY_INTMAX)},
    {"uintmax", Value((Uint)MY_UINTMAX)},
    {"floatmax", Value((Float)MY_FLOATMAX)},
    {"floatmin", Value((Float)MY_FLOATMIN)},
    {"bitmax", Value((Int)MY_BITMAX)},
    {"fmantmask", Value((Int)MY_FMANTMASK)},
    {"fexpmask", Value((Int)MY_FEXPMASK)},
    {"fexpbit", Value((Int)MY_FEXPBIT)},
};

static Value *constant_find(const char *name) noexcept {
    assert(name);
    for (auto& var : constants) {
        if (strcasecmp(name, var.name) == 0) {
            return &var.value;
        }
    }
    return NULL;
}

static void constant_save(const char *name, Value& val) noexcept {
    assert(name);
    Value *v = constant_find(name);
    if (v) {
        *v = val;
        return;
    }

    Variable variable = {name, val};
    constants.push_back(variable);
}

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
    assert(regex_is_initialized());
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
    assert(value);
    Node *n = node_new(value);
    if (!n) {
        EPRINT("stack: out of memory\n");
        exit(ENOMEM);
    }
    this->nodes.push_back(n);
    //rpn_print(this);
}

Rpn *rpn_create() noexcept {
    Rpn *self = new (std::nothrow) Rpn{};
    if (!self) {
        EPRINT("rpn: out of memory\n");
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

static Value maybe_a_constant(Value v) noexcept {
    // not a string
    if (v.type != TYPE_STRING) {
        return v;
    }

    Value *lookup = constant_find(v.number.s);
    // not a constant
    if (!lookup) {
        return v;
    }

    // a constant
    return *lookup;
}

void rpn_print(Rpn *self) noexcept {
    assert(self);
    if (self->stack.size() < 1) {
        EPRINT("print: Stack empty\n");
        exit(1);
    }

    Value& value = self->stack.top();
    value = maybe_a_constant(value);
    value.println();
}

void rpn_destroy(Rpn *self) noexcept {
    assert(self);
    delete self;
}

void rpn_help() noexcept {
    size_t len;
    printf("Operations can be binary or unary, following C-style convention\n");
    printf("Special operations are 'end' or 'sep' which print a newline or space\n");

    printf("Format is space-seperated RPN (Reverse Polish Notation)\n\n\t");
    len = 0;
    for (int i = 0; opLookup[i].name != NULL; i++) {
        if (len > 60) {
            printf("\n\t");
            len = 0;
        }
        printf("%s ", opLookup[i].name);
        len += strlen(opLookup[i].name) + 1;
    }
    printf("\n\n");

    printf("Types can be used as the rhs operand of 'as' or 'cast' operations\n\n\t");
    for (int i = 0; i < TYPE_COUNT; i++) {
        printf("%s ", typeTable[i]);
    }
    printf("\n\n");

    printf("Formats must be the rhs operand of 'as' operations\n\n\t");
    for (int i = 0; i < FORMAT_COUNT; i++) {
        printf("%s ", formatTable[i]);
    }
    printf("\n\n");

    printf("Constants consist of\n\n\t");
    len = 0;
    for (size_t i = 0; i < constants.size(); i++) {
        if (len > 60) {
            printf("\n\t");
            len = 0;
        }
        printf("%s ", constants[i].name);
        len += strlen(constants[i].name) + 1;
    }
    printf("\n");

    fflush(stdout);
}

static Node *node_new(char *value) noexcept {
    assert(value);

    if (value[0] == 0) {
        EPRINT("operation: <empty> does not exist\n");
        exit(1);
    }

    // floating point
    if (std::regex_match(value, *regex_float)) {
        Float number;
        if (sscanf(value, FMT_FLOAT, &number) == 1) {
            return (Node *) new (std::nothrow) NumNode(Value(number));
        }
    }

    // signed
    else if (std::regex_match(value, *regex_signed)) {
        Int number;
        if (sscanf(value, FMT_INT, &number) == 1) {
            Value tmp = Value(number);
            tmp.format(FORMAT_HEX);
            return (Node *) new (std::nothrow) NumNode(tmp);
        }
    }
    // unsigned
    else if (std::regex_match(value, *regex_unsigned)) {
        Uint number;
        if (sscanf(value, FMT_UINT, &number) == 1) {
            Value tmp = Value(number);
            tmp.format(FORMAT_HEX);
            return (Node *) new (std::nothrow) NumNode(tmp);
        }
    }
    // hexadecimal
    else if (std::regex_match(value, *regex_hexadecimal)) {
        Uint number;
        if (sscanf(&value[2], FMT_HEX, &number) == 1) {
            Value tmp = Value(number);
            return (Node *) new (std::nothrow) NumNode(tmp);
        }
    }
    // octal
    else if (std::regex_match(value, *regex_octal)) {
        Uint number;
        if (sscanf(&value[2], FMT_OCT, &number) == 1) {
            Value tmp = Value(number);
            return (Node *) new (std::nothrow) NumNode(tmp);
        }
    }
    // binary
    else if (std::regex_match(value, *regex_binary)) {
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
        EPRINT("exec: stack empty\n");
        exit(1);
    }

    if (op_isunary(this->op)) {
        size_t size = stack.size();
        if (size < 1) {
            EPRINT("unary op: Invalid stack\n");
            exit(1);
        }

        Value lhs = stack.top();
        lhs = maybe_a_constant(lhs);

        stack.pop();
        SymUnop op = (SymUnop)this->op;
        Value res = op(lhs);
        if (op != unop_end && op != unop_sep && op != unop_quiet) {
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
        rhs = maybe_a_constant(rhs);
        stack.pop();

        Value lhs = stack.top();
        lhs = maybe_a_constant(lhs);
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
    case TYPE_FLOAT: return Value((Float)(lhs.number.f + rhs.number.f));
    case TYPE_INT:   return Value((Int)(lhs.number.i + rhs.number.i));
    case TYPE_UINT:  return Value((Uint)(lhs.number.u + rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_sub(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)(lhs.number.f - rhs.number.f));
    case TYPE_INT:   return Value((Int)(lhs.number.i - rhs.number.i));
    case TYPE_UINT:  return Value((Uint)(lhs.number.u - rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_mul(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)(lhs.number.f * rhs.number.f));
    case TYPE_INT:   return Value((Int)(lhs.number.i * rhs.number.i));
    case TYPE_UINT:  return Value((Uint)(lhs.number.u * rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static void int_divbyzero(Int a, Int b) noexcept {
    EPRINT("Integer divide by zero: " FMT_INT " / " FMT_INT "\n",
        a, b);
    exit(ERANGE);
}

static void uint_divbyzero(Uint a, Uint b) noexcept {
    EPRINT("Uinteger divide by zero: " FMT_UINT " / " FMT_UINT "\n",
        a, b);
    exit(ERANGE);
}

static Value binop_div(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)(lhs.number.f / rhs.number.f));
    case TYPE_INT:
        if (rhs.number.i == 0) int_divbyzero(lhs.number.i, rhs.number.i);
        return Value((Int)(lhs.number.i / rhs.number.i));
    case TYPE_UINT:
        if (rhs.number.u == 0) uint_divbyzero(lhs.number.u, rhs.number.u);
        return Value((Uint)(lhs.number.u / rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_mod(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)FLOAT_MOD(lhs.number.f, rhs.number.f));
    case TYPE_INT:
        if (rhs.number.i == 0) int_divbyzero(lhs.number.i, rhs.number.i);
        return Value((Int)(lhs.number.i % rhs.number.i));
    case TYPE_UINT:
        if (rhs.number.u == 0) uint_divbyzero(lhs.number.u, rhs.number.u);
        return Value((Uint)(lhs.number.u % rhs.number.u));
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

static Uint ror1(Uint a) noexcept {
    const Uint lsb = a & 0x1;
    const Uint rest = a >> 1;
    const Uint result = (lsb << MY_BITMAX) | rest;
    return result;
}

static Uint ror(Uint a, Uint b) noexcept {
    for (Uint copy = 0; copy < b; copy++) {
        a = ror1(a);
    }
    return a;
}

static Uint rol1(Uint a) noexcept {
    const Uint msb = a >> MY_BITMAX;
    const Uint rest = a << 1;
    const Uint result = rest | msb;
    return result;
}

static Uint rol(Uint a, Uint b) noexcept {
    for (Uint copy = 0; copy < b; copy++) {
        a = rol1(a);
    }
    return a;
}

static Value binop_ror(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    switch (lhs.type) {
    case TYPE_FLOAT: {
        Value tmp = Value(ror((Uint)lhs.number.u, (Uint)rhs.number.f));
        tmp.pun(TYPE_FLOAT);
        return tmp;
    }
    case TYPE_INT:   return Value((Int)ror(lhs.number.u, (Uint)rhs.number.i));
    case TYPE_UINT:  return Value((Uint)ror(lhs.number.u, rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_rol(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    switch (lhs.type) {
    case TYPE_FLOAT: {
        Value tmp = Value(rol((Uint)lhs.number.u, (Uint)rhs.number.f));
        tmp.pun(TYPE_FLOAT);
        return tmp;
    }
    case TYPE_INT:   return Value((Int)rol(lhs.number.u, (Uint)rhs.number.i));
    case TYPE_UINT:  return Value((Uint)rol(lhs.number.u, rhs.number.u));
    default: break;
    }
    return lhs.unexpected_type();
}

#define FACTORIAL_CHK(out, value, check) \
do { \
    if (check) { \
        EPRINT("factorial: value must be nonnegative\n"); \
        exit(1); \
    } \
    decltype(out) my__tmp = 1; \
    for (size_t i = 2; i <= (size_t)(value); i++) { \
        my__tmp *= i; \
    } \
    out = my__tmp; \
} while (0)

#define FACTORIAL(out, value) FACTORIAL_CHK(out, value, (value < 0))
#define FACTORIAL_NO(out, value) FACTORIAL_CHK(out, value, false)

#define NCR_CHK(out, n, r, check, FACT) \
do { \
    if (check) { \
        EPRINT("ncr: requires n >= r >= 0\n"); \
        exit(1); \
    } \
    decltype(out) f__n, f__r, f__nr; \
    FACT(f__n, n); \
    FACT(f__r, r); \
    FACT(f__nr, n - r); \
    out = f__n / (f__r * f__nr); \
} while (0)

#define NCR(out, n, r) NCR_CHK(out, n, r, (r < 0) || (n < r), FACTORIAL)
#define NCR_NO(out, n, r) NCR_CHK(out, n, r, (n < r), FACTORIAL_NO)

#define NPR_CHK(out, n, r, check, FACT) \
do { \
    if (check) { \
        EPRINT("npr: requires n >= r >= 0\n"); \
        exit(1); \
    } \
    decltype(out) f__n, f__nr; \
    FACT(f__n, n); \
    FACT(f__nr, n - r); \
    out = f__n / f__nr; \
} while (0)

#define NPR(out, n, r) NPR_CHK(out, n, r, (r < 0) || (n < r), FACTORIAL)
#define NPR_NO(out, n, r) NPR_CHK(out, n, r, (n < r), FACTORIAL_NO)

static Value binop_ncr(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    switch (lhs.type) {
    case TYPE_FLOAT: {
        Float result;
        NCR(result, lhs.number.f, rhs.number.f);
        return Value(result);
    }
    case TYPE_INT: {
        Int result;
        NCR(result, lhs.number.i, rhs.number.i);
        return Value(result);
    }
    case TYPE_UINT: {
        Uint result;
        NCR_NO(result, lhs.number.u, rhs.number.u);
        return Value(result);
    }
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_npr(Value& lhs, Value& rhs) noexcept {
    lhs.coerce(rhs);
    switch (lhs.type) {
    case TYPE_FLOAT: {
        Float result;
        NPR(result, lhs.number.f, rhs.number.f);
        return Value(result);
    }
    case TYPE_INT: {
        Int result;
        NPR(result, lhs.number.i, rhs.number.i);
        return Value(result);
    }
    case TYPE_UINT: {
        Uint result;
        NPR_NO(result, lhs.number.u, rhs.number.u);
        return Value(result);
    }
    default: break;
    }
    return lhs.unexpected_type();
}

static Value binop_save(Value& val, Value& name) noexcept {
    if (name.type != TYPE_STRING) {
        return name.unexpected_type();
    }

    constant_save(name.number.s, val);
    return val;
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

static Value unop_quiet(Value& lhs) noexcept {
    (void)lhs;
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

static Value unop_ceil(Value& lhs) noexcept {
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)FLOAT_CEIL(lhs.number.f));
    case TYPE_INT:   return Value(lhs.number.i);
    case TYPE_UINT:  return Value(lhs.number.u);
    default: break;
    }
    return lhs.unexpected_type();
}

static Value unop_trunc(Value& lhs) noexcept {
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)FLOAT_TRUNC(lhs.number.f));
    case TYPE_INT:   return Value(lhs.number.i);
    case TYPE_UINT:  return Value(lhs.number.u);
    default: break;
    }
    return lhs.unexpected_type();
}

static Value unop_ord(Value &lhs) noexcept {
    char buf[512];
    switch (lhs.type) {
    case TYPE_FLOAT:
        snprintf(buf, sizeof(buf), FMT_FLOAT, lhs.number.f);
        return Value((Int)buf[0]);
    case TYPE_INT:
        snprintf(buf, sizeof(buf), FMT_INT, lhs.number.i);
        return Value((Int)buf[0]);
    case TYPE_UINT:
        snprintf(buf, sizeof(buf), FMT_UINT, lhs.number.u);
        return Value((Int)buf[0]);
    case TYPE_STRING:
        return Value((Int)lhs.number.s[0]); // ascii only :/
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
            (unsigned)fi.parts.sign, (unsigned)fi.parts.exponent, fi.parts.mantissa);
        fprintf(stdout, "%d, 0x%X, 0x" FMT_HEX "\n",
            (signed)fi.parts.sign, (unsigned)fi.parts.exponent, fi.parts.mantissa);
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
    case TYPE_FLOAT: return Value((Int)(fi.parts.sign));
    case TYPE_INT:   return Value((Int)(fi.parts.sign));
    case TYPE_UINT:  return Value((Int)(fi.parts.sign));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value unop_fexp(Value &lhs) noexcept {
    FloatInfo fi;
    fi.f = lhs.number.f;
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Int)(fi.parts.exponent));
    case TYPE_INT:   return Value((Int)(fi.parts.exponent));
    case TYPE_UINT:  return Value((Int)(fi.parts.exponent));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value unop_fmant(Value &lhs) noexcept {
    FloatInfo fi;
    fi.f = lhs.number.f;
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Int)(fi.parts.mantissa));
    case TYPE_INT:   return Value((Int)(fi.parts.mantissa));
    case TYPE_UINT:  return Value((Int)(fi.parts.mantissa));
    default: break;
    }
    return lhs.unexpected_type();
}

static Value unop_factorial(Value &lhs) noexcept {
    switch (lhs.type) {
    case TYPE_FLOAT: {
        Float result;
        FACTORIAL(result, lhs.number.f);
        return Value(result);
    }
    case TYPE_INT: {
        Int result;
        FACTORIAL(result, lhs.number.i);
        return Value(result);
    }
    case TYPE_UINT: {
        Uint result;
        FACTORIAL_CHK(result, lhs.number.u, false);
        return Value(result);
    }
    default: break;
    }
    return lhs.unexpected_type();
}

static Value unop_inverse(Value &lhs) noexcept {
    switch (lhs.type) {
    case TYPE_FLOAT: return Value((Float)FLOAT_POW(lhs.number.f, (Float)-1));
    case TYPE_INT:   return Value((Float)FLOAT_POW((Float)lhs.number.i, (Float)-1));
    case TYPE_UINT:  return Value((Float)FLOAT_POW((Float)lhs.number.u, (Float)-1));
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
    assert(value);
    number.s = value;
}

#ifndef NO_FLOAT
Value::Value(Float f) noexcept :
    number{0},
    type{TYPE_FLOAT},
    fmt{FORMAT_DEC}
{
    number.f = f;
}
#endif

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

#define LONG_PRINTF(prefix, longformat, format, value, end) \
do { \
    if (_longform) { \
        fprintf(stdout, prefix longformat "%s", value, end); \
    } else { \
        fprintf(stdout, prefix format "%s", value, end); \
    } \
} while (0)

static void do_print(Value& v, const char *end) noexcept {
    assert(end != NULL);
    switch (v.fmt) {
    case FORMAT_DEC:
        switch (v.type) {
        case TYPE_FLOAT:
            #ifdef FMT_LONG_FLOAT
                LONG_PRINTF("", FMT_LONG_FLOAT, FMT_FLOAT, v.number.f, end);
            #else
                fprintf(stdout, FMT_FLOAT "%s", v.number.f, end);
            #endif

            break;
        case TYPE_INT:
            fprintf(stdout, FMT_INT "%s", v.number.i, end);
            break;
        case TYPE_UINT:
            fprintf(stdout, FMT_UINT "%s", v.number.u, end);
            break;
        }
        break;
    case FORMAT_HEX:
    hex:
        LONG_PRINTF("0x", FMT_LONG_HEX, FMT_HEX, v.number.u, end);
        break;
    case FORMAT_OCT:
        LONG_PRINTF("0o", FMT_LONG_OCT, FMT_OCT, v.number.u, end);
        break;
    case FORMAT_BIN:
        print_binary(v.number.u);
        fprintf(stdout, "%s", end);
        break;
    case FORMAT_BIG:
        if (is_little_endian()) {
            print_reversed(v.number.u);
            fprintf(stdout, "%s", end);
            break;
        }
        else {
            goto hex;
        }
    case FORMAT_LITTLE:
        if (is_big_endian()) {
            print_reversed(v.number.u);
            fprintf(stdout, "%s", end);
            break;
        }
        else {
            goto hex;
        }
    case FORMAT_CHAR:
        switch (v.type) {
        case TYPE_FLOAT:
            fprintf(stdout, "%s%s", ascii_lookup(v.number.f), end);
            break;
        case TYPE_INT:
            fprintf(stdout, "%s%s", ascii_lookup(v.number.i), end);
            break;
        case TYPE_UINT:
            fprintf(stdout, "%s%s", ascii_lookup(v.number.u), end);
            break;
        }
        break;
    case FORMAT_TYPE:
        fprintf(stdout, "%s%s", typeTable[v.type], end);
        break;
    default:
        assert(0);
        break;
    }
    fflush(stdout);
}

void Value::print() noexcept {
    do_print(*this, " ");
}

void Value::println() noexcept {
    do_print(*this, "\n");
}

static void print_binary(Uint value) noexcept {
    int size = (int)sizeof(Uint) * 8 - 1;

    fprintf(stdout, "0b");
    fflush(stdout);
    for (int i = size; i >= 0; i--) {
        if (!_longform && ((Uint)(((Uint)1) << (Uint)i) > value)) {
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

#ifdef NO_FLOAT
#undef NO_FLOAT
#undef Float
#else
#undef FMT_LONG_FLOAT
#endif

#undef FMT_FLOAT
#undef FMT_INT
#undef FMT_UINT
#undef FMT_HEX
#undef FMT_OCT
#undef FMT_LONG_HEX
#undef FMT_LONG_OCT
#undef FLOAT_MOD
#undef FLOAT_POW
#undef FLOAT_SQRT
#undef FLOAT_SIN
#undef FLOAT_COS
#undef FLOAT_TAN
#undef FLOAT_FLOOR
#undef FLOAT_ROUND
#undef FLOAT_CEIL
#undef FLOAT_TRUNC
#undef FLOAT_LOG10
#undef FLOAT_LOG
#undef FLOAT_ZERO
#undef MY_INTMAX
#undef MY_UINTMAX
#undef MY_FLOATMAX
#undef MY_FLOATMIN
#undef MY_BITMAX
#undef MY_FMANTMASK
#undef MY_FEXPMASK
#undef MY_FEXPBIT
