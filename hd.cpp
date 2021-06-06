#include <regex>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "rpn.hpp"

static RpnVtable rpn64 = RPN_VTABLE(64);
static RpnVtable rpn32 = RPN_VTABLE(32);
static RpnVtable *rpn = &rpn64;

typedef void (* prog_func)(int argc, char **argv);

static void func_rpn(int argc, char **argv) noexcept;
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

static const char *ascii_lookup(int chr) noexcept;
static void print_section(int number, const char *term) noexcept;
static int get_pivot(int argc, char **argv) noexcept;

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
    XENTRY("-h", "--help", func_help, "View this help and exit"),
    XENTRY(NULL, "--32", func_32, "Set the operation word size to 32 bits"),
    XENTRY(NULL, "--64", func_64, "Set the operation word size to 64 bits (default)"),
    XENTRY(NULL, "--chr", func_chr, "Get the character of the first number and exit"),
    XENTRY(NULL, "--ord", func_ord, "Get the code of the first character and exit"),
    XENTRY("-r", "--rpn", func_rpn, "Parse the arguments in Reverse Polish Notation (RPN)"),
    XENTRY(NULL, "--table", func_table, "Get the ASCII table and exit"),
    XENTRY(NULL, "--extable", func_extable, "Get the ASCII table and its extended set and exit"),
    XENTRY("-v", "--verbose", func_verbose, "Display errors"),
    XENTRY(NULL, NULL, NULL, NULL)
};
#undef XENTRY

int main(int argc, char **argv)
{
    if (argc <= 1) {
        func_help(1, NULL);
    }

    for (int i = 0; argTable[i].program != NULL; i++) {
        int ndx = arg_check(argc, argv, argTable[i].da, argTable[i].ddarg);
        if (ndx) {
            argTable[i].program(argc - ndx, &argv[ndx]);
        }
    }

    func_rpn(argc, argv);

    return 0;
}

static void func_rpn(int argc, char **argv) noexcept {
    int pivot = get_pivot(argc, argv);
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
    printf("%d\n", argv[1][0]);
    exit(0);
}

static void func_chr(int argc, char **argv) noexcept {
    int chr;
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
    rpn32.set_verbose(true);
    rpn64.set_verbose(true);
}

static void func_table(int argc, char **argv) noexcept {
    for (int i = 0; i < 128 / 4; i++) {
        print_section(i, "\t");
        print_section(i + 128 / 4, "\t");
        print_section(i + 128 / 2, "\t");
        print_section(i + 128 * 3 / 4, "\n");
        fflush(stdout);
    }
    exit(0);
}

static void func_extable(int argc, char **argv) noexcept {
    func_table(0, NULL);
    for (int i = 128; i < 128 + 128 / 4; i++) {
        print_section(i, "\t");
        print_section(i + 128 / 4, "\t");
        print_section(i + 128 / 2, "\t");
        print_section(i + 128 * 3 / 4, "\n");
        fflush(stdout);
    }
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

static char *arg_get(int argc, char **argv, const char *da, const char *ddarg) noexcept {
    int i;
    for (i = 0; i < argc; i++) {
        if (((da && strcmp(argv[i], da) == 0) || (ddarg && strcmp(argv[i], ddarg) == 0)) && (i + 1 < argc)) {
            return argv[i + 1];
        }
    }
    return NULL;
}

static const char *ascii_lookup(int chr) noexcept {
    static bool firstTime = true;
    static char buf[4] = {0};
    static const char *table[128];

    if (firstTime) {
        firstTime = false;
        table[0] = "NUL"; // null
        table[1] = "SOH"; // start of heading
        table[2] = "STX"; // start of text
        table[3] = "ETX"; // end of text
        table[4] = "EOT"; // end of transmission
        table[5] = "ENQ"; // enquiry
        table[6] = "ACK"; // acknowledge
        table[7] = "\\a"; // BEL, ALERT, bell
        table[8] = "\\b"; // BS, backspace
        table[9] = "\\t"; // TAB, horizontal tab
        table[10] = "\\n"; // LF, line feed
        table[11] = "\\v"; // VT, vertical tab
        table[12] = "FF"; // NP, form feed, new page
        table[13] = "\\r"; // CR, carriage return
        table[14] = "SO"; // shift out
        table[15] = "SI"; // shift in
        table[16] = "DLE"; // data link escape
        table[17] = "DC1"; // device control 1
        table[18] = "DC2"; // device control 2
        table[19] = "DC3"; // device control 3
        table[20] = "DC4"; // device control 4
        table[21] = "NAK"; // negative acknowledge
        table[22] = "SYN"; // synchronous idle
        table[23] = "ETB"; // end of transmission block
        table[24] = "CAN"; // cancel
        table[25] = "EM"; // end of medium
        table[26] = "SUB"; // substitute
        table[27] = "\\e"; // ESC, escape
        table[28] = "FS"; // file seperator
        table[29] = "GS"; // group seperator
        table[30] = "RS"; // record seperator
        table[31] = "US"; // unit seperator
        table[32] = "SPACE"; // space
        table[127] = "DEL"; // delete
    }

    if (chr < 0 || chr > 255) {
        return "";
    }

    // less printable
    if ((0 <= chr && chr <= 32) || chr == 127) {
        return table[chr];
    }

    // printable
    snprintf(buf, sizeof(buf), "%c", chr);
    return buf;
}

static void print_section(int number, const char *term) noexcept {
    assert(term);
    fprintf(stdout, "%3d %2X %03o %5s%s", number, number, number, ascii_lookup(number), term);
}

static int get_pivot(int argc, char **argv) noexcept {
    int pivot = -1;

    // find pivot...
    auto re = std::regex(REG_NUM);
    for (int i = 0; i < argc; i++) {
        if (std::regex_match(argv[i], re)) {
            pivot = i;
            break;
        }
    }

    if (pivot == -1) {
        fprintf(stderr, "Missing PROGRAM\n");
        exit(1);
    }

    return pivot;
}
