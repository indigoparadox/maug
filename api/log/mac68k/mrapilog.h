
#if !defined( MAUG_API_LOG_H_DEFS )
#define MAUG_API_LOG_H_DEFS

void _maug_debug_printf( int line, int level, const char* fmt, ... );
   
void _maug_error_printf( int line, const char* fmt, ... );

#  define debug_printf( lvl, ... ) \
      _maug_debug_printf( __LINE__, lvl, __VA_ARGS__ )

#  define error_printf( ... ) _maug_error_printf( __LINE__, __VA_ARGS__ )

#  define size_printf( lvl, name, sz ) _maug_debug_printf( __LINE__, lvl, name " size is " SIZE_T_FMT " bytes", (sz) );

#  define size_multi_printf( lvl, name, sz, max ) debug_printf( __LINE__, lvl, "single " name " size is " SIZE_T_FMT " bytes, " name " array size is " SIZE_T_FMT " bytes", (sz), ((sz) * (max)) );

extern short g_log_ref;

#elif defined( UPRINTF_C )

#include <OSUtils.h>
#include <Devices.h>
#include <Errors.h>

short g_log_ref = 0;

void logging_init() {
   OSErr err;

   err = OpenDriver( "\p.AOut", &g_log_ref );
   if( noErr != err ) {
      retroflat_message( 1, /* RETROFLAT_MSG_FLAG_ERROR */
         "Error", "Unable to initiate logging: %d", err );
   }

}

/* === */

void logging_shutdown() {
   CloseDriver( g_log_ref );
}

/* === */

void _maug_debug_printf( int line, int level, const char* fmt, ... ) {
   va_list argp;
   char message[255] = { 0 };
   ParamBlockRec pb;

   if( level >= DEBUG_THRESHOLD ) {
      va_start( argp, fmt );
      maug_vsnprintf( message, 254, fmt, argp );
      va_end( argp );
      strcat( message, "\r" );
   }

   pb.ioParam.ioRefNum = g_log_ref;
   pb.ioParam.ioBuffer = message;
   pb.ioParam.ioReqCount = strlen( message );
   pb.ioParam.ioPosMode = fsAtMark;
   pb.ioParam.ioPosOffset = 0;

   PBWrite( &pb, false );
}

/* === */

void _maug_error_printf( int line, const char* fmt, ... ) {
   va_list argp;
   char message[255] = { 0 };
   ParamBlockRec pb;

   va_start( argp, fmt );
   maug_vsnprintf( message, 254, fmt, argp );
   va_end( argp );
   strcat( message, "\r" );

   pb.ioParam.ioRefNum = g_log_ref;
   pb.ioParam.ioBuffer = message;
   pb.ioParam.ioReqCount = strlen( message );
   pb.ioParam.ioPosMode = fsAtMark;
   pb.ioParam.ioPosOffset = 0;

   PBWrite( &pb, false );
}

#endif /* !MAUG_API_LOG_H_DEFS */

