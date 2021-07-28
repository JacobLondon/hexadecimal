#ifndef HD_TOKEN_H
#define HD_TOKEN_H

#include <string>

enum TokenizerToken {
    TOKEN_UNKN,
    TOKEN_VALUE,
    TOKEN_UNOP,
    TOKEN_BINOP,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_EOF,
    TOKEN_COUNT
};

struct Token {
    std::string value;
    TokenizerToken token;
    int precedence;
    Token();
    Token(const char *value, TokenizerToken token, int precedence);
    Token(std::string&& value, TokenizerToken token, int precedence);
};

void *tokenizer_create(void) noexcept;
void tokenizer_destroy(void *self) noexcept;

/**
 * Tokenize the text and hold the state in the tokenizer.
 * May be called multiple times to keep appending tokens.
 * 
 * @return
 *   0 on success
 *   1 on failure
 */
int tokenizer_tokenize(void *self, char *text, bool verbose) noexcept;

/**
 * Insert an EOF character at the end. Further tokenize calls will be
 * ignored.
 */
void tokenizer_end(void *self) noexcept;

/**
 * Get a shared pointer to the next available token. Must have
 * called @ref tokenizer_tokenize at least once
 * 
 * @param cursor
 *   Pass in a value set to zero, and continue to pass a ref
 *   to that value in to continue the traversal until EOF
 */
Token *tokenizer_gettok(void *self, size_t *cursor) noexcept;

#endif // HD_TOKEN_H
