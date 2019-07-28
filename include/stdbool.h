#if !defined(__INCLUDE_STDBOOL_H)
#define __INCLUDE_STDBOOL_H

#if !defined(__cplusplus)

/**
 * Define the Boolean macros only if they are not already defined.
 */
#ifndef __bool_true_false_are_defined
#define bool _Bool
#define false 0 
#define true 1
#define __bool_true_false_are_defined 1
#endif /* __bool_true_false_are_defined */

#endif


#endif // __INCLUDE_STDBOOL_H
