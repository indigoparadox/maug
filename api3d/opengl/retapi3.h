
#ifndef RETAPI3_H
#define RETAPI3_H

#  if defined( RETROFLAT_OPENGL )
#     include <GL/gl.h>
#     include <GL/glu.h>
#  endif /* RETROFLAT_OPENGL */

#define RETRO3D_COLOR_TABLE( idx, name_l, name_u, r, g, b, cgac, cgad ) \
   { (float)((float)r * 1.0f / 255.0f), \
     (float)((float)g * 1.0f / 255.0f), \
     (float)((float)b * 1.0f / 255.0f) },

MAUG_CONST float gc_retro3d_color_table[][3] = { \
RETROFLAT_COLOR_TABLE( RETRO3D_COLOR_TABLE )
};

void retro3d_scene_init() {
   /* Scale down up so glVertex3i operates on 100-basis. */
   glPushMatrix();
   glScalef( 0.01, 0.01, 0.01 );
   glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

/* === */

void retro3d_scene_complete() {
   glPopMatrix();
   glFlush();
}

/* === */

void retro3d_scene_translate( int x, int y, int z ) {
   glTranslatef( ((float)x), ((float)y), ((float)z) );
}

/* === */

void retro3d_scene_rotate( int x, int y, int z ) {
   if( 0 < x ) {
      glRotatef( x, 1.0f, 0.0f, 0.0f );
   }
   if( 0 < y ) {
      glRotatef( y, 0.0f, 1.0f, 0.0f );
   }
   if( 0 < z ) {
      glRotatef( z, 0.0f, 0.0f, 1.0f );
   }
}

/* === */

void retro3d_vx( int x, int y, int z, int s, int t ) {
   glTexCoord2i( s, t );
   glVertex3i( x, y, z );
}

/* === */

void retro3d_tri_begin( RETROFLAT_COLOR color, uint8_t flags ) {
   int normal_val = 1;
   if( RETRO3D_TRI_FLAG_NORMAL_NEG == (RETRO3D_TRI_FLAG_NORMAL_NEG & flags) ) {
      normal_val *= -1;
   }
   if( RETRO3D_TRI_FLAG_NORMAL_X == (RETRO3D_TRI_FLAG_NORMAL_X & flags) ) {
      glNormal3i( normal_val, 0, 0 );
   } else if(
      RETRO3D_TRI_FLAG_NORMAL_Y == (RETRO3D_TRI_FLAG_NORMAL_Y & flags)
   ) {
      glNormal3i( 0, normal_val, 0 );
   } else if(
      RETRO3D_TRI_FLAG_NORMAL_Z == (RETRO3D_TRI_FLAG_NORMAL_Z & flags)
   ) {
      glNormal3i( 0, 0, normal_val );
   }
   if( RETROFLAT_COLOR_NULL != color ) {
      glColor3fv( gc_retro3d_color_table[color] );
   }
   glBegin( GL_TRIANGLES );
}

/* === */

void retro3d_tri_end() {
   glEnd();
}

/* === */

MERROR_RETVAL retro3d_check_errors( const char* desc ) {
   MERROR_RETVAL retval = MERROR_OK;
#ifndef RETRO3D_NO_ERRORS
   GLenum gl_retval;

   do {
      gl_retval = glGetError();
      if( GL_NO_ERROR != gl_retval ) {
         error_printf( "GL error on %s: 0x%x", desc, gl_retval );
         retval = MERROR_GUI;
      }
   } while( GL_NO_ERROR != gl_retval );
#endif /* !RETRO3D_NO_ERRORS */

   return retval;
}

/* === */

MERROR_RETVAL retro3d_texture_lock( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = RETROFLAT_OK;

   assert( NULL != bmp && &(g_retroflat_state->buffer) != bmp );
   assert( !retroflat_bitmap_locked( bmp ) );

   bmp->flags |= RETROFLAT_FLAGS_LOCK;
   maug_mlock( bmp->tex.bytes_h, bmp->tex.bytes );

   return retval;
}

/* === */

/* TODO: Integrate as much of these retro3d_texture_* functions into mfmt_bmp_*
 *       as possible, to eliminate redundancy.
 */

MERROR_RETVAL retro3d_texture_release( struct RETROFLAT_BITMAP* bmp ) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( NULL != bmp && &(g_retroflat_state->buffer) != bmp );
   assert( retroflat_bitmap_locked( bmp ) );

#ifndef RETROGLU_NO_TEXTURES
   bmp->flags &= ~RETROFLAT_FLAGS_LOCK;
#  ifndef RETROGLU_NO_TEXTURE_LISTS
   assert( 0 < bmp->tex.id );
   assert( NULL != bmp->tex.bytes );

   /* Update stored texture if it exists. */
   glBindTexture( GL_TEXTURE_2D, bmp->tex.id );
   glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, bmp->tex.w, bmp->tex.h, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, bmp->tex.bytes ); 
   glBindTexture( GL_TEXTURE_2D, 0 );
#  endif /* !RETROGLU_NO_TEXTURE_LISTS */

   /* Unlock texture bitmap. */
   maug_munlock( bmp->tex.bytes_h, bmp->tex.bytes );
#endif /* !RETROGLU_NO_TEXTURES */

   return retval;
}

/* === */

MERROR_RETVAL retro3d_texture_load_bitmap(
   const char* filename_path, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
#ifndef RETROGLU_NO_TEXTURES
   mfile_t bmp_file;
   struct MFMT_STRUCT_BMPFILE header_bmp;
   MAUG_MHANDLE bmp_palette_h = (MAUG_MHANDLE)NULL;
   uint32_t bmp_color = 0;
   uint32_t* bmp_palette = NULL;
   MAUG_MHANDLE bmp_px_h = (MAUG_MHANDLE)NULL;
   uint8_t* bmp_px = NULL;
   off_t bmp_px_sz = 0;
   uint8_t bmp_r = 0,
      bmp_g = 0,
      bmp_b = 0,
      bmp_color_idx = 0,
      bmp_flags = 0;
   off_t i = 0;

   retval = mfile_open_read( filename_path, &bmp_file );
   maug_cleanup_if_not_ok();

   /* TODO: mfmt file detection system. */
   header_bmp.magic[0] = 'B';
   header_bmp.magic[1] = 'M';
   header_bmp.info.sz = 40;

   retval = mfmt_read_bmp_header(
      (struct MFMT_STRUCT*)&header_bmp,
      &bmp_file, 0, mfile_get_sz( &bmp_file ), &bmp_flags );
   maug_cleanup_if_not_ok();

   assert( 0 < mfile_get_sz( &bmp_file ) );

   /* Setup bitmap options from header. */
   bmp_out->tex.w = header_bmp.info.width;
   bmp_out->tex.h = header_bmp.info.height;
   bmp_out->tex.sz = bmp_out->tex.w * bmp_out->tex.h * 4;
   bmp_out->tex.bpp = 24;

   /* Allocate a space for the bitmap palette. */
   bmp_palette_h = maug_malloc( 4, header_bmp.info.palette_ncolors );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, bmp_palette_h );

   maug_mlock( bmp_palette_h, bmp_palette );
   maug_cleanup_if_null_alloc( uint32_t*, bmp_palette );

   retval = mfmt_read_bmp_palette( 
      (struct MFMT_STRUCT*)&header_bmp,
      bmp_palette, 4 * header_bmp.info.palette_ncolors,
      &bmp_file, 54 /* TODO */, mfile_get_sz( &bmp_file ) - 54, bmp_flags );
   maug_cleanup_if_not_ok();

   /* Allocate a space for the bitmap pixels. */
   bmp_px_sz = header_bmp.info.width * header_bmp.info.height;
   bmp_px_h = maug_malloc( 1, bmp_px_sz );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, bmp_px_h );

   maug_mlock( bmp_px_h, bmp_px );
   maug_cleanup_if_null_alloc( uint8_t*, bmp_px );

   retval = mfmt_read_bmp_px( 
      (struct MFMT_STRUCT*)&header_bmp,
      bmp_px, bmp_px_sz,
      &bmp_file, header_bmp.px_offset,
      mfile_get_sz( &bmp_file ) - header_bmp.px_offset, bmp_flags );
   maug_cleanup_if_not_ok();

   /* Allocate buffer for unpacking. */
   debug_printf( 0, "creating bitmap: " SIZE_T_FMT " x " SIZE_T_FMT,
      bmp_out->tex.w, bmp_out->tex.h );
   bmp_out->tex.bytes_h = maug_malloc( bmp_out->tex.w * bmp_out->tex.h, 4 );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, bmp_out->tex.bytes_h );

   maug_mlock( bmp_out->tex.bytes_h, bmp_out->tex.bytes );
   maug_cleanup_if_null_alloc( uint8_t*, bmp_out->tex.bytes );

   /* Unpack palletized bitmap into BGRA with color key. */
   for( i = 0 ; bmp_px_sz > i ; i++ ) {
      if( bmp_px_sz - i - 1 > bmp_px_sz ) {
         error_printf(
            "pixel overflow! (" OFF_T_FMT " of " OFF_T_FMT " bytes!)",
            bmp_px_sz - i - 1, bmp_px_sz );
         retval = MERROR_OVERFLOW;
         goto cleanup;
      }

      /* Grab the color from the palette by index. */
      bmp_color_idx = bmp_px[bmp_px_sz - i - 1]; /* Reverse image. */
      if( bmp_color_idx >= header_bmp.info.palette_ncolors ) {
         error_printf(
            "invalid color at px " OFF_T_FMT ": %02x",
            bmp_px_sz - i - 1, bmp_color_idx );
         continue;
      }
      bmp_color = bmp_palette[bmp_color_idx];
      bmp_r = (bmp_color >> 16) & 0xff;
      bmp_g = (bmp_color >> 8) & 0xff;
      bmp_b = bmp_color & 0xff;

      bmp_out->tex.bytes[i * 4] = bmp_r;
      bmp_out->tex.bytes[(i * 4) + 1] = bmp_g;
      bmp_out->tex.bytes[(i * 4) + 2] = bmp_b;
      if(
         RETROFLAT_FLAGS_OPAQUE != (RETROFLAT_FLAGS_OPAQUE & flags) &&
         RETROFLAT_TXP_R == bmp_r &&
         RETROFLAT_TXP_G == bmp_g &&
         RETROFLAT_TXP_B == bmp_b
      ) {
         /* Transparent pixel found. */
         bmp_out->tex.bytes[(i * 4) + 3] = 0x00;
      } else {
         bmp_out->tex.bytes[(i * 4) + 3] = 0xff;
      }
   }

#  ifndef RETROGLU_NO_TEXTURE_LISTS
   glGenTextures( 1, (GLuint*)&(bmp_out->tex.id) );
   glBindTexture( GL_TEXTURE_2D, bmp_out->tex.id );
   glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, bmp_out->tex.w, bmp_out->tex.h, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, bmp_out->tex.bytes ); 
   glBindTexture( GL_TEXTURE_2D, 0 );
#  endif /* !RETROGLU_NO_TEXTURE_LISTS */

cleanup:

   if( NULL != bmp_out->tex.bytes ) {
      maug_munlock( bmp_out->tex.bytes_h, bmp_out->tex.bytes );
   }

   if( NULL != bmp_px ) {
      maug_munlock( bmp_px_h, bmp_px );
   }

   if( NULL != bmp_px_h ) {
      maug_mfree( bmp_px_h );
   }

   if( NULL != bmp_palette ) {
      maug_munlock( bmp_palette_h, bmp_palette );
   }

   if( NULL != bmp_palette_h ) {
      maug_mfree( bmp_palette_h );
   }

   mfile_close( &bmp_file );

#endif /* !RETROGLU_NO_TEXTURES */

   return retval;
}

/* === */

MERROR_RETVAL retro3d_texture_blit(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   size_t s_x, size_t s_y, size_t d_x, size_t d_y, size_t w, size_t h,
   int16_t instance
) {
   MERROR_RETVAL retval = MERROR_OK;
   int y_iter = 0;

   /* TODO: Check memcpy, etc? */

   assert( NULL != target && retroflat_screen_buffer() != target );

   /* Blit to texture. */

   assert( NULL != target->tex.bytes );

   /* TODO: Some kind of source-autolock? */
   assert( !retroflat_bitmap_locked( src ) );
   maug_mlock( src->tex.bytes_h, src->tex.bytes );
   for( y_iter = 0 ; h > y_iter ; y_iter++ ) {
      /* TODO: Handle transparency! */
      memcpy(
         &(target->tex.bytes[(((y_iter * target->tex.w) + d_x) * 4)]),
         &(src->tex.bytes[(((y_iter * src->tex.w) + s_x) * 4)]),
         w * 4 );
   }
   maug_munlock( src->tex.bytes_h, src->tex.bytes );

   return retval;
}

/* === */

MERROR_RETVAL retro3d_texture_create(
   size_t w, size_t h, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;

#ifdef RETROGLU_NO_TEXTURES
   error_printf( "textures not enabled!" );
   retval = MERROR_GUI;
#else

   if( w > 256 ) {
      error_printf( "warning! attempting to create texture with w > 256 ("
         SIZE_T_FMT "). This may not work on Win32!", w );
   }

   if( h > 256 ) {
      error_printf( "warning! attempting to create texture with h > 256 ("
         SIZE_T_FMT "). This may not work on Win32!", h );
   }

   bmp_out->tex.w = w;
   bmp_out->tex.h = h;
   /* TODO: Overflow checking. */
   debug_printf( 0, "creating bitmap: " SIZE_T_FMT " x " SIZE_T_FMT,
      bmp_out->tex.w, bmp_out->tex.h );
   bmp_out->tex.bytes_h =
      maug_malloc( bmp_out->tex.w * bmp_out->tex.h, 4 );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, bmp_out->tex.bytes_h );

   maug_mlock( bmp_out->tex.bytes_h, bmp_out->tex.bytes );
   maug_cleanup_if_null_lock( uint8_t*, bmp_out->tex.bytes );

   /* TODO: Overflow checking. */
   maug_mzero(
      bmp_out->tex.bytes,
      bmp_out->tex.w * bmp_out->tex.h * sizeof( uint32_t ) );

#     ifndef RETROGLU_NO_TEXTURE_LISTS
   glGenTextures( 1, (GLuint*)&(bmp_out->tex.id) );
   debug_printf( RETROFLAT_BITMAP_TRACE_LVL,
      "assigned bitmap texture: " UPRINTF_U32_FMT, bmp_out->tex.id );
   retval = retro3d_check_errors( "gentextures" );
   maug_cleanup_if_not_ok();
#     endif /* !RETROGLU_NO_TEXTURE_LISTS */

cleanup:
   if( NULL != bmp_out->tex.bytes ) {
      maug_munlock( bmp_out->tex.bytes_h, bmp_out->tex.bytes );
   }

#endif /* !RETROGLU_NO_TEXTURES */

   return retval;
}

/* === */

void retro3d_texture_destroy( struct RETROFLAT_BITMAP* bmp ) {
#ifndef RETROGLU_NO_TEXTURES
   debug_printf( 1, "destroying bitmap..." );

   if( NULL != bmp->tex.bytes ) {
      maug_munlock( bmp->tex.bytes_h, bmp->tex.bytes );
   }
   
   if( NULL != bmp->tex.bytes_h ) {
      maug_mfree( bmp->tex.bytes_h );
   }

#  ifndef RETROGLU_NO_TEXTURE_LISTS
   if( 0 < bmp->tex.id ) {
      debug_printf( 0, 
         "destroying bitmap texture: " UPRINTF_U32_FMT, bmp->tex.id );
      glDeleteTextures( 1, (GLuint*)&(bmp->tex.id) );
   }
#  endif /* !RETROGLU_NO_TEXTURE_LISTS */
#endif /* !RETROGLU_NO_TEXTURES */
}

/* === */

void retro3d_texture_px(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   size_t x, size_t y, uint8_t flags
) {

   if(
      RETROFLAT_FLAGS_BITMAP_RO ==
         (RETROFLAT_FLAGS_BITMAP_RO & target->flags) ||
      target->tex.w <= x ||
      target->tex.h <= y
   ) {
      return;
   }

   assert( NULL != target->tex.bytes );
   /* assert( retroflat_bitmap_locked( target ) ); */

   /* Draw pixel colors from texture palette. */
   target->tex.bytes[(((y * target->tex.w) + x) * 4) + 0] =
      g_retroflat_state->tex_palette[color_idx][0];
   target->tex.bytes[(((y * target->tex.w) + x) * 4) + 1] =
      g_retroflat_state->tex_palette[color_idx][1];
   target->tex.bytes[(((y * target->tex.w) + x) * 4) + 2] =
      g_retroflat_state->tex_palette[color_idx][2];

   /* Set pixel as opaque. */
   target->tex.bytes[(((y * target->tex.w) + x) * 4) + 3] = 0xff;
}

#endif /* !RETAPI3_H */

