
#ifndef MLEGACY_H
#define MLEGACY_H

#ifdef PATH_MAX
#  define MAUG_PATH_SZ_MAX PATH_MAX
#else
#  define MAUG_PATH_SZ_MAX 255
#endif /* PATH_MAX */

#ifdef ANCIENT_C
#define  UPRINTF_ANCIENT_C
#endif /* ANCIENT_C */

#if defined( DEBUG_LOG )
#  define UPRINTF_LOG
#endif /* DEBUG_LOG */

#if defined( PLATFORM_PALM )
#  define MAUG_OS_PALM
#elif defined( PLATFORM_WIN ) || defined( RETROFLAT_OS_WIN )
#  define MAUG_OS_WIN
#endif /* PLATFORM_* */

#if defined( RETROFLAT_API_WIN16 )
#  define MAUG_API_WIN16
#elif defined( RETROFLAT_API_WIN32 )
#  define MAUG_API_WIN32
#endif /* RETROFLAT_API_WIN32 || RETROFLAT_API_WIN16 */

#if defined( DEBUG )
#include <assert.h>
#elif !defined( DOCUMENTATION )
#define assert( x )
#endif /* DEBUG */

#endif /* !MLEGACY_H */

