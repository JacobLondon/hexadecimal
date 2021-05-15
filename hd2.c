#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*a))

enum Type {
    TYPE_NONE,
    TYPE_DEC,
    TYPE_HEX,
    TYPE_OCT,
    TYPE_BIN,
    TYPE_FLT,
    TYPE_DBL,
    TYPE_COUNT,
};

typedef int (* convert_func)(char *, size_t *);
typedef int (* is_func)(int);

static int main_chr(int argc, char *argv[]);
static int main_ord(int argc, char *argv[]);
static int main_conv(int argc, char *argv[]);
static int main_table(int argc, char *argv[]);
static void usage(void);
static enum Type type_lookup(char *number, char **start_out);
static const char *print_lookup(enum Type type, enum Type reinterp);
static enum Type reinterpret_lookup(const char *typename);
static int type_convert(char *number, enum Type type, size_t *out);
static int type_convert_dec(char *number, size_t *out);
static int type_convert_hex(char *number, size_t *out);
static int type_convert_oct(char *number, size_t *out);
static int type_convert_bin(char *number, size_t *out);
static int type_convert_flt(char *number, size_t *out);
static int type_convert_dbl(char *number, size_t *out);
static int arg_check(int argc, char **argv, const char *da, const char *ddarg);
static char *arg_get(int argc, char **argv, const char *da, const char *ddarg);
static char *arg_find(int argc, char **argv, is_func is);

static convert_func convert_table[] = {
    [TYPE_NONE] = NULL,
    [TYPE_DEC] = type_convert_dec,
    [TYPE_HEX] = type_convert_hex,
    [TYPE_OCT] = type_convert_oct,
    [TYPE_BIN] = type_convert_bin,
    [TYPE_FLT] = type_convert_flt,
    [TYPE_DBL] = type_convert_dbl,
    [TYPE_COUNT] = NULL,
};

static enum Type print_specifiers[] = {
    [TYPE_NONE] = TYPE_NONE,
    [TYPE_DEC] = TYPE_HEX,
    [TYPE_HEX] = TYPE_DEC,
    [TYPE_OCT] = TYPE_DEC,
    [TYPE_BIN] = TYPE_DEC,
    [TYPE_FLT] = TYPE_HEX,
    [TYPE_DBL] = TYPE_HEX,
    [TYPE_COUNT] = TYPE_COUNT,
};

static const char *prefix_table[] = {
    [TYPE_NONE] = NULL,
    [TYPE_DEC] = "",
    [TYPE_HEX] = "0x",
    [TYPE_OCT] = "0o",
    [TYPE_BIN] = "0b",
    [TYPE_FLT] = "",
    [TYPE_DBL] = "",
    [TYPE_COUNT] = NULL,
};

static const char *reinterpret_table[] = {
    [TYPE_NONE] = NULL,
    [TYPE_DEC] = "dec",
    [TYPE_HEX] = "hex",
    [TYPE_OCT] = "oct",
    [TYPE_BIN] = "bin",
    [TYPE_FLT] = "float",
    [TYPE_DBL] = "double",
    [TYPE_COUNT] = NULL,
};

int main(int argc, char *argv[])
{
    if (argc <= 1) {
    help:
        usage();
        exit(1);
    }
    else if (arg_check(argc, argv, "-h", "--help")) {
        usage();
        exit(0);
    }
    else if (strcmp(argv[1], "chr") == 0) {
        if (argc < 3) {
            goto help;
        }
        return main_chr(argc - 2, &argv[2]);
    }
    else if (strcmp(argv[1], "ord") == 0) {
        if (argc < 3) {
            goto help;
        }
        return main_ord(argc - 2, &argv[2]);
    }
    else if (strcmp(argv[1], "table") == 0) {
        return main_table(argc - 2, &argv[2]);
    }

    return main_conv(argc - 1, &argv[1]);
}

static int main_chr(int argc, char *argv[])
{
    return 0;
}

static int main_ord(int argc, char *argv[])
{
    return 0;
}

static int main_conv(int argc, char *argv[])
{
    char *number = argv[0];
    enum Type type;
    enum Type reinterp;
    size_t value;
    const char *p;
    char *start;

    number = arg_find(argc, argv, isdigit);
    if (!number) {
        return 1;
    }

    if (arg_check(argc, argv, "-D", "--decimal")) {
        type = TYPE_DEC;
        start = number;
    }
    if (arg_check(argc, argv, "-H", "--hexadecimal")) {
        type = TYPE_HEX;
        start = number;
    }
    else if (arg_check(argc, argv, "-O", "--octal")) {
        type = TYPE_OCT;
        start = number;
    }
    else if (arg_check(argc, argv, "-B", "--binary")) {
        type = TYPE_BIN;
        start = number;
    }
    else if (arg_check(argc, argv, "-F", "--float")) {
        type = TYPE_FLT;
        start = number;
    }
    else if (arg_check(argc, argv, "-Z", "--double")) {
        type = TYPE_DBL;
        start = number;
    }
    else {
        type = type_lookup(number, &start);
    }

    p = arg_get(argc, argv, "-r", "--reinterpret");
    if (!p) {
        p = arg_get(argc, argv, "as", NULL);
    }

    reinterp = TYPE_NONE;
    if (p) {
        reinterp = reinterpret_lookup(p);
    }

    if (type_convert(start, type, &value) == 0) {
        // TODO: This just can't work for binary format or to floating point format
        p = print_lookup(type, reinterp);
        printf(p, value);
        printf("\n");
    }
    else {
        printf("Failed\n");
        return 1;
    }

    return 0;
}

static int main_table(int argc, char *argv[])
{
    return 0;
}

static void usage(void)
{
    printf(
"hd -h\n"
"hd NUMBER\n"
"hd -s,--seq NUMBER NUMBER NUMBER ...\n"
"\n"
    );
}

static inline const char *format_specifier(enum Type type)
{
    switch (type) {
    case TYPE_NONE:
        return NULL;
    case TYPE_DEC:
        return "%zu";
    case TYPE_HEX:
        if (sizeof(unsigned int) == sizeof(size_t)) {
            return "%X";
        }
        else if (sizeof(unsigned long int) == sizeof(size_t)) {
            return "%lX";
        }
        else {
            return "%llX";
        }
    case TYPE_OCT:
        if (sizeof(unsigned int) == sizeof(size_t)) {
            return "%O";
        }
        else if (sizeof(unsigned long int) == sizeof(size_t)) {
            return "%lO";
        }
        else {
            return "%llO";
        }
    case TYPE_BIN:
        return NULL;
    case TYPE_FLT:
        return "%f";
    case TYPE_DBL:
        return "%lf";
    case TYPE_COUNT:
        return NULL;
    }
}

static enum Type type_lookup(char *number, char **start_out)
{
    int p;
    enum Type ret = TYPE_DEC;

    assert(number);

    if (number[0] == 0) {
        return TYPE_NONE;
    }

    p = strchr(number, 'f') != NULL ||
        strchr(number, 'F') != NULL;
    if (p) {
        if (start_out) {
            *start_out = number;
        }
        return TYPE_FLT;
    }

    p = strchr(number, '.') != NULL;
    if (p) {
        if (start_out) {
            *start_out = number;
        }
        return TYPE_DBL;
    }

    if (number[0] == '0') {
        switch (number[1]) {
        case 'd': /* fallthrough */
        case 'D':
            ret = TYPE_DEC;
            goto by2;
        case 'x': /* fallthrough */
        case 'X':
            ret = TYPE_HEX;
            goto by2;
        case 'o': /* fallthrough */
        case 'O':
            ret = TYPE_OCT;
            goto by2;
        case 'b': /* fallthrough */
        case 'B':
            ret = TYPE_BIN;
            goto by2;
        default:
            break;
        }
    }

    if (0) {
by2:
        if (start_out) {
            *start_out = &number[2];
        }
    }

    return ret;
}

static const char *print_lookup(enum Type type, enum Type reinterp)
{
    static char buf[16];
    const char *p;
    enum Type printas = type;
    if (reinterp != TYPE_NONE) {
        printas = print_specifiers[type];
    }

    p = format_specifier(printas);
    snprintf(buf, sizeof(buf), "%s%s",
        prefix_table[printas],
        p);
    return buf;
}

static enum Type reinterpret_lookup(const char *typename)
{
    int i;
    assert(typename);

    for (i = 0; i < ARRAY_SIZE(reinterpret_table); i++) {
        if (reinterpret_table[i] && strcmp(reinterpret_table[i], typename) == 0) {
            return (enum Type)i;
        }
    }
    return TYPE_NONE;
}

static int type_convert(char *number, enum Type type, size_t *out)
{
    assert(number);
    assert(out);

    convert_func func = convert_table[type];
    if (!func) {
        return 1;
    }

    return func(number, out);
}

static int type_convert_dec(char *number, size_t *out)
{
    size_t value;
    const char *p;

    assert(number);
    assert(out);

    p = format_specifier(TYPE_DEC);
    if (sscanf(number, p, &value) == 1) {
        *out = value;
        return 0;
    }
    return 1;
}

static int type_convert_hex(char *number, size_t *out)
{
    size_t value;
    const char *p;

    assert(number);
    assert(out);

    p = format_specifier(TYPE_HEX);
    if (sscanf(number, p, &value) == 1) {
        *out = value;
        return 0;
    }
    return 1;
}

static int type_convert_oct(char *number, size_t *out)
{
    size_t value;
    const char *p;

    assert(number);
    assert(out);

    p = format_specifier(TYPE_OCT);
    if (sscanf(number, p, &value) == 1) {
        *out = value;
        return 0;
    }
    return 1;
}

static int type_convert_bin(char *number, size_t *out)
{
    size_t value;
    size_t len;
    size_t i;
    char ch;

    assert(number);
    assert(out);

    value = 0;
    len = strlen(number);
    for (i = 0; i < sizeof(size_t) * 8; i++) {
        ch = number[len - i - 1];
        switch (ch) {
        case 'b':
        case 'B':
            goto out;
        case '1':
            value = (size_t)(value | (size_t)(((size_t)1) << i));
            break;
        case '0':
            break;
        default:
            return 1;
        }

        if (number == &number[len - i - 1]) {
            break;
        }
    }
out:
    *out = value;
    return 0;
}

static int type_convert_flt(char *number, size_t *out)
{
    float value;
    const char *p;

    assert(number);
    assert(out);

    p = format_specifier(TYPE_FLT);
    if (sscanf(number, p, &value) == 1) {
        memset(out, 0, sizeof(*out));
        memcpy(out, &value, sizeof(value));
        return 0;
    }
    return 1;
}

static int type_convert_dbl(char *number, size_t *out)
{
    double value;
    const char *p;

    assert(number);
    assert(out);

    p = format_specifier(TYPE_DBL);
    if (sscanf(number, p, &value) == 1) {
        memset(out, 0, sizeof(*out));
        memcpy(out, &value, sizeof(value));
        return 0;
    }
    return 1;
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
