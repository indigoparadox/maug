
#ifndef MHTMR_H
#define MHTMR_H

#ifndef MHTMR_RENDER_NODES_INIT_SZ
#  define MHTMR_RENDER_NODES_INIT_SZ 10
#endif /* !MHTMR_RENDER_NODES_INIT_SZ */

struct MHTMR_RENDER_NODE {
   ssize_t x;
   ssize_t y;
   ssize_t w;
   ssize_t h;
   ssize_t parent;
   ssize_t first_child;
   ssize_t next_sibling;
};

struct MHTMR_RENDER_TREE {
   MAUG_MHANDLE nodes_h;
   struct MHTMR_RENDER_NODE* nodes;
   size_t nodes_sz;
   size_t nodes_sz_max;
};

#define mhtmr_tree_lock( tree ) \
   if( NULL == (tree)->nodes ) { \
      maug_mlock( (tree)->nodes_h, (tree)->nodes ); \
      maug_cleanup_if_null_alloc( struct MHTMR_RENDER_NODE*, (tree)->nodes ); \
   }

#define mhtmr_tree_unlock( tree ) \
   if( NULL != (tree)->nodes ) { \
      maug_munlock( (tree)->nodes_h, (tree)->nodes ); \
   }

MERROR_RETVAL mhtmr_init_tree( struct MHTMR_RENDER_TREE* tree );

#ifdef MHTMR_C

void mhtml_merge_styles(
   struct MCSS_STYLE* effect_style,
   struct MCSS_STYLE* parent_style,
   struct MCSS_STYLE* tag_style
) {

   /* Merge any styles present as we descend the parsed tree. */
   
   if( NULL == tag_style ) {
      memcpy( effect_style, parent_style, sizeof( struct MCSS_STYLE ) );
      goto cleanup;
   }

   #define MCSS_PROP_TABLE_MERGE( p_id, prop_n, prop_t, prop_p, def ) \
      if( \
         ( \
            /* Parent is important and new property isn't. */ \
            MCSS_PROP_FLAG_IMPORTANT == \
               (MCSS_PROP_FLAG_IMPORTANT & parent_style->prop_n ## _flags) \
            && MCSS_PROP_FLAG_IMPORTANT != \
               (MCSS_PROP_FLAG_IMPORTANT & tag_style->prop_n ## _flags) \
         ) || ( \
            /* New property is not active. */ \
            MCSS_PROP_FLAG_ACTIVE != \
               (MCSS_PROP_FLAG_ACTIVE & tag_style->prop_n ## _flags) \
         ) \
      ) { \
         /* Inherit parent property. */ \
         effect_style->prop_n = parent_style->prop_n; \
         effect_style->prop_n ## _flags = parent_style->prop_n ## _flags; \
      } else { \
         /* Use new property. */ \
         effect_style->prop_n = tag_style->prop_n; \
         effect_style->prop_n ## _flags = tag_style->prop_n ## _flags; \
      }

   MCSS_PROP_TABLE( MCSS_PROP_TABLE_MERGE )

cleanup:

   return;
}

#if 0
void mhtml_size_tree(
   struct MHTML_PARSER* parser, struct MHTML_RENDER* r,
   struct MCSS_STYLE* parent_style, ssize_t iter, size_t d
) {
   int text_w = 0,
      text_h = 0;
   struct MCSS_STYLE effect_style;
   char* tag_content = NULL;

   if( 0 > iter ) {
      return;
   }

   mhtml_merge_styles( &effect_style, parent_style,
      0 <= parser->tags[iter].base.style ?
         &(parser->styler.styles[parser->tags[iter].base.style]) : NULL );

   if( MHTML_TAG_TYPE_TEXT == parser->tags[iter].base.type ) {
      maug_mlock( parser->tags[iter].TEXT.content, tag_content );
      if( NULL == tag_content ) {
         error_printf( "could not lock tag content!" );
         return;
      }

      retroflat_string_sz(
         NULL, tag_content, parser->tags[iter].TEXT.content_sz,
         "", &text_w, &text_h, 0 );

      debug_printf( 1, "ss: %d", text_h );

      maug_munlock( parser->tags[iter].TEXT.content, tag_content );
   }

   /* TODO: Add padding, margins of children? */

   if( 
      MCSS_PROP_FLAG_ACTIVE ==
         (MCSS_PROP_FLAG_ACTIVE & effect_style.WIDTH_flags)
   ) {
      r->w = effect_style.WIDTH;
   } else {
      /* TODO: Dive into element and figure out children auto width. */
   }

   if( 
      MCSS_PROP_FLAG_ACTIVE ==
         (MCSS_PROP_FLAG_ACTIVE & effect_style.HEIGHT_flags)
   ) {
      r->h += effect_style.HEIGHT;
   } else if( 0 < text_h ) {
      /* TODO: Dive into element and figure out children auto height. */
      r->h += text_h;
      debug_printf( 1, SIZE_T_FMT " new h: " SIZE_T_FMT, d, r->h );
   }

   if( 
      MCSS_PROP_FLAG_AUTO ==
         (MCSS_PROP_FLAG_AUTO & effect_style.MARGIN_LEFT_flags) &&
      MCSS_PROP_FLAG_AUTO ==
         (MCSS_PROP_FLAG_AUTO & effect_style.MARGIN_RIGHT_flags)
   ) {
      /* Center */
      r->x = (r->w_max / 2) - (r->w / 2);

   } else if( 
      MCSS_PROP_FLAG_AUTO ==
         (MCSS_PROP_FLAG_AUTO & effect_style.MARGIN_LEFT_flags) &&
      MCSS_PROP_FLAG_AUTO !=
         (MCSS_PROP_FLAG_AUTO & effect_style.MARGIN_RIGHT_flags)
   ) {
      /* Justify right. */
      r->x = r->w_max - r->w;

   } else {
      /* Justify left. */
      r->x += effect_style.MARGIN_LEFT;
   }

   mhtml_size_tree(
      parser, r, &effect_style, parser->tags[iter].base.first_child, d + 1 );

   /*
   mhtml_size_tree(
      parser, r, parent_style, parser->tags[iter].base.next_sibling, d );
   */
}
#endif

void mhtmr_render_tree(
   struct MHTML_PARSER* parser, struct MHTMR_RENDER_TREE* tree,
   struct MCSS_STYLE* parent_style, ssize_t iter, size_t d
) {
   char* tag_content = NULL;
   struct MCSS_STYLE effect_style;

   if( 0 > iter ) {
      return;
   }

#if 0
   memcpy( &r, parent_r, sizeof( struct MHTML_RENDER ) );
   maug_mzero( &effect_style, sizeof( struct MCSS_STYLE ) );

   mhtml_merge_styles( &effect_style, parent_style,
      0 <= parser->tags[iter].base.style ?
         &(parser->styler.styles[parser->tags[iter].base.style]) : NULL );

   /* Perform the actual render. */

   if( MHTML_TAG_TYPE_TEXT == parser->tags[iter].base.type ) {
      maug_mlock( parser->tags[iter].TEXT.content, tag_content );
      if( NULL == tag_content ) {
         error_printf( "could not lock tag content!" );
         return;
      }

      retroflat_string(
         NULL, effect_style.COLOR,
         tag_content, 0, "", r.x, r.y, 0 );

      maug_munlock( parser->tags[iter].TEXT.content, tag_content );

   } else {

      if( 0 <= parser->tags[iter].base.first_child ) {
         /* Fit child elements inside this one. */
         mhtml_size_tree(
            parser, &r, &effect_style,
            parser->tags[iter].base.first_child, d + 1 );
      }

      if( RETROFLAT_COLOR_NULL != effect_style.BACKGROUND_COLOR ) {
         retroflat_rect(
            NULL, effect_style.BACKGROUND_COLOR, r.x, r.y, r.w, r.h,
            RETROFLAT_FLAGS_FILL );
      }
   }

   memcpy( &r, parent_r, sizeof( struct MHTML_RENDER ) );

   /* Descend into children/siblings. */
   r.y += effect_style.PADDING_TOP;
   r.x += effect_style.PADDING_LEFT;

   mhtml_render_tree(
      parser, &r, &effect_style, parser->tags[iter].base.first_child, d + 1 );

   /* TODO: Determine if block or inline. */
   memcpy( &r, parent_r, sizeof( struct MHTML_RENDER ) );
   r.y += effect_style.HEIGHT;

   mhtml_render_tree(
      parser, &r, parent_style, parser->tags[iter].base.next_sibling, d );
#endif
}

MERROR_RETVAL mhtmr_init_tree( struct MHTMR_RENDER_TREE* tree ) {
   MERROR_RETVAL retval = MERROR_OK;

   maug_mzero( tree, sizeof( struct MHTMR_RENDER_TREE ) );

   /* Perform initial node allocation. */
   tree->nodes_sz_max = MHTML_PARSER_TAGS_INIT_SZ;
   debug_printf( 1, "allocating " SIZE_T_FMT " nodes...", tree->nodes_sz_max );
   tree->nodes_h = maug_malloc(
      tree->nodes_sz_max, sizeof( struct MHTMR_RENDER_NODE ) );
   maug_cleanup_if_null_alloc( struct MHTMR_RENDER_NODE*, tree->nodes_h );

   /* XXX
   r.w_max = retroflat_screen_w();
   r.h_max = retroflat_screen_h(); */

cleanup:

   return retval;  
}

#endif /* MHTMR_C */

#endif /* !MHTMR_H */

