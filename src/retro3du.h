
#ifndef RETRO3DU_H
#define RETRO3DU_H

/**
 * \addtogroup maug_retro3d
 */

/**
 * \addtogroup maug_retro3d_util Retro3D API Utilities
 * \{
 */

/**
 * \file retro3du.h
 * \brief Collection of utilities built on top of the lower-level 3D engine
 *        primatives defined in retro3d.h.
 */

/**
 * \brief Lock a texture for modification.
 */
MERROR_RETVAL retro3d_texture_lock( struct RETROFLAT_BITMAP* bmp );

/**
 * \brief Unlock a locked texture.
 * \warning This must be called before a texture is used!
 */
MERROR_RETVAL retro3d_texture_release( struct RETROFLAT_BITMAP* bmp );

void retro3d_texture_px(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   size_t x, size_t y, uint8_t flags );

MERROR_RETVAL retro3d_texture_load_bitmap(
   const char* filename_path, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags );

MERROR_RETVAL retro3d_texture_blit(
   struct RETROFLAT_BITMAP* target, struct RETROFLAT_BITMAP* src,
   size_t s_x, size_t s_y, size_t d_x, size_t d_y, size_t w, size_t h,
   int16_t instance );

MERROR_RETVAL retro3d_texture_create(
   size_t w, size_t h, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags );

void retro3d_texture_destroy( struct RETROFLAT_BITMAP* bmp );

void retro3d_texture_px(
   struct RETROFLAT_BITMAP* target, const RETROFLAT_COLOR color_idx,
   size_t x, size_t y, uint8_t flags );

MERROR_RETVAL retro3d_draw_model( struct RETRO3DP_MODEL* model );

/*! \} */ /* maug_retro3d_util */

/*! \} */ /* maug_retro3d */

#ifdef RETRO3D_C

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

#ifndef RETRO3D_NO_TEXTURES
   bmp->flags &= ~RETROFLAT_FLAGS_LOCK;
#  ifndef RETRO3D_NO_TEXTURE_LISTS
   assert( 0 < bmp->tex.id );
   assert( NULL != bmp->tex.bytes );

   retval = retro3d_texture_platform_refresh( bmp, 0 );
#  endif /* !RETRO3D_NO_TEXTURE_LISTS */

   /* Unlock texture bitmap. */
   maug_munlock( bmp->tex.bytes_h, bmp->tex.bytes );
#endif /* !RETRO3D_NO_TEXTURES */

   return retval;
}

/* === */

MERROR_RETVAL retro3d_texture_load_bitmap(
   const char* filename_path, struct RETROFLAT_BITMAP* bmp_out, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
#ifndef RETRO3D_NO_TEXTURES
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

   retval = retro3d_texture_platform_refresh(
      bmp_out, RETRO3D_TEX_FLAG_GENERATE );

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

#endif /* !RETRO3D_NO_TEXTURES */

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

#ifdef RETRO3D_NO_TEXTURES
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

   retval = retro3d_texture_platform_refresh(
      bmp_out, RETRO3D_TEX_FLAG_GENERATE );

cleanup:
   if( NULL != bmp_out->tex.bytes ) {
      maug_munlock( bmp_out->tex.bytes_h, bmp_out->tex.bytes );
   }

#endif /* !RETRO3D_NO_TEXTURES */

   return retval;
}

/* === */

void retro3d_texture_destroy( struct RETROFLAT_BITMAP* bmp ) {
#ifndef RETRO3D_NO_TEXTURES
   debug_printf( 1, "destroying bitmap..." );

   if( NULL != bmp->tex.bytes ) {
      maug_munlock( bmp->tex.bytes_h, bmp->tex.bytes );
   }
   
   if( NULL != bmp->tex.bytes_h ) {
      maug_mfree( bmp->tex.bytes_h );
   }

   if( 0 < bmp->tex.id ) {
      debug_printf( 0, 
         "destroying bitmap texture: " UPRINTF_U32_FMT, bmp->tex.id );
      retro3d_texture_platform_refresh( bmp, RETRO3D_TEX_FLAG_DESTROY );
   }
#endif /* !RETRO3D_NO_TEXTURES */
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

/* === */

MERROR_RETVAL retro3d_draw_model( struct RETRO3DP_MODEL* model ) {
   MERROR_RETVAL retval = MERROR_OK;
   int i = 0;
   int j = 0;
   struct RETRO3DP_MATERIAL* m = NULL;
   struct RETRO3DP_FACE* f = NULL;
   struct RETRO3DP_VERTEX* v = NULL;

   debug_printf( RETRO3D_TRACE_LVL, "drawing poly..." );

#if 0
   /* Compensate for parsed integer model. */
   glPushMatrix();

   glBegin( GL_TRIANGLES );
#endif

   debug_printf( RETRO3D_TRACE_LVL, "locking faces..." );
   mdata_vector_lock( &(model->faces) );
   debug_printf( RETRO3D_TRACE_LVL, "locking materials..." );
   mdata_vector_lock( &(model->materials) );
   debug_printf( RETRO3D_TRACE_LVL, "locking vertices..." );
   mdata_vector_lock( &(model->vertices) );
   debug_printf( RETRO3D_TRACE_LVL, "locking normals..." );
   mdata_vector_lock( &(model->vnormals) );
   for( i = 0 ; mdata_vector_ct( &(model->faces) ) > i ; i++ ) {
      debug_printf( RETRO3D_TRACE_LVL,
         "getting face %d of " SIZE_T_FMT "...",
         i, mdata_vector_ct( &(model->faces) ) );
      f = mdata_vector_get( &(model->faces), i, struct RETRO3DP_FACE );
      assert( NULL != f );
      if( 0 < mdata_vector_ct( &(model->materials) ) ) {
         debug_printf(
            RETRO3D_TRACE_LVL, "getting material %d of " SIZE_T_FMT "...",
            f->material_idx, mdata_vector_ct( &(model->materials) ) );
         m = mdata_vector_get(
            &(model->materials), f->material_idx, struct RETRO3DP_MATERIAL );
         assert( NULL != m );
      
#if 0
         /* TODO: Handle material on NDS. */
         glMaterialfv( GL_FRONT, GL_DIFFUSE, m->diffuse );
            /*
         glMaterialfv( GL_FRONT, GL_AMBIENT,
            model->materials[faces[i].material_idx].ambient );
            */
         glMaterialfv( GL_FRONT, GL_SPECULAR, m->specular );
         glMaterialfv( GL_FRONT, GL_EMISSION, m->emissive );

         glColor3fv( m->diffuse );
         /* Use a specific macro here that can be overridden for e.g. the NDS.
          */
         glShininessf( GL_FRONT, GL_SHININESS, m->specular_exp );
#endif
      }

      assert( 0 < f->vertex_idxs_sz );
      assert( 3 == f->vertex_idxs_sz );
      retro3d_tri_begin( RETROFLAT_COLOR_NULL, 0 );
      for( j = 0 ; f->vertex_idxs_sz > j ; j++ ) {

         /*
         if( 0 < mdata_vector_ct( &(model->vnormals) ) ) {
            debug_printf( RETRO3D_TRACE_LVL,
               "getting normal %d of " SIZE_T_FMT "...",
               j, mdata_vector_ct( &(model->vnormals) ) );
            v = mdata_vector_get(
               &(model->vnormals), f->vnormal_idxs[j] - 1,
               struct RETRO3DP_VERTEX );
            assert( NULL != v );

            glNormal3i( v->x, v->y, v->z );
         }
         */

         debug_printf( RETRO3D_TRACE_LVL,
            "getting vertex %d of " SIZE_T_FMT "...",
            j, mdata_vector_ct( &(model->vertices) ) );
         v = mdata_vector_get(
            &(model->vertices), f->vertex_idxs[j] - 1, struct RETRO3DP_VERTEX );
         assert( NULL != v );
         /* glVertex3i( v->x, v->y, v->z ); */
      }

      retro3d_tri_end();
   }

   debug_printf( RETRO3D_TRACE_LVL, "drawing complete!" );

cleanup:

   mdata_vector_unlock( &(model->vnormals) );
   mdata_vector_unlock( &(model->faces) );
   mdata_vector_unlock( &(model->materials) );
   mdata_vector_unlock( &(model->vertices) );

   /*
   glEnd();

   glPopMatrix();
   */

   return retval;
}


#endif /* RETRO3D_C */

#endif /* !RETRO3DU_H */

