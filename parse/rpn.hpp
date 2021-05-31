#ifndef HD_RPN_H
#define HD_RPN_H

struct Rpn;

Rpn *rpn_new() noexcept;
void rpn_exec(Rpn *self) noexcept;
void rpn_push(Rpn *self, char *value) noexcept;
void rpn_print(Rpn *self) noexcept;
void rpn_free(Rpn *self) noexcept;

#endif // HD_RPN_H
