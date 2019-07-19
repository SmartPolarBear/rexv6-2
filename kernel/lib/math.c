#include "xv6/types.h"

float floor(float x)
{
    if (x == 0.0)
        return 0;

    union {
        float input; // assumes sizeof(float) == sizeof(int)
        int output;
    } data;

    data.input = x;

    // get the exponent 23~30 bit
    int exp = data.output & (255 << 23);
    exp = exp >> 23;

    // get the mantissa 0~22 bit
    int man = data.output & ((1 << 23) - 1);

    int pow = exp - 127;
    int mulFactor = 1;

    int i = abs(pow);
    while (i--)
        mulFactor *= 2;

    unsigned long long denominator = 1 << 23;
    unsigned long long numerator = man + denominator;

    // most significant bit represents the sign of the number
    BOOL negative = (data.output >> 31) != 0;

    if (pow < 0)
        denominator *= mulFactor;
    else
        numerator *= mulFactor;

    float res = 0.0;
    while (numerator >= denominator)
    {
        res++;
        numerator -= denominator;
    }

    if (negative)
    {
        res = -res;
        if (numerator != 0)
            res -= 1;
    }

    return res;
}

float ceil(float x)
{
    return -floor(-x);
}