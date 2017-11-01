#ifndef __LIB_KERNEL_FIXED_POINT_H
#define __LIB_KERNEL_FIXED_POINT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef int32_t fixed;

#define SHIFT   14
#define F       (1 << SHIFT)

fixed int_to_fixed (int);
int fixed_round_to_zero_int (fixed);
int fixed_round_to_nearest_int (fixed);
fixed fixed_add (fixed, fixed);
fixed fixed_sub (fixed, fixed);
fixed fixed_int_add (fixed, int);
fixed fixed_int_sub (fixed, int);
fixed fixed_mul (fixed, fixed);
fixed fixed_int_mul (fixed, int);
fixed fixed_div (fixed, fixed);
fixed fixed_int_div (fixed, int);

#endif /* lib/kernel/fixed_point.h */
