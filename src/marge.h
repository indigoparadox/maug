
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

#ifndef MAUG_CLI_ARG_LIST_SZ_MAX
#  define MAUG_CLI_ARG_LIST_SZ_MAX 20
#endif /* !MAUG_CLI_ARG_LIST_SZ_MAX */

#ifndef MAUG_CLI_ARG_HELP_SZ_MAX
#  define MAUG_CLI_ARG_HELP_SZ_MAX 127
#endif /* !MAUG_CLI_ARG_HELP_SZ_MAX */

#ifndef MAUG_CLI_ARG_SZ_MAX
#  define MAUG_CLI_ARG_SZ_MAX 20
#endif /* !MAUG_CLI_ARG_SZ_MAX */

typedef
MERROR_RETVAL (*maug_cli_cb)( const char* arg, ssize_t arg_c, void* data );

/*! \brief Default CLI arguments for all RetroFlat programs. */
#define MAUG_CLI( f ) \
   f( MAUG_CLI_SIGIL "h", 3, "Display help and exit.", maug_cli_h )

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

#define MAUG_CLI_ARG_ARG( arg, arg_sz, help, arg_callback ) \
   arg,

static char SEG_FAR g_maug_cli_args[MAUG_CLI_ARG_LIST_SZ_MAX][MAUG_CLI_ARG_SZ_MAX] = {
   MAUG_CLI( MAUG_CLI_ARG_ARG )
   ""
};

#define MAUG_CLI_ARG_SZ( arg, arg_sz, help, arg_callback ) \
   arg_sz,

static int SEG_FAR g_maug_cli_arg_sz[MAUG_CLI_ARG_LIST_SZ_MAX] = {
   MAUG_CLI( MAUG_CLI_ARG_SZ )
   0
};

#define MAUG_CLI_ARG_HELP( arg, arg_sz, help, arg_callback ) \
   help,

static char SEG_FAR g_maug_cli_arg_help[MAUG_CLI_ARG_LIST_SZ_MAX][MAUG_CLI_ARG_HELP_SZ_MAX] = {
   MAUG_CLI( MAUG_CLI_ARG_HELP )
   ""
};

#define MAUG_CLI_ARG_DATA( arg, arg_sz, help, arg_callback ) NULL,
static void* SEG_FAR g_maug_cli_data[MAUG_CLI_ARG_LIST_SZ_MAX] = {
   MAUG_CLI( MAUG_CLI_ARG_DATA )
   NULL
};

#define MAUG_CLI_ARG_CALLED( arg, arg_sz, help, arg_callback ) 0,
static int SEG_FAR g_maug_cli_arg_called[MAUG_CLI_ARG_LIST_SZ_MAX] = {
   MAUG_CLI( MAUG_CLI_ARG_CALLED )
   0
};

/* == Function Definitions = = */

static int maug_cli_h( const char* arg, ssize_t arg_c, void* args ) {
   int i = 0;

   if( 0 > arg_c ) {
      /* Just pass by on default case. */
      return 0;
   }

   fprintf( stderr, "usage:\n\n" );

   /* Display help for all available options. */
   while( '\0' != g_maug_cli_args[i][0] ) {
      fprintf( stderr, "\t%s\t%s\n", g_maug_cli_args[i],
         g_maug_cli_arg_help[i] );
      i++;
   }

   fprintf( stderr, "\n" );

   /* TODO: Coherent error code. */
   return -1;
}

/* === */

/* Define these below the built-in callbacks, above. */
#define MAUG_CLI_ARG_CB( arg, arg_sz, help, arg_callback ) \
   arg_callback,

maug_cli_cb g_maug_cli_arg_callbacks[MAUG_CLI_ARG_LIST_SZ_MAX] = {
   MAUG_CLI( MAUG_CLI_ARG_CB )
   NULL
};

int maug_parse_args( int argc, char* argv[] ) {
   int arg_i = 0,
      const_i = 0,
      last_i = 0,
      retval = 0;

   for( arg_i = 1 ; argc > arg_i ; arg_i++ ) {
      debug_printf( 1, "found arg: %s", argv[arg_i] );
      const_i = 0;
      while( '\0' != g_maug_cli_args[const_i][0] ) {
         if( 0 == strncmp(
            g_maug_cli_args[const_i],
            argv[arg_i],
            g_maug_cli_arg_sz[const_i]
         ) ) {
            /* Save this matched index for the next pass. */
            last_i = const_i;

            /* Increment called count and run the callback. */
            g_maug_cli_arg_called[const_i]++;
            retval = g_maug_cli_arg_callbacks[const_i]( argv[arg_i],
               g_maug_cli_arg_called[const_i], g_maug_cli_data[const_i] );
            if( MERROR_OK != retval ) {
               goto cleanup;
            }

            /* We found a match, so go to the next arg. */
            break;
         }
         const_i++;
      }

      if( '\0' == g_maug_cli_args[const_i][0] ) {
         /* No valid arg was found, so we must be passing data to the last one!
          */
         g_maug_cli_arg_called[last_i]++;
         retval = g_maug_cli_arg_callbacks[last_i]( argv[arg_i],
            g_maug_cli_arg_called[last_i], g_maug_cli_data[last_i] );
         if( MERROR_OK != retval ) {
            goto cleanup;
         }
      }
   }

   /* TODO: Run default callbacks for any args not called. */
   const_i = 0;
   while( '\0' != g_maug_cli_args[const_i][0] ) {
      if(
         0 == g_maug_cli_arg_called[const_i] &&
         NULL != g_maug_cli_arg_callbacks[const_i]
      ) {
         debug_printf( 1, "calling default arg for uncalled \"%s\"...",
            g_maug_cli_args[const_i] );
         retval =
            g_maug_cli_arg_callbacks[const_i](
               "", MAUG_CLI_ARG_C_DEFAULT, g_maug_cli_data[const_i] );
         if( MERROR_OK != retval ) {
            goto cleanup;
         }
      }
      const_i++;
   }

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL maug_add_arg(
   const char* arg, int arg_sz, const char* help, int help_sz,
   maug_cli_cb arg_cb, void* data
) {
   int slot_idx = 0;

   /* Find empty arg slot. */
   while(
      '\0' != g_maug_cli_args[slot_idx][0] &&
      MAUG_CLI_ARG_LIST_SZ_MAX > slot_idx
   ) {
      slot_idx++;
   }

   /* TODO: Check for duplicate arg. */

   /* Sanity checking and sizing. */
   if( MAUG_CLI_ARG_LIST_SZ_MAX - 1 <= slot_idx ) {
      error_printf( "too many CLI arguments specified!" );
      return MERROR_OVERFLOW;
   }

   if( 0 >= arg_sz ) {
      arg_sz = maug_strlen( arg );
   }
   assert( arg_sz < MAUG_CLI_ARG_SZ_MAX );

   if( 0 >= help_sz ) {
      help_sz = maug_strlen( help );
   }
   assert( help_sz < MAUG_CLI_ARG_HELP_SZ_MAX );

   /* Add arg to arrays. */

   maug_strncpy( g_maug_cli_args[slot_idx], arg, arg_sz );
   g_maug_cli_args[slot_idx + 1][0] = '\0';
   
   maug_strncpy( g_maug_cli_arg_help[slot_idx], help, help_sz );
   g_maug_cli_arg_help[slot_idx + 1][0] = '\0';
   
   g_maug_cli_arg_sz[slot_idx] = arg_sz;
   g_maug_cli_arg_sz[slot_idx + 1] = 0;
   
   g_maug_cli_arg_callbacks[slot_idx] = arg_cb;
   g_maug_cli_arg_callbacks[slot_idx + 1] = NULL;

   g_maug_cli_arg_called[slot_idx] = 0;
   g_maug_cli_arg_called[slot_idx + 1] = 0;

   g_maug_cli_data[slot_idx] = data;
   g_maug_cli_data[slot_idx + 1] = NULL;

   return MERROR_OK;
}

#endif /* MARGE_C */

/*! \} */ /* maug_cli */

#endif /* MAUG_NO_CLI */

#endif /* MARGE_H */

