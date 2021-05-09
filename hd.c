#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

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

static void usage(void)
{
    eprintf(
"HexaDecimal Usage:\n\n"
"hd help\n"
"hd [0x]NUMBER - Convert the number to its opposite, hex or dec\n"
"hd ord CHARACTER - Print the ASCII code of the first CHARACTER\n"
"hd chr NUMBER - Print the ASCII character of NUMBER\n"
"hd [0x]NUMBER [add|sub|and|or|xor|mul|div] [0x]NUMBER - Perform basic operation, keeping the hex or dec fmt of the first NUMBER\n"
"hd [0x]NUMBER [not] - Perform not operation, keeping the hex or dec fmt\n"
"hd [ex]table - Print the ASCII table, optionally also print the extended set\n\n"
);
}

int main(int argc, char **argv)
{
    size_t number;
    size_t number2;
    const char *fmt;
    int rv;
    int i;

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

        rv = convert(argv[1], &number, &fmt);
        if (rv != 0) {
            eprintf("Cannot convert '%s' to a number\n", argv[1]);
            return rv;
        }

        oprintf(fmt, number);
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
        else if (strcmp(argv[2], "not") == 0) {
            goto OperationNot;
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

    OperationNot:
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
        else if (strcmp("not", argv[2]) == 0) number = ~number;
        else if (strcmp("mul", argv[2]) == 0) number = number * number2;
        else if (strcmp("div", argv[2]) == 0) number = number / number2;
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
    return "0x" FORMAT_HEX "\n";
}

static const char *getdecfmt(void)
{
    return "%zu\n";
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
