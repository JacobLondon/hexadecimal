#ifndef HD_RPN_H
#define HD_RPN_H

#define REG_BIN R"(0[bB][01]+)"
#define REG_OCT R"(0[oO][01234567]+)"
#define REG_HEX R"(0[xX][0123456789aAbBcCdDeEfF]+)"
#define REG_DEC R"([+-]?[123456789][1234567890]*(\.[0123456789]+)?)"
#define REG_NUM "^" REG_BIN "|" REG_OCT "|" REG_HEX "|" REG_DEC "$"

#define REG_UNSIGNED R"([123456789][0123456789]*)"
#define REG_SIGNED R"([+-][123456789][0123456789]*)"
#define REG_FLOAT R"([+-]?[123456789][1234567890]*\.[0123456789]*)"

struct RpnVtable {
    void *(* create)() noexcept;
    void (* exec)(void *self) noexcept;
    void (* push)(void *self, char *value) noexcept;
    void (* print)(void *self) noexcept;
    void (* destroy)(void *self) noexcept;
    void (* set_verbose)(bool verbose) noexcept;
    void (* help)() noexcept;
};

#define RPN_VTABLE(Bits) RpnVtable{ \
    (void *(*)() noexcept)Rpn ##Bits::rpn_create, \
    (void (*)(void *) noexcept)Rpn ##Bits::rpn_exec, \
    (void (*)(void *, char *) noexcept)Rpn ##Bits::rpn_push, \
    (void (*)(void *) noexcept)Rpn ##Bits::rpn_print, \
    (void (*)(void *) noexcept)Rpn ##Bits::rpn_destroy, \
    (void (*)(bool) noexcept)Rpn ##Bits::rpn_set_verbose, \
    (void (*)() noexcept)Rpn ##Bits::rpn_help, \
}

namespace Rpn64 {

struct Rpn;
Rpn *rpn_create() noexcept;
void rpn_exec(Rpn *self) noexcept;
void rpn_push(Rpn *self, char *value) noexcept;
void rpn_print(Rpn *self) noexcept;
void rpn_destroy(Rpn *self) noexcept;
void rpn_set_verbose(bool verbose) noexcept;
void rpn_help() noexcept;

}

namespace Rpn32 {

struct Rpn;
Rpn *rpn_create() noexcept;
void rpn_exec(Rpn *self) noexcept;
void rpn_push(Rpn *self, char *value) noexcept;
void rpn_print(Rpn *self) noexcept;
void rpn_destroy(Rpn *self) noexcept;
void rpn_set_verbose(bool verbose) noexcept;
void rpn_help() noexcept;

}

#endif // HD_RPN_H
