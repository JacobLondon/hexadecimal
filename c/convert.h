#ifndef CONVERT_H_
#define CONVERT_H_

#include <stdio.h>

typedef struct {
    enum {
        CONVERT_RESULT_INT,
        CONVERT_RESULT_UINT,
        CONVERT_RESULT_HEX,
        CONVERT_RESULT_OCT,
        CONVERT_RESULT_BIN,
        CONVERT_RESULT_FLOAT,

        CONVERT_RESULT_ERROR, /* boundary */

        CONVERT_RESULT_NULL_STRING,
        CONVERT_RESULT_CANT_CONVERT,
    } type;
    union {
        unsigned long long uinteger;
        long long integer;
        double real;
    };
} ConvertResult;

ConvertResult ConvertString(const char *string);
int ConvertResultDump(ConvertResult *result, FILE *stream); /* return bytes written */
int ConvertResultDumpLong(ConvertResult *result, FILE *stream); /* return bytes written */

void ConvertSetMagnitudeChar(int sep); /* default is ',' */
void ConvertSetDecimalChar(int dec); /* default is '.' */

int ConvertRemoveMagnitudes(const char *input, char *output, size_t output_size);

/* the following will fail with magnitude chars, they shouldbe trimmed first */

int ConvertToFloat(const char *string, double *out);
int ConvertToHex(const char *string, unsigned long long *out);
int ConvertToDecSigned(const char *string, long long *out);
int ConvertToDecUnsigned(const char *string, unsigned long long *out);
int ConvertToOct(const char *string, unsigned long long *out);
int ConvertToBin(const char *string, unsigned long long *out);

#endif /* CONVERT_H_ */
