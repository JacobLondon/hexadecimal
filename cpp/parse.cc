#include <new>
#include <list>
#include <assert.h>
#include "parse.hpp"

enum CollectType {
    COLLECT_UNOP,
    COLLECT_BINOP,
    COLLECT_PAREN,
};

struct Btree {
    Token *token;
    Btree *parent;
    std::list<Btree *> children;

    Btree() :
        token(nullptr),
        parent(nullptr),
        children{},
    Btree(Btree *parent, Token *token) :
        token(token),
        parent(parent),
        children{}

    int process() noexcept;
    // collect at ndx and insert child node, return shared pointer
    // ndx is left-most item in the set, (, or LHS of BINOP, or UNOP
    Btree *collect(size_t ndx, CollectType type) noexcept;
};

struct Parser {
    Btree *root;
    Parser() :
        root(nullptr)
    {}
};

static const Token ptok_VAL{"", TOKEN_VALUE, 0};
static const char *geterror() noexcept;
static int errcode = 0;

void *parser_create() noexcept {
    Parser *self = new (std::nothrow) Parser{};
    assert(self);
    self->root = new (std::nothrow) Btree{};
    assert(self->root);
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
    size_t cursor;
    Token *tmp;
    Btree *tree;
    Parser *me = (Parser *)self;

    assert(self);
    assert(tokenizer);

    cursor = 0;
    while (1) {
        tmp = tokenizer_gettok(tokenizer, &cursor);
        if (!tmp) {
            break;
        }
        tree = new (std::nothrow) Btree{me->root, tmp};
        assert(tree);
        me->root->children.push_back(tree);
    }

    me->root->process();


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

int Btree::process() noexcept {
    Btree *tmp;
    // NULL term array of NULL term array of Token pointers, all in precedence order
    Token ***ptoks = tokenizer_getprec();
    Token *want;
    CollectType coll;
    // TODO: Create a collection only of used symbols instead of all
    for (size_t i = 0; ptoks[i] != NULL; i++) {
        for (size_t j = 0; ptoks[i][j] != NULL; j++) {
            want = ptoks[i][j];
            size_t ndx = 0;
            for (auto it = this->children.begin(); it != this->children.end(); it++, ndx++) {
                if (strcmp(want->value, it->token->value) == 0) {
                    if (it->token->token == TOKEN_UNOP) {
                        tmp = this->collect(ndx, COLLECT_UNOP);
                        // TODO
                    }
                    else if (it->token->token == TOKEN_BINOP) {
                        tmp = this->collect(ndx - 1, COLLECT_BINOP);
                        // TODO
                    }
                    else if (it->token->token == TOKEN_LPAREN) {
                        tmp = this->collect(ndx, COLLECT_PAREN);
                        // TODO
                    }
                    else {
                        // TODO: Some failure
                    }
                }
            }
        }
    }
}

Btree *Btree::collect(size_t ndx, CollectType type) noexcept {
    if (this->children.size() <= ndx) {
        errcode = 1;
        return NULL;
    }

    std::list<Btree *>& c = this->children;
    size_t endndx;
    switch (type) {
        case COLLECT_UNOP:
            if (c.size() - ndx < 2) {
                errcode = 2;
            }
            else if (c[ndx]->token != TOKEN_UNOP) {
                errcode = 3;
                return NULL;
            }
            else if (c[ndx + 1]->token != TOKEN_VALUE) {
                errcode = 4;
                return NULL;
            }
            endndx = ndx + 1;
            break;
        case COLLECT_BINOP:
            if (c.size() - ndx < 3) {
                errcode = 5;
                return NULL;
            }
            else if (c[ndx]->token != TOKEN_VALUE) {
                errcode = 6;
                return NULL;
            }
            else if (c[ndx + 1]->token != TOKEN_BINOP) {
                errcode = 7;
                return NULL;
            }
            else if (c[ndx + 2]->token != TOKEN_VALUE) {
                errcode = 8;
                return NULL;
            }
            endndx = ndx + 2;
            break;
        case COLLECT_PAREN: {
            // skip first paren, should be there anyway
            auto it = this->children.begin() + ndx + 1;
            size_t pcount = 1;
            endndx = ndx;
            for ( ; it != this->children.end(); it++, endndx++) {
                if (pcount = 0) {
                    break;
                }

                if (it->token->token == TOKEN_LPAREN) {
                    pcount++;
                }
                else if (it->token->token == TOKEN_RPAREN) {
                    pcount--;
                }
            }

            if (pcount != 0) {
                errcode = 9;
                return NULL;
            }
            break;
        }
    }

    Btree *f = new (std::nothrow) Btree{};
    assert(f);
    f->parent = this;
    f->token = &ptok_VAL;

    f->children.splice(
        f->children.begin(),
        this->children,
        this->children.begin() + ndx,
        this->children.begin() + endndx + 1
    );
    this->children.insert(
        this->children.begin() + ndx,
        f
    );
    return f;
}

static const char *geterror() noexcept {
    static const char *codes[] = {
        /* 1 */ "Collect index out-of-bounds",
        /* 2 */ "Collect can't find enough children",
        /* 3 */ "Collect found unbalanced parens",
    };

    return codes[errcode];
}

