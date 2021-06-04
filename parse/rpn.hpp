#ifndef HD_RPN_H
#define HD_RPN_H

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
