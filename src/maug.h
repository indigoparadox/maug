
#ifndef MAUG_H
#define MAUG_H

#include <mlegacy.h>

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

#ifdef MAUG_C
#  define MCONFIG_C
#endif /* MAUG_C */
#include <mconfig.h>

#endif /* MAUG_H */

