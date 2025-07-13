
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
   int stat_r = 0;

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
   stat_r = stat( filename, &file_stat );
#        ifdef RETROFLAT_OS_WASM
   if( stat_r ) {
      /* Retry the stat after fetch. */
      mfile_wasm_fetch( filename );
      stat_r = stat( filename, &file_stat );
   } else {
      debug_printf( 1, "file exists: %d", stat_r );
   }
#        endif /* RETROFLAT_OS_WASM */

   /* Perform *real* check after probe-checks above, which could cause the file
    * to be cached.
    */
   if( stat_r ) {
      error_printf( "could not stat: %s", filename );
      retval = MERROR_FILE;
      goto cleanup;
   }

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
   p_file->read_byte = mfile_file_read_byte;
   p_file->read_block = mfile_file_read_block;
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

