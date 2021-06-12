#ifndef HD_PARSE_H
#define HD_PARSE_H

struct Tokenizer;
Tokenizer *tokenizer_create(void) noexcept;
int tokenizer_lex(Tokenizer *self, char *text) noexcept;
void tokenizer_destroy(Tokenizer *self) noexcept;

#endif // HD_PARSE_H

