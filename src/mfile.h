
#ifndef MFILE_H
#define MFILE_H

#if !defined( MFILE_MMAP )
#  include <sys/stat.h>
#endif /* !MFILE_MMAP */

/**
 * \addtogroup maug_mfile RetroFile API
 * \brief Abstraction layer for dealing with files on retro systems.
 * \{
 */

/**
 * \file mfile.h
 */

/**
 * \addtogroup maug_mfile_types RetroFile Types
 * \brief Types of files/data stores that mfile can abstract from.
 *
 * This library is designed with the intention of including various archive
 * and "weird hardware storage" formats.
 * \{
 */

/**
 * \brief A standard UNIX file opened for reading.
 */
#define MFILE_CADDY_TYPE_FILE_READ 0x01

/**
 * \brief An array of bytes in memory abstracted through this library.
 */
#define MFILE_CADDY_TYPE_MEM_BUFFER 0x80

/*! \} */ /* maug_mfile_types */

#define MFILE_READ_FLAG_LSBF     0x01

struct MFILE_CADDY;

typedef MERROR_RETVAL (*mfile_seek_t)( struct MFILE_CADDY* p_file, off_t pos );
typedef MERROR_RETVAL (*mfile_read_int_t)(
   struct MFILE_CADDY* p_file, uint8_t* buf, size_t buf_sz, uint8_t flags );
typedef MERROR_RETVAL (*mfile_read_line_t)(
   struct MFILE_CADDY* p_file, char* buf, off_t buf_sz, uint8_t flags );

union MFILE_HANDLE {
   FILE* file;
   MAUG_MHANDLE mem;
};

struct MFILE_CADDY {
   /*! \brief The \ref maug_mfile_types flag describing this file. */
   uint8_t type;
   /*! \brief The physical handle or pointer to access the file by. */
   union MFILE_HANDLE h;
   off_t sz;
   off_t last_read;
   /*! \brief Current position if its type is ::MFILE_CADDY_TYPE_MEM_BUFFER. */
   off_t mem_cursor;
   /*! \brief Locked pointer for MFILE_HANDLE::mem. */
   uint8_t* mem_buffer;
   mfile_seek_t seek;
   mfile_read_int_t read_int;
   mfile_read_line_t read_line;
};

typedef struct MFILE_CADDY mfile_t;

#define mfile_check_lock( p_file ) (NULL != (p_file)->mem_buffer)

#define mfile_default_case( p_file ) \
   default: \
      error_printf( "unknown file type: %d", (p_file)->type ); \
      break;

#define mfile_has_bytes( p_file ) \
   ((MFILE_CADDY_TYPE_FILE_READ == ((p_file)->type) ? \
      (off_t)ftell( (p_file)->h.file ) : \
      (p_file)->mem_cursor) < (p_file)->sz)

#ifdef MFILE_LEGACY_MACROS

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

#define mfile_cread( p_file, p_c ) \
   switch( (p_file)->type ) { \
   case MFILE_CADDY_TYPE_FILE_READ: \
      (p_file)->last_read = fread( p_c, 1, 1, (p_file)->h.file ); \
      break; \
   case MFILE_CADDY_TYPE_MEM_BUFFER: \
      ((uint8_t*)(p_c))[0] = (p_file)->mem_buffer[(p_file)->mem_cursor++]; \
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

#endif /* MFILE_LEGACY_MACROS */

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

/**
 * \brief Lock a buffer and assign it to an ::mfile_t to read/write.
 */
MERROR_RETVAL mfile_lock_buffer( MAUG_MHANDLE, off_t, mfile_t* p_file );

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

/* === */

MERROR_RETVAL mfile_file_read_int(
   struct MFILE_CADDY* p_file, uint8_t* buf, size_t buf_sz, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t last_read = 0;

   assert( MFILE_CADDY_TYPE_FILE_READ == p_file->type );

   if( MFILE_READ_FLAG_LSBF == (MFILE_READ_FLAG_LSBF & flags) ) {
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

   assert( MFILE_CADDY_TYPE_FILE_READ == p_file->type );

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

   assert( MFILE_CADDY_TYPE_FILE_READ == p_f->type );

   /* Trivial case; use a native function. Much faster! */
   if( NULL == fgets( buffer, buffer_sz - 1, p_f->h.file ) ) {
      error_printf( "error while reading line from file!" );
      retval = MERROR_FILE;
   }

   return retval;
}

/* === */

MERROR_RETVAL mfile_mem_read_int(
   struct MFILE_CADDY* p_file, uint8_t* buf, size_t buf_sz, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( MFILE_CADDY_TYPE_MEM_BUFFER == p_file->type );

   if( MFILE_READ_FLAG_LSBF != (MFILE_READ_FLAG_LSBF & flags) ) {
      /* Shrink the buffer moving right and read into it. */
      while( 0 < buf_sz ) {
         /* Check for EOF. */
         if( p_file->mem_cursor >= p_file->sz ) {
            retval = MERROR_FILE;
            error_printf(
               "cursor " OFF_T_FMT " beyond end of buffer " OFF_T_FMT "!",
               p_file->mem_cursor, p_file->sz );
            goto cleanup;
         }

         buf[buf_sz - 1] = p_file->mem_buffer[p_file->mem_cursor];
         debug_printf( 1, "byte #" SIZE_T_FMT " = # " OFF_T_FMT,
            buf_sz - 1, p_file->mem_cursor );
         buf_sz--;
         p_file->mem_cursor++;
      }
   
   } else {
      /* Move to the end of the output buffer and read backwards. */
      while( 0 < buf_sz ) {
         /* Check for EOF. */
         if( p_file->mem_cursor >= p_file->sz ) {
            retval = MERROR_FILE;
            error_printf(
               "cursor " OFF_T_FMT " beyond end of buffer " OFF_T_FMT "!",
               p_file->mem_cursor, p_file->sz );
            goto cleanup;
         }

         buf[buf_sz - 1] = p_file->mem_buffer[p_file->mem_cursor];
         debug_printf( 1, "byte #" SIZE_T_FMT " = # " OFF_T_FMT,
            buf_sz - 1, p_file->mem_cursor );
         buf_sz--;
         buf++;
         p_file->mem_cursor++;
      }
   }

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL mfile_mem_seek( struct MFILE_CADDY* p_file, off_t pos ) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( MFILE_CADDY_TYPE_MEM_BUFFER == p_file->type );

   p_file->mem_cursor = pos;

   debug_printf( 1,
      "seeking memory buffer to position " OFF_T_FMT " (" OFF_T_FMT ")",
      pos, p_file->mem_cursor );

   return retval;
}

/* === */

MERROR_RETVAL mfile_mem_read_line(
   struct MFILE_CADDY* p_f, char* buffer, off_t buffer_sz, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   off_t i = 0;

   assert( MFILE_CADDY_TYPE_MEM_BUFFER == p_f->type );

   while( i < buffer_sz - 1 && mfile_has_bytes( p_f ) ) {
      /* Check for potential overflow. */
      if( i + 1 >= buffer_sz ) {
         error_printf( "overflow reading string from file!" );
         retval = MERROR_FILE;
         break;
      }

      p_f->read_int( p_f, (uint8_t*)&(buffer[i]), 1, 0 );
      if( '\n' == buffer[i] ) {
         /* Break on newline and overwrite it below. */
         break;
      }
      i++;
   }

   assert( i < buffer_sz ); /* while() above stops before buffer_sz! */

   /* Append a null terminator. */
   buffer[i] = '\0';

   return retval;
}

/* === */

MERROR_RETVAL mfile_lock_buffer(
   MAUG_MHANDLE handle, off_t handle_sz,  mfile_t* p_file
) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( 1,
      "locking handle %p as file %p (" OFF_T_FMT " bytes)...",
      handle, p_file, handle_sz );

   maug_mzero( p_file, sizeof( struct MFILE_CADDY ) );
   maug_mlock( handle, p_file->mem_buffer );
   p_file->type = MFILE_CADDY_TYPE_MEM_BUFFER;

   p_file->read_int = mfile_mem_read_int;
   p_file->seek = mfile_mem_seek;
   p_file->read_line = mfile_mem_read_line;

   p_file->sz = handle_sz;

   return retval;
}

MERROR_RETVAL mfile_open_read( const char* filename, mfile_t* p_file ) {
   MERROR_RETVAL retval = MERROR_OK;
#  ifdef MFILE_MMAP
   uint8_t* bytes_ptr = NULL;
   struct stat st;
   int in_file = 0;
#  else
   struct stat file_stat;
#  endif /* MFILE_MMAP */

   /* MAUG_MHANDLE* p_bytes_ptr_h, off_t* p_bytes_sz */

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

   /* Get the file size from the OS. */
   stat( filename, &file_stat );
   p_file->sz = file_stat.st_size;

#  define MFILE_GOT_FILE_SIZE 1

   /* Open the permanent file handle. */
   p_file->h.file = fopen( filename, "rb" );
   if( NULL == p_file->h.file ) {
      error_printf( "could not open file: %s", filename );
      retval = MERROR_FILE;
      goto cleanup;
   }

#ifndef MFILE_GOT_FILE_SIZE
   /* The standard is not required to support SEEK_END, among other issues.
    * This is probably the worst way to get file size.
    */
   debug_printf( 2, "falling back to seek file size..." );
   fseek( p_file->h.file, 0, SEEK_END );
   p_file->sz = ftell( p_file->h.file );
   fseek( p_file->h.file, 0, SEEK_SET );
#endif /* MAUG_OS_* */

   debug_printf( 1, "opened file %s (" SIZE_T_FMT " bytes)...",
      filename, p_file->sz );
   /* debug_printf( 3, "XXX %ld bytes", file_stat.st_size );
   debug_printf( 3, "XXX size_t: %d, off_t: %d", sizeof( size_t ), sizeof( off_t ) ); */

   p_file->type = MFILE_CADDY_TYPE_FILE_READ;

   p_file->read_int = mfile_file_read_int;
   p_file->seek = mfile_file_seek;
   p_file->read_line = mfile_file_read_line;

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

