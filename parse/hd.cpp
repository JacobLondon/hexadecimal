#include <vector>
#include "rpn.hpp"



int main(int argc, char **argv)
{
    std::vector<Node *> nodes;
    nodes.push_back(rpn_new((char *)"200.1"));
    nodes.push_back(rpn_new((char *)"128"));
    nodes.push_back(rpn_new((char *)"64"));
    nodes.push_back(rpn_new((char *)"+"));
    nodes.push_back(rpn_new((char *)"-"));

    for (Node *n : nodes) {
        n->exec(n);
    }

    rpn_print();

    for (Node *n : nodes) {
        rpn_free(n);
    }

    return 0;
}
