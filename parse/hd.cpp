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

// _MUST_ be in pairs...
#define XENTRY(Name, ProgFunc) {(const char *)Name, (prog_func)ProgFunc}
static struct {
    const char *name; prog_func program;
} argTable[] = {
    XENTRY("-h", func_help),
    XENTRY("--help", func_help),
    XENTRY("-32", func_32),
    XENTRY("-64", func_64),
    XENTRY("-o", func_ord),
    XENTRY("--ord", func_ord),
    XENTRY("-c", func_chr),
    XENTRY("--chr", func_chr),
    XENTRY("-v", func_verbose),
    XENTRY("--verbose", func_verbose),
    XENTRY("-t", func_table),
    XENTRY("--table", func_table),
    XENTRY("-e", func_extable),
    XENTRY("--extable", func_extable),
    XENTRY(NULL, NULL)
};
#undef XENTRY

const char *helpTable[] = {
    "View this help and exit",
    "Set the operation word size",
    "Get the code of the first character and exit",
    "Get the character of the first number and exit",
    "Verbose errors",
    "Get the ASCII table and exit",
    "Get the ASCII table and its extended set and exit",
    NULL,
};

int main(int argc, char **argv)
{
    int pivot = 1;

    if (argc <= 1) {
        func_help(1, NULL);
    }

    for (int i = pivot; i < argc; i++) {
        if (argv[i][0] == '-') {
            for (int j = 0; argTable[j].name; j++) {
                if (strcmp(argv[i], argTable[j].name) == 0) {
                    argTable[j].program(argc - i, &argv[i]);
                    goto next;
                }
            }

            // didn't find it
            goto skip;
        next:
            ;
        }
        // done with arguments, we are at pivot
        else {
        skip:
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
    fprintf(stderr,
        "Hexadecimal -- representation utility\n\n"
        "\thd [OPTIONS] [PROGRAM]\n\n"
        "OPTIONS\n"
    );

    for (int i = 0; argTable[i].name != NULL; i += 2) {
        fprintf(stderr, "\t%s, %s; %s\n",
            argTable[i].name,
            argTable[i + 1].name,
            helpTable[i / 2]);
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
