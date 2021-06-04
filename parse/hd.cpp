#include <vector>
#include "rpn.hpp"

int main(int argc, char **argv)
{
    rpn_set_verbose(true);
    Rpn *calc = rpn_new();

    for (int i = 1; i < argc; i++) {
        rpn_push(calc, (char *)argv[i]);
    }

    rpn_exec(calc);
    rpn_print(calc);
    rpn_free(calc);

    return 0;
}
