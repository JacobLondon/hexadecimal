#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(void);
static int convert(char *numberlike, size_t *out, const char **outfmt);
static int ishexfmt(char *fmt);
static const char *gethexfmt(void);
static const char *getdecfmt(void);

static void usage(void)
{
    fprintf(stdout,
"HexaDecimal Usage:\n\n"
"hd help\n"
"hd [0x]NUMBER - Convert the number to its opposite, hex or dec\n"
"hd ord CHARACTER - Print the ASCII code of CHARACTER\n"
"hd chr NUMBER - Print the ASCII character of NUMBER\n"
"hd [0x]NUMBER [add|sub|and|or|xor] [0x]NUMBER - Perform basic operation, keeping the hex or dec fmt of the first NUMBER\n\n");
    fflush(stdout);
}

int main(int argc, char **argv)
{
    size_t number;
    size_t number2;
    const char *fmt;
    int rv;

    if (argc <= 1) {
        usage();
        return 1;
    }

    if (argc == 2) {
        if (strcmp("help", argv[1]) == 0) {
            usage();
            return 0;
        }

        rv = convert(argv[1], &number, &fmt);
        if (rv != 0) {
            fprintf(stderr, "Cannot convert '%s' to a number\n", argv[1]);
            fflush(stderr);
            return rv;
        }

        fprintf(stdout, fmt, number);
        fflush(stdout);
        return 0;
    }

    if (argc == 3) {
        if (strcmp(argv[1], "ord") == 0) {
            fprintf(stdout, "%d\n", (int)argv[2][0]);
            fflush(stdout);
        }
        else if (strcmp(argv[1], "chr") == 0) {
            if (sscanf(argv[2], "%zu", &number) != 1) {
                fprintf(stderr, "Cannot convert '%s' to a number\n", argv[2]);
                fflush(stderr);
                return 1;
            }

            if (number > 255) {
                fprintf(stderr, "Number out of ASCII range: %zu\n", number);
                fflush(stderr);
                return 2;
            }

            fprintf(stdout, "%c\n", (int)number);
            fflush(stdout);
        }
        else {
            fprintf(stderr, "Unknown option '%s'\n", argv[1]);
            fflush(stderr);
            return 1;
        }
        return 0;
    }

    if (argc == 4) {
        rv = convert(argv[1], &number, &fmt);
        if (rv != 0) {
            fprintf(stderr, "Cannot convert '%s' to a number\n", argv[1]);
            fflush(stderr);
            return rv;
        }

        rv = convert(argv[3], &number2, NULL);
        if (rv != 0) {
            fprintf(stderr, "Cannot convert '%s' to a number\n", argv[3]);
            fflush(stderr);
            return rv;
        }

        // un-convert the format
        if (ishexfmt(fmt)) {
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
        else {
            fprintf(stderr, "Unknown option '%s'\n", argv[2]);
            fflush(stderr);
            return 1;
        }

        fprintf(stdout, fmt, number);
        fflush(stdout);
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
        if (sscanf(&numberlike[2], "%llx", out) != 1) {
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
    return "0x%llX\n";
}

static const char *getdecfmt(void)
{
    return "%zu\n";
}
