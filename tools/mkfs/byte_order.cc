#include "byte_order.h"

// convert to intel byte order
ushort byte_order_intel::xshort(ushort x)
{
    ushort y; 
    uchar *a = (uchar *)&y;
    a[0] = x;
    a[1] = x >> 8;
    return y;
}

uint byte_order_intel::xint(uint x)
{
    uint y;
    uchar *a = (uchar *)&y;
    a[0] = x;
    a[1] = x >> 8;
    a[2] = x >> 16;
    a[3] = x >> 24;
    return y;
}