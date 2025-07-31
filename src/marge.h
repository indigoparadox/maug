
#ifndef MARGE_H
#define MARGE_H

#include <merror.h>

#ifdef MAUG_NO_CLI

#  define maug_add_arg( arg, arg_sz, help, help_sz, arg_cb, data ) (0)

#else

/**
 * \addtogroup maug_cli Command Line API
 * \brief Tools for parsing command line arguments.
 * \{
 */

/*
 * | ::MAUG_CLI_SIGIL | Specify the sigil prepended to command-line args. |
 * | MAUG_NO_CLI_SZ   | Disable ::MAUG_CLI_SZ command-line args.     |
 */

#ifndef MAUG_CLI_SIGIL_SZ
#  define MAUG_CLI_SIGIL_SZ 1
#endif /* !MAUG_CLI_SIGIL_SZ */

#if !defined( MAUG_CLI_SIGIL ) && defined( MAUG_OS_WIN )
#  define MAUG_CLI_SIGIL "/"
#elif !defined( MAUG_CLI_SIGIL ) && defined( MAUG_OS_DOS )
#  define MAUG_CLI_SIGIL "/"
#elif !defined( MAUG_CLI_SIGIL ) && defined( MAUG_OS_DOS_REAL )
#  define MAUG_CLI_SIGIL "/"
#elif !defined( MAUG_CLI_SIGIL )
/**
 * \brief Default flag to prepend to CLI arguments. Is "/" on Windows/DOS and
 *        "-" on other platforms. Can be overridden with a -D flag or define.
 */
#  define MAUG_CLI_SIGIL "-"
#endif /* !MAUG_CLI_SIGIL */

#define MAUG_CLI_ARG_C_DEFAULT -1

#ifndef MAUG_CLI_ARG_HELP_SZ_MAX
#  define MAUG_CLI_ARG_HELP_SZ_MAX 127
#endif /* !MAUG_CLI_ARG_HELP_SZ_MAX */

#ifndef MAUG_CLI_ARG_SZ_MAX
#  define MAUG_CLI_ARG_SZ_MAX 20
#endif /* !MAUG_CLI_ARG_SZ_MAX */

#ifndef MAUG_CLI_TRACE_LVL
#  define MAUG_CLI_TRACE_LVL 0
#endif /* !MAUG_CLI_TRACE_LVL */

typedef
MERROR_RETVAL (*maug_cli_cb)( const char* arg, ssize_t arg_c, void* data );

struct MARGE_ARG {
   char arg[MAUG_CLI_ARG_SZ_MAX];
   size_t arg_sz;
   char help[MAUG_CLI_ARG_HELP_SZ_MAX];
   maug_cli_cb callback;
   void* data;
   uint8_t called;
};

MERROR_RETVAL maug_cli_h( const char* arg, ssize_t arg_c, void* args );

int maug_parse_args( int argc, char* argv[] );

/**
 * \brief Add a command-line argument to the built-in parser.
 * \param arg String containing the argument to look for.
 * \param arg_sz Length of arg in chars or 0 to autodetect.
 * \param help Help text for the arg when -h is invoked.
 * \param help_sz Length of help in chars or 0 to autodetect.
 * \param arg_cb ::maug_cli_cb to invoke when arg is found.
 */
MERROR_RETVAL maug_add_arg(
   const char* arg, int arg_sz, const char* help, int help_sz,
   maug_cli_cb arg_cb, void* data );

#ifdef MARGE_C

/* == Global Tables == */

struct MDATA_VECTOR g_marge_args;

/* == Function Definitions = = */

MERROR_RETVAL maug_cli_h( const char* arg, ssize_t arg_c, void* args ) {
   size_t i = 0;
   struct MARGE_ARG* arg_p = NULL;

   if( 0 > arg_c ) {
      /* Just pass by on default case. */
      return 0;
   }

   error_printf( "usage:" );
   error_printf( "---" );

   /* Display help for all available options. */
   for( i = 0 ; mdata_vector_ct( &g_marge_args ) > i ; i++ ) {
      arg_p = mdata_vector_get( &g_marge_args, i, struct MARGE_ARG );
      error_printf( "\t%s\t%s", arg_p->arg, arg_p->help );
   }

   error_printf( "---" );

   /* Return an error so we quit. */
   return MERROR_GUI;
}

/* === */

MERROR_RETVAL maug_parse_args( int argc, char* argv[] ) {
   MERROR_RETVAL retval = MERROR_OK;
   int arg_i = 0,
      const_i = 0,
      last_i = 0;
   struct MARGE_ARG* arg = NULL;

   debug_printf( MAUG_CLI_TRACE_LVL, "parsing %d args...", argc );

   mdata_vector_lock( &g_marge_args );
   for( arg_i = 1 ; argc > arg_i ; arg_i++ ) {
      debug_printf( MAUG_CLI_TRACE_LVL, "found CLI: %s", argv[arg_i] );

      /* Hunt through the list of available args to see if the current argv
       * index matches.
       */
      const_i = 0;
      for(
         const_i = 0 ; mdata_vector_ct( &g_marge_args ) > const_i ; const_i++
      ) {
         arg = mdata_vector_get( &g_marge_args, const_i, struct MARGE_ARG );

         if( 0 == strncmp( arg->arg, argv[arg_i], arg->arg_sz ) ) {
            debug_printf( MAUG_CLI_TRACE_LVL, "arg matched: %s", arg->arg );

            /* Save this matched index for the next pass. */
            last_i = const_i;

            /* Increment called count and run the callback. */
            arg->called++;
            retval = arg->callback( argv[arg_i], arg->called, arg->data );
            if( MERROR_OK != retval ) {
               error_printf( "error calling arg!" );
               goto cleanup;
            }

            /* We found a match, so go to the next arg. */
            debug_printf(
               MAUG_CLI_TRACE_LVL, "arg parsed, moving on to next..." );
            break;
         } else {
            debug_printf( MAUG_CLI_TRACE_LVL, "arg NOT matched: %s", arg->arg );
         }
      }

      /* No valid arg was found, so we must be passing data to the last one!
         */
      arg = mdata_vector_get( &g_marge_args, last_i, struct MARGE_ARG );
      if( NULL != arg ) {
         debug_printf(
            MAUG_CLI_TRACE_LVL, "passing data to previous arg..." );
         arg->called++;
         retval = arg->callback( argv[arg_i], arg->called, arg->data );
         if( MERROR_OK != retval ) {
            error_printf( "error calling arg!" );
            goto cleanup;
         }
      }
   }

   debug_printf( MAUG_CLI_TRACE_LVL, "calling defaults for uncalled args..." );

   /* Run default callbacks for any args not called. */
   const_i = 0;
   for( const_i = 0 ; mdata_vector_ct( &g_marge_args ) > const_i ; const_i++ ) {
      arg = mdata_vector_get( &g_marge_args, const_i, struct MARGE_ARG );

      debug_printf( MAUG_CLI_TRACE_LVL,
         "checking arg %d: %s (" SIZE_T_FMT "): callback: %p",
         const_i, arg->arg, arg->arg_sz, arg->callback );
      if( NULL == arg->callback ) {
         debug_printf(
            MAUG_CLI_TRACE_LVL, "arg %d callback is NULL!", const_i );
         continue;
      }
      if( 0 != arg->called ) {
         debug_printf(
            MAUG_CLI_TRACE_LVL, "arg %d was called; NOT calling default...",
            const_i );
         continue;
      }
      debug_printf( MAUG_CLI_TRACE_LVL,
         "calling default arg for uncalled \"%s\"...", arg->arg );
      retval = arg->callback( "", MAUG_CLI_ARG_C_DEFAULT, arg->data );
      if( MERROR_OK != retval ) {
         error_printf( "error calling arg default!" );
         goto cleanup;
      }
   }

cleanup:

   mdata_vector_unlock( &g_marge_args );

   return retval;
}

/* === */

MERROR_RETVAL maug_add_arg(
   const char* arg, int arg_sz, const char* help, int help_sz,
   maug_cli_cb arg_cb, void* data
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t idx = 0;
   struct MARGE_ARG new_arg;

   /* TODO: Check for duplicate arg. */

   debug_printf( MAUG_CLI_TRACE_LVL, "adding arg: \"%s\"", arg );

   maug_strncpy( new_arg.arg, arg, MAUG_CLI_ARG_SZ_MAX );
   maug_strncpy( new_arg.help, help, MAUG_CLI_ARG_HELP_SZ_MAX );
   new_arg.arg_sz = arg_sz;
   new_arg.callback = arg_cb;
   new_arg.called = 0;
   new_arg.data = data;

   idx = mdata_vector_append(
      &g_marge_args, &new_arg, sizeof( struct MARGE_ARG ) );

   if( 0 > idx ) {
      retval = mdata_retval( idx );
   }

   return retval;
}

#endif /* MARGE_C */

/*! \} */ /* maug_cli */

#endif /* MAUG_NO_CLI */

#endif /* MARGE_H */

