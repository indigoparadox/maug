
#if !defined( MAUG_API_FIL_H_DEFS )
#define MAUG_API_FIL_H_DEFS

union MFILE_HANDLE {
   MAUG_MHANDLE mem;
};

#elif defined( MFILE_C )

off_t mfile_file_cursor( struct MFILE_CADDY* p_file ) {
   
   /* TODO */
#  pragma message( "warning: file_cursor not implemented" )

   return 0;
}

/* === */

MERROR_RETVAL mfile_file_read_byte( struct MFILE_CADDY* p_file, uint8_t* buf ) {
   return mfile_file_read_block( p_file, buf, 1 );
}

/* === */

MERROR_RETVAL mfile_file_read_block(
   struct MFILE_CADDY* p_file, uint8_t* buf, size_t buf_sz
) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( MFILE_CADDY_TYPE_FILE == p_file->type );

   /* TODO */
#  pragma message( "warning: file_read_block not implemented" )

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_seek( struct MFILE_CADDY* p_file, off_t pos ) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( MFILE_CADDY_TYPE_FILE == p_file->type );

   /* TODO */
#  pragma message( "warning: file_seek not implemented" )

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_read_line(
   struct MFILE_CADDY* p_file, char* buffer, off_t buffer_sz, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( MFILE_CADDY_TYPE_FILE == p_file->type );

   /* TODO */
#  pragma message( "warning: file_read_line not implemented" )

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_vprintf(
   mfile_t* p_file, uint8_t flags, const char* fmt, va_list args
) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( MFILE_CADDY_TYPE_FILE == p_file->type );

   if( MFILE_FLAG_READ_ONLY == (MFILE_FLAG_READ_ONLY & p_file->flags) ) {
      return MERROR_FILE;
   }

   /* TODO */
#  pragma message( "warning: file_vprintf not implemented" )

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_write_block(
   struct MFILE_CADDY* p_f, const uint8_t* buf, size_t buf_sz
) {
   MERROR_RETVAL retval = MERROR_OK;

   if( MFILE_FLAG_READ_ONLY == (MFILE_FLAG_READ_ONLY & p_f->flags) ) {
      return MERROR_FILE;
   }

   /* TODO: Implement insert rather than replace like in UNIX mfile API. */
#  pragma message( "warning: file_write_block not implemented" )

   return retval;
}

/* === */

MERROR_RETVAL mfile_plt_init( void ) {
   
   /* TODO */
#  pragma message( "warning: init not implemented" )

   return MERROR_OK;
}

/* === */

MERROR_RETVAL mfile_plt_open_read( const char* filename, mfile_t* p_file ) {
   MERROR_RETVAL retval = MERROR_OK;

   /* TODO */
#  pragma message( "warning: open_read not implemented" )

   return retval;
}

/* === */

MERROR_RETVAL mfile_plt_open_write( const char* filename, mfile_t* p_file ) {
   MERROR_RETVAL retval = MERROR_OK;

   /* TODO */
#  pragma message( "warning: open_write not implemented" )

   return retval;
}

/* === */

void mfile_plt_close( mfile_t* p_file ) {
   /* TODO */
#  pragma message( "warning: close not implemented" )
}

#endif /* !MAUG_API_FIL_H_DEFS */

