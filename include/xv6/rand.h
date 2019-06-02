#ifndef __INCLUDE_XV6_RAND_H
#define __INCLUDE_XV6_RAND_H

void sgenrand(unsigned long);
long genrand(void);
long random_at_most(long);

#endif
