#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

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
static int sep_decimal = '.'; /* TODO */

int ConvertAsciiToOrd(char *string)
{
    if (!string) return -1;
    return (int)string[0];
}

int ConvertOrdToAscii(char *number)
{
    int ord;

    ConvertResult32 result = ConvertString32(number);
    if (result.type >= CONVERT_RESULT_ERROR) return -1;
    if (result.type == CONVERT_RESULT_INT) {
        ord = result.integer;
    }
    else if (result.type == CONVERT_RESULT_FLOAT) {
        ord = result.real;
    }
    else {
        ord = result.uinteger;
    }

    char actual = ord & 0xF;
    return (int)actual;
}

static ConvertResult32 downcast(ConvertResult64 in)
{
    ConvertResult32 out;
    out.type = in.type;

    switch (in.type) {
    case CONVERT_RESULT_INT:
        out.integer = in.integer;
        break;
    case CONVERT_RESULT_UINT: /* fallthrough */
    case CONVERT_RESULT_HEX: /* fallthrough */
    case CONVERT_RESULT_OCT: /* fallthrough */
    case CONVERT_RESULT_BIN:
        out.uinteger = in.uinteger;
        break;
    case CONVERT_RESULT_FLOAT:
        out.real = in.real;
        break;
    default:
        out.uinteger = 0;
        break;
    }
    return out;
}

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

ConvertResult64 ConvertString64(const char *string)
{
    char temp[512];
    ConvertResult64 result;
    memset(&result, 0, sizeof(result));

    if (!string) {
        result.type = CONVERT_RESULT_NULL_STRING;
        return result;
    }

    if (ConvertRemoveMagnitudes(string, temp, sizeof(temp)) != 0) goto error;
    DEBUG("Converting %s...\n", temp);

    if (ConvertToHex64(temp, &result.uinteger) == 0) {
        DEBUG("Chosen hex\n");
        result.type = CONVERT_RESULT_HEX;
    }
    else if (ConvertToOct64(temp, &result.uinteger) == 0) {
        DEBUG("Chosen oct\n");
        result.type = CONVERT_RESULT_OCT;
    }
    else if (ConvertToBin64(temp, &result.uinteger) == 0) {
        DEBUG("Chosen bin\n");
        result.type = CONVERT_RESULT_BIN;
    }
    else if (ConvertToDecSigned64(temp, &result.integer) == 0) {
        DEBUG("Chosen signed dec\n");
        result.type = CONVERT_RESULT_INT;
    }
    else if (ConvertToDecUnsigned64(temp, &result.uinteger) == 0) {
        DEBUG("Chosen unsigned dec\n");
        result.type = CONVERT_RESULT_UINT;
    }
    else if (ConvertToFloat64(temp, &result.real) == 0) {
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

ConvertResult32 ConvertString32(const char *string)
{
    return downcast(ConvertString64(string));
}

static int dump_int64(FILE *stream, int64_t value, int longform)
{
    (void)longform;
    if (!stream) return 0;
    return fprintf(stream, "%+"PRId64, value);
}

static int dump_int32(FILE *stream, int32_t value, int longform)
{
    (void)longform;
    if (!stream) return 0;
    return fprintf(stream, "%+"PRId32, value);
}

static int dump_uint64(FILE *stream, uint64_t value, int longform)
{
    (void)longform;
    if (!stream) return 0;
    return fprintf(stream, "%"PRIu64, value);
}

static int dump_uint32(FILE *stream, uint32_t value, int longform)
{
    (void)longform;
    if (!stream) return 0;
    return fprintf(stream, "%"PRIu32, value);
}

static int dump_hex64(FILE *stream, uint64_t value, int longform)
{
    (void)longform;
    if (!stream) return 0;
    return fprintf(stream, "0x%"PRIx64, value);
}

static int dump_hex32(FILE *stream, uint32_t value, int longform)
{
    (void)longform;
    if (!stream) return 0;
    return fprintf(stream, "0x%"PRIx32, value);
}

static int dump_oct64(FILE *stream, uint64_t value, int longform)
{
    (void)longform;
    if (!stream) return 0;
    return fprintf(stream, "0o%"PRIo64, value);
}

static int dump_oct32(FILE *stream, uint32_t value, int longform)
{
    (void)longform;
    if (!stream) return 0;
    return fprintf(stream, "0o%"PRIo32, value);
}

static int dump_float64(FILE *stream, double value, int longform)
{
    (void)longform;
    if (!stream) return 0;
    return fprintf(stream, "%lf", value);
}

static int dump_float32(FILE *stream, float value, int longform)
{
    (void)longform;
    if (!stream) return 0;
    return fprintf(stream, "%f", value);
}

static int dump_bin64(FILE *stream, uint64_t value, int longform)
{
    int i;
    int tmp;
    char buf[(sizeof(value) * 8) + 2 + 1] = "0b";
    char *p = &buf[2];

    if (!stream) return 0;

    for (i = (int)(sizeof(value) * 8) - 1; i >= 0; i--) {
        tmp = !!(value & (1ull << i));
        p[(sizeof(value) * 8 - 1) - i] = '0' + tmp;
    }

    if (!longform) {
        p = strchr(p, '1');
        /* at least a one */
        if (p) {
            p[-2] = '0';
            p[-1] = 'b';
            p = &p[-2];
        }
        /* all zero, 0b0 */
        else {
            buf[2] = '0';
            buf[3] = '\0';
            p = buf;
        }
    }
    else {
        p = buf;
    }

    return fprintf(stream, "%s", p);
}

static int dump_bin32(FILE *stream, uint32_t value, int longform)
{
    int i;
    int tmp;
    char buf[(sizeof(value) * 8) + 2 + 1] = "0b";
    char *p = &buf[2];

    if (!stream) return 0;

    for (i = (int)(sizeof(value) * 8) - 1; i >= 0; i--) {
        tmp = !!(value & (1ull << i));
        p[(sizeof(value) * 8 - 1) - i] = '0' + tmp;
    }

    if (!longform) {
        p = strchr(p, '1');
        /* at least a one */
        if (p) {
            p[-2] = '0';
            p[-1] = 'b';
            p = &p[-2];
        }
        /* all zero, 0b0 */
        else {
            buf[2] = '0';
            buf[3] = '\0';
            p = buf;
        }
    }
    else {
        p = buf;
    }

    return fprintf(stream, "%s", p);
}

static int dump_result64(ConvertResult64 *result, FILE *stream, int longform)
{
    if (!result || !stream) return 0;

    switch (result->type)
    {
    case CONVERT_RESULT_INT:   return dump_int64(stream, result->integer, longform);
    case CONVERT_RESULT_UINT:  return dump_uint64(stream, result->uinteger, longform);
    case CONVERT_RESULT_HEX:   return dump_hex64(stream, result->uinteger, longform);
    case CONVERT_RESULT_OCT:   return dump_oct64(stream, result->uinteger, longform);
    case CONVERT_RESULT_FLOAT: return dump_float64(stream, result->real, longform);
    case CONVERT_RESULT_BIN:   return dump_bin64(stream, result->uinteger, longform);
    default:
        return 0;
    }
}

static int dump_result32(ConvertResult32 *result, FILE *stream, int longform)
{
    if (!result || !stream) return 0;

    switch (result->type)
    {
    case CONVERT_RESULT_INT:   return dump_int32(stream, result->integer, longform);
    case CONVERT_RESULT_UINT:  return dump_uint32(stream, result->uinteger, longform);
    case CONVERT_RESULT_HEX:   return dump_hex32(stream, result->uinteger, longform);
    case CONVERT_RESULT_OCT:   return dump_oct32(stream, result->uinteger, longform);
    case CONVERT_RESULT_FLOAT: return dump_float32(stream, result->real, longform);
    case CONVERT_RESULT_BIN:   return dump_bin32(stream, result->uinteger, longform);
    default:
        return 0;
    }
}

int ConvertResultDump64(ConvertResult64 *result, FILE *stream)
{
    return dump_result64(result, stream, 0);
}

int ConvertResultDumpLong64(ConvertResult64 *result, FILE *stream)
{
    return dump_result64(result, stream, 1);
}

int ConvertResultDump32(ConvertResult32 *result, FILE *stream)
{
    return dump_result32(result, stream, 0);
}

int ConvertResultDumpLong32(ConvertResult32 *result, FILE *stream)
{
    return dump_result32(result, stream, 1);
}

void ConvertSetMagnitudeChar(int sep)
{
    sep_magnitude = sep;
}

void ConvertSetDecimalChar(int dec)
{
    sep_decimal = dec;
}

int ConvertToFloat64(const char *string, double *out)
{
    double convert;

    if (!string) return 1;
    if (sscanf(string, "%lf", &convert) != 1) return 1;
    if (out) *out = convert;

    return 0;
}

int ConvertToFloat32(const char *string, float *out)
{
    double result;
    int rv = ConvertToFloat64(string, &result);
    if (rv != 0) return rv;
    if (out) *out = result;
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

static int convert_hex(const char *string, uint64_t *out)
{
    if (!string || !out) return 1;
    if (sscanf(string, "%"PRIx64, out) != 1) return 1;
    return 0;
}

static int convert_oct(const char *string, uint64_t *out)
{
    if (!string || !out) return 1;
    if (sscanf(string, "%"PRIo64, out) != 1) return 1;
    return 0;
}

static int convert_bin(const char *string, uint64_t *out)
{
    const char *p;
    int64_t i;
    uint64_t builder;

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
    int (*convert)(const char *, uint64_t *))
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

int ConvertToHex64(const char *string, uint64_t *out)
{
    uint64_t convert;
    int rv;

    rv = convert_value(string, &convert, '0', 'x', 'X', convert_hex);
    if (rv != 0) return 1;
    if (out) *out = convert;
    return 0;
}

int ConvertToHex32(const char *string, uint32_t *out)
{
    uint64_t result;
    int rv = ConvertToHex64(string, &result);
    if (rv != 0) return rv;
    if (out) *out = result;
    return 0;
}

int ConvertToOct64(const char *string, uint64_t *out)
{
    uint64_t convert;
    int rv;

    rv = convert_value(string, &convert, '0', 'o', 'O', convert_oct);
    if (rv != 0) return 1;
    if (out) *out = convert;
    return 0;
}

int ConvertToOct32(const char *string, uint32_t *out)
{
    uint64_t result;
    int rv = ConvertToOct64(string, &result);
    if (rv != 0) return rv;
    if (out) *out = result;
    return 0;
}

int ConvertToBin64(const char *string, uint64_t *out)
{
    uint64_t convert;
    int rv;

    rv = convert_value(string, &convert, '0', 'b', 'B', convert_bin);
    if (rv != 0) return 1;
    if (out) *out = convert;
    return 0;
}

int ConvertToBin32(const char *string, uint32_t *out)
{
    uint64_t result;
    int rv = ConvertToBin64(string, &result);
    if (rv != 0) return rv;
    if (out) *out = result;
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

int ConvertToDecSigned64(const char *string, int64_t *out)
{
    int rv;
    int negative;
    int64_t convert;

    if (!string) return 1;
    negative = (string[0] == '-');
    if (negative || (string[0] == '+')) string++;

    rv = is_all_digits(string);
    if (rv != 0) return 1;

    if (out) {
        if (sscanf(string, "%"PRId64, &convert) != 1) return 1;
        *out = (negative ? -1 : 1) * convert;
    }
    return 0;
}

int ConvertToDecSigned32(const char *string, int32_t *out)
{
    int64_t result;
    int rv = ConvertToDecSigned64(string, &result);
    if (rv != 0) return rv;
    if (out) *out = result;
    return 0;
}

int ConvertToDecUnsigned64(const char *string, uint64_t *out)
{
    int rv;
    uint64_t convert;

    if (!string) return 1;
    rv = is_all_digits(string);
    if (rv != 0) return 1;

    if (out) {
        if (sscanf(string, "%"PRIu64, &convert) != 1) return 1;
        *out = convert;
    }
    return 0;
}

int ConvertToDecUnsigned32(const char *string, uint32_t *out)
{
    uint64_t result;
    int rv = ConvertToDecUnsigned64(string, &result);
    if (rv != 0) return rv;
    if (out) *out = result;
    return 0;
}


#ifndef CONVERT_TEST
#define CONVERT_TEST
#endif

#if defined(CONVERT_TEST)

static int test_float(void)
{
    ConvertResult64 result;
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
        result = ConvertString64(cases[i].value);

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
    ConvertResult64 result;
    size_t i;
    int rv = 0;

    struct {
        const char *value;
        uint64_t expected;
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
        result = ConvertString64(cases[i].value);

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
    ConvertResult64 result;
    size_t i;
    int rv = 0;

    struct {
        const char *value;
        uint64_t expected;
        int expected_rv;
    } cases[] = {
        {"awe32p9r0awfe,32", 0, 1},
        {"1", 0, 1},
        {".1234e3", 0, 1},
        {"0o10", 1, 0},
        {"-0b11", 0, 1},

        {"0x01", 0x01, 1},
        {"0x11", 0x11, 1},
        {"0x000F_0000", 0x000F0000, 1},

        {0, 0, 0}
    };

    DEBUG("%s\n", __func__);
    for (i = 0; cases[i].value != NULL; i++) {
        result = ConvertString64(cases[i].value);

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
    ConvertResult64 result;
    size_t i;
    int rv = 0;

    struct {
        const char *value;
        uint64_t expected;
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
        result = ConvertString64(cases[i].value);

        if ((result.uinteger != cases[i].expected) &&
           ((result.type >= CONVERT_RESULT_ERROR) && (cases[i].expected_rv == 0)))
        {
            rv += 1;
        }
    }
    return rv;

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
        ConvertResult64 result = ConvertString64(argv[1]);
        if (result.type >= CONVERT_RESULT_ERROR) {
            printf("Failed to convert: %s: ", argv[1]);
            return 1;
        }
        ConvertResultDump64(&result, stdout);
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
