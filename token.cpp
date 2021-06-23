#include <new>
#include <regex>
#include <vector>
#include <assert.h>
#include "token.hpp"
#include "rpn.hpp"

/* Operator precedence based on:
 *    C for functions and unary / casting
 *     https://en.cppreference.com/w/c/language/operator_precedence
 *   Golang for operators
 *     https://golang.org/ref/spec
 */

/**
 * -1: error
 *  0: don't look here
 * >0: The value
 */
static int tokPrecedenceTable[] = {
    /* TOKEN_UNKN */ -1,
    /* TOKEN_VALUE */ 1,
    /* TOKEN_UNOP */ 0,
    /* TOKEN_BINOP */ 0,
    /* TOKEN_LPAREN */ 500,
    /* TOKEN_RPAREN */ 500,
    /* TOKEN_EOF */ 2,
    /* TOKEN_COUNT */ -1,
};

/* 
 * All UNOPs are prefix and all BINOPs are infix, left to right
 */
#define TK(prec, str, tok) {str, tok, prec}

// functions
static Token tok_SQRT               TK(400, REG_OP_SQRT, TOKEN_UNOP);
static Token tok_SIN                TK(400, REG_OP_SIN, TOKEN_UNOP);
static Token tok_COS                TK(400, REG_OP_COS, TOKEN_UNOP);
static Token tok_TAN                TK(400, REG_OP_TAN, TOKEN_UNOP);
static Token tok_ABS                TK(400, REG_OP_ABS, TOKEN_UNOP);
static Token tok_SGN                TK(400, REG_OP_SGN, TOKEN_UNOP);
static Token tok_FLOOR              TK(400, REG_OP_FLOOR, TOKEN_UNOP);
static Token tok_ROUND              TK(400, REG_OP_ROUND, TOKEN_UNOP);
static Token tok_GCD                TK(400, REG_OP_GCD, TOKEN_BINOP);
static Token tok_LCM                TK(400, REG_OP_LCM, TOKEN_BINOP);
// unary
static Token tok_NOT_SYM            TK(370, REG_OP_NOT_SYM, TOKEN_UNOP);
static Token tok_NOT                TK(370, REG_OP_NOT, TOKEN_UNOP);
static Token tok_INV_SYM            TK(370, REG_OP_INV_SYM, TOKEN_UNOP);
static Token tok_INV                TK(370, REG_OP_INV, TOKEN_UNOP);
// conversion
static Token tok_CAST               TK(350, REG_OP_CAST, TOKEN_BINOP);
static Token tok_AS                 TK(350, REG_OP_AS, TOKEN_BINOP);
// multiplication
static Token tok_POW_SYM            TK(300, REG_OP_POW_SYM, TOKEN_BINOP);
static Token tok_POW                TK(300, REG_OP_POW, TOKEN_BINOP);
static Token tok_MUL_SYM            TK(295, REG_OP_MUL_SYM, TOKEN_BINOP);
static Token tok_MUL                TK(295, REG_OP_MUL, TOKEN_BINOP);
static Token tok_DIV_SYM            TK(295, REG_OP_DIV_SYM, TOKEN_BINOP);
static Token tok_DIV                TK(295, REG_OP_DIV, TOKEN_BINOP);
static Token tok_MOD_SYM            TK(295, REG_OP_MOD_SYM, TOKEN_BINOP);
static Token tok_MOD                TK(295, REG_OP_MOD, TOKEN_BINOP);
static Token tok_SHL_SYM            TK(295, REG_OP_SHL_SYM, TOKEN_BINOP);
static Token tok_SHL                TK(295, REG_OP_SHL, TOKEN_BINOP);
static Token tok_SHR_SYM            TK(295, REG_OP_SHR_SYM, TOKEN_BINOP);
static Token tok_SHR                TK(295, REG_OP_SHR, TOKEN_BINOP);
static Token tok_BITAND_SYM         TK(295, REG_OP_BITAND_SYM, TOKEN_BINOP);
static Token tok_BITAND             TK(295, REG_OP_BITAND, TOKEN_BINOP);
static Token tok_BITANDINV_SYM      TK(295, REG_OP_BITANDINV_SYM, TOKEN_BINOP);
static Token tok_BITANDINV          TK(295, REG_OP_BITANDINV, TOKEN_BINOP);
// addition
static Token tok_ADD_SYM            TK(250, REG_OP_ADD_SYM, TOKEN_BINOP);
static Token tok_ADD                TK(250, REG_OP_ADD, TOKEN_BINOP);
static Token tok_SUB_SYM            TK(250, REG_OP_SUB_SYM, TOKEN_BINOP);
static Token tok_SUB                TK(250, REG_OP_SUB, TOKEN_BINOP);
static Token tok_XOR_SYM            TK(250, REG_OP_XOR_SYM, TOKEN_BINOP);
static Token tok_XOR                TK(250, REG_OP_XOR, TOKEN_BINOP);
static Token tok_BITOR_SYM          TK(250, REG_OP_BITOR_SYM, TOKEN_BINOP);
static Token tok_BITOR              TK(250, REG_OP_BITOR, TOKEN_BINOP);
// equality and relational
static Token tok_EQ_SYM             TK(200, REG_OP_EQ_SYM, TOKEN_BINOP);
static Token tok_EQ                 TK(200, REG_OP_EQ, TOKEN_BINOP);
static Token tok_NEQ_SYM            TK(200, REG_OP_NEQ_SYM, TOKEN_BINOP);
static Token tok_NEQ                TK(200, REG_OP_NEQ, TOKEN_BINOP);
static Token tok_GT_SYM             TK(200, REG_OP_GT_SYM, TOKEN_BINOP);
static Token tok_GT                 TK(200, REG_OP_GT, TOKEN_BINOP);
static Token tok_GTE_SYM            TK(200, REG_OP_GTE_SYM, TOKEN_BINOP);
static Token tok_GTE                TK(200, REG_OP_GTE, TOKEN_BINOP);
static Token tok_LT_SYM             TK(200, REG_OP_LT_SYM, TOKEN_BINOP);
static Token tok_LT                 TK(200, REG_OP_LT, TOKEN_BINOP);
static Token tok_LTE_SYM            TK(200, REG_OP_LTE_SYM, TOKEN_BINOP);
static Token tok_LTE                TK(200, REG_OP_LTE, TOKEN_BINOP);
// and
static Token tok_AND_SYM            TK(190, REG_OP_AND_SYM, TOKEN_BINOP);
static Token tok_AND                TK(190, REG_OP_AND, TOKEN_BINOP);
// or
static Token tok_OR_SYM             TK(180, REG_OP_OR_SYM, TOKEN_BINOP);
static Token tok_OR                 TK(180, REG_OP_OR, TOKEN_BINOP);
// sep
static Token tok_END_SYM            TK(130, REG_OP_END_SYM, TOKEN_UNOP);
static Token tok_END                TK(130, REG_OP_END, TOKEN_UNOP);
static Token tok_SEP_SYM            TK(130, REG_OP_SEP_SYM, TOKEN_UNOP);
static Token tok_SEP                TK(130, REG_OP_SEP, TOKEN_UNOP);

static Token *tokenTable[] = {
    &tok_BITANDINV_SYM,
    &tok_BITANDINV,
    &tok_AND_SYM,
    &tok_AND,
    &tok_OR_SYM,
    &tok_OR,

    &tok_POW_SYM,
    &tok_POW,
    &tok_SHL_SYM,
    &tok_SHL,
    &tok_SHR_SYM,
    &tok_SHR,

    &tok_EQ_SYM,
    &tok_EQ,
    &tok_NEQ_SYM,
    &tok_NEQ,
    &tok_GTE_SYM,
    &tok_GTE,
    &tok_LTE_SYM,
    &tok_LTE,

    &tok_ADD_SYM,
    &tok_ADD,
    &tok_SUB_SYM,
    &tok_SUB,
    &tok_MUL_SYM,
    &tok_MUL,
    &tok_DIV_SYM,
    &tok_DIV,
    &tok_MOD_SYM,
    &tok_MOD,
    &tok_XOR_SYM,
    &tok_XOR,
    &tok_BITAND_SYM,
    &tok_BITAND,
    &tok_BITOR_SYM,
    &tok_BITOR,
    &tok_NOT_SYM,
    &tok_NOT,
    &tok_INV_SYM,
    &tok_INV,

    &tok_GT_SYM,
    &tok_GT,
    &tok_LT_SYM,
    &tok_LT,
    &tok_END_SYM,
    &tok_END,
    &tok_SEP_SYM,
    &tok_SEP,
    &tok_CAST,
    &tok_AS,
    &tok_SQRT,
    &tok_GCD,
    &tok_LCM,
    &tok_SIN,
    &tok_COS,
    &tok_TAN,
    &tok_ABS,
    &tok_SGN,
    &tok_FLOOR,
    &tok_ROUND,
    NULL
};

/**
 * Create a token, return the length of the token created, or <0 on failure
 */
static int tok_lookup(char *text, Token *out) noexcept;

Token::Token() :
    value(""),
    token(TOKEN_EOF),
    precedence(tokPrecedenceTable[TOKEN_EOF])
{

}

Token::Token(char *value, TokenizerToken token, int precedence) :
    value(value),
    token(token),
    precedence(precedence)
{}

Token::Token(std::string&& value, TokenizerToken token, int precedence) :
    value(value),
    token(token),
    precedence(0)
{}

struct Tokenizer {
    std::vector<Token> tokens;
};

void *tokenizer_create(void) noexcept {
    Tokenizer *self = (Tokenizer *) new (std::nothrow) Tokenizer{};
    return (void *)self;
}

void tokenizer_destroy(void *self) noexcept {
    Tokenizer *me = (Tokenizer *)self;
    assert(self);
    delete me;
}

int tokenizer_tokenize(void *self, char *text, bool verbose) noexcept {
    Tokenizer *me = (Tokenizer *)self;
    Token current;
    char *p;
    int rv;

    assert(self);
    assert(text);

    p = text;
    while (*p) {
        while (*p && (isspace(*p) || iscntrl(*p))) {
            p++;
        }
        if (!*p) {
            break;
        }

        rv = tok_lookup(p, &current);
        if (rv < 0) {
            if (verbose) {
                fprintf(stderr, "Unexpected token at: %s\n", p);
            }
            return 1;
        }

        p = &p[rv];
        me->tokens.push_back(current);
    }

    return 0;
}

void tokenizer_end(void *self) noexcept {
    Tokenizer *me = (Tokenizer *)self;
    assert(self);

    me->tokens.emplace_back();
}

Token *tokenizer_gettok(void *self, size_t *cursor) noexcept {
    Tokenizer *me = (Tokenizer *)self;
    assert(self);
    assert(cursor);

    size_t size = me->tokens.size();
    if (size == 0) {
        return NULL;
    }

    if (*cursor >= size) {
        *cursor = size - 1;
    }

    Token *token = &me->tokens[*cursor];
    *cursor = *cursor + 1;
    return token;
}

static int tok_lookup(char *text, Token *out) noexcept {
    static std::regex re = std::regex(REG_TOKEN);
    assert(text);
    assert(out);

    if (text[0] == '(') {
        *out = Token{"(", TOKEN_LPAREN, tokPrecedenceTable[TOKEN_LPAREN]};
        return 1;
    }
    else if (text[0] == ')') {
        *out = Token{")", TOKEN_RPAREN, tokPrecedenceTable[TOKEN_RPAREN]};
        return 1;
    }

    for (int i = 0; tokenTable[i] != NULL; i++) {
        if (strncmp(tokenTable[i]->value.c_str(), text, tokenTable[i]->value.size()) == 0) {
            *out = *tokenTable[i];
            return (int)out->value.size();
        }
    }

    std::string s = std::string(text);
    std::smatch m;
    if (std::regex_search(s, m, re)) {
        size_t size = m.length();
        *out = Token{std::move(s.substr(0, size)), TOKEN_VALUE, 1};

        // clean up extraneous characters
        for (size_t i = 0; i < size; i++) {
            if (isspace(out->value[i]) || iscntrl(out->value[i])) {
                out->value[i] = '\0';
                break;
            }
        }
        return (int)m.length();
    }

    return -1;
}
