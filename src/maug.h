
#ifndef MAUG_H
#define MAUG_H

#include <mlegacy.h>

#ifdef MAUG_OS_PALM
/* PalmOS doesn't work with const. */
#  define MAUG_CONST
#else
#  define MAUG_CONST const
#endif /* MAUG_OS_PALM */

#if (defined( MAUG_OS_DOS_REAL ) || defined( MAUG_API_WIN16 )) && \
   defined( MAUG_DOS_MEM_L ) && \
   defined( __WATCOMC__ )
#  define SEG_MCONST __based( __segname( "MCONST" ) )
#else
#  define SEG_MCONST
#endif /* MAUG_OS_DOS_REAL && __WATCOMC__ */

/* This can be included sooner if needed, since it has a guard. */
#include <msect.h>

#if !defined( MAUG_WINDOWS_H ) && defined( MAUG_OS_WIN )
#  include <windows.h>
#  define MAUG_WINDOWS_H
#endif /* !MAUG_WINDOWS_H */

#include <merror.h>

#include <mmem.h>

#if (defined( MAUG_OS_DOS_REAL ) || defined( MAUG_API_WIN16 ))
/* Undefine this here so the header can have its own object file from
 * dosstubs.
 */
#  undef MAUG_C

#  define SEG_FAR far
#else
#  define SEG_FAR
#endif /* MAUG_OS_DOS_REAL */

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

#ifdef MAUG_C
#  define MJSON_C
#  define MHTML_C
#  define MPARSER_C
#  define MLISP_C
#endif /* MAUG_C */

#ifndef MAUG_NO_RETRO
#  ifdef MAUG_C
#     define RETROFLT_C
#  endif /* MAUG_C */
#  include <retroflt.h>
#  ifdef MAUG_C
#     define RETROCFG_C
#     define RETROTIL_C
#     define RETROISO_C
#     define RETROGXC_C
#     define RETROGUI_C
#     define RETROCON_C
#     define RETROFNT_C
#     define MCSSMERG_C
#     define RETROHTR_C
#     define RETROANI_C
#     define RETROW3D_C
#     define RETROSND_C
#     ifdef RETROFLAT_OPENGL
#        define RETROGLU_C
#     endif /* RETROFLAT_OPENGL */
#  endif /* MAUG_C */
#endif /* !MAUG_NO_RETRO */

#endif /* MAUG_H */

