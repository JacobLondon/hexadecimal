#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "convert.h"

#define CONVERT_DEBUG

#if defined(CONVERT_DEBUG)
#  define DEBUG(...) (void)fprintf(stderr, __VA_ARGS__)
#else
#  define DEBUG(...)
#endif

/* always seperate the magnitudes with this */
#define SEP_MAGNITUDE_ALWAYS '_'

static int sep_magnitude = ',';
static int sep_decimal = '.';

int ConvertRemoveMagnitudes(const char *input, char *output, size_t output_size)
{
    size_t in_ndx, out_ndx;
    if (!input || !output) return 1;

    memset(output, 0, output_size);
    for (in_ndx = 0, out_ndx = 0; (input[in_ndx] != '\0') && (out_ndx < (output_size - 1)); in_ndx++) {
        if ((input[in_ndx] != SEP_MAGNITUDE_ALWAYS) && (input[in_ndx] != sep_magnitude)) {
            output[out_ndx] = input[in_ndx];
            out_ndx++;
        }
    }

    return 0;
}

ConvertResult ConvertString(const char *string)
{
    char temp[512];
    ConvertResult result;
    memset(&result, 0, sizeof(result));

    if (!string) {
        result.type = CONVERT_RESULT_NULL_STRING;
        return result;
    }

    if (ConvertRemoveMagnitudes(string, temp, sizeof(temp)) != 0) goto error;
    DEBUG("Converting %s...\n", temp);

    if (ConvertToHex(temp, &result.uinteger) == 0) {
        DEBUG("Chosen hex\n");
        result.type = CONVERT_RESULT_HEX;
    }
    else if (ConvertToOct(temp, &result.uinteger) == 0) {
        DEBUG("Chosen oct\n");
        result.type = CONVERT_RESULT_OCT;
    }
    else if (ConvertToBin(temp, &result.uinteger) == 0) {
        DEBUG("Chosen bin\n");
        result.type = CONVERT_RESULT_BIN;
    }
    else if (ConvertToDecSigned(temp, &result.integer) == 0) {
        DEBUG("Chosen signed dec\n");
        result.type = CONVERT_RESULT_INT;
    }
    else if (ConvertToDecUnsigned(temp, &result.uinteger) == 0) {
        DEBUG("Chosen unsigned dec\n");
        result.type = CONVERT_RESULT_UINT;
    }
    else if (ConvertToFloat(temp, &result.real) == 0) {
        result.type = CONVERT_RESULT_FLOAT;
        DEBUG("Chosen float\n");
    }
    else {
    error:
        DEBUG("Error\n");
        result.type = CONVERT_RESULT_CANT_CONVERT;
        result.integer = 0;
    }

    return result;
}

static int dump_int(FILE *stream, long long value)
{
    if (!stream) return 0;
    return fprintf(stream, "%+lld", value);
}

static int dump_uint(FILE *stream, unsigned long long value)
{
    if (!stream) return 0;
    return fprintf(stream, "%llu", value);
}

static int dump_hex(FILE *stream, unsigned long long value)
{
    if (!stream) return 0;
    return fprintf(stream, "0x%llX", value);
}

static int dump_oct(FILE *stream, unsigned long long value)
{
    if (!stream) return 0;
    return fprintf(stream, "0o%llo", value);
}

static int dump_float(FILE *stream, double value)
{
    if (!stream) return 0;
    return fprintf(stream, "%lf", value);
}

static int dump_bin(FILE *stream, unsigned long long value)
{
    int i;
    int bytes;
    int tmp;
    if (!stream) return 0;

    bytes = fprintf(stream, "0b");
    for (i = (int)(sizeof(value) * 8) - 1; i >= 0; i--) {
        tmp = !!(value & (1ull << i));
        if (tmp) {
            bytes += fprintf(stream, "%c", '0' + tmp);
        }
    }
    return bytes;
}

int ConvertResultDump(ConvertResult *result, FILE *stream)
{
    if (!result || !stream) return 0;

    switch (result->type)
    {
    case CONVERT_RESULT_INT:   return dump_int(stream, result->integer);
    case CONVERT_RESULT_UINT:  return dump_uint(stream, result->uinteger);
    case CONVERT_RESULT_HEX:   return dump_hex(stream, result->uinteger);
    case CONVERT_RESULT_OCT:   return dump_oct(stream, result->uinteger);
    case CONVERT_RESULT_FLOAT: return dump_float(stream, result->real);
    case CONVERT_RESULT_BIN:   return dump_bin(stream, result->uinteger);
    default:
        return 0;
    }
}

void ConvertSetMagnitudeChar(int sep)
{
    sep_magnitude = sep;
}

void ConvertSetDecimalChar(int dec)
{
    sep_decimal = dec;
}

int ConvertToFloat(const char *string, double *out)
{
    double convert;

    if (!string) return 1;
    if (sscanf(string, "%lf", &convert) != 1) return 1;
    if (out) *out = convert;

    return 0;
}

static int is_prefixed(const char *string, int first, int second_lower, int second_upper)
{
    if (!string) return 1;

    if ((string[0] == '\0') ||
        (string[1] == '\0') ||
        (string[2] == '\0'))
    {
        return 1;
    }

    if ((string[0] == first) &&
        ((string[1] == second_lower) || (string[1] == second_upper)))
    {
        return 0;
    }

    return 1;
}

static int convert_hex(const char *string, unsigned long long *out)
{
    if (!string || !out) return 1;
    if (sscanf(string, "%llx", out) != 1) return 1;
    return 0;
}

static int convert_oct(const char *string, unsigned long long *out)
{
    if (!string || !out) return 1;
    if (sscanf(string, "%llo", out) != 1) return 1;
    return 0;
}

static int convert_bin(const char *string, unsigned long long *out)
{
    const char *p;
    long long i;
    unsigned long long builder;

    if (!string || !out) return 1;
    builder = 0;
    i = strlen(string) - 1;

    for (p = string; *p != 0; p++, i--) {
        if (*p == '1') {
            builder |= ((1ull) << i);
        }
        else if (*p == '0') {
            ;
        }
        else {
            return 1;
        }
    }

    if (out) *out = builder;
    return 0;
}

static int convert_value(
    const char *string, void *correct_size_out,
    int first, int second_lower, int second_upper,
    int (*convert)(const char *, unsigned long long *))
{
    int rv;

    if (!string || !correct_size_out || !convert) return 1;
    if (string[0] == '\0') return 1;

    rv = is_prefixed(string, first, second_lower, second_upper);
    if (rv != 0) return 1;

    if (correct_size_out) {
        if ((string[1] == '\0') || (string[2] == '\0')) return 1;
        string += 2; /* bypass 0x,0o,0b */
        if (convert(string, correct_size_out) != 0) return 1;
    }
    return 0;
}

int ConvertToHex(const char *string, unsigned long long *out)
{
    unsigned long long convert;
    int rv;

    rv = convert_value(string, &convert, '0', 'x', 'X', convert_hex);
    if (rv != 0) return 1;
    if (out) *out = convert;
    return 0;
}

int ConvertToOct(const char *string, unsigned long long *out)
{
    unsigned long long convert;
    int rv;

    rv = convert_value(string, &convert, '0', 'o', 'O', convert_oct);
    if (rv != 0) return 1;
    if (out) *out = convert;
    return 0;
}

int ConvertToBin(const char *string, unsigned long long *out)
{
    unsigned long long convert;
    int rv;

    rv = convert_value(string, &convert, '0', 'b', 'B', convert_bin);
    if (rv != 0) return 1;
    if (out) *out = convert;
    return 0;
}

static int is_all_digits(const char *string)
{
    size_t i;
    if (!string) return 1;

    for (i = 0; string[i] != '\0'; i++) {
        if (!isdigit(string[i])) return 1;
    }

    return 0;
}

int ConvertToDecSigned(const char *string, long long *out)
{
    int rv;
    int negative;
    long long convert;

    if (!string) return 1;
    negative = (string[0] == '-');
    if (negative || (string[0] == '+')) string++;

    rv = is_all_digits(string);
    if (rv != 0) return 1;

    if (out) {
        if (sscanf(string, "%lld", &convert) != 1) return 1;
        *out = (negative ? -1 : 1) * convert;
    }
    return 0;
}

int ConvertToDecUnsigned(const char *string, unsigned long long *out)
{
    int rv;
    unsigned long long convert;

    if (!string) return 1;
    rv = is_all_digits(string);
    if (rv != 0) return 1;

    if (out) {
        if (sscanf(string, "%llu", &convert) != 1) return 1;
        *out = convert;
    }
    return 0;
}

#ifndef CONVERT_TEST
#define CONVERT_TEST
#endif

#if defined(CONVERT_TEST)

static int test_float(void)
{
    ConvertResult result;
    size_t i;
    int rv = 0;

    struct {
        const char *value;
        double expected;
        int expected_rv;
    } cases[] = {
        {"awe32p9r0awfe,32", 0, 1},
        {"1", 0, 1},
        {"0x10", 0, 1},
        {"0b10", 0, 1},
        {"0o10", 0, 1},

        {"1.2e3", 1200.0, 0},
        {"1.2E3", 1200.0, 0},
        {"1e3", 1000.0, 0},
        {"1e-3", 0.001, 0},
        {"-1.e-3", -0.001, 0},
        {"-1.E3", -1000.0, 0},
        {".1e3", 100.0, 0},
        {".1E3", 100.0, 0},

        {"1.2", 1.2, 0},
        {"-1.2", -1.2, 0},
        {"-.1", -.1, 0},
        {".1", .1, 0},

        {0, 0, 0}
    };

    DEBUG("%s\n", __func__);
    for (i = 0; cases[i].value != NULL; i++) {
        result = ConvertString(cases[i].value);

        if ((result.real != cases[i].expected) &&
           ((result.type >= CONVERT_RESULT_ERROR) && (cases[i].expected_rv == 0)))
        {
            rv += 1;
        }
    }
    return rv;
}

static int test_bin(void)
{
    ConvertResult result;
    size_t i;
    int rv = 0;

    struct {
        const char *value;
        unsigned long long expected;
        int expected_rv;
    } cases[] = {
        {"awe32p9r0awfe,32", 0, 1},
        {"1", 0, 1},
        {"0x10", 0, 1},
        {".1234e3", 0, 1},
        {"0o10", 0, 1},
        {"-0b11", 0, 1},

        {"0b01", 1, 0},
        {"0b11", 3, 0},
        {"0b0001_0000", 16, 0},

        {0, 0, 0}
    };

    DEBUG("%s\n", __func__);
    for (i = 0; cases[i].value != NULL; i++) {
        result = ConvertString(cases[i].value);

        if ((result.uinteger != cases[i].expected) &&
           ((result.type >= CONVERT_RESULT_ERROR) && (cases[i].expected_rv == 0)))
        {
            rv += 1;
        }
    }
    return rv;

    return 0;
}

static int test_oct(void)
{
    ConvertResult result;
    size_t i;
    int rv = 0;

    struct {
        const char *value;
        unsigned long long expected;
        int expected_rv;
    } cases[] = {
        {"awe32p9r0awfe,32", 0, 1},
        {"1", 0, 1},
        {".1234e3", 0, 1},
        {"0o10", 0, 1},
        {"-0b11", 0, 1},

        {"0x01", 0x01, 0},
        {"0x11", 0x11, 0},
        {"0x000F_0000", 0x000F0000, 0},

        {0, 0, 0}
    };

    DEBUG("%s\n", __func__);
    for (i = 0; cases[i].value != NULL; i++) {
        result = ConvertString(cases[i].value);

        if ((result.uinteger != cases[i].expected) &&
           ((result.type >= CONVERT_RESULT_ERROR) && (cases[i].expected_rv == 0)))
        {
            rv += 1;
        }
    }
    return rv;
}

static int test_hex(void)
{


    return 0;
}

static int test_int(void)
{


    return 0;
}

static int test_uint(void)
{


    return 0;
}

int main(int argc, char **argv)
{
    int rv = 0;

    if (argc == 2) {
        ConvertResult result = ConvertString(argv[1]);
        if (result.type >= CONVERT_RESULT_ERROR) {
            printf("Failed to convert: %s: ", argv[1]);
            return 1;
        }
        ConvertResultDump(&result, stdout);
        printf("\n");
        return 0;
    }

    rv += test_float();
    rv += test_bin();
    rv += test_oct();
    rv += test_hex();
    rv += test_int();
    rv += test_uint();

    return rv;
}
#endif
