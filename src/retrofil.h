
#ifndef RETROFIL_H
#define RETROFIL_H

/**
 * \addtogroup maug_retrofil RetroFile API
 * \brief Abstraction layer for dealing with files on retro systems.
 * \{
 */

/**
 * \file retrofil.h
 */

/**
 * \brief Open a file and read it into memory or memory-map it.
 * \param filename NULL-terminated path to file to open.
 * \param p_bytes_h Pointer to MAUG_MHANDLE to open into.
 * \param p_bytes_sz Pointer to size_t in which to store file size.
 */
MERROR_RETVAL retrofil_open_mread(
   const char* filename, MAUG_MHANDLE* p_bytes_h, size_t* p_bytes_sz );

/**
 * \brief Close a file opened with retrofil_open_mread().
 * \param bytes_ptr_h MAUG_MHANDLE opened by retrofil_open_mread().
 * \param bytes_sz Same size value passed from retrofil_open_mread().
 */
void retrofil_close_mread( MAUG_MHANDLE bytes_ptr_h, size_t bytes_sz );

#ifdef RETROFIL_C

#ifdef RETROFLAT_OS_UNIX
#  include <sys/mman.h> /* mmap() */
#  include <unistd.h> /* close() */
#  include <fcntl.h> /* open() */
#  include <sys/stat.h> /* fstat() */
#else
#  include <stdio.h>
#endif /* RETROFLAT_OS_UNIX */

MERROR_RETVAL retrofil_open_mread(
   const char* filename, MAUG_MHANDLE* p_bytes_ptr_h, size_t* p_bytes_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
#ifdef RETROFLAT_OS_UNIX
   struct stat st;
   int in_file = 0;
#else
   size_t bytes_rd = 0;
   FILE* in_file = NULL;
   uint8_t* bytes_ptr = NULL;
#endif /* !RETROFLAT_OS_UNIX */

#ifdef RETROFLAT_OS_UNIX

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

#else

   in_file = fopen( filename, "rb" );
   if( NULL == in_file ) {
      error_printf( "could not open file: %s", filename );
      retval = MERROR_FILE;
      goto cleanup;
   }

   fseek( in_file, 0, SEEK_END );
   *p_bytes_sz = ftell( in_file );
   fseek( in_file, 0, SEEK_SET );

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

#endif /* RETROFLAT_OS_UNIX */

   return retval;
}

void retrofil_close_mread( MAUG_MHANDLE bytes_ptr_h, size_t bytes_sz ) {
#ifdef RETROFLAT_OS_UNIX
   munmap( bytes_ptr_h, bytes_sz );
#else
   maug_mfree( bytes_ptr_h );
#endif /* RETROFLAT_OS_UNIX */
}

#endif /* RETROFIL_C */

/*! \} */ /* maug_retrofil */

#endif /* !RETROFIL_H */

