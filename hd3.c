#include <stdlib.h>
#include <string.h>

struct Type {
    const char *name;
    const char *interp; // which fmt to turn this into a string?
    const char *prefix; // if do_interpret, this is the prefix to check for, case insensitive
    const char *bkstr; // backup thing to look for to match the type if prefix is NULL
    const char *switch_da; // command line switch to force its use
    const char *switch_ddarg; // command line switch to force its use
    int (* can_interpret)(struct Type *self, char *string);
    int (* from_string)(struct Type *self, char *string, int do_interpret);
    int (* to_string)(struct Type *self, char *out, size_t n, char *typename);
    const char *(* get_info)(struct Type *self);

    // uninitialized
    size_t value;
};

typedef int (* is_func)(int);

struct Type Decimal = {
    .name="dec",
    .interp="hex",
    .prefix="0d",
    .bkstr=NULL,
    .switch_da="-D",
    .switch_ddarg="--decimal",
};

struct Type Hexadecimal = {
    .name="hex",
    .interp="dec",
    .prefix="0x",
    .bkstr=NULL,
    .switch_da="-H",
    .switch_ddarg="--hexadecimal",
};

struct Type Octal = {
    .name="oct",
    .interp="dec",
    .prefix="0o",
    .bkstr=NULL,
    .switch_da="-O",
    .switch_ddarg="--octal",
};

struct Type Binary = {
    .name="bin",
    .interp="dec",
    .prefix="0b",
    .bkstr=NULL,
    .switch_da="-B",
    .switch_ddarg="--binary",
};

struct Type Char = {
    .name="char",
    .interp="dec",
    .prefix=NULL,
    .bkstr=NULL,
    .switch_da="-C",
    .switch_ddarg="--char",
};

struct Type Float = {
    .name="flt",
    .interp="hex",
    .prefix=NULL,
    .bkstr="f",
    .switch_da="-F",
    .switch_ddarg="--float",
};

struct Type Double = {
    .name="dbl",
    .interp="hex",
    .prefix=NULL,
    .bkstr=".",
    .switch_da="-Z",
    .switch_ddarg="--double",
};

struct Type *typelist[] = {
    &Decimal,
    &Hexadecimal,
    &Octal,
    &Binary,
    &Char,
    &Float,
    &Double,
    NULL,
};

static int main_conv(int argc, char **argv);
static struct Type *type_get_str(char *name);
static struct Type *type_get_converts(char *string);
static const char *type_get_fmt(char *name);
static char *arg_get(int argc, char **argv, const char *da, const char *ddarg);
static char *arg_find(int argc, char **argv, is_func is);

static int main_conv(int argc, char **argv)
{
    assert(argc > 0);
    assert(argv);

    

    return 0;
}

static struct Type *type_get_str(char *name)
{
    size_t i;
    struct Type *t;
    assert(name);

    for (i = 0; typelist[i] != NULL; i++) {
        t = typelist[i];
        assert(t.name);
        if (strcmp(t->name, name) == 0) {
            return t;
        }
    }

    return NULL;
}

static struct Type *type_get_converts(char *string)
{
    size_t i;
    struct Type *t;
    assert(string);

    for (i = 0; typelist[i] != NULL; i++) {
        t = typelist[i];
        if (t->can_interpret(t, string) == 0) {
            return t;
        }
    }

    return NULL;
}

static const char *type_get_fmt(char *name)
{
    struct Type *t;
    assert(name);

    t = type_get_str(name);
    if (t == NULL) {
        return NULL;
    }
    return t->interp;
}

int arg_check(int argc, char **argv, const char *da, const char *ddarg)
{
    int i;
    for (i = 0; i < argc; i++) {
        if ((strcmp(argv[i], da) == 0) || (strcmp(argv[i], ddarg) == 0)) {
            return 1;
        }
    }
    return 0;
}

char *arg_get(int argc, char **argv, const char *da, const char *ddarg)
{
    int i;
    for (i = 0; i < argc; i++) {
        if (((da && strcmp(argv[i], da) == 0) || (ddarg && strcmp(argv[i], ddarg) == 0)) && (i + 1 < argc)) {
            return argv[i + 1];
        }
    }
    return NULL;
}

static char *arg_find(int argc, char **argv, is_func is)
{
    int i;
    for (i = 0; i < argc; i++) {
        if (is(argv[i][0])) {
            return argv[i];
        }
    }
    return NULL;
}
