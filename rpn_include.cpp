#define RPN_INCLUDE
#include "rpn.hh"

/**
 * Shared Implementation
 */

#define EPRINT(...) \
do { \
    if (_verbose) { \
        fprintf(stderr, __VA_ARGS__); \
    } \
} while (0)

static std::regex *regex_unsigned;
static std::regex *regex_signed;
static std::regex *regex_float;
static std::regex *regex_binary;
static std::regex *regex_octal;
static std::regex *regex_hexadecimal;

void regex_init(void)
{
#define XINIT(RegP, Regexp) do { \
        if (!RegP) { \
            RegP = new (std::nothrow) std::regex(Regexp); \
            if (!RegP) { \
                EPRINT("Out of memory\n"); \
                exit(ENOMEM); \
            } \
        } \
    } while (0)

    XINIT(regex_unsigned, REG_UNSIGNED);
    XINIT(regex_signed, REG_SIGNED);
    XINIT(regex_float, REG_FLOAT);
    XINIT(regex_binary, REG_BIN);
    XINIT(regex_octal, REG_OCT);
    XINIT(regex_hexadecimal, REG_HEX);

#undef XINIT
}

void regex_cleanup(void)
{
#define XCLEANUP(RegP) do { \
        if (RegP) { \
            delete RegP; \
        } \
    } while (0)

    XCLEANUP(regex_unsigned);
    XCLEANUP(regex_signed);
    XCLEANUP(regex_float);
    XCLEANUP(regex_binary);
    XCLEANUP(regex_octal);
    XCLEANUP(regex_hexadecimal);

#undef XCLEANUP
}

/**
 * Sized Implementation
 */

namespace Rpn8 {
    #define RPN_8BITS
    #include "rpn.cc"
    #undef RPN_8BITS
}

namespace Rpn16 {
    #define RPN_16BITS
    #include "rpn.cc"
    #undef RPN_16BITS
}

namespace Rpn32 {
    #define RPN_32BITS
    #include "rpn.cc"
    #undef RPN_32BITS
}

namespace Rpn64 {
    #define RPN_64BITS
    #include "rpn.cc"
    #undef RPN_64BITS
}
