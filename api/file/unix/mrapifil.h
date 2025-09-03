
#if !defined( MAUG_API_FIL_H_DEFS )
#define MAUG_API_FIL_H_DEFS

#ifdef MFILE_MMAP
#  include <sys/mman.h> /* mmap() */
#  include <unistd.h> /* close() */
#  include <fcntl.h> /* open() */
#  include <sys/stat.h> /* fstat() */
#endif /* MFILE_MMAP */

#ifdef RETROFLAT_OS_WASM
#  include <emscripten.h>
#endif /* RETROFLAT_OS_WASM */

#ifdef RETROFLAT_OS_NX
#  include <switch.h>
#endif /* RETROFLAT_OS_WASM */

union MFILE_HANDLE {
   FILE* file;
   MAUG_MHANDLE mem;
};

#elif defined( MFILE_C )

#  ifdef RETROFLAT_OS_WASM
EM_JS( void, mfile_wasm_fetch, (const char* path), {
   var path_str = UTF8ToString( path );

   console.log( 'fetching "' + path_str + '" from remote!' );

   /* This wrapper turns the async fetch() call into a sync call, so this
    * function can emulate a standard sync C function.
    */
   return Asyncify.handleAsync( async() => {
      let response = await fetch( path_str );

      /* TODO: Handle fetch error. */

      /* Create the containing directory. */
      let path_arr = path_str.split( '/' );
      console.log( path_arr );
      for( var i = path_arr.length - 1 ; 0 < i ; i-- ) {
         let dir_path = '/' + path_arr.slice( 0, i * -1 ).join( '/' );
         console.log( 'testing path: ' + dir_path );
         /* Handle directory existing. */
         let dir_path_an = FS.analyzePath( dir_path );
         if( !dir_path_an.exists ) {
            console.log( 'creating parent "' + dir_path + '"...' );
            FS.mkdir( dir_path );
         }
      }

      /* Write the fetched file to WASM virtual FS. */
      let response_buf = await response.arrayBuffer();
      console.log( response_buf );
      const byte_array = new Uint8Array( response_buf );
      console.log( byte_array );
      FS.writeFile( path_str, byte_array );

      console.log( 'fetch complete!' );
   } );
} );
#endif /* RETROFLAT_OS_WASM */

/* === */

off_t mfile_file_cursor( struct MFILE_CADDY* p_file ) {
   return ftell( (p_file)->h.file );
}

/* === */

MERROR_RETVAL mfile_file_read_byte( struct MFILE_CADDY* p_file, uint8_t* buf ) {
   return p_file->read_block( p_file, buf, 1 );
}

/* === */

MERROR_RETVAL mfile_file_read_block(
   struct MFILE_CADDY* p_file, uint8_t* buf, size_t buf_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t last_read = 0;

   last_read = fread( buf, 1, buf_sz, p_file->h.file );
   if( 1 > last_read ) {
      error_printf( "unable to read from file!" );
      retval = MERROR_FILE;
   }
 
   return retval;
}

/* === */

MERROR_RETVAL mfile_file_seek( struct MFILE_CADDY* p_file, off_t pos ) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( MFILE_CADDY_TYPE_FILE == p_file->type );

   debug_printf( MFILE_SEEK_TRACE_LVL,
      "seeking cursor to " OFF_T_FMT " of " OFF_T_FMT " bytes...",
      pos, p_file->sz );

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

MERROR_RETVAL mfile_plt_init() {
#ifdef RETROFLAT_OS_NX
   /* Nintendo Switch ROMFS mount. */
   romfsInit();
#endif /* RETROFLAT_OS_NX */
   return MERROR_OK;
}

/* === */

static MERROR_RETVAL _mfile_plt_open(
   uint8_t flags, off_t sz, const char* filename, mfile_t* p_file
) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( MFILE_MMAP )

   in_file = open( filename,
      MFILE_FLAG_READ_ONLY == (MFILE_FLAG_READ_ONLY & flags) ? O_RDONLY : 0 );
   if( 0 >= in_file ) {
      error_printf( "could not open file: %s", filename );
      retval = MERROR_FILE;
      goto cleanup;
   } else {
      debug_printf( MFILE_SEEK_TRACE_LVL, "opened file: %s", filename );
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

   p_file->sz = sz;

   /* Open the permanent file handle. */
   p_file->h.file = fopen( filename, 
      MFILE_FLAG_READ_ONLY == (MFILE_FLAG_READ_ONLY & flags) ? "rb" : "wb+" );
   if( NULL == p_file->h.file ) {
      error_printf( "could not open file: %s", filename );
      retval = MERROR_FILE;
      goto cleanup;
   } else {
      debug_printf( MFILE_SEEK_TRACE_LVL, "opened file: %s", filename );
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
   p_file->cursor = mfile_file_cursor;
   p_file->read_byte = mfile_file_read_byte;
   p_file->read_block = mfile_file_read_block;
   p_file->read_int = mfile_file_read_int;
   p_file->seek = mfile_file_seek;
   p_file->read_line = mfile_file_read_line;
   p_file->printf = mfile_file_printf;
   p_file->vprintf = mfile_file_vprintf;
   p_file->write_block = mfile_file_write_block;

   p_file->flags = flags;

cleanup:

#  endif /* MFILE_MMAP */

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_vprintf(
   mfile_t* p_file, uint8_t flags, const char* fmt, va_list args
) {
   MERROR_RETVAL retval = MERROR_OK;

   if( MFILE_FLAG_READ_ONLY == (MFILE_FLAG_READ_ONLY & p_file->flags) ) {
      return MERROR_FILE;
   }

   /* TODO: Shift remaining contents down first. Break out function from write_block for this. */

   vfprintf( p_file->h.file, fmt, args );

   /* TODO: Update file size! */

   return retval;
}

/* === */

MERROR_RETVAL mfile_file_write_block(
   struct MFILE_CADDY* p_f, uint8_t* buf, size_t buf_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t written = 0;
   MAUG_MHANDLE end_buf_h = (MAUG_MHANDLE)NULL;
   uint8_t* end_buf = NULL;
   size_t cursor = 0;
   size_t end_buf_sz = 0;
   ssize_t read = 0;
#if MFILE_WRITE_TRACE_LVL > 0
   size_t i = 0;
#endif /* MFILE_WRITE_TRACE_LVL */

   if( MFILE_FLAG_READ_ONLY == (MFILE_FLAG_READ_ONLY & p_f->flags) ) {
      return MERROR_FILE;
   }

   cursor = p_f->cursor( p_f );

   if( 0 < p_f->sz && cursor < p_f->sz ) {
      /* Grab the rest of the file to shift down if we're not at the end. */
      end_buf_sz = p_f->sz - cursor;
#if MFILE_WRITE_TRACE_LVL > 0
      debug_printf( MFILE_WRITE_TRACE_LVL,
         "allocating buffer to hold remaining " SIZE_T_FMT
            " bytes from " SIZE_T_FMT " to " SIZE_T_FMT,
         end_buf_sz, cursor, p_f->sz );
#endif /* MFILE_WRITE_TRACE_LVL */
      end_buf_h = maug_malloc( end_buf_sz, 1 );
      maug_mlock( end_buf_h, end_buf );
      read = fread( end_buf, 1, end_buf_sz, p_f->h.file );
      if( read < end_buf_sz ) {
         error_printf(
            "error reading remainder of file! (read " SIZE_T_FMT " of "
               SIZE_T_FMT " bytes)",
            read, end_buf_sz );
         retval = MERROR_FILE;
         goto cleanup;
      }

      /* Rewind after reading the file end into the buffer. */
      fseek( p_f->h.file, cursor, SEEK_SET );
   }

#if MFILE_WRITE_TRACE_LVL > 0
   debug_printf( MFILE_WRITE_TRACE_LVL,
      "writing binary data to cursor position " OFF_T_FMT "...",
      cursor );
   for( i = 0 ; buf_sz > i ; i++ ) {
      debug_printf( MFILE_CONTENTS_TRACE_LVL, " > 0x%02x", buf[i] );
   }
#endif /* MFILE_WRITE_TRACE_LVL */

   written = fwrite( buf, 1, buf_sz, p_f->h.file ); 
   if( written < buf_sz ) {
      error_printf( "error inserting into file!" );
      retval = MERROR_FILE;
      goto cleanup;
   }

   p_f->sz += written;

   if( NULL != end_buf ) {
      cursor += written;
#if MFILE_WRITE_TRACE_LVL > 0
      debug_printf( MFILE_WRITE_TRACE_LVL,
         "shifting " SIZE_T_FMT " bytes of binary data to cursor position "
            OFF_T_FMT "...",
         end_buf_sz, cursor );
      for( i = 0 ; end_buf_sz > i ; i++ ) {
         debug_printf( MFILE_CONTENTS_TRACE_LVL, " > 0x%02x", end_buf[i] );
      }
#endif /* MFILE_WRITE_TRACE_LVL */
      written = fwrite( end_buf, 1, end_buf_sz, p_f->h.file );
      if( written < end_buf_sz ) {
         error_printf( "error re-inserting file remainder!" );
         retval = MERROR_FILE;
         goto cleanup;
      }
   }

cleanup:

   if( NULL != end_buf ) {
      maug_munlock( end_buf_h, end_buf );
   }

   if( (MAUG_MHANDLE)NULL != end_buf_h ) {
      maug_mfree( end_buf_h );
   }

   return retval;
}

/* === */

MERROR_RETVAL mfile_plt_open_read( const char* filename, mfile_t* p_file ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t st_size = 0;
   char filename_prefixed[MAUG_PATH_SZ_MAX + 1];
#  if defined( MFILE_MMAP )
   uint8_t* bytes_ptr = NULL;
   struct stat st;
   int in_file = 0;
#  endif /* MFILE_MMAP */
#  ifndef MAUG_NO_STAT
   struct stat file_stat;
   int stat_r = 0;
#  endif /* !MAUG_NO_STAT */

   maug_mzero( filename_prefixed, MAUG_PATH_SZ_MAX + 1 );
#  ifdef RETROFLAT_OS_NX
   maug_snprintf( filename_prefixed, MAUG_PATH_SZ_MAX, "romfs:/%s", filename );
#  else
   /* Generic case: no prefix. */
   maug_snprintf( filename_prefixed, MAUG_PATH_SZ_MAX, "%s", filename );
#  endif /* RETROFLAT_OS_NX */

#  ifndef MAUG_NO_STAT
   /* Get the file size from the OS. */
   stat_r = stat( filename_prefixed, &file_stat );
#     ifdef RETROFLAT_OS_WASM
   if( stat_r ) {
      /* Retry the stat after fetch. */
      mfile_wasm_fetch( filename_prefixed );
      stat_r = stat( filename_prefixed, &file_stat );
   } else {
      debug_printf( 1, "file exists: %d", stat_r );
   }
#     endif /* RETROFLAT_OS_WASM */
   st_size = file_stat.st_size;

   /* Perform *real* check after probe-checks above, which could cause the file
    * to be cached.
    */
   if( stat_r ) {
      error_printf( "could not stat: %s", filename_prefixed );
      retval = MERROR_FILE;
      goto cleanup;
   }
#  endif /* !MAUG_NO_STAT */

   retval = _mfile_plt_open(
      MFILE_FLAG_READ_ONLY, st_size, filename_prefixed, p_file );
   if( MERROR_OK == retval ) {
      p_file->flags |= MFILE_FLAG_READ_ONLY;
   }

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL mfile_plt_open_write( const char* filename, mfile_t* p_file ) {
   MERROR_RETVAL retval = MERROR_OK;
   retval = _mfile_plt_open( 0, 0, filename, p_file );
   return retval;
}

/* === */

void mfile_plt_close( mfile_t* p_file ) {
   fclose( p_file->h.file );
}

#endif /* !MAUG_API_FIL_H_DEFS */

