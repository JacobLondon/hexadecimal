#include <new>
#include <assert.h>
#include "parse.hpp"

struct Btree {
    Token *token;
    Btree *left;
    Btree *right;
    Btree *parent;
};

struct Parser {
    Btree *root;
    Parser() :
        root(new Btree{})
    {}
};

static Btree *btree_insert(Btree *parent, Token *shared) noexcept;

void *parser_create() noexcept {
    Parser *self = new (std::nothrow) Parser{};
    return self;
}

void parser_destroy(void *self) noexcept {
    assert(self);
    delete (Parser *)self;
}

int parser_parse(void *self, void *tokenizer) noexcept {
    enum ParseTok {
        PARSE_NONE = 0x0,
        PARSE_VALUE = 0x1,
        PARSE_UNOP = 0x2,
        PARSE_BINOP = 0x4,
        PARSE_LPAREN = 0x8,
        PARSE_RPAREN = 0x10,
        PARSE_EOF = 0x20,
    };

    int current = PARSE_NONE;
    int want = PARSE_NONE;
    size_t cursor = 0;
    Token *tmp;
    Parser *me = (Parser *)self;

    assert(self);
    assert(tokenizer);

    want = PARSE_VALUE | PARSE_UNOP | PARSE_LPAREN;
    while (1) {
        tmp = tokenizer_gettok(tokenizer, &cursor);
        if (!tmp) {
            break;
        }

        switch (tmp->token) {
        case TOKEN_VALUE:   current = PARSE_VALUE;  break;
        case TOKEN_UNOP:    current = PARSE_UNOP;   break;
        case TOKEN_BINOP:   current = PARSE_BINOP;  break;
        case TOKEN_LPAREN:  current = PARSE_LPAREN; break;
        case TOKEN_RPAREN:  current = PARSE_RPAREN; break;
        case TOKEN_EOF:     current = PARSE_EOF;    break;
        case TOKEN_UNKN: // fallthrough
        case TOKEN_COUNT:
            goto unexpected;
        }

        if (!(current & want)) {
            goto unexpected;
        }

        //if ()

    }

    return 0;

unexpected:
    if (tmp) {
        fprintf(stderr, "Unexpected token '%s'\n", tmp->value.c_str());
    }
    else {
        fprintf(stderr, "Unexpected EOF\n");
    }
    return 1;
}

Token *parser_gettok(void *self, void *cursor) noexcept {
    assert(self);
    assert(cursor);
}
