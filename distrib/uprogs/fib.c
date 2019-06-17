/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-06-16 23:35:05
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-06-17 22:48:09
 * @ Description:
 */

#include "xv6/types.h"
#include "xv6/stat.h"
#include "xv6/user.h"

long long fib(long long n)
{
    return n < 2 ? n : (fib(n - 1) + fib(n - 2));
}

int main()
{
     printf(1, "FIB(%d)=%d\n", 1000, fib(1000));
    return 0;
}