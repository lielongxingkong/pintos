#include "fixed_point.h"

fixed int_to_fixed (int n)
{
  return n * F;
}

int fixed_round_to_zero_int (fixed x)
{
  return x / F;
}

int fixed_round_to_nearest_int (fixed x)
{
  if (x > 0)
    return (x + F / 2) / F;
  else if (x < 0)
    return (x - F / 2) / F;
  else
    return 0;
}

fixed fixed_add (fixed x, fixed y)
{
  return x + y;
}

fixed fixed_sub (fixed x, fixed y)
{
  return x - y;
}

fixed fixed_int_add (fixed x, int n)
{
  return x + n * F;
}

fixed fixed_int_sub (fixed x, int n)
{
  return x - n * F;
}

fixed fixed_mul (fixed x, fixed y)
{
  return ((int64_t) x) * y / F;
}

fixed fixed_int_mul (fixed x, int n)
{
  return x * n;
}

fixed fixed_div (fixed x, fixed y)
{
  return ((int64_t) x) * F / y;
}

fixed fixed_int_div (fixed x, int n)
{
  return x / n;
}
