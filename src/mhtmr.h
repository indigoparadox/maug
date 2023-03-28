
#ifndef MHTMR_H
#define MHTMR_H

#ifndef MHTMR_RENDER_NODES_INIT_SZ
#  define MHTMR_RENDER_NODES_INIT_SZ 10
#endif /* !MHTMR_RENDER_NODES_INIT_SZ */

struct MHTMR_RENDER_NODE {
   ssize_t x;
   ssize_t y;
   size_t w;
   size_t h;
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

#define mhtmr_node( tree, idx ) (&((tree)->nodes[idx]))

#define mhtmr_tree_lock( tree ) \
   if( NULL == (tree)->nodes ) { \
      maug_mlock( (tree)->nodes_h, (tree)->nodes ); \
      maug_cleanup_if_null_alloc( struct MHTMR_RENDER_NODE*, (tree)->nodes ); \
   }

#define mhtmr_tree_unlock( tree ) \
   if( NULL != (tree)->nodes ) { \
      maug_munlock( (tree)->nodes_h, (tree)->nodes ); \
   }

MERROR_RETVAL mhtmr_tree_size(
   struct MHTML_PARSER* parser, struct MHTMR_RENDER_TREE* tree,
   ssize_t parent_style_idx, ssize_t tag_idx, ssize_t node_idx, size_t d );

void mhtmr_tree_draw( struct MHTMR_RENDER_NODE* node, size_t d );

MERROR_RETVAL mhtmr_tree_init( struct MHTMR_RENDER_TREE* tree );

#ifdef MHTMR_C

void mhtml_merge_styles(
   struct MCSS_STYLE* effect_style,
   struct MCSS_STYLE* parent_style,
   struct MCSS_STYLE* tag_style
) {

   mcss_init_style( effect_style );

   /* Merge any styles present as we descend the parsed tree. */

   if( NULL == tag_style && NULL != parent_style ) {
      memcpy( effect_style, parent_style, sizeof( struct MCSS_STYLE ) );
      goto cleanup;

   } else if( NULL != tag_style && NULL == parent_style ) {
      memcpy( effect_style, tag_style, sizeof( struct MCSS_STYLE ) );
      goto cleanup;
 
   } else if( NULL == tag_style && NULL == parent_style ) {
      debug_printf( 1, "root style?" );
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

ssize_t mhtmr_get_next_free_node( struct MHTMR_RENDER_TREE* tree ) {
   uint8_t auto_unlocked = 0;
   ssize_t retidx = -1;
   MAUG_MHANDLE new_nodes_h = (MAUG_MHANDLE)NULL;

   if( NULL != tree->nodes ) {
      debug_printf( 1, "auto-unlocking nodes..." );
      maug_munlock( tree->nodes_h, tree->nodes );
      auto_unlocked = 1;
   }

   assert( 0 < tree->nodes_sz_max );
   assert( NULL == tree->nodes );
   assert( (MAUG_MHANDLE)NULL != tree->nodes_h );
   if( tree->nodes_sz_max <= tree->nodes_sz + 1 ) {
      /* We've run out of nodes, so double the available number. */
      /* TODO: Check for sz overflow. */
      new_nodes_h = maug_mrealloc( tree->nodes_h, tree->nodes_sz_max * 2,
         sizeof( struct MHTMR_RENDER_NODE ) );
      if( (MAUG_MHANDLE)NULL == new_nodes_h ) {
         error_printf(
            "unable to reallocate " SIZE_T_FMT " nodes!",
            tree->nodes_sz_max * 2 );
         goto cleanup;
      }
      tree->nodes_h = new_nodes_h;
      tree->nodes_sz_max *= 2;
   }

   /* Assume handle is unlocked. */
   assert( NULL == tree->nodes );
   maug_mlock( tree->nodes_h, tree->nodes );
   if( NULL == tree->nodes ) {
      error_printf( "unable to lock nodes!" );
      goto cleanup;
   }

   /* Zero out the last node, add it to the list, and return its index. */
   debug_printf( 1, "zeroing node " SIZE_T_FMT " (of " SIZE_T_FMT ")...",
      tree->nodes_sz, tree->nodes_sz_max );
   maug_mzero( &(tree->nodes[tree->nodes_sz]),
      sizeof( struct MHTMR_RENDER_NODE ) );
   retidx = tree->nodes_sz;
   tree->nodes_sz++;

   /* Compensate for cleanup below. */
   maug_munlock( tree->nodes_h, tree->nodes );

cleanup:

   if( auto_unlocked ) {
      debug_printf( 1, "auto-locking nodes..." );
      maug_mlock( tree->nodes_h, tree->nodes );
   }

   return retidx;
}

ssize_t mhtmr_add_node_child(
   struct MHTMR_RENDER_TREE* tree, ssize_t node_parent_idx
) {
   ssize_t node_new_idx = -1,
      node_sibling_idx = -1;

   node_new_idx = mhtmr_get_next_free_node( tree );
   if( 0 > node_new_idx ) {
      goto cleanup;
   }

   mhtmr_node( tree, node_new_idx )->parent = node_parent_idx;
   mhtmr_node( tree, node_new_idx )->first_child = -1;
   mhtmr_node( tree, node_new_idx )->next_sibling = -1;

   if( 0 > node_parent_idx ) {
      debug_printf( 1, "added root node under " SSIZE_T_FMT, node_parent_idx );
      goto cleanup;
   } else {
      debug_printf( 1, "added node under " SSIZE_T_FMT, node_parent_idx );
   }

   /* Add new child under current node. */
   if( 0 > mhtmr_node( tree, node_parent_idx )->first_child ) {
      mhtmr_node( tree, node_parent_idx )->first_child = node_new_idx;
   } else {
      node_sibling_idx = mhtmr_node( tree, 
         mhtmr_node( tree, node_parent_idx )->first_child );
      debug_printf( 1, "sib: " SSIZE_T_FMT, node_sibling_idx );
      while( 0 <= mhtmr_node( tree, node_sibling_idx )->next_sibling ) {
         node_sibling_idx = mhtmr_node( tree, 
            mhtmr_node( tree, node_sibling_idx )->next_sibling );
         debug_printf( 1, "sib: " SSIZE_T_FMT, node_sibling_idx );
      }
      mhtmr_node( tree, node_sibling_idx )->next_sibling = node_new_idx;
   }

cleanup:
   
   return node_new_idx;
}

MERROR_RETVAL mhtmr_tree_size(
   struct MHTML_PARSER* parser, struct MHTMR_RENDER_TREE* tree,
   ssize_t parent_style_idx, ssize_t tag_idx, ssize_t node_idx, size_t d
) {
   struct MCSS_STYLE effect_style;
   char* tag_content = NULL;
   ssize_t node_new_idx = -1;
   ssize_t tag_iter_idx = -1;
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( 1, "tree size d: " SIZE_T_FMT, d );

   if( NULL == mhtml_tag( parser, tag_idx ) ) {
      debug_printf( 1, "return" );
      goto cleanup;
   }

   /* Make sure we have a root style. */
   mhtml_merge_styles(
      &effect_style,
      mcss_style( &(parser->styler), parent_style_idx ),
      mcss_tag_style( parser, tag_idx ) );

   if( MHTML_TAG_TYPE_TEXT == mhtml_tag( parser, tag_idx )->base.type ) {
      /* Get text size to use in calculations below. */

      maug_mlock( mhtml_tag( parser, tag_idx )->TEXT.content, tag_content );
      maug_cleanup_if_null_alloc( char*, tag_content );

      retroflat_string_sz( NULL, tag_content,
         mhtml_tag( parser, tag_idx )->TEXT.content_sz, "",
         &(mhtmr_node( tree, node_idx )->w),
         &(mhtmr_node( tree, node_idx )->h), 0 );

      maug_munlock( mhtml_tag( parser, tag_idx )->TEXT.content, tag_content );

   } else {
      /* Get sizing of child nodes. */

      tag_iter_idx = mhtml_tag( parser, tag_idx )->base.first_child;
      while( 0 <= tag_iter_idx ) {
         node_new_idx = mhtmr_add_node_child( tree, node_idx );
         if( 0 > node_new_idx ) {
            goto cleanup;
         }

         debug_printf( 1,
            "rendering node " SSIZE_T_FMT " under node " SSIZE_T_FMT,
            node_new_idx, node_idx );

         mhtmr_tree_size(
            parser, tree, mhtml_tag( parser, tag_idx )->base.style,
            tag_iter_idx, node_new_idx, d + 1 );
      }

      /*
      mhtml_size_tree(
         parser, r, parent_style, parser->tags[tag_iter].base.next_sibling, d );
      */
      /* TODO: Add padding, margins of children? */

      /* TODO: Loop through children and add sizes. */
   }

   if( 
      MCSS_PROP_FLAG_ACTIVE ==
         (MCSS_PROP_FLAG_ACTIVE & effect_style.WIDTH_flags)
   ) {
      mhtmr_node( tree, node_idx )->w = effect_style.WIDTH;
   }

   if( 
      MCSS_PROP_FLAG_ACTIVE ==
         (MCSS_PROP_FLAG_ACTIVE & effect_style.HEIGHT_flags)
   ) {
      mhtmr_node( tree, node_idx )->h = effect_style.HEIGHT;
   }

   if( 
      MCSS_PROP_FLAG_AUTO ==
         (MCSS_PROP_FLAG_AUTO & effect_style.MARGIN_LEFT_flags) &&
      MCSS_PROP_FLAG_AUTO ==
         (MCSS_PROP_FLAG_AUTO & effect_style.MARGIN_RIGHT_flags)
   ) {
      /* Center */
      /* mhtmr_node( tree, node_idx )->x = (r->w_max / 2) - (r->w / 2); */

   } else if( 
      MCSS_PROP_FLAG_AUTO ==
         (MCSS_PROP_FLAG_AUTO & effect_style.MARGIN_LEFT_flags) &&
      MCSS_PROP_FLAG_AUTO !=
         (MCSS_PROP_FLAG_AUTO & effect_style.MARGIN_RIGHT_flags)
   ) {
      /* Justify right. */
      /* r->x = r->w_max - r->w; */

   } else {
      /* Justify left. */
      /* r->x += effect_style.MARGIN_LEFT; */
   }

cleanup:

   return retval;
}

void mhtmr_tree_draw( struct MHTMR_RENDER_NODE* node, size_t d ) {
   char* tag_content = NULL;
   struct MCSS_STYLE effect_style;

   if( NULL == node ) {
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

MERROR_RETVAL mhtmr_tree_init( struct MHTMR_RENDER_TREE* tree ) {
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

