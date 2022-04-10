#ifndef CONVERT_H_
#define CONVERT_H_

#include <stdio.h>
#include <stdint.h>

enum _convert_result_enum_ {
    CONVERT_RESULT_INT,
    CONVERT_RESULT_UINT,
    CONVERT_RESULT_HEX,
    CONVERT_RESULT_OCT,
    CONVERT_RESULT_BIN,
    CONVERT_RESULT_FLOAT,
    /* boundary */
    CONVERT_RESULT_ERROR,
    CONVERT_RESULT_NULL_STRING,
    CONVERT_RESULT_CANT_CONVERT,
};

union _convert_result_value32_ {
    int32_t integer;
    uint32_t uinteger;
    float real;
};

typedef struct {
    enum _convert_result_enum_ type;
    union {
        int64_t integer;
        uint64_t uinteger;
        double real;
    };
} ConvertResult64;

typedef struct {
    enum _convert_result_enum_ type;
    union {
        int32_t integer;
        uint32_t uinteger;
        float real;
    };
} ConvertResult32;

int ConvertAsciiToOrd(char *string);
int ConvertOrdToAscii(char *number);

void ConvertSetMagnitudeChar(int sep); /* default is ',' */
void ConvertSetDecimalChar(int dec); /* default is '.' */
int ConvertRemoveMagnitudes(const char *input, char *output, size_t output_size);

#define CONVERT_DEFINE_PROTOTYPES_MAIN(BITS)                                  \
    ConvertResult##BITS ConvertString##BITS(const char *string);              \
    /* return bytes written */                                                \
    int ConvertResultDump##BITS(ConvertResult##BITS *result, FILE *stream);   \
    int ConvertResultDumpLong##BITS(ConvertResult##BITS *result, FILE *stream);

/* the following will fail with magnitude chars, they should be trimmed first */
#define CONVERT_DEFINE_PROTOTYPES_HELPERS(BITS, FLOAT)                        \
    int ConvertToFloat##BITS(const char *string, FLOAT *out);                 \
    int ConvertToHex##BITS(const char *string, uint##BITS##_t *out);          \
    int ConvertToDecSigned##BITS(const char *string, int##BITS##_t *out);     \
    int ConvertToDecUnsigned##BITS(const char *string, uint##BITS##_t *out);  \
    int ConvertToOct##BITS(const char *string, uint##BITS##_t *out);          \
    int ConvertToBin##BITS(const char *string, uint##BITS##_t *out);          \


/* 64 bits */
CONVERT_DEFINE_PROTOTYPES_MAIN(64)
CONVERT_DEFINE_PROTOTYPES_HELPERS(64, double)

/* 32 bits */
CONVERT_DEFINE_PROTOTYPES_MAIN(32)
CONVERT_DEFINE_PROTOTYPES_HELPERS(32, float)

#endif /* CONVERT_H_ */
