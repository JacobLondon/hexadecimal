#ifndef HD_RPN_H
#define HD_RPN_H

#define REG_BIN "(0[bB][01]+)"
#define REG_OCT "(0[oO][01234567]+)"
#define REG_HEX "(0[xX][0123456789aAbBcCdDeEfF]+)"
#define REG_DEC "([+-]?((0)|([123456789][1234567890]*)))"

#define REG_CONSTANT "pi|e|inf|nan"
#define REG_UNSIGNED "(0|([123456789][0123456789]*))"
#define REG_SIGNED "([+-](0|([123456789][0123456789]*)))"
#define REG_FLOAT "([+-]?((0?\\.[0123456789]+)|((0\\.)|([123456789][0123456789]*\\.[0123456789]*))))"
#define REG_NUMBER "^(" REG_BIN "|" REG_OCT "|" REG_HEX "|" REG_FLOAT "|" REG_DEC "|" REG_CONSTANT ")"
#define REG_TYPE "int|uint|float|string"
#define REG_FORMAT "dec|hex|oct|bin|big|little"
#define REG_TOKEN REG_NUMBER "|" "(" REG_TYPE "|" REG_FORMAT ")" "[\\s\\n\\r]*"
#define REG_PIVOT REG_NUMBER "$"

#define REG_OP_BITANDINV_SYM "&~"
#define REG_OP_BITANDINV "bitandinv"
#define REG_OP_AND_SYM "&&"
#define REG_OP_AND "and"
#define REG_OP_OR_SYM "||"
#define REG_OP_OR "or"

#define REG_OP_POW_SYM "**"
#define REG_OP_POW "pow"
#define REG_OP_SHL_SYM "<<"
#define REG_OP_SHL "shl"
#define REG_OP_SHR_SYM ">>"
#define REG_OP_SHR "shr"

#define REG_OP_EQ_SYM "=="
#define REG_OP_EQ "eq"
#define REG_OP_NEQ_SYM "!="
#define REG_OP_NEQ "neq"
#define REG_OP_GTE_SYM ">="
#define REG_OP_GTE "gte"
#define REG_OP_LTE_SYM "<="
#define REG_OP_LTE "lte"

#define REG_OP_ADD_SYM "+"
#define REG_OP_ADD "add"
#define REG_OP_SUB_SYM "-"
#define REG_OP_SUB "sub"
#define REG_OP_MUL_SYM "*"
#define REG_OP_MUL "mul"
#define REG_OP_DIV_SYM "/"
#define REG_OP_DIV "div"
#define REG_OP_MOD_SYM "%"
#define REG_OP_MOD "mod"
#define REG_OP_XOR_SYM "^"
#define REG_OP_XOR "xor"
#define REG_OP_BITAND_SYM "&"
#define REG_OP_BITAND "bitand"
#define REG_OP_BITOR_SYM "|"
#define REG_OP_BITOR "bitor"
#define REG_OP_NOT_SYM "!"
#define REG_OP_NOT "not"
#define REG_OP_INV_SYM "~"
#define REG_OP_INV "inv"

#define REG_OP_GT_SYM ">"
#define REG_OP_GT "gt"
#define REG_OP_LT_SYM "<"
#define REG_OP_LT "lt"
#define REG_OP_END_SYM ";"
#define REG_OP_END "end"
#define REG_OP_SEP_SYM ","
#define REG_OP_SEP "sep"
#define REG_OP_CAST "cast"
#define REG_OP_AS "as"
#define REG_OP_SQRT "sqrt"
#define REG_OP_GCD "gcd"
#define REG_OP_LCM "lcm"
#define REG_OP_SIN "sin"
#define REG_OP_COS "cos"
#define REG_OP_TAN "tan"
#define REG_OP_ABS "abs"
#define REG_OP_SGN "sgn"
#define REG_OP_FLOOR "floor"
#define REG_OP_ROUND "round"

struct RpnVtable {
    void *(* create)() noexcept;
    void (* exec)(void *self) noexcept;
    void (* push)(void *self, char *value) noexcept;
    void (* print)(void *self) noexcept;
    void (* destroy)(void *self) noexcept;
    void (* help)() noexcept;
};

#define RPN_VTABLE(Bits) RpnVtable{ \
    (void *(*)() noexcept)Rpn ##Bits::rpn_create, \
    (void (*)(void *) noexcept)Rpn ##Bits::rpn_exec, \
    (void (*)(void *, char *) noexcept)Rpn ##Bits::rpn_push, \
    (void (*)(void *) noexcept)Rpn ##Bits::rpn_print, \
    (void (*)(void *) noexcept)Rpn ##Bits::rpn_destroy, \
    (void (*)() noexcept)Rpn ##Bits::rpn_help, \
}

namespace Rpn64 {

struct Rpn;
Rpn *rpn_create() noexcept;
void rpn_exec(Rpn *self) noexcept;
void rpn_push(Rpn *self, char *value) noexcept;
void rpn_print(Rpn *self) noexcept;
void rpn_destroy(Rpn *self) noexcept;
void rpn_help() noexcept;

}

namespace Rpn32 {

struct Rpn;
Rpn *rpn_create() noexcept;
void rpn_exec(Rpn *self) noexcept;
void rpn_push(Rpn *self, char *value) noexcept;
void rpn_print(Rpn *self) noexcept;
void rpn_destroy(Rpn *self) noexcept;
void rpn_help() noexcept;

}

extern bool _verbose;

#endif // HD_RPN_H
