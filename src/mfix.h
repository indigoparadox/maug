
#ifndef MFIX_H
#define MFIX_H

/**
 * \addtogroup maug_mfix Maug Fixed Point
 * \{
 *
 * \file mfix.h
 * \brief Contains functions and macros for working with fixed-point numbers.
 *
 * Maug fixed point numbers are rounded to 3 decimal places (e.g. ::RETROFP_PI).
 */

/* Needed for PI constant. */
#include <math.h>

#ifndef MFIX_PRECISION
#  define MFIX_PRECISION (1000.0f)
#endif /* !MFIX_PRECISION */

#define MFIX_PRECISION_DIV (1.0f / (MFIX_PRECISION))

/*! \brief Fixed-point representation of Pi (3.141). */
#define MFIX_PI (int)((float)(M_PI) * (float)(MFIX_PRECISION))

#define MFIX_LUT_CT 63

typedef int32_t mfix_t;

#define mfix_from_f( n ) ((mfix_t)((n) * MFIX_PRECISION))

#define mfix_from_i( n ) ((mfix_t)((n) * MFIX_PRECISION))

#define mfix_to_i( n ) ((mfix_t)((n) / MFIX_PRECISION))

#ifdef MFIX_DEBUG
#define mfix_sin( num ) _mfix_lut( gc_mfix_sin, "sin", num )
#define mfix_cos( num ) _mfix_lut( gc_mfix_cos, "cos", num )
#else
#define mfix_sin( num ) _mfix_lut( gc_mfix_sin, num )
#define mfix_cos( num ) _mfix_lut( gc_mfix_cos, num )
#endif /* MFIX_DEBUG */

/**
 * \brief Given a lookup table, return the corresponding value.
 *
 * Intended to work with the mfix_sin() and mfix_cos() macros.
 */
mfix_t _mfix_lut( const mfix_t* SEG_MCONST lut,
#ifdef MFIX_DEBUG
   const char* fun,
#endif /* MFIX_DEBUG */
   mfix_t num );

/**
 * \brief Divide a mfix_t by an integer.
 *
 * This is a function to enforce type checks and allow for other safety checks.
 */
mfix_t mfix_div( mfix_t dividend, int divisor );

mfix_t mfix_mult( mfix_t m1, int m2 );

#ifdef MFIX_C

MAUG_CONST mfix_t SEG_MCONST gc_mfix_cos[MFIX_LUT_CT] = {
   1000,
   995,
   980,
   955,
   921,
   877,
   825,
   764,
   696,
   621,
   540,
   453,
   362,
   267,
   169,
   70,
   -29,
   -128,
   -227,
   -323,
   -416,
   -504,
   -588,
   -666,
   -737,
   -801,
   -856,
   -904,
   -942,
   -970,
   -989,
   -999,
   -998,
   -987,
   -966,
   -936,
   -896,
   -848,
   -790,
   -725,
   -653,
   -574,
   -490,
   -400,
   -307,
   -210,
   -112,
   -12,
   87,
   186,
   283,
   377,
   468,
   554,
   634,
   708,
   775,
   834,
   885,
   927,
   960,
   983,
   996,
};

/* === */

MAUG_CONST mfix_t SEG_MCONST gc_mfix_sin[MFIX_LUT_CT] = {
   0,
   99,
   198,
   295,
   389,
   479,
   564,
   644,
   717,
   783,
   841,
   891,
   932,
   963,
   985,
   997,
   999,
   991,
   973,
   946,
   909,
   863,
   808,
   745,
   675,
   598,
   515,
   427,
   334,
   239,
   141,
   41,
   -58,
   -157,
   -255,
   -350,
   -442,
   -529,
   -611,
   -687,
   -756,
   -818,
   -871,
   -916,
   -951,
   -977,
   -993,
   -999,
   -996,
   -982,
   -958,
   -925,
   -883,
   -832,
   -772,
   -705,
   -631,
   -550,
   -464,
   -373,
   -279,
   -182,
   -83,
};

/* === */

#ifdef MFIX_DEBUG
mfix_t _mfix_lut( const mfix_t* SEG_MCONST lut, const char* fun, mfix_t num ) {
#else
mfix_t _mfix_lut( const mfix_t* SEG_MCONST lut, mfix_t num ) {
#endif /* MFIX_DEBUG */
   mfix_t cos_out;
   uint8_t neg = 0;
#ifdef MFIX_DEBUG
   mfix_t num_orig = num;
#endif /* MFIX_DEBUG */

   assert( MFIX_PRECISION == 1000 );

   /* Can't take an index of a negative number, so hold on to neg for later. */
   if( num < 0 ) {
      neg = 1;
      num *= -1;
   }

   /* cos/sin repeat after every 2PI. */
   while( num >= (2 * MFIX_PI) ) {
      num -= (2 * MFIX_PI);
   }

   /* Remove num precision to get index. */
   num /= (MFIX_PRECISION / 10);
   assert( MFIX_LUT_CT > num );
   assert( 0 <= num );
   cos_out = lut[num];

   /* Restore neg taken earlier. */
   if( neg ) {
      cos_out *= -1;
   }

#ifdef MFIX_DEBUG
   debug_printf( 1, "%s %d = %d (%s %d = %f)",
      fun, num_orig, cos_out, fun, num,
      0 == strcmp( fun, "sin" ) ? sin( num_orig / (MFIX_PRECISION) ) :
         cos( num_orig / (MFIX_PRECISION) ) );
#endif /* MFIX_DEBUG */

   return cos_out;
}

/* === */

mfix_t mfix_div( mfix_t dividend, int divisor ) {
   /* TODO: Range checks. */
   return dividend / divisor;
}

/* === */

mfix_t mfix_mult( mfix_t m1, int m2 ) {
   return (mfix_t)((m1) * ((float)(m2) / 1000.f));
}

#else

extern MAUG_CONST mfix_t SEG_MCONST gc_mfix_cos[];
extern MAUG_CONST mfix_t SEG_MCONST gc_mfix_sin[];

#endif /* MFIX_C */

#endif /* !MFIX_H */

