#include <vector>
#include "rpn.hpp"

int main(int argc, char **argv)
{
    Rpn *calc = rpn_new();

    rpn_push(calc, (char *)"200.1");
    rpn_push(calc, (char *)"128");
    rpn_push(calc, (char *)"64");
    rpn_push(calc, (char *)"+");
    rpn_push(calc, (char *)"-");

    rpn_exec(calc);
    rpn_print(calc);
    rpn_free(calc);

    return 0;
}
