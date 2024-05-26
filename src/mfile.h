
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

#define MFILE_CADDY_TYPE_FILE_READ 0x01

#define MFILE_CADDY_TYPE_MEM_BUFFER 0x80

union MFILE_HANDLE {
   FILE* file;
   MAUG_MHANDLE mem;
};

struct MFILE_CADDY {
   uint8_t type;
   union MFILE_HANDLE h;
   size_t sz;
   size_t last_read;
   size_t mem_cursor;
   uint8_t* mem_buffer;
};

typedef struct MFILE_CADDY mfile_t;

#define mfile_default_case( p_file ) \
   default: \
      error_printf( "unknown file type: %d", (p_file)->type ); \
      break;

#define mfile_seek( p_file, idx ) \
   switch( (p_file)->type ) { \
   case MFILE_CADDY_TYPE_FILE_READ: \
      fseek( (p_file)->h.file, idx, SEEK_SET ); \
      break; \
   case MFILE_CADDY_TYPE_MEM_BUFFER: \
      (p_file)->mem_cursor = idx; \
      break; \
   mfile_default_case( p_file ); \
   }

#define mfile_has_bytes( p_file ) \
   ((MFILE_CADDY_TYPE_FILE_READ == ((p_file)->type) ? \
      (size_t)ftell( (p_file)->h.file ) : \
      (p_file)->mem_cursor) < (p_file)->sz)

#define mfile_cread( p_file, p_c ) \
   switch( (p_file)->type ) { \
   case MFILE_CADDY_TYPE_FILE_READ: \
      (p_file)->last_read = fread( p_c, 1, 1, (p_file)->h.file ); \
      break; \
   mfile_default_case( p_file ); \
   }

#define mfile_cread_at( p_file, p_c, idx ) \
   switch( (p_file)->type ) { \
   case MFILE_CADDY_TYPE_FILE_READ: \
      fseek( (p_file)->h.file, idx, SEEK_SET ); \
      (p_file)->last_read = fread( p_c, 1, 1, (p_file)->h.file ); \
      break; \
   case MFILE_CADDY_TYPE_MEM_BUFFER: \
      *p_c = (p_file)->mem_buffer[idx]; \
      (p_file)->mem_cursor += 1; \
      break; \
   mfile_default_case( p_file ); \
   }

#define mfile_u16read_at( p_file, p_u16, idx ) \
   switch( (p_file)->type ) { \
   case MFILE_CADDY_TYPE_FILE_READ: \
      fseek( (p_file)->h.file, idx, SEEK_SET ); \
      (p_file)->last_read = \
         fread( (((uint8_t*)p_u16) + 1), 1, 1, (p_file)->h.file ); \
      (p_file)->last_read += \
         fread( ((uint8_t*)p_u16), 1, 1, (p_file)->h.file ); \
      break; \
   case MFILE_CADDY_TYPE_MEM_BUFFER: \
      ((uint8_t*)(p_u16))[0] = (p_file)->mem_buffer[idx]; \
      ((uint8_t*)(p_u16))[1] = (p_file)->mem_buffer[idx + 1]; \
      (p_file)->mem_cursor += 2; \
      break; \
   mfile_default_case( p_file ); \
   }

#define mfile_u16read_lsbf_at( p_file, p_u16, idx ) \
   switch( (p_file)->type ) { \
   case MFILE_CADDY_TYPE_FILE_READ: \
      fseek( (p_file)->h.file, idx, SEEK_SET ); \
      (p_file)->last_read = fread( p_u16, 1, 2, (p_file)->h.file ); \
      break; \
   case MFILE_CADDY_TYPE_MEM_BUFFER: \
      ((uint8_t*)(p_u16))[0] = (p_file)->mem_buffer[idx + 1]; \
      ((uint8_t*)(p_u16))[1] = (p_file)->mem_buffer[idx]; \
      (p_file)->mem_cursor += 2; \
      break; \
   mfile_default_case( p_file ); \
   }

#define mfile_u32read_at( p_file, p_u32, idx ) \
   switch( (p_file)->type ) { \
   case MFILE_CADDY_TYPE_FILE_READ: \
      fseek( (p_file)->h.file, idx, SEEK_SET ); \
      (p_file)->last_read = \
         fread( (((uint8_t*)p_u32) + 3), 1, 1, (p_file)->h.file ); \
      (p_file)->last_read += \
         fread( (((uint8_t*)p_u32) + 2), 1, 1, (p_file)->h.file ); \
      (p_file)->last_read += \
         fread( (((uint8_t*)p_u32) + 1), 1, 1, (p_file)->h.file ); \
      (p_file)->last_read += \
         fread( ((uint8_t*)p_u32), 1, 1, (p_file)->h.file ); \
      break; \
   case MFILE_CADDY_TYPE_MEM_BUFFER: \
      ((uint8_t*)(p_u32))[0] = (p_file)->mem_buffer[idx]; \
      ((uint8_t*)(p_u32))[1] = (p_file)->mem_buffer[idx + 1]; \
      ((uint8_t*)(p_u32))[2] = (p_file)->mem_buffer[idx + 2]; \
      ((uint8_t*)(p_u32))[3] = (p_file)->mem_buffer[idx + 3]; \
      (p_file)->mem_cursor += 4; \
      break; \
   mfile_default_case( p_file ); \
   }

#define mfile_u32read_lsbf( p_file, p_u32 ) \
   switch( (p_file)->type ) { \
   case MFILE_CADDY_TYPE_FILE_READ: \
      (p_file)->last_read = fread( p_u32, 1, 4, (p_file)->h.file ); \
      break; \
   case MFILE_CADDY_TYPE_MEM_BUFFER: \
      ((uint8_t*)(p_u32))[3] = (p_file)->mem_buffer[(p_file)->mem_cursor]; \
      ((uint8_t*)(p_u32))[2] = (p_file)->mem_buffer[(p_file)->mem_cursor + 1]; \
      ((uint8_t*)(p_u32))[1] = (p_file)->mem_buffer[(p_file)->mem_cursor + 2]; \
      ((uint8_t*)(p_u32))[0] = (p_file)->mem_buffer[(p_file)->mem_cursor + 3]; \
      (p_file)->mem_cursor += 4; \
      break; \
   mfile_default_case( p_file ); \
   }

#define mfile_u32read_lsbf_at( p_file, p_u32, idx ) \
   switch( (p_file)->type ) { \
   case MFILE_CADDY_TYPE_FILE_READ: \
      fseek( (p_file)->h.file, idx, SEEK_SET ); \
      (p_file)->last_read = fread( p_u32, 1, 4, (p_file)->h.file ); \
      break; \
   case MFILE_CADDY_TYPE_MEM_BUFFER: \
      ((uint8_t*)(p_u32))[3] = (p_file)->mem_buffer[idx]; \
      ((uint8_t*)(p_u32))[2] = (p_file)->mem_buffer[idx + 1]; \
      ((uint8_t*)(p_u32))[1] = (p_file)->mem_buffer[idx + 2]; \
      ((uint8_t*)(p_u32))[0] = (p_file)->mem_buffer[idx + 3]; \
      (p_file)->mem_cursor += 4; \
      break; \
   mfile_default_case( p_file ); \
   }

#define mfile_get_sz( p_file ) ((p_file)->sz)

#define mfile_reset( p_file ) \
   switch( (p_file)->type ) { \
   case MFILE_CADDY_TYPE_FILE_READ: \
      fseek( (p_file)->h.file, 0, SEEK_SET ); \
      break; \
   case MFILE_CADDY_TYPE_MEM_BUFFER: \
      (p_file)->mem_cursor = 0; \
      break; \
   mfile_default_case( p_file ); \
   }

MERROR_RETVAL mfile_read_line( mfile_t*, char* buffer, size_t buffer_sz );

/**
 * \brief Lock a buffer and assign it to an ::mfile_t to read/write.
 */
MERROR_RETVAL mfile_lock_buffer( MAUG_MHANDLE, mfile_t* p_file );

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

MERROR_RETVAL mfile_read_line( mfile_t* p_f, char* buffer, size_t buffer_sz ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   if( MFILE_CADDY_TYPE_FILE_READ == p_f->type ) {
      /* Trivial case; use a native function. Much faster! */
      fgets( buffer, buffer_sz - 1, p_f->h.file );
      goto cleanup;
   }

   while( i < buffer_sz - 1 && mfile_has_bytes( p_f ) ) {
      /* Check for potential overflow. */
      if( i + 1 >= buffer_sz ) {
         error_printf( "overflow reading string from file!" );
         retval = MERROR_OVERFLOW;
         break;
      }

      mfile_cread( p_f, &(buffer[i]) );
      if( '\n' == buffer[i] ) {
         /* Break on newline and overwrite it below. */
         break;
      }
      i++;
   }

   assert( i < buffer_sz ); /* while() above stops before buffer_sz! */

   /* Append a null terminator. */
   buffer[i] = '\0';

cleanup:

   return retval;
}

MERROR_RETVAL mfile_lock_buffer( MAUG_MHANDLE handle, mfile_t* p_file ) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( 1, "locking handle %p as file %p...",
      handle, p_file );

   maug_mzero( p_file, sizeof( mfile_t ) );
   maug_mlock( handle, p_file->mem_buffer );
   p_file->type = MFILE_CADDY_TYPE_MEM_BUFFER;

   return retval;
}

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

   p_file->h.file = fopen( filename, "rb" );
   if( NULL == p_file->h.file ) {
      error_printf( "could not open file: %s", filename );
      retval = MERROR_FILE;
      goto cleanup;
   }

   fseek( p_file->h.file, 0, SEEK_END );
   p_file->sz = ftell( p_file->h.file );
   fseek( p_file->h.file, 0, SEEK_SET );

   debug_printf( 1, "opened file %s (" SIZE_T_FMT " bytes)...",
      filename, p_file->sz );

   p_file->type = MFILE_CADDY_TYPE_FILE_READ;

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
   switch( p_file->type ) {
   case 0:
      /* Do nothing silently. */
      break;

   case MFILE_CADDY_TYPE_FILE_READ:
      fclose( p_file->h.file );
      p_file->type = 0;
      break;

   case MFILE_CADDY_TYPE_MEM_BUFFER:
      if( NULL != p_file->mem_buffer ) {
         maug_munlock( p_file->h.mem, p_file->mem_buffer );
         debug_printf( 1, "unlocked handle %p from file %p...",
            p_file->h.mem, p_file );
         p_file->type = 0;
      }
      break;
      
   mfile_default_case( p_file );
   }
#  endif /* MFILE_MMAP */
}

#endif /* MFILE_C */

/*! \} */ /* maug_mfile */

#endif /* !MFILE_H */

