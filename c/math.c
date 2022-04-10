#include <assert.h>
#include "math.h"

static int cmp1(ConvertResult64 *a, enum _convert_result_enum_ type)
{
    assert(a != NULL);
    if (a->type & type) return 1;
    return 0;
}

static int cmp2(ConvertResult64 *a, ConvertResult64 *b, enum _convert_result_enum_ type)
{
    assert(a != NULL);
    assert(b != NULL);
    return (cmp1(a, type) && cmp1(b, type));
}

static int cmpdiff(ConvertResult64 *a, enum _convert_result_enum_ typea, ConvertResult64 *b, enum _convert_result_enum_ typeb)
{
    assert(a != NULL);
    assert(b != NULL);
    return (cmp1(a, typea) && cmp1(b, typeb));
}

static int coerce(ConvertResult64 *a, ConvertResult64 *b)
{
    assert(a != NULL);
    assert(b != NULL);

    if (a->type & CONVERT_RESULT_INT) {
        if (b->type & CONVERT_RESULT_INT) {
            return 0;
        }
        else if (b->type & CONVERT_RESULT_UINT) {
            a->type = b->type;
            return 0;
        }
        else if (b->type & CONVERT_RESULT_FLOAT) {
            a->type = b->type;
            a->real = a->integer;
            return 0;
        }
        else {
            return 1;
        }
    }
    else if (a->type & CONVERT_RESULT_UINT) {
        if (b->type & CONVERT_RESULT_INT) {
            b->type = a->type;
            return 0;
        }
        else if (b->type & CONVERT_RESULT_UINT) {
            return 0;
        }
        else if (b->type & CONVERT_RESULT_FLOAT) {
            a->type = b->type;
            a->real = a->uinteger;
            return 0;
        }
        else {
            return 1;
        }
    }
    else if (a->type & CONVERT_RESULT_FLOAT) {
        if (b->type & CONVERT_RESULT_INT) {
            b->type = a->type;
            b->real = b->integer;
            return 0;
        }
        else if (b->type & CONVERT_RESULT_UINT) {
            b->type = a->type;
            b->real = b->uinteger;
            return 0;
        }
        else if (b->type & CONVERT_RESULT_FLOAT) {
            return 0;
        }
        else {
            return 1;
        }
    }

    return 1;
}

ConvertResult64 ConvertAdd(ConvertResult64 a, ConvertResult64 b)
{
    if ((a.type&CONVERT_RESULT_INT) == (a.type&CONVERT_RESULT_INT)) {

    }
    else if ((a.type&CONVERT_RESULT_INT) == (a.type&CONVERT_RESULT_INT)) {

    }
    else if ((a.type&CONVERT_RESULT_INT) == (a.type&CONVERT_RESULT_INT)) {

    }
    else {

    }
}

ConvertResult64 ConvertSub(ConvertResult64 a, ConvertResult64 b);
ConvertResult64 ConvertMul(ConvertResult64 a, ConvertResult64 b);
ConvertResult64 ConvertDiv(ConvertResult64 a, ConvertResult64 b);
ConvertResult64 ConvertMod(ConvertResult64 a, ConvertResult64 b);
ConvertResult64 ConvertNeg(ConvertResult64 a);
ConvertResult64 ConvertNot(ConvertResult64 a);
ConvertResult64 ConvertPow(ConvertResult64 a, ConvertResult64 b);
ConvertResult64 ConvertAbs(ConvertResult64 a);

ConvertResult64 ConvertEqu(ConvertResult64 a, ConvertResult64 b);
ConvertResult64 ConvertNeq(ConvertResult64 a, ConvertResult64 b);
ConvertResult64 ConvertGt(ConvertResult64 a, ConvertResult64 b);
ConvertResult64 ConvertGte(ConvertResult64 a, ConvertResult64 b);
ConvertResult64 ConvertLt(ConvertResult64 a, ConvertResult64 b);
ConvertResult64 ConvertLte(ConvertResult64 a, ConvertResult64 b);

ConvertResult64 ConvertAnd(ConvertResult64 a, ConvertResult64 b);
ConvertResult64 ConvertOr(ConvertResult64 a, ConvertResult64 b);

ConvertResult64 ConvertShl(ConvertResult64 a, ConvertResult64 b);
ConvertResult64 ConvertShr(ConvertResult64 a, ConvertResult64 b);

ConvertResult64 ConvertXor(ConvertResult64 a, ConvertResult64 b);
ConvertResult64 ConvertBitand(ConvertResult64 a, ConvertResult64 b);
ConvertResult64 ConvertBitor(ConvertResult64 a, ConvertResult64 b);
ConvertResult64 ConvertBitclear(ConvertResult64 a, ConvertResult64 b);
ConvertResult64 ConvertInv(ConvertResult64 a);

ConvertResult64 ConvertCast(ConvertResult64 a, enum _convert_result_enum_ type)
{
    if (type & CONVERT_RESULT_INT) {

    }
    else if (type & CONVERT_RESULT_UINT) {

    }
    else if (type & CONVERT_RESULT_FLOAT) {

    }
}

ConvertResult64 ConvertPun(ConvertResult64 a, enum _convert_result_enum_ type)
{

}