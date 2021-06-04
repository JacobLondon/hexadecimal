#define RPN_INCLUDE
#include "rpn.hh"

namespace Rpn32 {

#define RPN_32BITS
#include "rpn.cc"
#undef RPN_32BITS

}
