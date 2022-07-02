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
static std::regex *regex_binary_post;
static std::regex *regex_octal;
static std::regex *regex_octal_pre;
static std::regex *regex_octal_post;
static std::regex *regex_hexadecimal;
static std::regex *regex_hexadecimal_post;

void regex_init(void)
{
#define XINIT(RegP, Regexp) do { \
        if (!RegP) { \
            RegP = new (std::nothrow) std::regex(Regexp); \
            if (!RegP) { \
                EPRINT("regex: out of memory\n"); \
                exit(ENOMEM); \
            } \
        } \
    } while (0)

    XINIT(regex_unsigned, REG_UNSIGNED);
    XINIT(regex_signed, REG_SIGNED);
    XINIT(regex_float, REG_FLOAT);
    XINIT(regex_binary, REG_BIN);
    XINIT(regex_binary_post, REG_BIN_POST);
    XINIT(regex_octal, REG_OCT);
    XINIT(regex_octal_pre, REG_OCT_PRE);
    XINIT(regex_octal_post, REG_OCT_POST);
    XINIT(regex_hexadecimal, REG_HEX);
    XINIT(regex_hexadecimal_post, REG_HEX_POST);

#undef XINIT
}

void regex_cleanup(void)
{
#define XCLEANUP(RegP) do { \
        if (RegP) { \
            delete RegP; \
            RegP = nullptr; \
        } \
    } while (0)

    XCLEANUP(regex_unsigned);
    XCLEANUP(regex_signed);
    XCLEANUP(regex_float);
    XCLEANUP(regex_binary);
    XCLEANUP(regex_binary_post);
    XCLEANUP(regex_octal);
    XCLEANUP(regex_octal_pre);
    XCLEANUP(regex_octal_post);
    XCLEANUP(regex_hexadecimal);
    XCLEANUP(regex_hexadecimal_post);

#undef XCLEANUP
}

bool regex_is_initialized(void)
{
    return
        regex_unsigned &&
        regex_signed &&
        regex_float &&
        regex_binary &&
        regex_binary_post &&
        regex_octal &&
        regex_octal_pre &&
        regex_octal_post &&
        regex_hexadecimal &&
        regex_hexadecimal_post;
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
