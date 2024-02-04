
#ifndef MAUG_H
#define MAUG_H

#include <mlegacy.h>

#ifdef MAUG_OS_PALM
/* PalmOS doesn't work with const. */
#  define MAUG_CONST
#else
#  define MAUG_CONST const
#endif /* MAUG_OS_PALM */

/* This can be included sooner if needed, since it has a guard. */
#include <msect.h>

#if !defined( MAUG_WINDOWS_H ) && defined( MAUG_OS_WIN )
#  include <windows.h>
#  define MAUG_WINDOWS_H
#endif /* !MAUG_WINDOWS_H */

#include <merror.h>

#include <mmem.h>

#ifdef MAUG_C
#  define UPRINTF_C
#endif /* MAUG_C */
#include <uprintf.h>

#ifdef MAUG_C
#  define MARGE_C
#endif /* MAUG_C */
#include <marge.h>

#ifdef MAUG_C
#  define MFILE_C
#endif /* MAUG_C */
#include <mfile.h>

#ifdef MAUG_C
#  define MFMT_C
#endif /* MAUG_C */
#include <mfmt.h>

#ifdef MAUG_C
#  define MPLUG_C
#endif /* MAUG_C */
#include <mplug.h>

#ifndef MAUG_NO_RETRO
#ifdef MAUG_C
#  define RETROFLT_C
#endif /* MAUG_C */
#include <retroflt.h>
#endif /* !MAUG_NO_RETRO */

#endif /* MAUG_H */

