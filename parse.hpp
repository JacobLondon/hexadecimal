#ifndef HD_PARSE_H
#define HD_PARSE_H

#include "token.hpp"

void *parser_create() noexcept;
void parser_destroy(void *self) noexcept;

/**
 * A tokenizer which is done tokenizing
 */
int parser_parse(void *self, void *tokenizer) noexcept;

Token *parser_gettok(void *self, void *cursor) noexcept;

#endif // HD_PARSE_H
