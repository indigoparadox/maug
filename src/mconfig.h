
#ifndef MCONFIG_H
#define MCONFIG_H

MERROR_RETVAL maug_config_init( const char* app_name, uint16_t app_name_sz );

int maug_config_read_str(
   const char* sect_name, const char* key_name, const char* def_out,
   char* buffer_out, int buffer_sz );

#ifdef MCONFIG_C

static char g_maug_config_appname[MAUG_PATH_SZ_MAX];

MERROR_RETVAL maug_config_init( const char* app_name, uint16_t app_name_sz ) {
   strncpy( g_maug_config_appname, app_name, MAUG_PATH_SZ_MAX );
   return MERROR_OK;
}

int maug_config_read_str(
   const char* sect_name, const char* key_name, const char* def_out,
   char* buffer_out, int buffer_sz
) {
   int retval = 0;
#  if defined( MAUG_API_WIN32 )
   HKEY key = (HKEY)NULL;
#  endif /* MAUG_API_WIN32 */

#  if defined( MAUG_API_WIN16 )
   retval = GetPrivateProfileString(
      sect_name, key_name, def_out, buffer_out, buffer_sz,
      g_retroflat_config_filename );
#  elif defined( MAUG_API_WIN32 )
   /* TODO */
   if( ERROR_SUCCESS != RegOpenKeyExA(
      HKEY_CURRENT_USER, 
#  else
#     warning "read str not implemented"
#  endif

   return retval;
}

#endif /* MCONFIG_C */

#endif /* !MCONFIG_H */

