
#ifndef RETROFP_H
#define RETROFP_H

/* TODO: Base on mfp. */

/**
 * \addtogroup maug_retrofp Maug Retro Fixed Point
 * \{
 *
 * \file retrofp.h
 * \brief Contains functions and macros for working with fixed-point numbers.
 *
 * Maug retro fixed point numbers are rounded to 3 decimal places (e.g. 
 * ::RETROFP_PI).
 */

/*! \brief Fixed-point representation of Pi (3.141). */
#define RETROFP_PI (3141)

/**
 * \brief Given a lookup table, return the corresponding value after trimming
 *        precision and making sure number is valid.
 */
int16_t retrofp_lut(
   const int16_t* SEG_MCONST lut, int16_t num, int16_t mult );

#define retrofp_sin( num, mult ) retrofp_lut( g_retrofp_sin, num, mult )

#define retrofp_cos( num, mult ) retrofp_lut( g_retrofp_cos, num, mult )

#ifdef RETROFP_C

MAUG_CONST int16_t SEG_MCONST g_retrofp_cos[] = {
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

MAUG_CONST int16_t SEG_MCONST g_retrofp_sin[] = {
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

int16_t retrofp_lut(
   const int16_t* SEG_MCONST lut, int16_t num, int16_t mult
) {
   int16_t cos_out;
   uint8_t neg = 0;

   /* Can't take an index of a negative number, so hold on to neg for later. */
   if( num < 0 ) {
      neg = 1;
      num *= -1;
   }

   /* cos/sin repeat after every 2PI. */
   if( num >= (2 * RETROFP_PI) ) {
      num -= (2 * RETROFP_PI);
   }

   /* Remove num precision to get index. */
   num /= 100;
   cos_out = lut[num];

   /* Multiply by multiplier before removing precision. */
   cos_out *= mult;
   cos_out /= 1000;

   /* Restore neg taken earlier. */
   if( neg ) {
      cos_out *= -1;
   }

   return (int16_t)cos_out;
}

#else

extern MAUG_CONST int16_t SEG_MCONST g_retrofp_cos[];
extern MAUG_CONST int16_t SEG_MCONST g_retrofp_sin[];

#endif /* RETROFP_C */

/*! \} */ /* maug_retrofp */

#endif /* !RETROFP_H */

