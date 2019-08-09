#if !defined(__TOOLS_MKFS_BYTE_ORDER_H)
#define __TOOLS_MKFS_BYTE_ORDER_H

#include "common.h"

// convert to intel byte order
namespace byte_order_intel
{
ushort xshort(ushort x);
uint xint(uint x);
}; // namespace byte_order_intel

#endif // __TOOLS_MKFS_BYTE_ORDER_H
