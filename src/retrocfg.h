
#ifndef RETROCFG_H
#define RETROCFG_H

/**
 * \addtogroup maug_retroflt_config RetroFlat Config API
 * \brief Tools for loading and saving config data between sessions.
 * \{
 * \file retrocfg.h
 */

#include <mfile.h>

#ifndef RETROFLAT_CONFIG_LN_SZ_MAX
#  define RETROFLAT_CONFIG_LN_SZ_MAX 255
#endif /* !RETROFLAT_CONFIG_LN_SZ_MAX */

#ifndef RETROFLAT_CONFIG_EXT
#  define RETROFLAT_CONFIG_EXT ".ini"
#endif /* !RETROFLAT_CONFIG_EXT */

#  ifdef RETROFLAT_CONFIG_USE_FILE
typedef mfile_t RETROFLAT_CONFIG;
#  elif defined( RETROFLAT_API_WIN32 )
typedef HKEY RETROFLAT_CONFIG;
#  else
/**
 * \brief A configuration object to use with the \ref maug_retroflt_config.
 *
 * This is a file by default, but may be different things (e.g. a registry key
 * or battery-backed SRAM block) on different platforms.
 */
typedef int RETROFLAT_CONFIG;
#  endif /* RETROFLAT_API_WIN32 */

/**
 * \addtogroup maug_retroflt_config_btypes RetroFlat Config Buffer Types
 * \{
 */

#define RETROFLAT_BUFFER_INT 0x00

#define RETROFLAT_BUFFER_STRING 0x01

#define RETROFLAT_BUFFER_FLOAT 0x02

#define RETROFLAT_BUFFER_BOOL 0x04

/*! \} */ /* maug_retroflt_config_btypes */

/**
 * \addtogroup maug_retroflt_config_flags RetroFlat Config Flags
 * \brief Flags for the flags parameter of retroflat_config_open().
 * \{
 */

/**
 * \brief Flag indicating config object should be opened for WRITING only.
 *
 * If this is not specified, the buffer will only be opened for READING.
 *
 * These modes are mutually exclusive! A config object must be closed and
 * reopened to switch between READING and WRITING!
 */
#define RETROFLAT_CONFIG_FLAG_W     0x01

/**
 * \brief Flag indicating config object holds BINARY data.
 */
#define RETROFLAT_CONFIG_FLAG_BIN   0x02

/*! \} */ /* maug_retroflt_config_flags */

MERROR_RETVAL retroflat_config_init( struct RETROFLAT_ARGS* args );

/**
 * \brief Open a configuration file/registry/SRAM/etc handle.
 * \param flags \ref maug_retroflt_config_flags to open config with.
 */
MERROR_RETVAL retroflat_config_open( RETROFLAT_CONFIG* config, uint8_t flags );

void retroflat_config_close( RETROFLAT_CONFIG* config );

/**
 * \brief Write the contents of a buffer to a config object.
 * \warning The config object must have been opened with the
 *          ::RETROFLAT_CONFIG_FLAG_W flag enabled!
 * \param buffer_type \ref maug_retroflt_config_btypes of the buffer provided.
 * \param buffer_sz_max Size of the buffer provided in bytes.
 */
size_t retroflat_config_write(
   RETROFLAT_CONFIG* config,
   const char* sect_name, const char* key_name, uint8_t buffer_type,
   void* buffer, size_t buffer_sz_max );

/**
 * \brief Write the contents of a buffer to a config object.
 * \warning The config object must have been opened with the
 *          ::RETROFLAT_CONFIG_FLAG_W flag disabled!
 * \param buffer_type \ref maug_retroflt_config_btypes of the buffer provided.
 * \param buffer_out_sz_max Size of the buffer provided in bytes.
 */
size_t retroflat_config_read(
   RETROFLAT_CONFIG* config,
   const char* sect_name, const char* key_name, uint8_t buffer_type,
   void* buffer_out, size_t buffer_out_sz_max,
   const void* default_out, size_t default_out_sz );

#ifdef RETROCFG_C

#ifndef MAUG_NO_CLI

static MERROR_RETVAL retroflat_cli_c(
   const char* arg, ssize_t arg_c, struct RETROFLAT_ARGS* args
) {
   if( -1 == arg_c ) {
      memset( g_retroflat_state->config_path, '\0', RETROFLAT_PATH_MAX + 1 );

      if( NULL != args->config_path ) {
         debug_printf( 1, "setting config path to: %s", args->config_path );
         maug_strncpy(
            g_retroflat_state->config_path,
            args->config_path, RETROFLAT_PATH_MAX );
      } else {
         debug_printf( 1, "setting config path to: %s%s",
            args->title, RETROFLAT_CONFIG_EXT );
         maug_strncpy(
            g_retroflat_state->config_path, args->title, RETROFLAT_PATH_MAX );
         strncat(
            g_retroflat_state->config_path,
            RETROFLAT_CONFIG_EXT, RETROFLAT_PATH_MAX );
      }
   } else if(
      0 == strncmp( MAUG_CLI_SIGIL "rfc", arg, MAUG_CLI_SIGIL_SZ + 4 )
   ) {
      /* The next arg must be the new var. */
   } else {
      debug_printf( 1, "setting config path to: %s", arg );
      maug_strncpy( g_retroflat_state->config_path, arg, RETROFLAT_PATH_MAX );
   }
   return MERROR_OK;
}

#endif /* !MAUG_NO_CLI */

/* === */

MERROR_RETVAL retroflat_config_init( struct RETROFLAT_ARGS* args ) {
   MERROR_RETVAL retval = MERROR_OK;

   maug_add_arg( MAUG_CLI_SIGIL "rfc", MAUG_CLI_SIGIL_SZ + 4,
      "Set the config path.", 0,
      (maug_cli_cb)retroflat_cli_c, args );

   return retval;
}

/* === */

MERROR_RETVAL retroflat_config_open( RETROFLAT_CONFIG* config, uint8_t flags ) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( RETROFLAT_CONFIG_USE_FILE )

   debug_printf( 1, "opening config file %s...",
      g_retroflat_state->config_path );

   /* TODO: Open read/write when implemented. */
   retval = mfile_open_read( g_retroflat_state->config_path, config );
   maug_cleanup_if_not_ok();

cleanup:
#  elif defined( RETROFLAT_API_WIN16 )

   /* == Win16 (.ini file) == */

   /* TODO */

#  elif defined( RETROFLAT_API_WIN32 )

   /* == Win32 (Registry) == */

   char key_path[RETROFLAT_PATH_MAX + 1] = "SOFTWARE\\";

   /* TODO */
   strncat( key_path, "RetroFlat", RETROFLAT_PATH_MAX );

   /* TODO */
   if( ERROR_SUCCESS != RegOpenKey(
      HKEY_CURRENT_USER, 
      key_path,
      (HKEY*)config
   ) ) {
      retval = MERROR_FILE;
   }

#  else
#     pragma message( "warning: config close not implemented" )
#  endif

   return retval;
}

/* === */

void retroflat_config_close( RETROFLAT_CONFIG* config ) {

#  if defined( RETROFLAT_CONFIG_USE_FILE )

   debug_printf( 1, "closing config file..." );
   mfile_close( config );

#  elif defined( RETROFLAT_API_WIN16 )

   /* TODO */

#  elif defined( RETROFLAT_API_WIN32 )

   /* == Win32 (Registry) == */

   /* TODO */

#  else
#     pragma message( "warning: config close not implemented" )
#  endif

}

/* === */

#ifdef RETROFLAT_CONFIG_USE_FILE

/**
 * \param sect_match Pointer to size_t to hold strncmp() result.
 * \return Unmodified line if *any* section is found, with brackets stripped
 *         if sect_name is found.
 */
static char* retroflat_config_tok_sect(
   char* line, size_t line_sz, const char* sect_name, size_t* sect_match
) {

   /* Section check. */
   if( '[' == line[0] ) {
      *sect_match = strncmp( sect_name, &(line[1]), line_sz - 2 );
      if( 0 == *sect_match ) {
         /* TODO: Tokenize at closing bracket. */
         line[line_sz - 1] = '\0';
         debug_printf( 1, "found section: %s", &(line[1]) );
      }
      return line;
   }

   return NULL;
}  

#endif /* RETROFLAT_CONFIG_USE_FILE */

/* === */

size_t retroflat_config_write(
   RETROFLAT_CONFIG* config,
   const char* sect_name, const char* key_name, uint8_t buffer_type,
   void* buffer, size_t buffer_sz_max
) {
   size_t retval = 0;

   /* TODO */

   return retval;
}

/* === */

size_t retroflat_config_read(
   RETROFLAT_CONFIG* config,
   const char* sect_name, const char* key_name, uint8_t buffer_type,
   void* buffer_out, size_t buffer_out_sz_max,
   const void* default_out, size_t default_out_sz
) {
   size_t retval = 0;
#  if defined( RETROFLAT_CONFIG_USE_FILE )
   char line[RETROFLAT_CONFIG_LN_SZ_MAX + 1];
   char* line_val = NULL;
   size_t line_sz = 0;
   size_t sect_match = 1;
#  endif /* RETROFLAT_API_WIN32 */

#  if defined( RETROFLAT_CONFIG_USE_FILE )

   /* == SDL / Allegro == */

   while( MERROR_OK == config->read_line(
      config, line, RETROFLAT_CONFIG_LN_SZ_MAX, 0 )
   ) {
      /* Size check. */
      line_sz = maug_strlen( line );
      if( 1 >= line_sz || RETROFLAT_CONFIG_LN_SZ_MAX <= line_sz ) {
         error_printf( "invalid line sz: " SIZE_T_FMT, line_sz );
         continue;
      }
      
      /* Strip off trailing newline. */
      if( '\n' == line[line_sz - 1] || '\r' == line[line_sz - 1] ) {
         debug_printf( 1, "stripping newline!" );
         line_sz--;
         line[line_sz] = '\0'; /* NULL goes after maug_strlen() finishes! */
      }

      /* Section check. */
      if(
         retroflat_config_tok_sect( line, line_sz, sect_name, &sect_match )
      ) {
         continue;
      }
      
      /* Split up key/value pair. */
      line_val = maug_strchr( line, '=' );
      if( NULL == line_val || line_val == line ) {
         error_printf( "invalid line: %s", line );
         continue;
      }

      /* Terminate key. */
      line_val[0] = '\0';
      line_val++;

      if( 0 == sect_match && 0 == strcmp( key_name, line ) ) {
         debug_printf( 1, "found %s: %s", line, line_val );

         switch( buffer_type ) {
         case RETROFLAT_BUFFER_INT:
            *((int*)buffer_out) = atoi( line_val );
            goto cleanup;

         case RETROFLAT_BUFFER_FLOAT:
            *((float*)buffer_out) = atof( line_val );
            goto cleanup;

         case RETROFLAT_BUFFER_STRING:
            maug_strncpy( (char*)buffer_out, line_val, buffer_out_sz_max );
            goto cleanup;

         case RETROFLAT_BUFFER_BOOL:
            /* TODO */
            goto cleanup;
         }
      }
   }

cleanup:

   config->seek( config, 0 );

#  elif defined( RETROFLAT_API_WIN16 )

   /* == Win16 (.ini file) == */

/* XXX
   retval = GetPrivateProfileString(
      sect_name, key_name, default_out, buffer_out, buffer_out_sz_max,
      g_retroflat_state->config_path ); */

#  elif defined( RETROFLAT_API_WIN32 )

   /* == Win32 (Registry) == */

   /* TODO */

#  else
#     pragma message( "warning: config read not implemented" )
#  endif

   return retval;
}

#endif /* RETROCFG_C */

/*! \} */ /* maug_retroflt_config */

#endif /* RETROCFG_H */

