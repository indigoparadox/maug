
#ifndef MAUG_H
#define MAUG_H

#include <mlegacy.h>

#ifdef MAUG_OS_PALM
/* PalmOS doesn't work with const. */
#  define MAUG_CONST
#else
#  define MAUG_CONST const
#endif /* MAUG_OS_PALM */

#if !defined( MAUG_WINDOWS_H ) && defined( MAUG_OS_WIN )
#  include <windows.h>
#  define MAUG_WINDOWS_H
#endif /* !MAUG_WINDOWS_H */

#include <merror.h>

#ifdef MAUG_C
#  define UPRINTF_C
#endif /* MAUG_C */
#include <uprintf.h>

#ifdef MAUG_C
#  define MARGE_C
#endif /* MAUG_C */
#include <marge.h>

#ifndef MAUG_NO_CONFIG
#ifdef MAUG_C
#  define MCONFIG_C
#endif /* MAUG_C */
#include <mconfig.h>
#endif /* !MAUG_NO_CONFIG */

#endif /* MAUG_H */

