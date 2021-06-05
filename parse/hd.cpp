#include <regex>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "rpn.hpp"

static RpnVtable rpn64 = RPN_VTABLE(64);
static RpnVtable rpn32 = RPN_VTABLE(32);
static RpnVtable *rpn = &rpn64;

typedef void (* prog_func)(int argc, char **argv);

static void func_help(int argc, char **argv) noexcept;
static void func_32(int argc, char **argv) noexcept;
static void func_64(int argc, char **argv) noexcept;
static void func_ord(int argc, char **argv) noexcept;
static void func_chr(int argc, char **argv) noexcept;
static void func_verbose(int argc, char **argv) noexcept;
static void func_table(int argc, char **argv) noexcept;
static void func_extable(int argc, char **argv) noexcept;
static int arg_check(int argc, char **argv, const char *da, const char *ddarg) noexcept;
static char *arg_get(int argc, char **argv, const char *da, const char *ddarg) noexcept;

// _MUST_ be in pairs...
#define XENTRY(Da, Ddarg, ProgFunc, Whatdo) { \
    (const char *)Da, \
    (const char *)Ddarg, \
    (prog_func)ProgFunc, \
    (const char *)Whatdo\
}
static struct {
    const char *da;
    const char *ddarg;
    prog_func program;
    const char *whatdo;
} argTable[] = {
    XENTRY("-h", "--help", func_help, "View this help and exit"),
    XENTRY(NULL, "--32", func_32, "Set the operation word size to 32 bits"),
    XENTRY(NULL, "--64", func_64, "Set the operation word size to 64 bits (default)"),
    XENTRY(NULL, "--ord", func_ord, "Get the code of the first character and exit"),
    XENTRY(NULL, "--chr", func_chr, "Get the character of the first number and exit"),
    XENTRY("-v", "--verbose", func_verbose, "Display errors"),
    XENTRY(NULL, "--table", func_table, "Get the ASCII table and exit"),
    XENTRY(NULL, "--extable", func_extable, "Get the ASCII table and its extended set and exit"),
    XENTRY(NULL, NULL, NULL, NULL)
};
#undef XENTRY

int main(int argc, char **argv)
{
    int pivot;

    if (argc <= 1) {
        func_help(1, NULL);
    }

    for (int i = 0; argTable[i].program != NULL; i++) {
        int ndx = arg_check(argc, argv, argTable[i].da, argTable[i].ddarg);
        if (ndx) {
            argTable[i].program(argc - i, &argv[ndx]);
        }
    }

    // find pivot...
    auto re = std::regex(REG_NUM);
    for (int i = 0; i < argc; i++) {
        if (std::regex_match(argv[i], re)) {
            pivot = i;
            break;
        }
    }

    void *calc = rpn->create();
    for (int i = pivot; i < argc; i++) {
        rpn->push(calc, (char *)argv[i]);
    }

    rpn->exec(calc);
    rpn->print(calc);
    rpn->destroy(calc);

    return 0;
}

static void func_help(int argc, char **argv) noexcept {
    char buf[256];
    int n;

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
    func_verbose(0, NULL);
    rpn->help();
    fprintf(stderr, "\n");

    if (argc == 1) {
        exit(1);
    }
    exit(0);
}

static void func_32(int argc, char **argv) noexcept {
    rpn = &rpn32;
}

static void func_64(int argc, char **argv) noexcept {
    rpn = &rpn64;
}

static void func_ord(int argc, char **argv) noexcept {
    fprintf(stderr, "ord: Not implemented\n");
    exit(1);
}

static void func_chr(int argc, char **argv) noexcept {
    fprintf(stderr, "chr: Not implemented\n");
    exit(1);
}

static void func_verbose(int argc, char **argv) noexcept {
    rpn32.set_verbose(true);
    rpn64.set_verbose(true);
}

static void func_table(int argc, char **argv) noexcept {
    fprintf(stderr, "table: Not implemented\n");
    exit(1);
}

static void func_extable(int argc, char **argv) noexcept {
    fprintf(stderr, "extable: Not implemented\n");
    exit(1);
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

static char *arg_get(int argc, char **argv, const char *da, const char *ddarg) noexcept {
    int i;
    for (i = 0; i < argc; i++) {
        if (((da && strcmp(argv[i], da) == 0) || (ddarg && strcmp(argv[i], ddarg) == 0)) && (i + 1 < argc)) {
            return argv[i + 1];
        }
    }
    return NULL;
}
