#define RPN_INCLUDE
#include "rpn.hh"

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
