#ifndef _Q_MATH_H
#define Q__MATH_H

#ifdef _MSC_VER
#include <stddef.h>
#include <stdint.h>
#else
#include <stdint.h>
#endif

double Q_fabs(double f);

double Q_rsqrt(double f);		// reciprocal square root

#define SQRTFAST( x ) ( 1.0f / Q_rsqrt( x ) )

int32_t	Q_rand(int32_t *seed);

double Q_random(int32_t *seed);

double Q_crandom(int32_t *seed);

#endif /* _Q_MATH_H */
