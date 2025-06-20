
#ifndef UPRINTF_H
#define UPRINTF_H

#ifndef MAUG_NO_LEGACY
#  include <mlegacy.h>
#endif /* !MAUG_NO_LEGACY */

/**
 * \addtogroup maug_uprintf Micro-PrintF API
 * \brief Independently-implemented printf API for old compilers.
 * \{
 */

/**
 * \file uprintf.h
 */

/* == Autodetection == */

/* == Manual Config == */

#ifndef UPRINTF_BUFFER_SZ_MAX
#  define UPRINTF_BUFFER_SZ_MAX 1024
#endif /* !UPRINTF_BUFFER_SZ_MAX */

#ifdef LOG_TO_FILE
#  ifndef UPRINTF_LOG
#     define UPRINTF_LOG
#  endif /* !UPRINTF_LOG */
#  define LOG_ERR_TARGET g_log_file
#  define LOG_STD_TARGET g_log_file
#else
#  define LOG_ERR_TARGET stderr
#  define LOG_STD_TARGET stdout
#endif /* LOG_TO_FILE */

#if !defined( DEBUG_THRESHOLD )
#  define DEBUG_THRESHOLD 1
#endif /* !DEBUG_THRESHOLD */

#include <mrapilog.h>

#ifdef UPRINTF_C

#include <mrapilog.h>

#endif /* UPRINTF_C */

/*! \} */ /* maug_uprintf */

#endif /* !UPRINTF_H */

