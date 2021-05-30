#ifndef HD_RPN_H
#define HD_RPN_H

struct Node {
    void (* exec)(Node *self);
};

Node *rpn_new(char *value) noexcept;
void rpn_free(Node *self) noexcept;
void rpn_exec(void) noexcept;
void rpn_print(void) noexcept;

#endif // HD_RPN_H
