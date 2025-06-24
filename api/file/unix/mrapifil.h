
#if !defined( MAUG_API_FIL_H_DEFS )
#define MAUG_API_FIL_H_DEFS

#ifdef MFILE_MMAP
#  include <sys/mman.h> /* mmap() */
#  include <unistd.h> /* close() */
#  include <fcntl.h> /* open() */
#  include <sys/stat.h> /* fstat() */
#endif /* RETROFLAT_OS_UNIX */

union MFILE_HANDLE {
   FILE* file;
   MAUG_MHANDLE mem;
};

#elif defined( MFILE_C )

off_t mfile_file_has_bytes( struct MFILE_CADDY* p_file ) {
   if( 0 <= ftell( (p_file)->h.file ) ) {
      debug_printf( MFILE_TRACE_LVL, "file has " OFF_T_FMT " bytes left...",
         p_file->sz - (off_t)ftell( (p_file)->h.file ) );
      return p_file->sz - (off_t)ftell( (p_file)->h.file );
   } else {
      debug_printf( MFILE_TRACE_LVL, "file has error bytes left!" );
      return 0;
   }
}

/* === */

MERROR_RETVAL mfile_file_read_int(
   struct MFILE_CADDY* p_file, uint8_t* buf, size_t buf_sz, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t last_read = 0;

   assert( MFILE_CADDY_TYPE_FILE == p_file->type );

   if(
#ifdef MAUG_LSBF
      MFILE_READ_FLAG_LSBF == (MFILE_READ_FLAG_LSBF & flags)
#else
      MFILE_READ_FLAG_MSBF == (MFILE_READ_FLAG_MSBF & flags)
#endif
   ) {
      /* Shrink the buffer moving right and read into it. */
      last_read = fread( buf, 1, buf_sz, p_file->h.file );
      if( buf_sz > last_read ) {
         error_printf( "unable to read from file!" );
         retval = MERROR_FILE;
         goto cleanup;
      }
   
   } else {
      /* Move to the end of the output buffer and read backwards. */
      while( 0 < buf_sz ) {
         last_read = fread( (buf + (buf_sz - 1)), 1, 1, p_file->h.file );
         if( 0 >= last_read ) {
            error_printf( "unable to read from file!" );
            retval = MERROR_FILE;
            goto cleanup;
         }
         buf_sz--;
      }
   }

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_seek( struct MFILE_CADDY* p_file, off_t pos ) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( MFILE_CADDY_TYPE_FILE == p_file->type );

   if( fseek( p_file->h.file, pos, SEEK_SET ) ) {
      error_printf( "unable to seek file!" );
      retval = MERROR_FILE;
   }

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_read_line(
   struct MFILE_CADDY* p_f, char* buffer, off_t buffer_sz, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( MFILE_CADDY_TYPE_FILE == p_f->type );

   /* Trivial case; use a native function. Much faster! */
   if( NULL == fgets( buffer, buffer_sz - 1, p_f->h.file ) ) {
      error_printf( "error while reading line from file!" );
      retval = MERROR_FILE;
   }

   return retval;
}

/* === */

MERROR_RETVAL mfile_plt_open_read( const char* filename, mfile_t* p_file ) {
   MERROR_RETVAL retval = MERROR_OK;
   struct stat file_stat;

#  if defined( MFILE_MMAP )

   in_file = open( filename, O_RDONLY );
   if( 0 >= in_file ) {
      error_printf( "could not open file: %s", filename );
      retval = MERROR_FILE;
      goto cleanup;
   } else {
      debug_printf( MFILE_TRACE_LVL, "opened file: %s", filename );
   }

   fstat( in_file, &st );

   *p_bytes_ptr_h = 
      mmap( (caddr_t)0, st.st_size, PROT_READ, MAP_SHARED,  in_file, 0 );
   maug_cleanup_if_null_alloc( uint8_t*, *p_bytes_ptr_h );
   *p_bytes_sz = st.st_size;

cleanup:

   if( 0 < in_file ) {
      close( in_file );
   }

#  else

#     ifndef MAUG_NO_STAT
   /* Get the file size from the OS. */
   stat( filename, &file_stat );
   p_file->sz = file_stat.st_size;
#     endif /* !MAUG_NO_STAT */

   /* Open the permanent file handle. */
   p_file->h.file = fopen( filename, "rb" );
   if( NULL == p_file->h.file ) {
      error_printf( "could not open file: %s", filename );
      retval = MERROR_FILE;
      goto cleanup;
   } else {
      debug_printf( MFILE_TRACE_LVL, "opened file: %s", filename );
   }

#     ifdef MAUG_NO_STAT
   /* The standard is not required to support SEEK_END, among other issues.
    * This is probably the worst way to get file size.
    */
   debug_printf( 2, "falling back to seek file size..." );
   fseek( p_file->h.file, 0, SEEK_END );
   p_file->sz = ftell( p_file->h.file );
   fseek( p_file->h.file, 0, SEEK_SET );
#     endif /* MAUG_NO_STAT */

   debug_printf( 1, "opened file %s (" OFF_T_FMT " bytes)...",
      filename, p_file->sz );

   p_file->type = MFILE_CADDY_TYPE_FILE;

   p_file->has_bytes = mfile_file_has_bytes;
   p_file->read_int = mfile_file_read_int;
   p_file->seek = mfile_file_seek;
   p_file->read_line = mfile_file_read_line;
   p_file->flags = MFILE_FLAG_READ_ONLY;

cleanup:

#  endif /* MFILE_MMAP */

   return retval;
}

/* === */

void mfile_plt_close( mfile_t* p_file ) {
   fclose( p_file->h.file );
}

#endif /* !MAUG_API_FIL_H_DEFS */

