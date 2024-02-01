
#ifndef MFILE_H
#define MFILE_H

/**
 * \addtogroup maug_mfile RetroFile API
 * \brief Abstraction layer for dealing with files on retro systems.
 * \{
 */

/**
 * \file mfile.h
 */

/* TODO: open_cread: return a handle and reach chars via macro by offset. */

struct MFILE_CADDY {
   FILE* handle;
   size_t sz;
};

typedef struct MFILE_CADDY mfile_t;

#define mfile_has_bytes( p_file ) (ftell( (p_file)->handle ) < (p_file)->sz)

#define mfile_cread( p_file, p_c ) fread( p_c, 1, 1, (p_file)->handle )

#define mfile_cread_at( p_file, p_c, idx ) fseek( (p_file)->handle, idx, SEEK_SET ); fread( p_c, 1, 1, (p_file)->handle )

#define mfile_u16read_at( p_file, p_u16, idx ) fseek( (p_file)->handle, idx, SEEK_SET ); fread( p_u16, 1, 2, (p_file)->handle )

#define mfile_u16read_lsbf_at( p_file, p_u16, idx ) \
   fseek( (p_file)->handle, idx, SEEK_SET ); \
   fread( p_u16, 1, 2, (p_file)->handle );

#if 0
   fread( (((uint8_t*)p_u16) + 1), 1, 1, (p_file)->handle ); \
   fread( ((uint8_t*)p_u16), 1, 1, (p_file)->handle );
#endif

#define mfile_u32read_at( p_file, p_u32, idx ) \
   fseek( (p_file)->handle, idx, SEEK_SET ); \
   fread( p_u32, 1, 4, (p_file)->handle )

#define mfile_u32read_lsbf_at( p_file, p_u32, idx ) \
   fseek( (p_file)->handle, idx, SEEK_SET ); \
   fread( p_u32, 1, 4, (p_file)->handle ); \

#if 0
   fread( (((uint8_t*)p_u32) + 3), 1, 1, (p_file)->handle ); \
   fread( (((uint8_t*)p_u32) + 2), 1, 1, (p_file)->handle ); \
   fread( (((uint8_t*)p_u32) + 1), 1, 1, (p_file)->handle ); \
   fread( ((uint8_t*)p_u32), 1, 1, (p_file)->handle );
#endif

#define mfile_get_sz( p_file ) ((p_file)->sz)

#define mfile_reset( p_file ) fseek( (p_file)->handle, 0, SEEK_SET )

/**
 * \brief Open a file and read it into memory or memory-map it.
 * \param filename NULL-terminated path to file to open.
 */
MERROR_RETVAL mfile_open_read( const char* filename, mfile_t* p_file );

/**
 * \brief Close a file opened with mfile_open_read().
 */
void mfile_close( mfile_t* p_file );

#ifdef MFILE_C

#ifdef MFILE_MMAP
#  include <sys/mman.h> /* mmap() */
#  include <unistd.h> /* close() */
#  include <fcntl.h> /* open() */
#  include <sys/stat.h> /* fstat() */
#else
#  include <stdio.h>
#endif /* RETROFLAT_OS_UNIX */

MERROR_RETVAL mfile_open_read( const char* filename, mfile_t* p_file ) {
   MERROR_RETVAL retval = MERROR_OK;
#  ifdef MFILE_MMAP
   uint8_t* bytes_ptr = NULL;
   struct stat st;
   int in_file = 0;
#  endif /* MFILE_MMAP */

   /* MAUG_MHANDLE* p_bytes_ptr_h, size_t* p_bytes_sz */

#  ifdef MFILE_MMAP

   in_file = open( filename, O_RDONLY );
   if( 0 >= in_file ) {
      error_printf( "could not open file: %s", filename );
      retval = MERROR_FILE;
      goto cleanup;
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

   p_file->handle = fopen( filename, "rb" );
   if( NULL == p_file->handle ) {
      error_printf( "could not open file: %s", filename );
      retval = MERROR_FILE;
      goto cleanup;
   }

   fseek( p_file->handle, 0, SEEK_END );
   p_file->sz = ftell( p_file->handle );
   fseek( p_file->handle, 0, SEEK_SET );

   debug_printf( 1, "opened file %s (" SIZE_T_FMT " bytes)...",
      filename, p_file->sz );

/*
   *p_bytes_ptr_h = maug_malloc( 1, *p_bytes_sz );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, *p_bytes_ptr_h );

   maug_mlock( *p_bytes_ptr_h, bytes_ptr );
   maug_cleanup_if_null_alloc( uint8_t*, bytes_ptr );

   bytes_rd = fread( bytes_ptr, 1, *p_bytes_sz, in_file );
   if( bytes_rd < *p_bytes_sz ) {
      error_printf( "could not read entire file!" );
      retval = MERROR_FILE;
      goto cleanup;
   }

cleanup:

   if( NULL != bytes_ptr ) {
      maug_munlock( *p_bytes_ptr_h, bytes_ptr );
   }

   if( NULL != in_file ) {
      fclose( in_file );
   }

*/

cleanup:

#  endif /* MFILE_MMAP */

   return retval;
}

void mfile_close( mfile_t* p_file ) {
#  ifdef MFILE_MMAP
   munmap( bytes_ptr_h, bytes_sz );
#  else
   /* maug_mfree( bytes_ptr_h ); */
   fclose( p_file->handle );
#  endif /* MFILE_MMAP */
}

#endif /* MFILE_C */

/*! \} */ /* maug_mfile */

#endif /* !MFILE_H */

