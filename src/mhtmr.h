
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
   RETROFLAT_COLOR bg;
   RETROFLAT_COLOR fg;
   ssize_t tag;
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

#define mhtmr_node( tree, idx ) (0 <= idx ? &((tree)->nodes[idx]) : NULL)

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
   ssize_t parent_style_idx, ssize_t node_idx, size_t d );

MERROR_RETVAL mhtmr_tree_pos(
   struct MHTML_PARSER* parser, struct MHTMR_RENDER_TREE* tree,
   ssize_t parent_style_idx, ssize_t node_idx, size_t d );

void mhtmr_tree_draw(
   struct MHTML_PARSER* parser, struct MHTMR_RENDER_TREE* tree,
   ssize_t node_idx, size_t d );

void mhtmr_tree_dump(
   struct MHTMR_RENDER_TREE* tree, struct MHTML_PARSER* parser,
   ssize_t iter, size_t d );

void mhtmr_tree_free( struct MHTMR_RENDER_TREE* tree );

MERROR_RETVAL mhtmr_tree_init( struct MHTMR_RENDER_TREE* tree );

#ifdef MHTMR_C

void mhtml_merge_styles(
   struct MCSS_STYLE* effect_style,
   struct MCSS_STYLE* parent_style,
   struct MCSS_STYLE* tag_style,
   size_t tag_type
) {

   mcss_style_init( effect_style );

   /* Merge any styles present as we descend the parsed tree. */

   if( NULL == tag_style && NULL != parent_style ) {
      memcpy( effect_style, parent_style, sizeof( struct MCSS_STYLE ) );
      goto block_defs;

   } else if( NULL != tag_style && NULL == parent_style ) {
      memcpy( effect_style, tag_style, sizeof( struct MCSS_STYLE ) );
      goto block_defs;
 
   } else if( NULL == tag_style && NULL == parent_style ) {
      debug_printf( 1, "root style?" );
      goto block_defs;
   }

   #define MCSS_PROP_TABLE_MERGE( p_id, prop_n, prop_t, prop_p, def ) \
      if( \
         /* Special cases: do not inherit! */ \
         MCSS_PROP_DISPLAY != p_id && \
         MCSS_PROP_HEIGHT != p_id && \
         MCSS_PROP_WIDTH != p_id && ( \
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
         )) \
      ) { \
         /* Inherit parent property. */ \
         debug_printf( 1, "%s using parent %s", \
            gc_mhtml_tag_names[tag_type], #prop_n ); \
         effect_style->prop_n = parent_style->prop_n; \
         effect_style->prop_n ## _flags = parent_style->prop_n ## _flags; \
      } else { \
         /* Use new property. */ \
         debug_printf( 1, "%s using style %s", \
            gc_mhtml_tag_names[tag_type], #prop_n ); \
         effect_style->prop_n = tag_style->prop_n; \
         effect_style->prop_n ## _flags = tag_style->prop_n ## _flags; \
      }

   MCSS_PROP_TABLE( MCSS_PROP_TABLE_MERGE )

block_defs:

   if(
      MCSS_PROP_FLAG_ACTIVE !=
      (MCSS_PROP_FLAG_ACTIVE & effect_style->DISPLAY_flags)
   ) {
      /* Set the display property based on the tag's default. */

      #define MHTML_TAG_TABLE_DISP( tag_id, tag_name, fields, disp ) \
         } else if( tag_id == tag_type ) { \
            effect_style->DISPLAY = MCSS_DISPLAY_ ## disp; \
            debug_printf( 1, "%s defaulting to %s DISPLAY", \
               gc_mhtml_tag_names[tag_type], \
               gc_mcss_display_names[effect_style->DISPLAY] );

      if( 0 ) {
      MHTML_TAG_TABLE( MHTML_TAG_TABLE_DISP )
      }

   }

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
      debug_printf(
         1, "adding root node under " SSIZE_T_FMT "...", node_parent_idx );
      goto cleanup;
   } else {
      debug_printf(
         1, "adding node " SSIZE_T_FMT " under " SSIZE_T_FMT,
         node_new_idx, node_parent_idx );
   }

   /* Add new child under current node. */
   if( 0 > mhtmr_node( tree, node_parent_idx )->first_child ) {
      debug_printf( 1, "adding first child..." );
      assert( -1 == mhtmr_node( tree, node_parent_idx )->first_child );
      mhtmr_node( tree, node_parent_idx )->first_child = node_new_idx;
   } else {
      assert( NULL != mhtmr_node( tree, node_parent_idx ) );
      node_sibling_idx = mhtmr_node( tree, node_parent_idx )->first_child;
      assert( NULL != mhtmr_node( tree, node_sibling_idx ) );
      while( 0 <= mhtmr_node( tree, node_sibling_idx )->next_sibling ) {
         node_sibling_idx = 
            mhtmr_node( tree, node_sibling_idx )->next_sibling;
      }
      mhtmr_node( tree, node_sibling_idx )->next_sibling = node_new_idx;
   }

cleanup:
   
   return node_new_idx;
}

MERROR_RETVAL mhtmr_tree_create(
   struct MHTML_PARSER* parser, struct MHTMR_RENDER_TREE* tree,
   ssize_t parent_style_idx, ssize_t tag_idx, ssize_t node_idx, size_t d
) {
   ssize_t node_new_idx = -1;
   ssize_t tag_iter_idx = -1;
   MERROR_RETVAL retval = MERROR_OK;

   if( NULL == mhtml_tag( parser, tag_idx ) ) {
      debug_printf( 1, "return" );
      goto cleanup;
   }

   /* Make sure we have a single root node. */
   if( 0 > node_idx ) {
      node_new_idx = mhtmr_add_node_child( tree, node_idx );
      if( 0 > node_new_idx ) {
         goto cleanup;
      }
      debug_printf( 1, "created initial root node: " SIZE_T_FMT, node_new_idx );

      node_idx = node_new_idx;

      /* The common root doesn't belong to any specific tag. */
      mhtmr_node( tree, node_idx )->tag = -1;

      mhtmr_node( tree, node_idx )->w = retroflat_screen_w();
   }

   tag_iter_idx = mhtml_tag( parser, tag_idx )->base.first_child;
   while( 0 <= tag_iter_idx ) {
      node_new_idx = mhtmr_add_node_child( tree, node_idx );
      if( 0 > node_new_idx ) {
         goto cleanup;
      }

      mhtmr_node( tree, node_new_idx )->tag = tag_iter_idx;

      debug_printf( 1,
         "rendering node " SSIZE_T_FMT " under node " SSIZE_T_FMT,
         node_new_idx, node_idx );

      mhtmr_tree_create(
         parser, tree, mhtml_tag( parser, tag_idx )->base.style,
         tag_iter_idx, node_new_idx, d + 1 );

      tag_iter_idx = mhtml_tag( parser, tag_iter_idx )->base.next_sibling;
   }

cleanup:

   return retval;
}

MERROR_RETVAL mhtmr_tree_size(
   struct MHTML_PARSER* parser, struct MHTMR_RENDER_TREE* tree,
   ssize_t parent_style_idx, ssize_t node_idx, size_t d
) {
   struct MCSS_STYLE effect_style;
   char* tag_content = NULL;
   ssize_t child_iter_idx = -1;
   ssize_t tag_idx = -1;
   ssize_t tag_style_idx = -1;
   ssize_t node_iter_idx = -1;
   size_t tag_type = 0;
   MERROR_RETVAL retval = MERROR_OK;

   if( NULL == mhtmr_node( tree, node_idx ) ) {
      goto cleanup;
   }

   tag_idx = mhtmr_node( tree, node_idx )->tag;

   if( 0 <= tag_idx ) {
      tag_style_idx = mhtml_tag( parser, tag_idx )->base.style;
      tag_type = mhtml_tag( parser, tag_idx )->base.type;
   }

   /* Make sure we have a root style. */
   mhtml_merge_styles(
      &effect_style,
      mcss_style( &(parser->styler), parent_style_idx ),
      0 <= tag_style_idx ? &(parser->styler.styles[tag_style_idx]) : NULL,
      tag_type );
   
   if(
      0 <= tag_idx &&
      MHTML_TAG_TYPE_TEXT == mhtml_tag( parser, tag_idx )->base.type
   ) {
      /* Get text size to use in calculations below. */

      maug_mlock( mhtml_tag( parser, tag_idx )->TEXT.content, tag_content );
      maug_cleanup_if_null_alloc( char*, tag_content );

      retroflat_string_sz( NULL, tag_content,
         mhtml_tag( parser, tag_idx )->TEXT.content_sz, "",
         &(mhtmr_node( tree, node_idx )->w),
         &(mhtmr_node( tree, node_idx )->h), 0 );

      debug_printf( 1, "TEXT w: " SIZE_T_FMT, 
         mhtmr_node( tree, node_idx )->w );

      maug_munlock( mhtml_tag( parser, tag_idx )->TEXT.content, tag_content );

   } else {
      /* Get sizing of child nodes. */

      node_iter_idx = mhtmr_node( tree, node_idx )->first_child;
      while( 0 <= node_iter_idx ) {
         mhtmr_tree_size( parser, tree, tag_style_idx, node_iter_idx, d + 1 );

         node_iter_idx = mhtmr_node( tree, node_iter_idx )->next_sibling;
      }
   }

   /* width */

   if( 
      MCSS_PROP_FLAG_ACTIVE ==
         (MCSS_PROP_FLAG_ACTIVE & effect_style.WIDTH_flags)
   ) {
      mhtmr_node( tree, node_idx )->w = effect_style.WIDTH;
   } else {
      if( MCSS_DISPLAY_INLINE == effect_style.DISPLAY ) {
         debug_printf( 1, "%s: INLINE", gc_mhtml_tag_names[tag_type] );
         /* Cycle through children and use greatest width. */
         child_iter_idx = mhtmr_node( tree, node_idx )->first_child;
         while( 0 <= child_iter_idx ) {
            if(
               mhtmr_node( tree, child_iter_idx )->w >
                  mhtmr_node( tree, node_idx )->w
            ) {
               mhtmr_node( tree, node_idx )->w =
                  mhtmr_node( tree, child_iter_idx )->w;
            }
            child_iter_idx = mhtmr_node( tree, child_iter_idx )->next_sibling;
         }
      } else if( 0 <= mhtmr_node( tree, node_idx )->parent ) {
         debug_printf( 1, "%s: BLOCK", gc_mhtml_tag_names[tag_type] );
         /* Use parent width. */
         mhtmr_node( tree, node_idx )->w =
            mhtmr_node( tree, mhtmr_node( tree, node_idx )->parent )->w;
      }
   }

   /* height */

   if( 
      MCSS_PROP_FLAG_ACTIVE ==
         (MCSS_PROP_FLAG_ACTIVE & effect_style.HEIGHT_flags)
   ) {
      mhtmr_node( tree, node_idx )->h = effect_style.HEIGHT;
   } else {
      /* Cycle through children and add heights. */
      child_iter_idx = mhtmr_node( tree, node_idx )->first_child;
      while( 0 <= child_iter_idx ) {
         mhtmr_node( tree, node_idx )->h +=
            mhtmr_node( tree, child_iter_idx )->h;

         child_iter_idx = mhtmr_node( tree, child_iter_idx )->next_sibling;
      }
   }

cleanup:

   return retval;
}

MERROR_RETVAL mhtmr_tree_pos(
   struct MHTML_PARSER* parser, struct MHTMR_RENDER_TREE* tree,
   ssize_t parent_style_idx, ssize_t node_idx, size_t d
) {
   struct MCSS_STYLE effect_style;
   ssize_t child_iter_idx = -1;
   ssize_t tag_idx = -1;
   ssize_t tag_style_idx = -1;
   ssize_t node_iter_idx = -1;
   size_t tag_type = 0;
   MERROR_RETVAL retval = MERROR_OK;

   if( NULL == mhtmr_node( tree, node_idx ) ) {
      goto cleanup;
   }

   tag_idx = mhtmr_node( tree, node_idx )->tag;

   if( 0 <= tag_idx ) {
      tag_style_idx = mhtml_tag( parser, tag_idx )->base.style;
      tag_type = mhtml_tag( parser, tag_idx )->base.type;
   }

   /* Make sure we have a root style. */
   mhtml_merge_styles(
      &effect_style,
      mcss_style( &(parser->styler), parent_style_idx ),
      0 <= tag_style_idx ? &(parser->styler.styles[tag_style_idx]) : NULL,
      tag_type );
   
   /* y */

   /* TODO: Add margins of children? */

   child_iter_idx = mhtmr_node( tree, node_idx )->parent;
   if( 0 <= child_iter_idx ) {
      /* Add top offset of parent. */
      mhtmr_node( tree, node_idx )->y +=
         mhtmr_node( tree, child_iter_idx )->y;

      child_iter_idx = mhtmr_node( tree, child_iter_idx )->first_child;
   }
   while( 0 <= child_iter_idx && node_idx != child_iter_idx ) {
      /* Add top offset of prior siblings. */
      mhtmr_node( tree, node_idx )->y +=
         mhtmr_node( tree, child_iter_idx )->h;

      child_iter_idx = mhtmr_node( tree, child_iter_idx )->next_sibling;
   }

   /* margin-left, margin-right */

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

   /* color */

   if( 
      MCSS_PROP_FLAG_ACTIVE ==
         (MCSS_PROP_FLAG_ACTIVE & effect_style.COLOR_flags)
   ) {
      mhtmr_node( tree, node_idx )->fg = effect_style.COLOR;
   }

   if( 
      MCSS_PROP_FLAG_ACTIVE ==
         (MCSS_PROP_FLAG_ACTIVE & effect_style.BACKGROUND_COLOR_flags)
   ) {
      mhtmr_node( tree, node_idx )->bg = effect_style.BACKGROUND_COLOR;
   }

   node_iter_idx = mhtmr_node( tree, node_idx )->first_child;
   while( 0 <= node_iter_idx ) {
      mhtmr_tree_pos( parser, tree, tag_style_idx, node_iter_idx, d + 1 );

      node_iter_idx = mhtmr_node( tree, node_iter_idx )->next_sibling;
   }
 
cleanup:

   return retval;
}

void mhtmr_tree_draw(
   struct MHTML_PARSER* parser, struct MHTMR_RENDER_TREE* tree,
   ssize_t node_idx, size_t d
) {
   char* tag_content = NULL;
   union MHTML_TAG* tag = NULL;
   struct MHTMR_RENDER_NODE* node = NULL;

   node = mhtmr_node( tree, node_idx );

   if( NULL == node ) {
      return;
   }

   if( 0 <= node->tag ) {
      /* Perform drawing. */
      tag = mhtml_tag( parser, node->tag );

      if( MHTML_TAG_TYPE_TEXT == tag->base.type ) {
         maug_mlock( tag->TEXT.content, tag_content );
         if( NULL == tag_content ) {
            error_printf( "could not lock tag content!" );
            return;
         }

         retroflat_string(
            NULL, node->fg,
            tag_content, 0, "", node->x, node->y, 0 );

         maug_munlock( tag->TEXT.content, tag_content );

      } else {
         if( RETROFLAT_COLOR_NULL != node->bg ) {
            retroflat_rect(
               NULL, node->bg, node->x, node->y, node->w, node->h,
               RETROFLAT_FLAGS_FILL );
         }
      }
   }

   mhtmr_tree_draw( parser, tree, node->first_child, d + 1 );

   mhtmr_tree_draw( parser, tree, node->next_sibling, d );

#if 0
   /* Perform the actual render. */


   memcpy( &r, parent_r, sizeof( struct MHTML_RENDER ) );

   /* Descend into children/siblings. */
   r.y += effect_style.PADDING_TOP;
   r.x += effect_style.PADDING_LEFT;

   #endif
}

void mhtmr_tree_dump(
   struct MHTMR_RENDER_TREE* tree, struct MHTML_PARSER* parser,
   ssize_t iter, size_t d
) {
   size_t i = 0;

   if( 0 > iter ) {
      return;
   }

   for( i = 0 ; d > i ; i++ ) {
      printf( "   " );
   }

   printf(
      SSIZE_T_FMT " (tag %s): x: " SSIZE_T_FMT ", y: " SSIZE_T_FMT
      " (" SSIZE_T_FMT " x " SSIZE_T_FMT ")",
      iter,
      0 <= tree->nodes[iter].tag ?
         gc_mhtml_tag_names[parser->tags[tree->nodes[iter].tag].base.type] : 
            "ROOT",
      tree->nodes[iter].x, tree->nodes[iter].y,
      tree->nodes[iter].w, tree->nodes[iter].h );

   printf( "\n" );

   mhtmr_tree_dump( tree, parser, tree->nodes[iter].first_child, d + 1 );

   mhtmr_tree_dump( tree, parser, tree->nodes[iter].next_sibling, d );
}

void mhtmr_tree_free( struct MHTMR_RENDER_TREE* tree ) {

   debug_printf( 1, "freeing render nodes..." );

   mhtmr_tree_unlock( tree );

   if( NULL != tree->nodes_h ) {
      maug_mfree( tree->nodes_h );
   }
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

