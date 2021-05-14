#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#define FORMAT_HEX "%lX"
#define flush_printf(fd, ...) \
    do { \
        (void)fprintf((fd), __VA_ARGS__); \
        (void)fflush((fd)); \
    } while (0)

#define oprintf(...) flush_printf(stdout, __VA_ARGS__)
#define eprintf(...) flush_printf(stderr, __VA_ARGS__)

static void usage(void);
static int convert(char *numberlike, size_t *out, const char **outfmt);
static int ishexfmt(char *fmt);
static const char *gethexfmt(void);
static const char *getdecfmt(void);
static const char *ascii_lookup(int chr);
static void print_section(int number, const char *term);
static int chr_count(char *str, char chr);
static void strreplace(char *str, char old, char new);

static char hexformat[8] = "0x" FORMAT_HEX "\n";
static char decformat[8] = "%zu\n";

static void usage(void)
{
    eprintf(
"HexaDecimal\n\n"
"Note all operations are for unsigned integers\n\n"
"hd help\n"
"hd [0x]NUMBER - Convert the number to its opposite, hex or dec\n"
"hd '[0x]NUMBER [0x]NUMBER [0x]NUMBER ...'\n"
"hd ord CHARACTER - Print the ASCII code of the first CHARACTER\n"
"hd chr NUMBER - Print the ASCII character NUMBER\n"
"hd [0x]NUMBER BINOP [0x]NUMBER - Perform basic operation, keeping the fmt of the first NUMBER\n"
"hd [0x]NUMBER UNOP - Perform operation, keeping the fmt\n"
"hd [ex]table - Print the ASCII table, optionally also print the extended set\n"
"\n"
"NUMBER: [0-9]+\n"
"CHARACTER: <ASCII>\n"
"BINOP: [add|sub|and|or|xor|mul|div|pow|shl|shr|[gn]t[e]|ne|eq|mod]\n"
"UNOP: [inv|not]\n"
"\n"
);
}

int main(int argc, char **argv)
{
    size_t number;
    size_t number2;
    const char *fmt;
    int rv;
    int i;
    char *p;

    if (argc <= 1) {
        usage();
        return 1;
    }

    if (argc == 2) {
        if (strcmp("help", argv[1]) == 0) {
            usage();
            return 0;
        }
        else if ((rv = (strcmp("extable", argv[1]) == 0)) || strcmp("table", argv[1]) == 0) {
            for (i = 0; i < 128 / 4; i++) {
                print_section(i, "\t");
                print_section(i + 128 / 4, "\t");
                print_section(i + 128 / 2, "\t");
                print_section(i + 128 * 3 / 4, "\n");
                fflush(stdout);
            }
            // doing extended table
            if (rv) {
                for (i = 128; i < 128 + 128 / 4; i++) {
                    print_section(i, "\t");
                    print_section(i + 128 / 4, "\t");
                    print_section(i + 128 / 2, "\t");
                    print_section(i + 128 * 3 / 4, "\n");
                    fflush(stdout);
                }
            }
            return 0;
        }

        i = chr_count(argv[1], ' ') + 1;
        p = argv[1];
        strreplace(hexformat, '\n', 0);
        strreplace(decformat, '\n', 0);
        for ( ; i > 0; i--) {
            rv = convert(p, &number, &fmt);
            if (rv != 0) {
                eprintf("Cannot convert '%s' to a number\n", argv[1]);
                return rv;
            }
            oprintf(fmt, number);

            p = strchr(p, ' ');
            if (p) {
                p++;
                oprintf(" ");
            }
        }
        oprintf("\n");
        return 0;
    }

    if (argc == 3) {
        if (strcmp(argv[1], "ord") == 0) {
            oprintf("%d\n", argv[2][0]);
        }
        else if (strcmp(argv[1], "chr") == 0) {
            if (sscanf(argv[2], "%zu", &number) != 1) {
                eprintf("'%s' cannot be converted to a number\n", argv[2]);
                return 1;
            }

            fmt = ascii_lookup((int)number);
            if (fmt[0] == 0) {
                eprintf("Cannot convert '%s' to ASCII\n", argv[2]);
                return 2;
            }

            oprintf("%s\n", fmt);
        }
        else if (
            (strcmp(argv[2], "inv") == 0) ||
            (strcmp(argv[2], "not") == 0) ||
            (strcmp(argv[2], "pos") == 0) ||
            (strcmp(argv[2], "neg") == 0)
        )
        {
            goto OperationUnary;
        }
        else {
            eprintf("Unknown option '%s'\n", argv[1]);
            return 1;
        }
        return 0;
    }

    if (argc == 4) {
        rv = convert(argv[3], &number2, NULL);
        if (rv != 0) {
            eprintf("Cannot convert '%s' to a number\n", argv[3]);
            return rv;
        }

    OperationUnary:
        rv = convert(argv[1], &number, &fmt);
        if (rv != 0) {
            eprintf("Cannot convert '%s' to a number\n", argv[1]);
            return rv;
        }

        // un-convert the format
        if (ishexfmt((char *)fmt)) {
            fmt = getdecfmt();
        }
        else {
            fmt = gethexfmt();
        }

        if      (strcmp("add", argv[2]) == 0) number = number + number2;
        else if (strcmp("sub", argv[2]) == 0) number = number - number2;
        else if (strcmp("and", argv[2]) == 0) number = number & number2;
        else if (strcmp("or",  argv[2]) == 0) number = number | number2;
        else if (strcmp("xor", argv[2]) == 0) number = number ^ number2;
        else if (strcmp("inv", argv[2]) == 0) number = ~number;
        else if (strcmp("not", argv[2]) == 0) number = !number;
        else if (strcmp("pos", argv[2]) == 0) number = +number;
        else if (strcmp("neg", argv[2]) == 0) number = -number;
        else if (strcmp("mul", argv[2]) == 0) number = number * number2;
        else if (strcmp("div", argv[2]) == 0) number = number / number2;
        else if (strcmp("pow", argv[2]) == 0) number = (size_t)pow(number, number2);
        else if (strcmp("shl", argv[2]) == 0) number = number << number2;
        else if (strcmp("shr", argv[2]) == 0) number = number >> number2;
        else if (strcmp("gt",  argv[2]) == 0) number = number > number2;
        else if (strcmp("lt",  argv[2]) == 0) number = number < number2;
        else if (strcmp("gte", argv[2]) == 0) number = number >= number2;
        else if (strcmp("lte", argv[2]) == 0) number = number <= number2;
        else if (strcmp("ne",  argv[2]) == 0) number = number != number2;
        else if (strcmp("eq",  argv[2]) == 0) number = number == number2;
        else if (strcmp("mod", argv[2]) == 0) number = number % number2;
        else {
            eprintf("Unknown option '%s'\n", argv[2]);
            return 1;
        }

        oprintf(fmt, number);
        return 0;
    }

    usage();
    return 1;
}

static int convert(char *numberlike, size_t *out, const char **outfmt)
{
    size_t len;
    assert(numberlike);
    assert(out);

    len = strlen(numberlike);
    if (len == 0) {
        return 1;
    }

    /* interpret a hex number */
    if (ishexfmt(numberlike)) {
        if (sscanf(&numberlike[2], FORMAT_HEX, out) != 1) {
            return 2;
        }

        if (outfmt) {
            *outfmt = getdecfmt();
        }
        return 0;
    }

    /* interpret a number */
    if (sscanf(numberlike, "%zu", out) != 1) {
        return 3;
    }

    if (outfmt) {
        *outfmt = gethexfmt();
    }

    return 0;
}

static int ishexfmt(char *fmt)
{
    assert(fmt);
    return (strncmp("0x", fmt, 2) == 0) || (strncmp("0X", fmt, 2) == 0);
}

static const char *gethexfmt(void)
{
    return hexformat;
}

static const char *getdecfmt(void)
{
    return decformat;
}

static const char *ascii_lookup(int chr)
{
    static char buf[4] = {0};
    static const char *table[] = {
        [0] = "NUL", // null
        [1] = "SOH", // start of heading
        [2] = "STX", // start of text
        [3] = "ETX", // end of text
        [4] = "EOT", // end of transmission
        [5] = "ENQ", // enquiry
        [6] = "ACK", // acknowledge
        [7] = "\\a", // BEL, ALERT, bell
        [8] = "\\b", // BS, backspace
        [9] = "\\t", // TAB, horizontal tab
        [10] = "\\n", // LF, line feed
        [11] = "\\v", // VT, vertical tab
        [12] = "FF", // NP, form feed, new page
        [13] = "\\r", // CR, carriage return
        [14] = "SO", // shift out
        [15] = "SI", // shift in
        [16] = "DLE", // data link escape
        [17] = "DC1", // device control 1
        [18] = "DC2", // device control 2
        [19] = "DC3", // device control 3
        [20] = "DC4", // device control 4
        [21] = "NAK", // negative acknowledge
        [22] = "SYN", // synchronous idle
        [23] = "ETB", // end of transmission block
        [24] = "CAN", // cancel
        [25] = "EM", // end of medium
        [26] = "SUB", // substitute
        [27] = "\\e", // ESC, escape
        [28] = "FS", // file seperator
        [29] = "GS", // group seperator
        [30] = "RS", // record seperator
        [31] = "US", // unit seperator
        [32] = "SPACE", // space
        [127] = "DEL", // delete
    };

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

static void print_section(int number, const char *term)
{
    assert(term);
    fprintf(stdout, "%3d %2X %03o %5s%s", number, number, number, ascii_lookup(number), term);
}

static int chr_count(char *str, char chr)
{
    int count = 0;
    assert(str);

    while (*str) {
        if (*str == chr) {
            count++;
        }
        str++;
    }
    return count;
}

static void strreplace(char *str, char old, char new)
{
    assert(str);
    while (*str) {
        if (*str == old) {
            *str = new;
        }
        str++;
    }
}
