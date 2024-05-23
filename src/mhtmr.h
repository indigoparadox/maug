
#ifndef MHTMR_H
#define MHTMR_H

#ifndef MHTMR_RENDER_NODES_INIT_SZ
#  define MHTMR_RENDER_NODES_INIT_SZ 10
#endif /* !MHTMR_RENDER_NODES_INIT_SZ */

#ifndef MHTMR_TRACE_LVL
#  define MHTMR_TRACE_LVL 0
#endif /* !MHTMR_TRACE_LVL */

struct MHTMR_RENDER_NODE {
   /* TODO: Maybe get rid of these and replace them with MCSS_STYLE node? */
   ssize_t x;
   ssize_t y;
   size_t w;
   size_t h;
   size_t m_l;
   size_t m_r;
   size_t m_t;
   size_t m_b;
   uint8_t pos;
   uint8_t pos_flags;
   RETROFLAT_COLOR bg;
   RETROFLAT_COLOR fg;
   ssize_t tag;
   /*! \brief Index of container's render node in ::MHTMR_RENDER_TREE. */
   ssize_t parent;
   /*! \brief Index of first child's render node in ::MHTMR_RENDER_TREE. */
   ssize_t first_child;
   /*! \brief Index of next sibling's render node in ::MHTMR_RENDER_TREE. */
   ssize_t next_sibling;
   struct RETROFLAT_BITMAP bitmap;
};

struct MHTMR_RENDER_TREE {
   MAUG_MHANDLE nodes_h;
   /*! \brief Locked pointer to nodes when locked with mhtmr_tree_lock(). */
   struct MHTMR_RENDER_NODE* nodes;
   /*! \brief Current active number of nodes in MHTMR_RENDER_NODE::nodes_h. */
   size_t nodes_sz;
   /*! \brief Current alloc'd number of nodes in MHTMR_RENDER_NODE::nodes_h. */
   size_t nodes_sz_max;
};

/* TODO: Function names should be verb_noun! */

#define mhtmr_node( tree, idx ) (0 <= idx ? &((tree)->nodes[idx]) : NULL)

#define mhtmr_node_parent( tree, idx ) \
   (0 <= (tree)->nodes[idx].parent ? \
      &((tree)->nodes[(tree)->nodes[idx].parent]) : NULL)

#define mhtmr_tree_lock( tree ) \
   if( NULL == (tree)->nodes ) { \
      maug_mlock( (tree)->nodes_h, (tree)->nodes ); \
      maug_cleanup_if_null_alloc( struct MHTMR_RENDER_NODE*, (tree)->nodes ); \
   }

#define mhtmr_tree_unlock( tree ) \
   if( NULL != (tree)->nodes ) { \
      maug_munlock( (tree)->nodes_h, (tree)->nodes ); \
   }

#define mhtmr_tree_is_locked( tree ) (NULL != (tree)->nodes)

MERROR_RETVAL mhtmr_tree_create(
   struct MHTML_PARSER* parser, struct MHTMR_RENDER_TREE* tree,
   size_t x, size_t y, size_t w, size_t h,
   ssize_t tag_idx, ssize_t node_idx, size_t d );

/**
 * \brief Create a style node that is a composite of a parent style and the
 *        styles applicable to the classes/IDs of a tag and its immediate
 *        styles.
 * \param tag_idx Index of a tag in the parser tree to derive styles from.
 * \param parent_style Locked pointer to an MCSS_STYLE for the parent node
 *                     to build on.
 * \param effect_style Locked pointer to an MCSS_STYLE to clear and replace
 *                     with the combined style information from the parent and
 *                     indexed tag.
 */
MERROR_RETVAL mhtmr_apply_styles(
   struct MHTML_PARSER* parser, struct MHTMR_RENDER_TREE* tree,
   struct MCSS_STYLE* parent_style, struct MCSS_STYLE* effect_style,
   ssize_t tag_idx );

MERROR_RETVAL mhtmr_tree_size(
   struct MHTML_PARSER* parser, struct MHTMR_RENDER_TREE* tree,
   struct MCSS_STYLE* prev_sibling_style,
   struct MCSS_STYLE* parent_style, ssize_t node_idx, size_t d );

MERROR_RETVAL mhtmr_tree_pos(
   struct MHTML_PARSER* parser, struct MHTMR_RENDER_TREE* tree,
   struct MCSS_STYLE* prev_sibling_style,
   struct MCSS_STYLE* parent_style, ssize_t node_idx, size_t d );

void mhtmr_tree_draw(
   struct MHTML_PARSER* parser, struct MHTMR_RENDER_TREE* tree,
   ssize_t node_idx, size_t d );

void mhtmr_tree_dump(
   struct MHTMR_RENDER_TREE* tree, struct MHTML_PARSER* parser,
   ssize_t iter, size_t d );

void mhtmr_tree_free( struct MHTMR_RENDER_TREE* tree );

MERROR_RETVAL mhtmr_tree_init( struct MHTMR_RENDER_TREE* tree );

#ifdef MHTMR_C

static void mhtml_merge_styles(
   struct MCSS_STYLE* effect_style,
   struct MCSS_STYLE* parent_style,
   struct MCSS_STYLE* tag_style,
   size_t tag_type
) {

   if(
      MCSS_STYLE_FLAG_ACTIVE !=
      (MCSS_STYLE_FLAG_ACTIVE & effect_style->flags)
   ) {
      mcss_style_init( effect_style );
   }

   /* Perform inheritence of special cases. */

   #define MCSS_PROP_TABLE_MERGE( p_id, prop_n, prop_t, prop_p, def ) \
      if( \
         ( \
            /* Only do inheritence for some special cases.
             * e.g. We don't want to inherit width/height/X/etc! */ \
            mcss_prop_is_heritable( p_id ) \
         ) && (NULL != parent_style && ( \
            ( \
               NULL != tag_style && \
               /* Parent is important and new property isn't. */ \
               mcss_prop_is_active_flag( parent_style->prop_n, IMPORTANT ) && \
               /* TODO: Is not active OR important? */ \
               !mcss_prop_is_important( tag_style->prop_n ) && \
               !mcss_prop_is_important( effect_style->prop_n ) \
            ) || ( \
               NULL != tag_style && \
               /* New property is not active. */ \
               !mcss_prop_is_active( tag_style->prop_n ) && \
               !mcss_prop_is_active( effect_style->prop_n ) \
            ) || (\
               /* No competition. */ \
               NULL == tag_style && \
               !mcss_prop_is_active( effect_style->prop_n ) \
            ) \
         )) \
      ) { \
         /* Inherit parent property. */ \
         if( MCSS_PROP_BACKGROUND_COLOR == p_id ) { \
            debug_printf( MHTMR_TRACE_LVL, "background color was %s", \
               0 <= effect_style->prop_n ? \
               gc_retroflat_color_names[effect_style->prop_n] : "NULL" ); \
         } else if( MCSS_PROP_COLOR == p_id ) { \
            debug_printf( MHTMR_TRACE_LVL, "color was %s", \
               0 <= effect_style->prop_n ? \
               gc_retroflat_color_names[effect_style->prop_n] : "NULL" ); \
         } \
         debug_printf( MHTMR_TRACE_LVL, "%s using parent %s", \
            gc_mhtml_tag_names[tag_type], #prop_n ); \
         effect_style->prop_n = parent_style->prop_n; \
         effect_style->prop_n ## _flags = parent_style->prop_n ## _flags; \
         if( MCSS_PROP_BACKGROUND_COLOR == p_id ) { \
            debug_printf( MHTMR_TRACE_LVL, "background color %s", \
               0 <= effect_style->prop_n ? \
               gc_retroflat_color_names[effect_style->prop_n] : "NULL" ); \
         } else if( MCSS_PROP_COLOR == p_id ) { \
            debug_printf( MHTMR_TRACE_LVL, "color %s", \
               0 <= effect_style->prop_n ? \
               gc_retroflat_color_names[effect_style->prop_n] : "NULL" ); \
         } \
      } else if( \
         NULL != tag_style && \
         mcss_prop_is_active( tag_style->prop_n ) \
      ) { \
         /* Use new property. */ \
         debug_printf( MHTMR_TRACE_LVL, "%s using style %s", \
            gc_mhtml_tag_names[tag_type], #prop_n ); \
         if( MCSS_PROP_COLOR == p_id ) { \
            debug_printf( MHTMR_TRACE_LVL, "color %s", \
               0 <= effect_style->prop_n ? \
               gc_retroflat_color_names[effect_style->prop_n] : "NULL" ); \
         } \
         effect_style->prop_n = tag_style->prop_n; \
         effect_style->prop_n ## _flags = tag_style->prop_n ## _flags; \
      }

   MCSS_PROP_TABLE( MCSS_PROP_TABLE_MERGE )

   /* Apply defaults for display. */

   if(
      MCSS_PROP_FLAG_ACTIVE !=
      (MCSS_PROP_FLAG_ACTIVE & effect_style->DISPLAY_flags)
   ) {
      /* Set the display property based on the tag's default. */

      #define MHTML_TAG_TABLE_DISP( tag_id, tag_name, fields, disp ) \
         } else if( tag_id == tag_type ) { \
            effect_style->DISPLAY = MCSS_DISPLAY_ ## disp; \
            debug_printf( MHTMR_TRACE_LVL, "%s defaulting to %s DISPLAY", \
               gc_mhtml_tag_names[tag_type], \
               gc_mcss_display_names[effect_style->DISPLAY] );

      if( 0 ) {
      MHTML_TAG_TABLE( MHTML_TAG_TABLE_DISP )
      }

   }

   return;
}

ssize_t mhtmr_get_next_free_node( struct MHTMR_RENDER_TREE* tree ) {
   uint8_t auto_unlocked = 0;
   ssize_t retidx = -1;
   MAUG_MHANDLE new_nodes_h = (MAUG_MHANDLE)NULL;

   if( NULL != tree->nodes ) {
      debug_printf( MHTMR_TRACE_LVL, "auto-unlocking nodes..." );
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
   debug_printf( MHTMR_TRACE_LVL,
      "zeroing node " SIZE_T_FMT " (of " SIZE_T_FMT ")...",
      tree->nodes_sz, tree->nodes_sz_max );
   maug_mzero( &(tree->nodes[tree->nodes_sz]),
      sizeof( struct MHTMR_RENDER_NODE ) );
   retidx = tree->nodes_sz;
   tree->nodes_sz++;

   /* Compensate for cleanup below. */
   maug_munlock( tree->nodes_h, tree->nodes );

cleanup:

   if( auto_unlocked ) {
      debug_printf( MHTMR_TRACE_LVL, "auto-locking nodes..." );
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
      debug_printf( MHTMR_TRACE_LVL, "adding first child..." );
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
   size_t x, size_t y, size_t w, size_t h,
   ssize_t tag_idx, ssize_t node_idx, size_t d
) {
   ssize_t node_new_idx = -1;
   ssize_t tag_iter_idx = -1;
   MERROR_RETVAL retval = MERROR_OK;

   if( NULL == mhtml_tag( parser, tag_idx ) ) {
      goto cleanup;
   }

   /* Make sure we have a single root node. */
   if( 0 > node_idx ) {
      assert( MHTML_TAG_TYPE_BODY == mhtml_tag( parser, tag_idx )->base.type );

      node_new_idx = mhtmr_add_node_child( tree, node_idx );
      if( 0 > node_new_idx ) {
         goto cleanup;
      }
      debug_printf( MHTMR_TRACE_LVL,
         "created initial root node: " SIZE_T_FMT, node_new_idx );

      node_idx = node_new_idx;

      /* The common root is the body tag. */
      mhtmr_node( tree, node_idx )->tag = tag_idx;

      mhtmr_node( tree, node_idx )->x = x;
      mhtmr_node( tree, node_idx )->y = y;
      mhtmr_node( tree, node_idx )->w = w;
      mhtmr_node( tree, node_idx )->h = h;
   }

   tag_iter_idx = mhtml_tag( parser, tag_idx )->base.first_child;
   while( 0 <= tag_iter_idx ) {
      node_new_idx = mhtmr_add_node_child( tree, node_idx );
      if( 0 > node_new_idx ) {
         goto cleanup;
      }

      mhtmr_node( tree, node_new_idx )->tag = tag_iter_idx;

      debug_printf( MHTMR_TRACE_LVL,
         "rendering node " SSIZE_T_FMT " (%s) under node " SSIZE_T_FMT,
         node_new_idx,
         gc_mhtml_tag_names[mhtml_tag( parser, tag_iter_idx )->base.type],
         node_idx );

      /* Tag-specific rendering preparations. */
      if( MHTML_TAG_TYPE_IMG == mhtml_tag( parser, tag_iter_idx )->base.type ) {
         /* Load the image for rendering later. */
         retval = retroflat_load_bitmap(
            mhtml_tag( parser, tag_iter_idx )->IMG.src,
            &(mhtmr_node( tree, node_new_idx )->bitmap),
            RETROFLAT_FLAGS_LITERAL_PATH );
         if( MERROR_OK == retval ) {
            debug_printf( MHTMR_TRACE_LVL, "loaded img: %s", 
               mhtml_tag( parser, tag_iter_idx )->IMG.src );
         } else {
            error_printf( "could not load img: %s",
               mhtml_tag( parser, tag_iter_idx )->IMG.src );
         }
      }

      mhtmr_tree_create( parser, tree, x, y, w, h,
         tag_iter_idx, node_new_idx, d + 1 );

      tag_iter_idx = mhtml_tag( parser, tag_iter_idx )->base.next_sibling;
   }

cleanup:

   return retval;
}

MERROR_RETVAL mhtmr_apply_styles(
   struct MHTML_PARSER* parser, struct MHTMR_RENDER_TREE* tree,
   struct MCSS_STYLE* parent_style, struct MCSS_STYLE* effect_style,
   ssize_t tag_idx
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t tag_style_idx = -1;
   size_t tag_type = 0,
      i = 0;

   maug_mzero( effect_style, sizeof( struct MCSS_STYLE ) );

   if( 0 >= tag_idx ) {
      goto cleanup;
   }

   tag_type = mhtml_tag( parser, tag_idx )->base.type;

   /* Try to apply class/ID styles. */
   if( 0 < mhtml_tag( parser, tag_idx )->base.classes_sz ) {
      for( i = 0 ; parser->styler.styles_sz > i ; i++ ) {
         if(
            0 == strncmp(
               mhtml_tag( parser, tag_idx )->base.classes,
               parser->styler.styles[i].class,
               mhtml_tag( parser, tag_idx )->base.classes_sz
            )
         ) {
            debug_printf( MHTMR_TRACE_LVL, "found style for tag class: %s",
               parser->styler.styles[i].class );

            mhtml_merge_styles(
               effect_style,
               parent_style,
               &(parser->styler.styles[i]),
               tag_type );
         }
      }
   }
   if( 0 < mhtml_tag( parser, tag_idx )->base.id_sz ) {
      for( i = 0 ; parser->styler.styles_sz > i ; i++ ) {
         if(
            0 == strncmp(
               mhtml_tag( parser, tag_idx )->base.id,
               parser->styler.styles[i].id,
               mhtml_tag( parser, tag_idx )->base.id_sz
            )
         ) {
            debug_printf( MHTMR_TRACE_LVL, "found style for tag ID: %s",
               parser->styler.styles[i].id );

            mhtml_merge_styles(
               effect_style,
               parent_style,
               &(parser->styler.styles[i]),
               tag_type );
         }
      }
   }

   /* Grab element-specific style last. */
   tag_style_idx = mhtml_tag( parser, tag_idx )->base.style;

cleanup:

   /* Make sure we have a root style. */
   mhtml_merge_styles(
      effect_style,
      parent_style,
      0 <= tag_style_idx ? &(parser->styler.styles[tag_style_idx]) : NULL,
      tag_type );

   return retval;
}

MERROR_RETVAL mhtmr_tree_size(
   struct MHTML_PARSER* parser, struct MHTMR_RENDER_TREE* tree,
   struct MCSS_STYLE* prev_sibling_style,
   struct MCSS_STYLE* parent_style, ssize_t node_idx, size_t d
) {
   struct MCSS_STYLE effect_style;
   struct MCSS_STYLE child_prev_sibling_style;
   struct MCSS_STYLE child_style;
   char* tag_content = NULL;
   ssize_t child_iter_idx = -1;
   ssize_t tag_idx = -1;
   ssize_t node_iter_idx = -1;
   size_t this_line_w = 0;
   size_t this_line_h = 0;
   MERROR_RETVAL retval = MERROR_OK;

   if( NULL == mhtmr_node( tree, node_idx ) ) {
      goto cleanup;
   }

   tag_idx = mhtmr_node( tree, node_idx )->tag;

   mhtmr_apply_styles(
      parser, tree, parent_style, &effect_style, tag_idx );

   if( mcss_prop_is_active( effect_style.POSITION ) ) {
      debug_printf( MHTMR_TRACE_LVL,
         "node " SSIZE_T_FMT ": applying %s positioning",
         node_idx, gc_mcss_position_names[effect_style.POSITION] );
      /* TODO: MCSS_POS_NOTE: I'd like to get rid of this so all positioning
       *       is done through CSS... unfortunately, we only track the current
       *       and previous effective styles while working that out later, so
       *       we need to pin this to the element directly so we can rule it
       *       out of the box model e.g. when determining x/y coords of its
       *       neighbors.
       */
      mhtmr_node( tree, node_idx )->pos = effect_style.POSITION;
      mhtmr_node( tree, node_idx )->pos_flags = effect_style.POSITION_flags;
   }
   
   /* Figure out how big the contents of this node are. */
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

      debug_printf( MHTMR_TRACE_LVL, "TEXT w: " SIZE_T_FMT, 
         mhtmr_node( tree, node_idx )->w );

      maug_munlock( mhtml_tag( parser, tag_idx )->TEXT.content, tag_content );

   } else if(
      0 <= tag_idx &&
      MHTML_TAG_TYPE_IMG == mhtml_tag( parser, tag_idx )->base.type
   ) {

      if( retroflat_bitmap_ok( &(mhtmr_node( tree, node_idx )->bitmap) ) ) {
         mhtmr_node( tree, node_idx )->w =
            retroflat_bitmap_w( &(mhtmr_node( tree, node_idx )->bitmap) );
         mhtmr_node( tree, node_idx )->h =
            retroflat_bitmap_h( &(mhtmr_node( tree, node_idx )->bitmap) );
      }

      debug_printf( MHTMR_TRACE_LVL, "TEXT w: " SIZE_T_FMT, 
         mhtmr_node( tree, node_idx )->w );

   } else {
      /* Get sizing of child nodes. */

      maug_mzero( &child_prev_sibling_style, sizeof( struct MCSS_STYLE ) );
      node_iter_idx = mhtmr_node( tree, node_idx )->first_child;
      while( 0 <= node_iter_idx ) {
         mhtmr_tree_size(
            parser, tree, &child_prev_sibling_style, &effect_style,
            node_iter_idx, d + 1 );

         node_iter_idx = mhtmr_node( tree, node_iter_idx )->next_sibling;
      }
   }

   /* width */

   if( mcss_prop_is_active_NOT_flag( effect_style.WIDTH, AUTO ) ) {
      mhtmr_node( tree, node_idx )->w = effect_style.WIDTH;

   } else {
      if(
         MCSS_DISPLAY_BLOCK == effect_style.DISPLAY &&
         0 <= mhtmr_node( tree, node_idx )->parent
      ) {
         /* Use parent width. */
         /* TODO: Subtract parent padding! */
         mhtmr_node( tree, node_idx )->w =
            mhtmr_node( tree, mhtmr_node( tree, node_idx )->parent )->w;
      }

      /* Cycle through children and use greatest width. */
      child_iter_idx = mhtmr_node( tree, node_idx )->first_child;
      while( 0 <= child_iter_idx ) {
         mhtmr_apply_styles(
            parser, tree, &effect_style, &child_style,
            mhtmr_node( tree, child_iter_idx )->tag );
         
         /* Skip ABSOLUTE nodes. */
         if( MCSS_POSITION_ABSOLUTE == child_style.POSITION ) {
            child_iter_idx = mhtmr_node( tree, child_iter_idx )->next_sibling;
            continue;
         }

         if( MCSS_DISPLAY_BLOCK == child_style.DISPLAY ) {
            /* Reset the line width counter for coming BLOCK node. */
            this_line_w = 0;

            if(
               mhtmr_node( tree, child_iter_idx )->w >
                  mhtmr_node( tree, node_idx )->w
            ) {
               /* This BLOCK node is the longest yet! */
               mhtmr_node( tree, node_idx )->w =
                  mhtmr_node( tree, child_iter_idx )->w;
            }
         } else {
            /* Add inline node to this node line's width. */
            this_line_w += mhtmr_node( tree, child_iter_idx )->w;

            if( this_line_w > mhtmr_node( tree, node_idx )->w ) {
               /* The line of nodes we've been adding up is the longest yet! */
               mhtmr_node( tree, node_idx )->w = this_line_w;
            }
         }
         child_iter_idx = mhtmr_node( tree, child_iter_idx )->next_sibling;
      }
   }

   /* Apply additional modifiers (padding, etc) after children have all been
    * calculated.
    */

   /* Try specific left padding first, then try general padding. */
   if( mcss_prop_is_active_NOT_flag( effect_style.PADDING_LEFT, AUTO ) ) {
      mhtmr_node( tree, node_idx )->w += effect_style.PADDING_LEFT;
   } else if( mcss_prop_is_active_NOT_flag( effect_style.PADDING, AUTO ) ) {
      mhtmr_node( tree, node_idx )->w += effect_style.PADDING;
   }

   /* Try specific right padding first, then try general padding. */
   if( mcss_prop_is_active_NOT_flag( effect_style.PADDING_RIGHT, AUTO ) ) {
      mhtmr_node( tree, node_idx )->w += effect_style.PADDING_RIGHT;
   } else if( mcss_prop_is_active_NOT_flag( effect_style.PADDING, AUTO ) ) {
      mhtmr_node( tree, node_idx )->w += effect_style.PADDING;
   }

   /* Try specific top padding first, then try general padding. */
   if( mcss_prop_is_active_NOT_flag( effect_style.PADDING_TOP, AUTO ) ) {
      mhtmr_node( tree, node_idx )->h += effect_style.PADDING_TOP;
   } else if( mcss_prop_is_active_NOT_flag( effect_style.PADDING, AUTO ) ) {
      mhtmr_node( tree, node_idx )->h += effect_style.PADDING;
   }

   /* Try specific bottom padding first, then try general padding. */
   if( mcss_prop_is_active_NOT_flag( effect_style.PADDING_BOTTOM, AUTO ) ) {
      mhtmr_node( tree, node_idx )->h += effect_style.PADDING_BOTTOM;
   } else if( mcss_prop_is_active_NOT_flag( effect_style.PADDING, AUTO ) ) {
      mhtmr_node( tree, node_idx )->h += effect_style.PADDING;
   }

   /* height */

   if( mcss_prop_is_active_NOT_flag( effect_style.HEIGHT, AUTO ) ) {
      mhtmr_node( tree, node_idx )->h = effect_style.HEIGHT;

   } else {
      /* Cycle through children and add heights. */
      child_iter_idx = mhtmr_node( tree, node_idx )->first_child;
      while( 0 <= child_iter_idx ) {
         mhtmr_apply_styles(
            parser, tree, &effect_style, &child_style,
            mhtmr_node( tree, child_iter_idx )->tag );

         /* Skip ABSOLUTE nodes. */
         if( MCSS_POSITION_ABSOLUTE == child_style.POSITION ) {
            child_iter_idx = mhtmr_node( tree, child_iter_idx )->next_sibling;
            continue;
         }

         if( MCSS_DISPLAY_BLOCK == child_style.DISPLAY ) {
            /* Add the last line to the running height. */
            mhtmr_node( tree, node_idx )->h += this_line_h;
            
            /* Start a new running line height with this BLOCK node. */
            this_line_h = mhtmr_node( tree, child_iter_idx )->h;
         } else {
            /* Make sure this line is at least as tall as this INLINE node. */
            if( this_line_h < mhtmr_node( tree, child_iter_idx )->h ) {
               this_line_h = mhtmr_node( tree, child_iter_idx )->h;
            }
         }

         child_iter_idx = mhtmr_node( tree, child_iter_idx )->next_sibling;
      }

      /* Add the last line height the node height. */
      mhtmr_node( tree, node_idx )->h += this_line_h;
      this_line_h = 0;
   }

cleanup:

   /* We're done with the prev_sibling_style for this iter, so prepare it for
    * the next called by the parent!
    */
   if( NULL != prev_sibling_style ) {
      maug_mcpy(
         prev_sibling_style, &effect_style,
         sizeof( struct MCSS_STYLE ) );
   }

   return retval;
}

/* TODO: See MCSS_POS_NOTE. */
/*! \brief Break when we hit explicit position parent. */
#define mhtmr_break_on_active_pos( iter_idx ) \
   if( mcss_prop_is_active( mhtmr_node( tree, iter_idx )->pos ) ) { \
      break; \
   }

static ssize_t mhtmr_find_prev_sibling_in_box_model(
   struct MHTMR_RENDER_TREE* tree,
   size_t node_idx
) {
   ssize_t sibling_iter_idx = -1;
   ssize_t sibling_found_idx = -1;

   if( 0 > mhtmr_node( tree, node_idx )->parent ) {
      /* Can't determine sibling! */
      goto cleanup;
   }
   
   sibling_iter_idx = mhtmr_node_parent( tree, node_idx )->first_child;

   if( sibling_iter_idx == node_idx ) {
      /* No previous siblings! */
      goto cleanup;
   }

   while( 0 <= sibling_iter_idx && node_idx != sibling_iter_idx ) {
      if(
         /* TODO: See MCSS_POS_NOTE. This is what we were talking about. */
         MCSS_POSITION_ABSOLUTE != mhtmr_node( tree, sibling_iter_idx )->pos
      ) {
         sibling_found_idx = sibling_iter_idx;
      }

      /* TODO: Reset on <br />? */

      sibling_iter_idx = mhtmr_node( tree, sibling_iter_idx )->next_sibling;
   }

cleanup:
   return sibling_found_idx;
}

MERROR_RETVAL mhtmr_tree_pos(
   struct MHTML_PARSER* parser, struct MHTMR_RENDER_TREE* tree,
   struct MCSS_STYLE* prev_sibling_style,
   struct MCSS_STYLE* parent_style, ssize_t node_idx, size_t d
) {
   struct MCSS_STYLE child_prev_sibling_style;
   struct MCSS_STYLE effect_style;
   ssize_t child_iter_idx = -1;
   ssize_t tag_idx = -1;
   ssize_t node_iter_idx = -1;
   MERROR_RETVAL retval = MERROR_OK;

   if( NULL == mhtmr_node( tree, node_idx ) ) {
      goto cleanup;
   }

   tag_idx = mhtmr_node( tree, node_idx )->tag;

   mhtmr_apply_styles(
      parser, tree, parent_style, &effect_style, tag_idx );

   /* x */

   if( MCSS_POSITION_ABSOLUTE == effect_style.POSITION ) {
      /* This node is positioned absolutely. (Relatively) simple! */

      if( mcss_prop_is_active_NOT_flag( effect_style.LEFT, AUTO ) ) {

         child_iter_idx = mhtmr_node( tree, node_idx )->parent;
         while( 0 <= mhtmr_node( tree, child_iter_idx )->parent ) {
            mhtmr_break_on_active_pos( child_iter_idx );
            child_iter_idx = mhtmr_node( tree, child_iter_idx )->parent;
         }

         /* Set X to highest non-explicit ancestor. */
         mhtmr_node( tree, node_idx )->x =
            mhtmr_node( tree, child_iter_idx )->x + effect_style.LEFT;
      }
      if( mcss_prop_is_active_NOT_flag( effect_style.RIGHT, AUTO ) ) {

         child_iter_idx = mhtmr_node( tree, node_idx )->parent;
         while( 0 <= mhtmr_node( tree, child_iter_idx )->parent ) {
            mhtmr_break_on_active_pos( child_iter_idx );
            child_iter_idx = mhtmr_node( tree, child_iter_idx )->parent;
         }

         /* Set X to highest non-explicit ancestor. */
         mhtmr_node( tree, node_idx )->x = 
            mhtmr_node( tree, child_iter_idx )->w - 
            mhtmr_node( tree, node_idx )->w - 
            effect_style.RIGHT;
      }

   } else if(
      MCSS_DISPLAY_INLINE == effect_style.DISPLAY &&
      MCSS_DISPLAY_INLINE == prev_sibling_style->DISPLAY
   ) {

      child_iter_idx = mhtmr_find_prev_sibling_in_box_model( tree, node_idx );
      if( 0 <= child_iter_idx ) {
         /* Place to the right of the previous sibling. */
         mhtmr_node( tree, node_idx )->x =
            mhtmr_node( tree, child_iter_idx )->x +
            mhtmr_node( tree, child_iter_idx )->w;
      } else {
         /* No previous siblings, so just put it inside its parent. */
         mhtmr_node( tree, node_idx )->x =
            mhtmr_node_parent( tree, node_idx )->x;
      }

   } else if( 0 <= mhtmr_node( tree, node_idx )->parent ) {
      mhtmr_node( tree, node_idx )->x = mhtmr_node_parent( tree, node_idx )->x;
   }
   
   /* y */

   /* TODO: Add margins of children? */

   if( MCSS_POSITION_ABSOLUTE == effect_style.POSITION ) {
      /* This node is positioned absolutely. (Relatively) simple! */

      if( mcss_prop_is_active_NOT_flag( effect_style.TOP, AUTO ) ) {

         child_iter_idx = mhtmr_node( tree, node_idx )->parent;
         while( 0 <= mhtmr_node( tree, child_iter_idx )->parent ) {
            mhtmr_break_on_active_pos( child_iter_idx );
            child_iter_idx = mhtmr_node( tree, child_iter_idx )->parent;
         }

         /* Set Y to highest non-explicit ancestor. */
         mhtmr_node( tree, node_idx )->y = 
            mhtmr_node( tree, child_iter_idx )->y + effect_style.TOP;
      }
      if( mcss_prop_is_active_NOT_flag( effect_style.BOTTOM, AUTO ) ) {

         child_iter_idx = mhtmr_node( tree, node_idx )->parent;
         while( 0 <= mhtmr_node( tree, child_iter_idx )->parent ) {
            mhtmr_break_on_active_pos( child_iter_idx );
            child_iter_idx = mhtmr_node( tree, child_iter_idx )->parent;
         }

         /* Set Y to highest non-explicit ancestor. */
         mhtmr_node( tree, node_idx )->y = 
            mhtmr_node( tree, child_iter_idx )->h - 
            mhtmr_node( tree, node_idx )->h - 
            effect_style.BOTTOM;
      }

   } else if(
      MCSS_DISPLAY_INLINE == effect_style.DISPLAY &&
      MCSS_DISPLAY_INLINE == prev_sibling_style->DISPLAY
   ) {
      child_iter_idx = mhtmr_find_prev_sibling_in_box_model( tree, node_idx );
      if( 0 <= child_iter_idx ) {
         /* Place to the right of the previous sibling. */
         mhtmr_node( tree, node_idx )->y =
            mhtmr_node( tree, child_iter_idx )->y;
      } else {
         /* No previous siblings, so just put it inside its parent. */
         mhtmr_node( tree, node_idx )->y =
            mhtmr_node_parent( tree, node_idx )->y;
      }

   } else {
      /* Position relative to other nodes. */

      child_iter_idx = mhtmr_node( tree, node_idx )->parent;
      if( 0 <= child_iter_idx ) {
         /* Add top offset of parent. */
         mhtmr_node( tree, node_idx )->y +=
            mhtmr_node( tree, child_iter_idx )->y;

         child_iter_idx = mhtmr_node( tree, child_iter_idx )->first_child;
      }
      while( 0 <= child_iter_idx && node_idx != child_iter_idx ) {
         if(
            /* Don't add absolute positioned siblings to this offset! */
            MCSS_POSITION_ABSOLUTE != mhtmr_node( tree, child_iter_idx )->pos
         ) {
            /* Add top offset of prior siblings. */
            mhtmr_node( tree, node_idx )->y +=
               mhtmr_node( tree, child_iter_idx )->h;
         }

         child_iter_idx = mhtmr_node( tree, child_iter_idx )->next_sibling;
      }
   }

   /* margin-left, margin-right */

   if( 
      MCSS_POSITION_ABSOLUTE != mhtmr_node( tree, node_idx )->pos &&
      0 <= mhtmr_node( tree, node_idx )->parent &&
      mcss_prop_is_active_flag( effect_style.MARGIN_LEFT, AUTO ) &&
      mcss_prop_is_active_flag( effect_style.MARGIN_RIGHT, AUTO )
   ) {
      /* Center */
      mhtmr_node( tree, node_idx )->x =
         mhtmr_node( tree, mhtmr_node( tree, node_idx )->parent )->x +
         (mhtmr_node( tree, mhtmr_node( tree, node_idx )->parent )->w / 2) -
         (mhtmr_node( tree, node_idx )->w / 2);

   } else if( 
      0 <= mhtmr_node( tree, node_idx )->parent &&
      mcss_prop_is_active_flag( effect_style.MARGIN_LEFT, AUTO ) &&
      mcss_prop_is_active_NOT_flag( effect_style.MARGIN_RIGHT, AUTO )
   ) {
      /* Justify right. */
      /* TODO: Subtract padding below, as well. */
      mhtmr_node( tree, node_idx )->x = 
         mhtmr_node_parent( tree, node_idx )->w -
         mhtmr_node( tree, node_idx )->w;

   } else if( mcss_prop_is_active( effect_style.MARGIN_LEFT ) ) {
      /* Justify left. */
      mhtmr_node( tree, node_idx )->x += effect_style.MARGIN_LEFT;
   }

   /* padding */

   if( 
      NULL != parent_style &&
      MCSS_POSITION_ABSOLUTE != effect_style.POSITION &&
      (
         /* Block elements should all be on new lines, so pad left. */
         MCSS_DISPLAY_BLOCK == effect_style.DISPLAY ||
         (
            NULL != prev_sibling_style &&
            MCSS_DISPLAY_BLOCK == prev_sibling_style->DISPLAY
         ) ||
         /* Otherwise only pad the first element. */
         node_idx == mhtmr_node_parent( tree, node_idx )->first_child
      )
   ) {
      /* Try specific left padding first, then try general padding. */
      if( mcss_prop_is_active_NOT_flag( parent_style->PADDING_LEFT, AUTO ) ) {
         mhtmr_node( tree, node_idx )->x += parent_style->PADDING_LEFT;
      } else if( mcss_prop_is_active_NOT_flag( parent_style->PADDING, AUTO ) ) {
         mhtmr_node( tree, node_idx )->x += parent_style->PADDING;
      }
   }

   if( 
      NULL != parent_style &&
      MCSS_POSITION_ABSOLUTE != effect_style.POSITION &&
      (
         /* Inline elements should all be on the same line, so pad top. */

         /* This doesn't work, because padding is added into the X of the
          * previous element... it pushes each successive element down further.
          * Maybe padding should be separate until render?
          */
         /* MCSS_DISPLAY_INLINE == effect_style.DISPLAY || */

         /* Otherwise only pad the first element. */
         node_idx == mhtmr_node_parent( tree, node_idx )->first_child
      )
   ) {
      /* Try specific top padding first, then try general padding. */
      if( mcss_prop_is_active_NOT_flag( parent_style->PADDING_TOP, AUTO ) ) {
         mhtmr_node( tree, node_idx )->y += parent_style->PADDING_TOP;
      } else if( mcss_prop_is_active_NOT_flag( parent_style->PADDING, AUTO ) ) {
         mhtmr_node( tree, node_idx )->y += parent_style->PADDING;
      }
   }

   /* color */

   if( mcss_prop_is_active( effect_style.COLOR ) ) {
      mhtmr_node( tree, node_idx )->fg = effect_style.COLOR;
   }

   if( mcss_prop_is_active( effect_style.BACKGROUND_COLOR ) ) {
      mhtmr_node( tree, node_idx )->bg = effect_style.BACKGROUND_COLOR;
   }

   /* Figure out child positions. */

   maug_mzero( &child_prev_sibling_style, sizeof( struct MCSS_STYLE ) );
   node_iter_idx = mhtmr_node( tree, node_idx )->first_child;
   while( 0 <= node_iter_idx ) {
      mhtmr_tree_pos(
         parser, tree, &child_prev_sibling_style, &effect_style,
         node_iter_idx, d + 1 );

      node_iter_idx = mhtmr_node( tree, node_iter_idx )->next_sibling;
   }
 
cleanup:

   /* We're done with the prev_sibling_style for this iter, so prepare it for
    * the next called by the parent!
    */
   if( NULL != prev_sibling_style ) {
      maug_mcpy(
         prev_sibling_style, &effect_style,
         sizeof( struct MCSS_STYLE ) );
   }

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

   /* TODO: Multi-pass, draw absolute pos afterwards. */

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

      } else if( MHTML_TAG_TYPE_BODY == tag->base.type ) {
         /* Draw body BG. */
         if( RETROFLAT_COLOR_NULL != node->bg ) {
            retroflat_rect(
               NULL, node->bg,
               mhtmr_node( tree, node_idx )->x,
               mhtmr_node( tree, node_idx )->y,
               mhtmr_node( tree, node_idx )->w,
               mhtmr_node( tree, node_idx )->h,
               RETROFLAT_FLAGS_FILL );
         }

      } else if( MHTML_TAG_TYPE_IMG == tag->base.type ) {
         /* TODO: Load and blit the image. */
         
         if( retroflat_bitmap_ok( &(mhtmr_node( tree, node_idx )->bitmap) ) ) {
            retroflat_blit_bitmap(
               NULL, &(mhtmr_node( tree, node_idx )->bitmap),
               0, 0,
               mhtmr_node( tree, node_idx )->x,
               mhtmr_node( tree, node_idx )->y,
               16, 16
               /* mhtmr_node( tree, node_idx )->w,
               mhtmr_node( tree, node_idx )->h */ );
         }

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
   char indents[31];

   if( 0 > iter ) {
      return;
   }

   /* Generate the indentation. */
   maug_mzero( indents, 30 );
   for( i = 0 ; d > i ; i++ ) {
      if( strlen( indents ) >= 30 ) {
         break;
      }
      strcat( indents, "   " );
   }

   /* Print the debug line. */
   debug_printf(
      1,
      "%s" SSIZE_T_FMT " (tag %s): x: " SSIZE_T_FMT ", y: " SSIZE_T_FMT
      " (" SSIZE_T_FMT " x " SSIZE_T_FMT ")",
      indents, iter,
      0 <= tree->nodes[iter].tag ?
         gc_mhtml_tag_names[parser->tags[tree->nodes[iter].tag].base.type]
            : "ROOT",
      tree->nodes[iter].x, tree->nodes[iter].y,
      tree->nodes[iter].w, tree->nodes[iter].h );

   mhtmr_tree_dump( tree, parser, tree->nodes[iter].first_child, d + 1 );

   mhtmr_tree_dump( tree, parser, tree->nodes[iter].next_sibling, d );
}

void mhtmr_tree_free( struct MHTMR_RENDER_TREE* tree ) {

   debug_printf( MHTMR_TRACE_LVL, "freeing render nodes..." );

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
   debug_printf( MHTMR_TRACE_LVL,
      "allocating " SIZE_T_FMT " nodes...", tree->nodes_sz_max );
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

