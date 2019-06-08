/**
 * @ Author: SmartPolarBear
 * @ Create Time: 2019-06-08 17:37:48
 * @ Modified by: SmartPolarBear
 * @ Modified time: 2019-06-08 20:22:59
 * @ Description: Inspired by the version used by the klibc 1.5.20
 */

/* 
 *
 * Original License 
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * Any copyright notice(s) and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "xv6/types.h"
#include "stdint.h"

//small to big
int __builtin_intascendingcmp(const void *a, const void *b)
{
    return (*((int *)a)) > (*((int *)b));
}

//big to small
int __builtin_intdescendingcmp(const void *a, const void *b)
{
    return (*((int *)a)) < (*((int *)b));
}

void memswap(void *m1, void *m2, size_t n)
{
    char *p = m1, *q = m2, t = 0;
    while (n--)
    {
        t = *p;
        *p = *q;
        *q = t;

        p++;
        q++;
    }
}

static inline size_t newgap(size_t gap)
{
    gap = (gap * 10) / 13;
    if (gap == 9 || gap == 13)
        gap = 11;

    if (gap < 1)
        gap = 1;
    return gap;
}

//cmp returns true if needs swap
void qsort(void *base, size_t nmemb, size_t size, int (*cmp)(const void *, const void *))
{
    size_t gap = nmemb, i = 0, j = 0;
    char *p1 = NULL, *p2 = NULL;
    BOOL swapped = FALSE;

    if (!nmemb)
        return;

    do
    {
        gap = newgap(gap);
        swapped = FALSE;

        for (i = 0, p1 = base; i < nmemb - gap; i++, p1 += size)
        {
            j = i + gap;
            if (cmp(p1, p2 = (char *)base + j * size) > 0)
            {
                memswap(p1, p2, size);
                swapped = TRUE;
            }
        }
    } while (gap > 1 || swapped);
}