#include <regex>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "rpn.hpp"
#include "util.hpp"

static RpnVtable rpn64 = RPN_VTABLE(64);
static RpnVtable rpn32 = RPN_VTABLE(32);
static RpnVtable rpn16 = RPN_VTABLE(16);
static RpnVtable rpn8  = RPN_VTABLE(8);
static RpnVtable *rpn = &rpn64;
bool _verbose = true; // extern
bool _longform = false; // extern

typedef void (* prog_func)(int argc, char **argv);

static void func_rpn(int argc, char **argv) noexcept;
static void func_help(int argc, char **argv) noexcept;
static void func_8(int argc, char **argv) noexcept;
static void func_16(int argc, char **argv) noexcept;
static void func_32(int argc, char **argv) noexcept;
static void func_64(int argc, char **argv) noexcept;
static void func_ord(int argc, char **argv) noexcept;
static void func_chr(int argc, char **argv) noexcept;
static void func_long(int argc, char **argv) noexcept;
static void func_verbose(int argc, char **argv) noexcept;
static void func_endian(int argc, char **argv) noexcept;
static void func_table(int argc, char **argv) noexcept;
static void func_extable(int argc, char **argv) noexcept;
static int arg_check(int argc, char **argv, const char *da, const char *ddarg) noexcept;
//static char *arg_get(int argc, char **argv, const char *da, const char *ddarg) noexcept;

static void print_section(int number, const char *term) noexcept;
//static int get_pivot(int argc, char **argv) noexcept;

#define XENTRY(Da, Ddarg, ProgFunc, Whatdo) { \
    (const char *)Da, \
    (const char *)Ddarg, \
    (prog_func)ProgFunc, \
    (const char *)Whatdo \
}
static struct {
    const char *da;
    const char *ddarg;
    prog_func program;
    const char *whatdo;
} argTable[] = {
    XENTRY(NULL, "--8", func_8, "Set the operation word size to 8 bits, no floats"),
    XENTRY(NULL, "--16", func_16, "Set the operation word size to 16 bits, no floats"),
    XENTRY(NULL, "--32", func_32, "Set the operation word size to 32 bits"),
    XENTRY(NULL, "--64", func_64, "Set the operation word size to 64 bits (default)"),
    XENTRY("-c", "--chr", func_chr, "Get the character of the first number and exit"),
    XENTRY("-o", "--ord", func_ord, "Get the code of the first character and exit"),
    XENTRY("-l", "--long", func_long, "Print all parts of the number, including leading zeros"),
    XENTRY("-t", "--table", func_table, "Get the ASCII table and exit"),
    XENTRY("-e", "--extable", func_extable, "Get the ASCII table and its extended set and exit"),
    XENTRY("-q", "--quiet", func_verbose, "Don't print errors to stderr"),
    XENTRY(NULL, "--endianness", func_endian, "Display the endianness of the system to stdout"),
    XENTRY("-h", "--help", func_help, "View this help and exit"),
    XENTRY(NULL, NULL, NULL, NULL)
};
#undef XENTRY

int main(int argc, char **argv)
{
    int pivot = 0;
    if (argc <= 1) {
        func_help(-1, NULL);
        exit(1);
    }

    for (int i = 0; argTable[i].program != NULL; i++) {
        int ndx = arg_check(argc, argv, argTable[i].da, argTable[i].ddarg);
        if (ndx) {
            if (ndx > pivot) {
                pivot = ndx;
            }
            argTable[i].program(argc - ndx, &argv[ndx]);
        }
    }

    func_rpn(argc - pivot, &argv[pivot]);

    return 0;
}

static void func_rpn(int argc, char **argv) noexcept {
    int pivot = 1; // always 1 after '-r / --rpn' arg
    void *calc = rpn->create();

    for (int i = pivot; i < argc; i++) {
        rpn->push(calc, (char *)argv[i]);
    }

    rpn->exec(calc);
    rpn->print(calc);
    rpn->destroy(calc);
    exit(0);
}

static void func_help(int argc, char **argv) noexcept {
    char buf[256];
    int n;

    (void)argv;

    fprintf(stderr,
        "Hexadecimal -- representation utility\n\n"
        "\thd [OPTIONS] [PROGRAM]\n\n"
        "OPTIONS\n"
    );

    for (int i = 0; argTable[i].program != NULL; i += 1) {
        n = 0;
        n += snprintf(&buf[n], sizeof(buf) - n, "\t%s", argTable[i].da ? argTable[i].da : "");
        n += snprintf(&buf[n], sizeof(buf) - n, "%s%s", argTable[i].da ? ", " : "", argTable[i].ddarg);
        n += snprintf(&buf[n], sizeof(buf) - n, "; %s", argTable[i].whatdo);
        fprintf(stderr, "%s\n", buf);
    }

    fprintf(stderr, "\nPROGRAM\n\n");
    rpn->help();
    fprintf(stderr, "\n");

    if (argc == -1) {
        exit(1);
    }
    exit(0);
}

static void func_8(int argc, char **argv) noexcept {
    (void)argc;
    (void)argv;
    rpn = &rpn8;
}

static void func_16(int argc, char **argv) noexcept {
    (void)argc;
    (void)argv;
    rpn = &rpn16;
}

static void func_32(int argc, char **argv) noexcept {
    (void)argc;
    (void)argv;
    rpn = &rpn32;
}

static void func_64(int argc, char **argv) noexcept {
    (void)argc;
    (void)argv;
    rpn = &rpn64;
}

static void func_ord(int argc, char **argv) noexcept {
    if (argc < 2) {
        if (_verbose) fprintf(stderr, "ord: Missing value\n");
        exit(1);
    }

    printf("%d\n", argv[1][0]);
    exit(0);
}

static void func_chr(int argc, char **argv) noexcept {
    int chr;

    if (argc < 2) {
        if (_verbose) fprintf(stderr, "chr: Missing value\n");
        exit(1);
    }

    if (sscanf(argv[1], "%d", &chr) != 1) {
        fprintf(stderr, "'%s' cannot be converted to a code\n", argv[1]);
        exit(1);
    }

    const char *str = ascii_lookup(chr);
    if (str[0] == 0) {
        fprintf(stderr, "Cannot convert '%s' to ASCII\n", argv[1]);
        exit(1);
    }

    printf("%s\n", str);
    exit(0);
}

static void func_verbose(int argc, char **argv) noexcept {
    (void)argc;
    (void)argv;
    _verbose = false;
}

static void func_long(int argc, char **argv) noexcept {
    (void)argc;
    (void)argv;
    _longform = true;
}

static void func_endian(int argc, char **argv) noexcept {
    (void)argc;
    (void)argv;
    if (is_little_endian()) {
        printf("little\n");
    }
    else {
        printf("big\n");
    }
    fflush(stdout);
    exit(0);
}

static void print_table(void) noexcept {
    for (int i = 0; i < (128 / 4); i++) {
        print_section(i, "\t");
        print_section(i + 128 / 4, "\t");
        print_section(i + 128 / 2, "\t");
        print_section(i + 128 * 3 / 4, "\n");
        fflush(stdout);
    }
}

static void ext_table(void) noexcept {
    for (int i = 128; i < (128 + (128 / 4)); i++) {
        print_section(i, "\t");
        print_section(i + 128 / 4, "\t");
        print_section(i + 128 / 2, "\t");
        print_section(i + 128 * 3 / 4, "\n");
        fflush(stdout);
    }
}

static void func_table(int argc, char **argv) noexcept {
    (void)argc;
    (void)argv;
    print_table();
    exit(0);
}

static void func_extable(int argc, char **argv) noexcept {
    (void)argc;
    (void)argv;
    print_table();
    ext_table();
    exit(0);
}

static int arg_check(int argc, char **argv, const char *da, const char *ddarg) noexcept {
    int i;
    for (i = 0; i < argc; i++) {
        if ((da && strcmp(argv[i], da) == 0) || (ddarg && strcmp(argv[i], ddarg) == 0)) {
            return i;
        }
    }
    return 0;
}

#if 0
static char *arg_get(int argc, char **argv, const char *da, const char *ddarg) noexcept {
    int i;
    for (i = 0; i < argc; i++) {
        if (((da && strcmp(argv[i], da) == 0) || (ddarg && strcmp(argv[i], ddarg) == 0)) && (i + 1 < argc)) {
            return argv[i + 1];
        }
    }
    return NULL;
}
#endif

static void print_section(int number, const char *term) noexcept {
    assert(term);
    fprintf(stdout, "%3d %2X %03o %5s%s", number, number, number, ascii_lookup(number), term);
}
