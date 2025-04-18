
#ifndef MFIX_H
#define MFIX_H

#ifndef MFIX_PRECISION
#  define MFIX_PRECISION (100.0f)
#endif /* !MFIX_PRECISION */

#define MFIX_PRECISION_DIV (1.0f / (MFIX_PRECISION))

typedef int16_t mfix_t;

#define mfix( n ) ((mfix_t)((n) * MFIX_PRECISION))

#endif /* !MFIX_H */

