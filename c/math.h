#ifndef CONVERT_MATH_H_
#define CONVERT_MATH_H_

#include "convert.h"

ConvertResult64 ConvertAdd(ConvertResult64 a, ConvertResult64 b);
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

ConvertResult64 ConvertCast(ConvertResult64 a, enum _convert_result_enum_ type);
ConvertResult64 ConvertPun(ConvertResult64 a, enum _convert_result_enum_ type);


#endif /* CONVERT_MATH_H_ */
