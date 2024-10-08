
#ifndef RETROHTR_H
#define RETROHTR_H

/**
 * \addtogroup retrohtr Retro HyperText Rendering API
 * \{
 * \file retrohtr.h
 */

#define RETROHTR_TREE_FLAG_GUI_ACTIVE 1

#define RETROHTR_NODE_FLAG_DIRTY 2

#ifndef RETROHTR_RENDER_NODES_INIT_SZ
#  define RETROHTR_RENDER_NODES_INIT_SZ 10
#endif /* !RETROHTR_RENDER_NODES_INIT_SZ */

#ifndef RETROHTR_TRACE_LVL
#  define RETROHTR_TRACE_LVL 0
#endif /* !RETROHTR_TRACE_LVL */

#define RETROHTR_EDGE_UNKNOWN 0
#define RETROHTR_EDGE_LEFT    1
#define RETROHTR_EDGE_TOP     2
#define RETROHTR_EDGE_INSIDE  4

struct RETROHTR_RENDER_NODE {
   uint8_t flags;
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
   uint8_t edge;
   RETROFLAT_COLOR bg;
   RETROFLAT_COLOR fg;
#ifdef RETROGXC_PRESENT
   ssize_t font_idx;
#else
   MAUG_MHANDLE font_h;
#endif /* RETROGXC_PRESENT */
   ssize_t tag;
   /*! \brief Index of container's render node in ::RETROHTR_RENDER_TREE. */
   ssize_t parent;
   /*! \brief Index of first child's render node in ::RETROHTR_RENDER_TREE. */
   ssize_t first_child;
   /*! \brief Index of next sibling's render node in ::RETROHTR_RENDER_TREE. */
   ssize_t next_sibling;
   struct RETROFLAT_BITMAP bitmap;
};

struct RETROHTR_RENDER_TREE {
   uint8_t flags;
   MAUG_MHANDLE nodes_h;
   /*! \brief Locked pointer to nodes when locked with retrohtr_tree_lock(). */
   struct RETROHTR_RENDER_NODE* nodes;
   /*! \brief Current active number of nodes in RETROHTR_RENDER_NODE::nodes_h. */
   size_t nodes_sz;
   /*! \brief Current alloc'd number of nodes in RETROHTR_RENDER_NODE::nodes_h. */
   size_t nodes_sz_max;
   struct RETROGUI gui;
};

/* TODO: Function names should be verb_noun! */

#define retrohtr_node( tree, idx ) \
   (0 <= (ssize_t)idx ? &((tree)->nodes[idx]) : NULL)

#define retrohtr_node_parent( tree, idx ) \
   (0 <= idx && 0 <= (tree)->nodes[idx].parent ? \
      &((tree)->nodes[(tree)->nodes[idx].parent]) : NULL)

#define retrohtr_tree_lock( tree ) \
   if( NULL == (tree)->nodes ) { \
      maug_mlock( (tree)->nodes_h, (tree)->nodes ); \
      maug_cleanup_if_null_alloc( struct RETROHTR_RENDER_NODE*, (tree)->nodes ); \
   }

#define retrohtr_tree_unlock( tree ) \
   if( NULL != (tree)->nodes ) { \
      maug_munlock( (tree)->nodes_h, (tree)->nodes ); \
   }

#define retrohtr_tree_is_locked( tree ) (NULL != (tree)->nodes)

/* TODO: Make these offset by element scroll on screen. */

#define retrohtr_node_screen_x( tree, node_idx ) \
   ((tree)->nodes[node_idx].x)

#define retrohtr_node_screen_y( tree, node_idx ) \
   ((tree)->nodes[node_idx].y)

MERROR_RETVAL retrohtr_tree_create(
   struct MHTML_PARSER* parser, struct RETROHTR_RENDER_TREE* tree,
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
MERROR_RETVAL retrohtr_apply_styles(
   struct MHTML_PARSER* parser, struct RETROHTR_RENDER_TREE* tree,
   struct MCSS_STYLE* parent_style, struct MCSS_STYLE* effect_style,
   ssize_t tag_idx );

MERROR_RETVAL retrohtr_tree_size(
   struct MHTML_PARSER* parser, struct RETROHTR_RENDER_TREE* tree,
   struct MCSS_STYLE* prev_sibling_style,
   struct MCSS_STYLE* parent_style, ssize_t node_idx, size_t d );

MERROR_RETVAL retrohtr_tree_pos(
   struct MHTML_PARSER* parser, struct RETROHTR_RENDER_TREE* tree,
   struct MCSS_STYLE* prev_sibling_style,
   struct MCSS_STYLE* parent_style, ssize_t node_idx, size_t d );

MERROR_RETVAL retrohtr_tree_draw(
   struct MHTML_PARSER* parser, struct RETROHTR_RENDER_TREE* tree,
   ssize_t node_idx, size_t d );

retrogui_idc_t retrohtr_tree_poll_ctls(
   struct RETROHTR_RENDER_TREE* tree,
   RETROFLAT_IN_KEY* input,
   struct RETROFLAT_INPUT* input_evt );

MERROR_RETVAL retrohtr_tree_dump(
   struct RETROHTR_RENDER_TREE* tree, struct MHTML_PARSER* parser,
   ssize_t iter, size_t d );

void retrohtr_tree_free( struct RETROHTR_RENDER_TREE* tree );

MERROR_RETVAL retrohtr_tree_init( struct RETROHTR_RENDER_TREE* tree );

#ifdef RETROHTR_C

ssize_t retrohtr_get_next_free_node( struct RETROHTR_RENDER_TREE* tree ) {
   uint8_t auto_unlocked = 0;
   ssize_t retidx = -1;
   MAUG_MHANDLE new_nodes_h = (MAUG_MHANDLE)NULL;

   if( NULL != tree->nodes ) {
      debug_printf( RETROHTR_TRACE_LVL, "auto-unlocking nodes..." );
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
         sizeof( struct RETROHTR_RENDER_NODE ) );
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
   debug_printf( RETROHTR_TRACE_LVL,
      "zeroing node " SIZE_T_FMT " (of " SIZE_T_FMT ")...",
      tree->nodes_sz, tree->nodes_sz_max );
   maug_mzero( &(tree->nodes[tree->nodes_sz]),
      sizeof( struct RETROHTR_RENDER_NODE ) );
   retidx = tree->nodes_sz;
   tree->nodes_sz++;

   /* Compensate for cleanup below. */
   maug_munlock( tree->nodes_h, tree->nodes );

cleanup:

   if( auto_unlocked ) {
      debug_printf( RETROHTR_TRACE_LVL, "auto-locking nodes..." );
      maug_mlock( tree->nodes_h, tree->nodes );
   }

   return retidx;
}

ssize_t retrohtr_add_node_child(
   struct RETROHTR_RENDER_TREE* tree, ssize_t node_parent_idx
) {
   ssize_t node_new_idx = -1,
      node_sibling_idx = -1;

   node_new_idx = retrohtr_get_next_free_node( tree );
   if( 0 > node_new_idx ) {
      goto cleanup;
   }

#ifdef RETROGXC_PRESENT
   retrohtr_node( tree, node_new_idx )->font_idx = -1;
#endif /* RETROGXC_PRESENT */
   retrohtr_node( tree, node_new_idx )->parent = node_parent_idx;
   retrohtr_node( tree, node_new_idx )->first_child = -1;
   retrohtr_node( tree, node_new_idx )->next_sibling = -1;

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
   if( 0 > retrohtr_node( tree, node_parent_idx )->first_child ) {
      debug_printf( RETROHTR_TRACE_LVL, "adding first child..." );
      assert( -1 == retrohtr_node( tree, node_parent_idx )->first_child );
      retrohtr_node( tree, node_parent_idx )->first_child = node_new_idx;
   } else {
      assert( NULL != retrohtr_node( tree, node_parent_idx ) );
      node_sibling_idx = retrohtr_node( tree, node_parent_idx )->first_child;
      assert( NULL != retrohtr_node( tree, node_sibling_idx ) );
      while( 0 <= retrohtr_node( tree, node_sibling_idx )->next_sibling ) {
         node_sibling_idx = 
            retrohtr_node( tree, node_sibling_idx )->next_sibling;
      }
      retrohtr_node( tree, node_sibling_idx )->next_sibling = node_new_idx;
   }

cleanup:
   
   return node_new_idx;
}

MERROR_RETVAL retrohtr_tree_create(
   struct MHTML_PARSER* parser, struct RETROHTR_RENDER_TREE* tree,
   size_t x, size_t y, size_t w, size_t h,
   ssize_t tag_idx, ssize_t node_idx, size_t d
) {
   ssize_t node_new_idx = -1;
   ssize_t tag_iter_idx = -1;
   MERROR_RETVAL retval = MERROR_OK;
   union MHTML_TAG* p_tag_iter = NULL;
   ssize_t tag_next_idx = 0;

   debug_printf( RETROHTR_TRACE_LVL, 
      "creating render node for tag: " SSIZE_T_FMT, tag_idx );

   mdata_vector_lock( &(parser->tags) );

   if( 0 > tag_idx ) {
      goto cleanup;
   }
   p_tag_iter = mdata_vector_get( &(parser->tags), tag_idx, union MHTML_TAG );
   if( NULL == p_tag_iter ) {
      goto cleanup;
   }

   /* Make sure we have a single root node. */
   if( 0 > node_idx ) {
      assert( MHTML_TAG_TYPE_BODY == p_tag_iter->base.type );

      node_new_idx = retrohtr_add_node_child( tree, node_idx );
      if( 0 > node_new_idx ) {
         goto cleanup;
      }
      debug_printf( RETROHTR_TRACE_LVL,
         "created initial root node: " SIZE_T_FMT, node_new_idx );

      node_idx = node_new_idx;

      /* The common root is the body tag. */
      retrohtr_node( tree, node_idx )->tag = tag_idx;

      retrohtr_node( tree, node_idx )->x = x;
      retrohtr_node( tree, node_idx )->y = y;
      retrohtr_node( tree, node_idx )->w = w;
      retrohtr_node( tree, node_idx )->h = h;
   }

   tag_iter_idx = p_tag_iter->base.first_child;
   while( 0 <= tag_iter_idx ) {
      node_new_idx = retrohtr_add_node_child( tree, node_idx );
      p_tag_iter = mdata_vector_get(
         &(parser->tags), tag_iter_idx, union MHTML_TAG );
      assert( NULL != p_tag_iter );
      if( 0 > node_new_idx ) {
         goto cleanup;
      }

      retrohtr_node( tree, node_new_idx )->tag = tag_iter_idx;

      debug_printf( RETROHTR_TRACE_LVL,
         "rendering node " SSIZE_T_FMT " (%s) under node " SSIZE_T_FMT,
         node_new_idx,
         gc_mhtml_tag_names[p_tag_iter->base.type],
         node_idx );

      /* Tag-specific rendering preparations. */
      if( MHTML_TAG_TYPE_IMG == p_tag_iter->base.type ) {
         /* Load the image for rendering later. */
         retval = retroflat_load_bitmap(
            p_tag_iter->IMG.src,
            &(retrohtr_node( tree, node_new_idx )->bitmap),
            RETROFLAT_FLAGS_LITERAL_PATH );
         if( MERROR_OK == retval ) {
            debug_printf( RETROHTR_TRACE_LVL, "loaded img: %s", 
               p_tag_iter->IMG.src );
         } else {
            error_printf( "could not load img: %s", p_tag_iter->IMG.src );
         }
      }

      tag_next_idx = p_tag_iter->base.next_sibling;

      mdata_vector_unlock( &(parser->tags) );

      retval = retrohtr_tree_create( parser, tree, x, y, w, h,
         tag_iter_idx, node_new_idx, d + 1 );
      maug_cleanup_if_not_ok();

      mdata_vector_lock( &(parser->tags) );

      tag_iter_idx = tag_next_idx;
   }

cleanup:

   if( mdata_vector_is_locked( &(parser->tags) ) ) {
      mdata_vector_unlock( &(parser->tags) );
   }

   return retval;
}

MERROR_RETVAL retrohtr_apply_styles(
   struct MHTML_PARSER* parser, struct RETROHTR_RENDER_TREE* tree,
   struct MCSS_STYLE* parent_style, struct MCSS_STYLE* effect_style,
   ssize_t tag_idx
) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t tag_style_idx = -1;
   size_t tag_type = 0,
      i = 0;
   struct MCSS_STYLE* style = NULL;
   union MHTML_TAG* p_tag_iter = NULL;

   debug_printf( RETROHTR_TRACE_LVL,
      "applying styles for tag: " SSIZE_T_FMT, tag_idx );

   assert( !mdata_vector_is_locked( &(parser->tags) ) );
   mdata_vector_lock( &(parser->styler.styles) );
   mdata_vector_lock( &(parser->tags) );

   maug_mzero( effect_style, sizeof( struct MCSS_STYLE ) );

   if( 0 >= tag_idx ) {
      goto cleanup;
   }
   p_tag_iter = mdata_vector_get( &(parser->tags), tag_idx, union MHTML_TAG );
   if( NULL == p_tag_iter ) {
      goto cleanup;
   }

   tag_type = p_tag_iter->base.type;

   /* Merge style based on HTML element class. */
   if( 0 < p_tag_iter->base.classes_sz ) {
      for( i = 0 ; mdata_vector_ct( &(parser->styler.styles) ) > i ; i++ ) {
         style = mdata_vector_get(
            &(parser->styler.styles), i, struct MCSS_STYLE );

         if(
            NULL != style &&
            0 == strncmp(
               p_tag_iter->base.classes,
               style->class,
               p_tag_iter->base.classes_sz
            )
         ) {
            debug_printf( RETROHTR_TRACE_LVL, "found style for tag class: %s",
               style->class );

            mcssmerge_styles( effect_style, parent_style, style, tag_type );
         }
      }
   }

   /* Merge style based on HTML element ID. */
   if( 0 < p_tag_iter->base.id_sz ) {
      for( i = 0 ; mdata_vector_ct( &(parser->styler.styles) ) > i ; i++ ) {
         style = mdata_vector_get(
            &(parser->styler.styles), i, struct MCSS_STYLE );

         if(
            NULL != style &&
            0 == strncmp(
               p_tag_iter->base.id,
               style->id,
               p_tag_iter->base.id_sz
            )
         ) {
            debug_printf( RETROHTR_TRACE_LVL, "found style for tag ID: %s",
               style->id );

            mcssmerge_styles( effect_style, parent_style, style, tag_type );
         }
      }
   }

   /* Grab element-specific style last. */
   tag_style_idx = p_tag_iter->base.style;

cleanup:

   /* TODO: Separate this out of cleanup phase. */

   /* This might be NULL! */
   style = mdata_vector_get(
      &(parser->styler.styles), tag_style_idx, struct MCSS_STYLE );

   /* Make sure we have a root style. */
   mcssmerge_styles( effect_style, parent_style, style, tag_type );

   mdata_vector_unlock( &(parser->tags) );
   mdata_vector_unlock( &(parser->styler.styles) );

   return retval;
}

static MERROR_RETVAL retrohtr_load_font(
   struct MCSS_PARSER* styler,
#ifdef RETROGXC_PRESENT
   ssize_t* font_idx_p,
#else
   MAUG_MHANDLE* font_h_p,
#endif /* RETROGXC_PRESENT */
   struct MCSS_STYLE* effect_style
) {
   MERROR_RETVAL retval = MERROR_OK;
   char* strpool = NULL;

#ifdef RETROGXC_PRESENT
   if( 0 <= *font_idx_p ) {
      error_printf( "tried to load font but font already loaded, idx: "
         SSIZE_T_FMT, *font_idx_p );
#else
   if( (MAUG_MHANDLE)NULL != *font_h_p ) {
      error_printf( "tried to load font but font already loaded, p: %p",
         *font_h_p );
#endif /* RETROGXC_PRESENT */
      goto cleanup;
   }

   mdata_strpool_lock( &(styler->strpool), strpool );

   debug_printf( RETROHTR_TRACE_LVL,
      "loading font: %s (" SSIZE_T_FMT ")",
      &(strpool[effect_style->FONT_FAMILY]), effect_style->FONT_FAMILY );

   if( 0 >= effect_style->FONT_FAMILY ) {
      error_printf( "style has no font associated!" );
      /* TODO: Load fallback font? */
      retval = MERROR_GUI;
      goto cleanup;
   }

   /* Load the font into the cache. */
#ifdef RETROGXC_PRESENT
   *font_idx_p =
      retrogxc_load_font( &(strpool[effect_style->FONT_FAMILY]), 0, 33, 93 );
#else
   retval = retrofont_load(
      &(strpool[effect_style->FONT_FAMILY]), font_h_p, 0, 33, 93 );
#endif /* RETROGXC_PRESENT */

cleanup:

   mdata_strpool_unlock( &(styler->strpool), strpool );

   return retval;
}

MERROR_RETVAL retrohtr_tree_gui(
   struct RETROHTR_RENDER_TREE* tree, struct MCSS_PARSER* styler,
   struct MCSS_STYLE* effect_style
) {
   MERROR_RETVAL retval = MERROR_OK;

   /* Create a GUI handler just for this tree. */
   if(
      RETROHTR_TREE_FLAG_GUI_ACTIVE ==
         (RETROHTR_TREE_FLAG_GUI_ACTIVE & tree->flags)
   ) {
      debug_printf( RETROHTR_TRACE_LVL, "tree GUI already active!" );
      goto cleanup;
   }

   /* This means all GUI items will use the font from the first node
      * loaded with a GUI item!
      */
   retval = retrogui_init( &(tree->gui) );
   maug_cleanup_if_not_ok();

   retval = retrohtr_load_font(
      styler,
#ifdef RETROGXC_PRESENT
      &(tree->gui.font_idx),
#else
      &(tree->gui.font_h),
#endif /* RETROGXC_PRESENT */
      effect_style );
   maug_cleanup_if_not_ok();

   tree->flags |= RETROHTR_TREE_FLAG_GUI_ACTIVE;

   debug_printf( RETROHTR_TRACE_LVL, "tree GUI initialized!" );

cleanup:
   return retval;
}

MERROR_RETVAL retrohtr_tree_size(
   struct MHTML_PARSER* parser, struct RETROHTR_RENDER_TREE* tree,
   struct MCSS_STYLE* prev_sibling_style,
   struct MCSS_STYLE* parent_style, ssize_t node_idx, size_t d
) {
   struct MCSS_STYLE effect_style;
   struct MCSS_STYLE child_prev_sibling_style;
   struct MCSS_STYLE child_style;
   char* strpool = NULL;
   ssize_t child_iter_idx = -1;
   ssize_t tag_idx = -1;
   ssize_t node_iter_idx = -1;
   size_t this_line_w = 0;
   size_t this_line_h = 0;
   MERROR_RETVAL retval = MERROR_OK;
   union RETROGUI_CTL ctl;
   union MHTML_TAG* p_tag_iter = NULL;
   union MHTML_TAG* p_tag_node = NULL;

   if( NULL == retrohtr_node( tree, node_idx ) ) {
      goto cleanup;
   }

   tag_idx = retrohtr_node( tree, node_idx )->tag;

   retval = retrohtr_apply_styles(
      parser, tree, parent_style, &effect_style, tag_idx );
   maug_cleanup_if_not_ok();

   assert( !mdata_vector_is_locked( &(parser->tags) ) );
   mdata_vector_lock( &(parser->tags) );

   p_tag_iter = mdata_vector_get( &(parser->tags), tag_idx, union MHTML_TAG );
   assert( NULL != p_tag_iter );

   /* position */

   if( mcss_prop_is_active( effect_style.POSITION ) ) {
      debug_printf( RETROHTR_TRACE_LVL,
         "node " SSIZE_T_FMT ": applying %s positioning",
         node_idx, gc_mcss_position_names[effect_style.POSITION] );
      /* TODO: MCSS_POS_NOTE: We'd like to get rid of this so all positioning
       *       is done through CSS... unfortunately, we only track the current
       *       and previous effective styles while working that out later, so
       *       we need to pin this to the element directly so we can rule it
       *       out of the box model e.g. when determining x/y coords of its
       *       neighbors.
       */
      retrohtr_node( tree, node_idx )->pos = effect_style.POSITION;
      retrohtr_node( tree, node_idx )->pos_flags = effect_style.POSITION_flags;
   }

   /* Grab fixed dimensions before content-based calculations of children, so
    * we know if there are constraints. If these aren't set, then we'll size
    * based on childrens' sizes after we determine childrens' sizes below.
    */

   if( mcss_prop_is_active_NOT_flag( effect_style.WIDTH, AUTO ) ) {
      retrohtr_node( tree, node_idx )->w = effect_style.WIDTH;
   }

   if( mcss_prop_is_active_NOT_flag( effect_style.HEIGHT, AUTO ) ) {
      retrohtr_node( tree, node_idx )->h = effect_style.HEIGHT;
   }
   
   /* Figure out how big the contents of this node are. */

   /* Font is heritable, so load it for all nodes even if we don't use it. */
   retval = retrohtr_load_font(
      &(parser->styler),
#ifdef RETROGXC_PRESENT
      &(retrohtr_node( tree, node_idx )->font_idx),
#else
      &(retrohtr_node( tree, node_idx )->font_h),
#endif /* RETROGXC_PRESENT */
      &effect_style );
   maug_cleanup_if_not_ok();

   if( 0 <= tag_idx && MHTML_TAG_TYPE_TEXT == p_tag_iter->base.type ) {
      /* Get text size to use in calculations below. */

      mdata_strpool_lock( &(parser->strpool), strpool );

#ifdef RETROGXC_PRESENT
      retrogxc_string_sz(
#else
      retrofont_string_sz(
#endif /* RETROGXC_PRESENT */
         NULL, &(strpool[p_tag_iter->TEXT.content_idx]),
         p_tag_iter->TEXT.content_sz,
#ifdef RETROGXC_PRESENT
         retrohtr_node( tree, node_idx )->font_idx,
#else
         retrohtr_node( tree, node_idx )->font_h,
#endif /* RETROGXC_PRESENT */
         /* Constrain node text size to parent size. */
         retrohtr_node_parent( tree, node_idx )->w,
         retrohtr_node_parent( tree, node_idx )->h,
         &(retrohtr_node( tree, node_idx )->w),
         &(retrohtr_node( tree, node_idx )->h), 0 );

      debug_printf( RETROHTR_TRACE_LVL, "TEXT w: " SIZE_T_FMT, 
         retrohtr_node( tree, node_idx )->w );

      mdata_strpool_unlock( &(parser->strpool), strpool );

   } else if(
      0 <= tag_idx &&
      MHTML_TAG_TYPE_INPUT == p_tag_iter->base.type
   ) {
      /* Push the control (for the client renderer to redraw later). */

      retval = retrohtr_tree_gui( tree, &(parser->styler), &effect_style );

      retrogui_lock( &(tree->gui) );

      if(
         /* Use the same ID for the node and control it creates. */
         MERROR_OK != retrogui_init_ctl(
            &ctl, RETROGUI_CTL_TYPE_BUTTON, node_idx )
      ) {
         error_printf( "could not initialize control!" );
         retrogui_unlock( &(tree->gui) );
         goto cleanup;
      }

      p_tag_node = mdata_vector_get(
         &(parser->tags),
         retrohtr_node( tree, node_idx )->tag, union MHTML_TAG );
      
      ctl.base.x = retrohtr_node( tree, node_idx )->x;
      ctl.base.y = retrohtr_node( tree, node_idx )->y;
      ctl.base.w = 0;
      ctl.base.h = 0;
      ctl.BUTTON.label = p_tag_node->INPUT.value;

      /* Grab determined size back from control. */
      retrohtr_node( tree, node_idx )->w = ctl.base.w;
      retrohtr_node( tree, node_idx )->h = ctl.base.h;

      debug_printf( RETROHTR_TRACE_LVL, "initialized control for INPUT..." );

      retrogui_push_ctl( &(tree->gui), &ctl );

      retrogui_unlock( &(tree->gui) );

   } else if( 0 <= tag_idx && MHTML_TAG_TYPE_IMG == p_tag_iter->base.type ) {

      if( retroflat_bitmap_ok( &(retrohtr_node( tree, node_idx )->bitmap) ) ) {
         retrohtr_node( tree, node_idx )->w =
            retroflat_bitmap_w( &(retrohtr_node( tree, node_idx )->bitmap) );
         retrohtr_node( tree, node_idx )->h =
            retroflat_bitmap_h( &(retrohtr_node( tree, node_idx )->bitmap) );
      }

      debug_printf( RETROHTR_TRACE_LVL, "TEXT w: " SIZE_T_FMT, 
         retrohtr_node( tree, node_idx )->w );

   } else {
      /* Get sizing of child nodes. */

      maug_mzero( &child_prev_sibling_style, sizeof( struct MCSS_STYLE ) );
      node_iter_idx = retrohtr_node( tree, node_idx )->first_child;
      mdata_vector_unlock( &(parser->tags) );
      while( 0 <= node_iter_idx ) {
         retrohtr_tree_size(
            parser, tree, &child_prev_sibling_style, &effect_style,
            node_iter_idx, d + 1 );

         node_iter_idx = retrohtr_node( tree, node_iter_idx )->next_sibling;
      }
   }

   if( mdata_vector_is_locked( &(parser->tags) ) ) {
      mdata_vector_unlock( &(parser->tags) );
   }

   /* If our width is still zero, then size based on children. */
   if( 0 == retrohtr_node( tree, node_idx )->w ) {
      if(
         MCSS_DISPLAY_BLOCK == effect_style.DISPLAY &&
         0 <= retrohtr_node( tree, node_idx )->parent
      ) {
         /* Use parent width. */
         /* TODO: Subtract parent padding! */
         retrohtr_node( tree, node_idx )->w =
            retrohtr_node_parent( tree, node_idx )->w;
      }

      /* Cycle through children and use greatest width. */
      child_iter_idx = retrohtr_node( tree, node_idx )->first_child;
      while( 0 <= child_iter_idx ) {
         assert( !mdata_vector_is_locked( &(parser->tags) ) );
         retval = retrohtr_apply_styles(
            parser, tree, &effect_style, &child_style,
            retrohtr_node( tree, child_iter_idx )->tag );
         maug_cleanup_if_not_ok();
         
         /* Skip ABSOLUTE nodes. */
         if( MCSS_POSITION_ABSOLUTE == child_style.POSITION ) {
            child_iter_idx = 
               retrohtr_node( tree, child_iter_idx )->next_sibling;
            continue;
         }

         if( MCSS_DISPLAY_BLOCK == child_style.DISPLAY ) {
            /* Reset the line width counter for coming BLOCK node. */
            this_line_w = 0;

            if(
               retrohtr_node( tree, child_iter_idx )->w >
                  retrohtr_node( tree, node_idx )->w
            ) {
               /* This BLOCK node is the longest yet! */
               retrohtr_node( tree, node_idx )->w =
                  retrohtr_node( tree, child_iter_idx )->w;
            }
         } else {
            /* Add inline node to this node line's width. */
            this_line_w += retrohtr_node( tree, child_iter_idx )->w;

            if( this_line_w > retrohtr_node( tree, node_idx )->w ) {
               /* The line of nodes we've been adding up is the longest yet! */
               retrohtr_node( tree, node_idx )->w = this_line_w;
            }
         }
         child_iter_idx = retrohtr_node( tree, child_iter_idx )->next_sibling;
      }
   }

   /* If our height is still zero, then size based on children. */
   if( 0 == retrohtr_node( tree, node_idx )->h ) {
      /* Cycle through children and add heights. */
      child_iter_idx = retrohtr_node( tree, node_idx )->first_child;
      while( 0 <= child_iter_idx ) {
         assert( !mdata_vector_is_locked( &(parser->tags) ) );
         retrohtr_apply_styles(
            parser, tree, &effect_style, &child_style,
            retrohtr_node( tree, child_iter_idx )->tag );

         /* Skip ABSOLUTE nodes. */
         if( MCSS_POSITION_ABSOLUTE == child_style.POSITION ) {
            child_iter_idx = retrohtr_node( tree, child_iter_idx )->next_sibling;
            continue;
         }

         if( MCSS_DISPLAY_BLOCK == child_style.DISPLAY ) {
            /* Add the last line to the running height. */
            retrohtr_node( tree, node_idx )->h += this_line_h;
            
            /* Start a new running line height with this BLOCK node. */
            this_line_h = retrohtr_node( tree, child_iter_idx )->h;
         } else {
            /* Make sure this line is at least as tall as this INLINE node. */
            if( this_line_h < retrohtr_node( tree, child_iter_idx )->h ) {
               this_line_h = retrohtr_node( tree, child_iter_idx )->h;
            }
         }

         child_iter_idx = retrohtr_node( tree, child_iter_idx )->next_sibling;
      }

      /* Add the last line height the node height. */
      retrohtr_node( tree, node_idx )->h += this_line_h;
      this_line_h = 0;
   }

   /* Apply additional modifiers (padding, etc) after children have all been
    * calculated.
    */

   /* Try specific left padding first, then try general padding. */
   if( mcss_prop_is_active_NOT_flag( effect_style.PADDING_LEFT, AUTO ) ) {
      retrohtr_node( tree, node_idx )->w += effect_style.PADDING_LEFT;
   } else if( mcss_prop_is_active_NOT_flag( effect_style.PADDING, AUTO ) ) {
      retrohtr_node( tree, node_idx )->w += effect_style.PADDING;
   }

   /* Try specific right padding first, then try general padding. */
   if( mcss_prop_is_active_NOT_flag( effect_style.PADDING_RIGHT, AUTO ) ) {
      retrohtr_node( tree, node_idx )->w += effect_style.PADDING_RIGHT;
   } else if( mcss_prop_is_active_NOT_flag( effect_style.PADDING, AUTO ) ) {
      retrohtr_node( tree, node_idx )->w += effect_style.PADDING;
   }

   /* Try specific top padding first, then try general padding. */
   if( mcss_prop_is_active_NOT_flag( effect_style.PADDING_TOP, AUTO ) ) {
      retrohtr_node( tree, node_idx )->h += effect_style.PADDING_TOP;
   } else if( mcss_prop_is_active_NOT_flag( effect_style.PADDING, AUTO ) ) {
      retrohtr_node( tree, node_idx )->h += effect_style.PADDING;
   }

   /* Try specific bottom padding first, then try general padding. */
   if( mcss_prop_is_active_NOT_flag( effect_style.PADDING_BOTTOM, AUTO ) ) {
      retrohtr_node( tree, node_idx )->h += effect_style.PADDING_BOTTOM;
   } else if( mcss_prop_is_active_NOT_flag( effect_style.PADDING, AUTO ) ) {
      retrohtr_node( tree, node_idx )->h += effect_style.PADDING;
   }

   debug_printf( RETROHTR_TRACE_LVL,
      "setting node " SIZE_T_FMT " dirty...", node_idx );
   retrohtr_node( tree, node_idx )->flags |= RETROHTR_NODE_FLAG_DIRTY;

cleanup:

   if( mdata_vector_is_locked( &(parser->tags) ) ) {
      mdata_vector_unlock( &(parser->tags) );
   }

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
#define retrohtr_break_on_active_pos( iter_idx ) \
   if( mcss_prop_is_active( retrohtr_node( tree, iter_idx )->pos ) ) { \
      break; \
   }

static ssize_t retrohtr_find_prev_sibling_in_box_model(
   struct RETROHTR_RENDER_TREE* tree,
   ssize_t node_idx
) {
   ssize_t sibling_iter_idx = -1;
   ssize_t sibling_found_idx = -1;

   if( 0 > retrohtr_node( tree, node_idx )->parent ) {
      /* Can't determine sibling! */
      goto cleanup;
   }
   
   sibling_iter_idx = retrohtr_node_parent( tree, node_idx )->first_child;

   if( sibling_iter_idx == node_idx ) {
      /* No previous siblings! */
      goto cleanup;
   }

   while( 0 <= sibling_iter_idx && node_idx != sibling_iter_idx ) {
      if(
         /* TODO: See MCSS_POS_NOTE. This is what we were talking about. */
         MCSS_POSITION_ABSOLUTE != retrohtr_node( tree, sibling_iter_idx )->pos
      ) {
         sibling_found_idx = sibling_iter_idx;
      }

      /* TODO: Reset on <br />? */

      sibling_iter_idx = retrohtr_node( tree, sibling_iter_idx )->next_sibling;
   }

cleanup:
   return sibling_found_idx;
}

static MERROR_RETVAL retrohtr_mark_edge_child_nodes(
   struct MHTML_PARSER* parser, struct RETROHTR_RENDER_TREE* tree,
   ssize_t node_parent_idx
) {
   ssize_t node_sibling_idx = -1;
   MERROR_RETVAL retval = MERROR_OK;
   struct MCSS_STYLE effect_style;
   size_t col_idx = 0; /* How many nodes right (X)? */
   size_t row_idx = 0; /* How many nodes down (Y)? */
   union MHTML_TAG* p_tag_iter = NULL;

   node_sibling_idx = retrohtr_node( tree, node_parent_idx )->first_child;
   while( 0 <= node_sibling_idx ) {
      maug_mzero( &effect_style, sizeof( struct MCSS_STYLE ) );
      retrohtr_apply_styles(
         parser, tree, NULL, &effect_style,
         retrohtr_node( tree, node_sibling_idx )->tag );

      if( MCSS_POSITION_ABSOLUTE == effect_style.POSITION ) {
         /* Absolute nodes are never on the edge. */
         retrohtr_node( tree, node_sibling_idx )->edge |= RETROHTR_EDGE_INSIDE;

      } else if( MCSS_DISPLAY_INLINE == effect_style.DISPLAY ) {
         /* Inline, or something that follows previous column. */
         if( 0 == col_idx ) {
            retrohtr_node( tree, node_sibling_idx )->edge |= RETROHTR_EDGE_LEFT;
         }
         if( 0 == row_idx ) {
            retrohtr_node( tree, node_sibling_idx )->edge |= RETROHTR_EDGE_TOP;
         }
         if( 0 < row_idx && 0 < col_idx ) {
            retrohtr_node( tree, node_sibling_idx )->edge |= RETROHTR_EDGE_INSIDE;
         }
         col_idx++;

      } else {
         /* Block element will be on the next line, so take that into account
          * when deciding the edge below.
          */
         row_idx++;
         col_idx = 0;

         /* Block, or something else in a new row. */
         if( 0 == row_idx ) {
            retrohtr_node( tree, node_sibling_idx )->edge |= RETROHTR_EDGE_TOP;
         } 

         /* Assume block is always on a new line. */
         retrohtr_node( tree, node_sibling_idx )->edge |= RETROHTR_EDGE_LEFT;
      }

      assert( !mdata_vector_is_locked( &(parser->tags) ) );
      mdata_vector_lock( &(parser->tags) );

      p_tag_iter = mdata_vector_get( &(parser->tags),
         retrohtr_node( tree, node_sibling_idx )->tag,
         union MHTML_TAG );
      assert( NULL != p_tag_iter );

      debug_printf( 1, "marking node " SIZE_T_FMT " (%s) edge: %u",
         node_sibling_idx,
         gc_mhtml_tag_names[p_tag_iter->base.type],
         retrohtr_node( tree, node_sibling_idx )->edge );

      mdata_vector_unlock( &(parser->tags) );

      node_sibling_idx = 
         retrohtr_node( tree, node_sibling_idx )->next_sibling;
   }

cleanup:

   if( mdata_vector_is_locked( &(parser->tags) ) ) {
      mdata_vector_unlock( &(parser->tags) );
   }

   return retval;
}

MERROR_RETVAL retrohtr_tree_pos(
   struct MHTML_PARSER* parser, struct RETROHTR_RENDER_TREE* tree,
   struct MCSS_STYLE* prev_sibling_style,
   struct MCSS_STYLE* parent_style, ssize_t node_idx, size_t d
) {
   struct MCSS_STYLE child_prev_sibling_style;
   struct MCSS_STYLE effect_style;
   ssize_t child_iter_idx = -1;
   ssize_t tag_idx = -1;
   ssize_t node_iter_idx = -1;
   ssize_t prev_sibling_idx = -1;
   MERROR_RETVAL retval = MERROR_OK;
   union MHTML_TAG* p_tag_iter = NULL;

   if( NULL == retrohtr_node( tree, node_idx ) ) {
      goto cleanup;
   }

   tag_idx = retrohtr_node( tree, node_idx )->tag;

   retrohtr_apply_styles(
      parser, tree, parent_style, &effect_style, tag_idx );

   prev_sibling_idx =
      retrohtr_find_prev_sibling_in_box_model( tree, node_idx );

   /* x */

   if( MCSS_POSITION_ABSOLUTE == effect_style.POSITION ) {
      /* This node is positioned absolutely. (Relatively) simple! */

      if( mcss_prop_is_active_NOT_flag( effect_style.LEFT, AUTO ) ) {

         child_iter_idx = retrohtr_node( tree, node_idx )->parent;
         while( 0 <= retrohtr_node( tree, child_iter_idx )->parent ) {
            retrohtr_break_on_active_pos( child_iter_idx );
            child_iter_idx = retrohtr_node( tree, child_iter_idx )->parent;
         }

         /* Set X to highest non-explicit ancestor. */
         retrohtr_node( tree, node_idx )->x =
            retrohtr_node( tree, child_iter_idx )->x + effect_style.LEFT;
      }
      if( mcss_prop_is_active_NOT_flag( effect_style.RIGHT, AUTO ) ) {

         child_iter_idx = retrohtr_node( tree, node_idx )->parent;
         while( 0 <= retrohtr_node( tree, child_iter_idx )->parent ) {
            retrohtr_break_on_active_pos( child_iter_idx );
            child_iter_idx = retrohtr_node( tree, child_iter_idx )->parent;
         }

         /* Set X to highest non-explicit ancestor. */
         retrohtr_node( tree, node_idx )->x = 
            retrohtr_node( tree, child_iter_idx )->w - 
            retrohtr_node( tree, node_idx )->w - 
            effect_style.RIGHT;
      }

   } else if(
      MCSS_DISPLAY_INLINE == effect_style.DISPLAY &&
      MCSS_DISPLAY_INLINE == prev_sibling_style->DISPLAY &&
      0 <= prev_sibling_idx
   ) {
      /* Place to the right of the previous sibling. */
      retrohtr_node( tree, node_idx )->x =
         retrohtr_node( tree, prev_sibling_idx )->x +
         retrohtr_node( tree, prev_sibling_idx )->w;

   } else if( 0 <= retrohtr_node( tree, node_idx )->parent ) {
      retrohtr_node( tree, node_idx )->x = retrohtr_node_parent( tree, node_idx )->x;
   }
   
   /* y */

   /* TODO: Add margins of children? */

   if( MCSS_POSITION_ABSOLUTE == effect_style.POSITION ) {
      /* This node is positioned absolutely. (Relatively) simple! */

      if( mcss_prop_is_active_NOT_flag( effect_style.TOP, AUTO ) ) {

         child_iter_idx = retrohtr_node( tree, node_idx )->parent;
         while( 0 <= retrohtr_node( tree, child_iter_idx )->parent ) {
            retrohtr_break_on_active_pos( child_iter_idx );
            child_iter_idx = retrohtr_node( tree, child_iter_idx )->parent;
         }

         /* Set Y to highest non-explicit ancestor. */
         retrohtr_node( tree, node_idx )->y = 
            retrohtr_node( tree, child_iter_idx )->y + effect_style.TOP;
      }
      if( mcss_prop_is_active_NOT_flag( effect_style.BOTTOM, AUTO ) ) {

         child_iter_idx = retrohtr_node( tree, node_idx )->parent;
         while( 0 <= retrohtr_node( tree, child_iter_idx )->parent ) {
            retrohtr_break_on_active_pos( child_iter_idx );
            child_iter_idx = retrohtr_node( tree, child_iter_idx )->parent;
         }

         /* Set Y to highest non-explicit ancestor. */
         retrohtr_node( tree, node_idx )->y = 
            retrohtr_node( tree, child_iter_idx )->h - 
            retrohtr_node( tree, node_idx )->h - 
            effect_style.BOTTOM;
      }

   } else if(
      MCSS_DISPLAY_INLINE == effect_style.DISPLAY &&
      MCSS_DISPLAY_INLINE == prev_sibling_style->DISPLAY &&
      0 <= prev_sibling_idx
   ) {
      /* Place to the right of the previous sibling. */
      retrohtr_node( tree, node_idx )->y = retrohtr_node( tree, prev_sibling_idx )->y;

   } else if( 0 <= prev_sibling_idx ) {
      /* Place below the previous block sibling. */

      /* TODO: We should probably use the tallest element on the prev sibling's
       *       line, but that seems hard...
       */

      retrohtr_node( tree, node_idx )->y =
         retrohtr_node( tree, prev_sibling_idx )->y +
         retrohtr_node( tree, prev_sibling_idx )->h;

   } else if( 0 <= retrohtr_node( tree, node_idx )->parent ) {
      /* Position relative to other nodes. */

      retrohtr_node( tree, node_idx )->y = retrohtr_node_parent( tree, node_idx )->y;
   }

   /* margin-left, margin-right */

   if( 
      MCSS_POSITION_ABSOLUTE != retrohtr_node( tree, node_idx )->pos &&
      0 <= retrohtr_node( tree, node_idx )->parent &&
      mcss_prop_is_active_flag( effect_style.MARGIN_LEFT, AUTO ) &&
      mcss_prop_is_active_flag( effect_style.MARGIN_RIGHT, AUTO )
   ) {
      /* Center */
      retrohtr_node( tree, node_idx )->x =
         retrohtr_node_parent( tree, node_idx )->x +
         (retrohtr_node_parent( tree, node_idx )->w >> 1) -
         (retrohtr_node( tree, node_idx )->w >> 1);

   } else if( 
      0 <= retrohtr_node( tree, node_idx )->parent &&
      mcss_prop_is_active_flag( effect_style.MARGIN_LEFT, AUTO ) &&
      mcss_prop_is_active_NOT_flag( effect_style.MARGIN_RIGHT, AUTO )
   ) {
      /* Justify right. */
      /* TODO: Subtract padding below, as well. */
      retrohtr_node( tree, node_idx )->x = 
         retrohtr_node_parent( tree, node_idx )->w -
         retrohtr_node( tree, node_idx )->w;

   } else if( mcss_prop_is_active( effect_style.MARGIN_LEFT ) ) {
      /* Justify left. */
      retrohtr_node( tree, node_idx )->x += effect_style.MARGIN_LEFT;
   }

   /* padding */

   /* TODO: Padding is still broken. Needs more involved understanding of
    *       where elements are in their container.
    */

   debug_printf( 1, "(d: " SIZE_T_FMT ") node " SIZE_T_FMT " is on edge: %u",
      d, node_idx, retrohtr_node( tree, node_idx )->edge );

   assert(
      0 == node_idx ||
      RETROHTR_EDGE_UNKNOWN != retrohtr_node( tree, node_idx )->edge );

   if(
      RETROHTR_EDGE_LEFT ==
         (RETROHTR_EDGE_LEFT & retrohtr_node( tree, node_idx )->edge)
   ) {
      /* Try specific left padding first, then try general padding. */
      if( mcss_prop_is_active_NOT_flag( parent_style->PADDING_LEFT, AUTO ) ) {
         retrohtr_node( tree, node_idx )->x += parent_style->PADDING_LEFT;
      } else if( mcss_prop_is_active_NOT_flag( parent_style->PADDING, AUTO ) ) {
         retrohtr_node( tree, node_idx )->x += parent_style->PADDING;
      }
   }

   if(
      RETROHTR_EDGE_TOP ==
         (RETROHTR_EDGE_TOP & retrohtr_node( tree, node_idx )->edge) &&
      /* Only apply padding to first node in line. The rest will pick it up. */
      RETROHTR_EDGE_LEFT ==
         (RETROHTR_EDGE_LEFT & retrohtr_node( tree, node_idx )->edge)
   ) {
      /* Try specific top padding first, then try general padding. */
      if( mcss_prop_is_active_NOT_flag( parent_style->PADDING_TOP, AUTO ) ) {
         retrohtr_node( tree, node_idx )->y += parent_style->PADDING_TOP;
      } else if( mcss_prop_is_active_NOT_flag( parent_style->PADDING, AUTO ) ) {
         retrohtr_node( tree, node_idx )->y += parent_style->PADDING;
      }
   }

   /* color */

   if( mcss_prop_is_active( effect_style.COLOR ) ) {
      retrohtr_node( tree, node_idx )->fg = effect_style.COLOR;
   }

   if( mcss_prop_is_active( effect_style.BACKGROUND_COLOR ) ) {
      retrohtr_node( tree, node_idx )->bg = effect_style.BACKGROUND_COLOR;
   }

   /* Figure out child positions. */

   retrohtr_mark_edge_child_nodes( parser, tree, node_idx );

   maug_mzero( &child_prev_sibling_style, sizeof( struct MCSS_STYLE ) );
   node_iter_idx = retrohtr_node( tree, node_idx )->first_child;
   while( 0 <= node_iter_idx ) {
      /* Mark child nodes on the edge so applying padding can be done. */

      /* Figure out child node positioning. */
      retrohtr_tree_pos(
         parser, tree, &child_prev_sibling_style, &effect_style,
         node_iter_idx, d + 1 );

      node_iter_idx = retrohtr_node( tree, node_iter_idx )->next_sibling;
   }

   assert( !mdata_vector_is_locked( &(parser->tags) ) );
   mdata_vector_lock( &(parser->tags) );
   p_tag_iter = mdata_vector_get( &(parser->tags), tag_idx, union MHTML_TAG );
   assert( NULL != p_tag_iter );

   if( MHTML_TAG_TYPE_INPUT == p_tag_iter->base.type ) {
      /* Feed the position back to the GUI control created during tree_size. */
      retval = retrogui_pos_ctl( &(tree->gui), node_idx,
         retrohtr_node_screen_x( tree, node_idx ),
         retrohtr_node_screen_y( tree, node_idx ),
         retrohtr_node( tree, node_idx )->w,
         retrohtr_node( tree, node_idx )->h );
      maug_cleanup_if_not_ok();
   }

   debug_printf( RETROHTR_TRACE_LVL,
      "setting node " SIZE_T_FMT " dirty...", node_idx );
   retrohtr_node( tree, node_idx )->flags |= RETROHTR_NODE_FLAG_DIRTY;
 
cleanup:

   if( mdata_vector_is_locked( &(parser->tags) ) ) {
      mdata_vector_unlock( &(parser->tags) );
   }

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

MERROR_RETVAL retrohtr_tree_draw(
   struct MHTML_PARSER* parser, struct RETROHTR_RENDER_TREE* tree,
   ssize_t node_idx, size_t d
) {
   char* strpool = NULL;
   union MHTML_TAG* p_tag = NULL;
   struct RETROHTR_RENDER_NODE* node = NULL;
   MERROR_RETVAL retval = MERROR_OK;

   node = retrohtr_node( tree, node_idx );

   if( NULL == node ) {
      return MERROR_OK;
   }

   /* TODO: Multi-pass, draw absolute pos afterwards. */

   if( 0 > node->tag ) {
      goto cleanup;
   }

   if( RETROHTR_NODE_FLAG_DIRTY != (RETROHTR_NODE_FLAG_DIRTY & node->flags) ) {
      goto cleanup;
   }

   assert( !mdata_vector_is_locked( &(parser->tags) ) );
   mdata_vector_lock( &(parser->tags) );

   p_tag = mdata_vector_get( &(parser->tags), node->tag, union MHTML_TAG );
   if( NULL == p_tag ) {
      goto cleanup;
   }

   /* Perform drawing. */
   if( MHTML_TAG_TYPE_TEXT == p_tag->base.type ) {

      if(
         0 > p_tag->TEXT.content_idx ||
#ifdef RETROGXC_PRESENT
         0 > node->font_idx
#else
         NULL == node->font_h
#endif /* RETROGXC_PRESENT */
      ) {
         goto cleanup;
      }

      mdata_strpool_lock( &(parser->strpool), strpool );

#ifdef RETROGXC_PRESENT
      retrogxc_string(
#else
      retrofont_string(
#endif /* RETROGXC_PRESENT */
         NULL, node->fg,
         &(strpool[p_tag->TEXT.content_idx]), p_tag->TEXT.content_sz,
#ifdef RETROGXC_PRESENT
         node->font_idx,
#else
         node->font_h,
#endif /* RETROGXC_PRESENT */
         retrohtr_node_screen_x( tree, node_idx ),
         retrohtr_node_screen_y( tree, node_idx ),
         node->w, node->h, 0 );

      mdata_strpool_unlock( &(parser->strpool), strpool );

   } else if( MHTML_TAG_TYPE_BODY == p_tag->base.type ) {

      debug_printf(
         RETROHTR_TRACE_LVL, "drawing BODY node " SIZE_T_FMT "...", node_idx );

      /* Draw body BG. */
      if( RETROFLAT_COLOR_NULL != node->bg ) {
         retroflat_rect(
            NULL, node->bg,
            retrohtr_node_screen_x( tree, node_idx ),
            retrohtr_node_screen_y( tree, node_idx ),
            retrohtr_node( tree, node_idx )->w,
            retrohtr_node( tree, node_idx )->h,
            RETROFLAT_FLAGS_FILL );
      }

   } else if( MHTML_TAG_TYPE_IMG == p_tag->base.type ) {
      /* Blit the image. */
      
      if( !retroflat_bitmap_ok( &(retrohtr_node( tree, node_idx )->bitmap) ) ) {
         goto cleanup;
      }

      debug_printf(
         RETROHTR_TRACE_LVL, "drawing IMG node " SIZE_T_FMT "...", node_idx );

      retroflat_blit_bitmap(
         NULL, &(retrohtr_node( tree, node_idx )->bitmap),
         0, 0,
         retrohtr_node_screen_x( tree, node_idx ),
         retrohtr_node_screen_y( tree, node_idx ),
         retroflat_bitmap_w( &(retrohtr_node( tree, node_idx )->bitmap) ),
         retroflat_bitmap_h( &(retrohtr_node( tree, node_idx )->bitmap) ),
         RETROFLAT_INSTANCE_NULL
         /* retrohtr_node( tree, node_idx )->w,
         retrohtr_node( tree, node_idx )->h */ );

   } else if( MHTML_TAG_TYPE_INPUT == p_tag->base.type ) {

      debug_printf(
         RETROHTR_TRACE_LVL, "setting tree GUI dirty..." );

      tree->gui.flags |= RETROGUI_FLAGS_DIRTY;

   } else {
      if( RETROFLAT_COLOR_NULL == node->bg ) {
         goto cleanup;
      }

      debug_printf(
         RETROHTR_TRACE_LVL, "drawing xs node " SIZE_T_FMT "...",
         /* gc_mhtml_tag_names[mhtml_tag( parser,
            retrohtr_node( tree, node_idx )->tag )->base.type], */
         node_idx );

      retroflat_rect(
         NULL, node->bg,
         retrohtr_node_screen_x( tree, node_idx ),
         retrohtr_node_screen_y( tree, node_idx ),
         node->w, node->h,
         RETROFLAT_FLAGS_FILL );
   }

   node->flags &= ~RETROHTR_NODE_FLAG_DIRTY;

cleanup:

   if( retrogui_is_locked( &(tree->gui) ) ) {
      retrogui_unlock( &(tree->gui) );
   }

   if( mdata_vector_is_locked( &(parser->tags) ) ) {
      mdata_vector_unlock( &(parser->tags) );
   }

   if( MERROR_OK != retval ) {
      error_printf( "failed drawing node: " SIZE_T_FMT, node_idx );
   }

   /* Keep trying to render children, tho. */  

   retrohtr_tree_draw( parser, tree, node->first_child, d + 1 );

   retrohtr_tree_draw( parser, tree, node->next_sibling, d );

   /* If this is the root redraw call, redraw GUI elements. */
   if(
      0  == d &&
      RETROHTR_TREE_FLAG_GUI_ACTIVE ==
         (tree->flags & RETROHTR_TREE_FLAG_GUI_ACTIVE)
   ) {
      retrogui_lock( &(tree->gui) );
      retrogui_redraw_ctls( &(tree->gui) );
      retrogui_unlock( &(tree->gui) );
   }

   return retval;
}

retrogui_idc_t retrohtr_tree_poll_ctls(
   struct RETROHTR_RENDER_TREE* tree,
   RETROFLAT_IN_KEY* input,
   struct RETROFLAT_INPUT* input_evt
) {
   retrogui_idc_t idc = 0;
   MERROR_RETVAL retval = MERROR_OK;

   assert( retrohtr_tree_is_locked( tree ) );

   if(
      RETROHTR_TREE_FLAG_GUI_ACTIVE !=
         (RETROHTR_TREE_FLAG_GUI_ACTIVE & tree->flags)
   ) {
      /* No GUI, so exit without even unlocking. */
      return 0;
   }

   idc = retrogui_poll_ctls( &(tree->gui), input, input_evt );

   if( 0 < idc ) {
      debug_printf(
         RETROHTR_TRACE_LVL, "setting node " SIZE_T_FMT " dirty...", idc );
      retrohtr_node( tree, idc )->flags |= RETROHTR_NODE_FLAG_DIRTY;
   }

   if( MERROR_OK != retval ) {
      idc = 0;
   }

   return idc;
}

MERROR_RETVAL retrohtr_tree_dump(
   struct RETROHTR_RENDER_TREE* tree, struct MHTML_PARSER* parser,
   ssize_t node_idx, size_t d
) {
   size_t i = 0;
   char indents[31];
   union MHTML_TAG* p_tag_iter = NULL;
   MERROR_RETVAL retval = MERROR_OK;

   if( 0 > node_idx ) {
      return MERROR_OK;
   }

   assert( !mdata_vector_is_locked( &(parser->tags) ) );
   mdata_vector_lock( &(parser->tags) );

   p_tag_iter = mdata_vector_get(
      &(parser->tags), tree->nodes[node_idx].tag, union MHTML_TAG );
   if( NULL == p_tag_iter ) {
      goto cleanup;
   }

   /* Generate the indentation. */
   maug_mzero( indents, 30 );
   for( i = 0 ; d > i ; i++ ) {
      if( maug_strlen( indents ) >= 30 ) {
         break;
      }
      strcat( indents, "   " );
   }

   /* Print the debug line. */
   debug_printf(
      1,
      "%s" SSIZE_T_FMT " (tag %s): x: " SSIZE_T_FMT ", y: " SSIZE_T_FMT
      " (" SSIZE_T_FMT " x " SSIZE_T_FMT ") f: "
#ifdef RETROGXC_PRESENT
      SSIZE_T_FMT,
#else
      "%p",
#endif /* RETROGXC_PRESENT */
      indents, node_idx,
      0 <= tree->nodes[node_idx].tag ?
         gc_mhtml_tag_names[p_tag_iter->base.type] : "ROOT",
      tree->nodes[node_idx].x, tree->nodes[node_idx].y,
      tree->nodes[node_idx].w, tree->nodes[node_idx].h,
#ifdef RETROGXC_PRESENT
      tree->nodes[node_idx].font_idx
#else
      tree->nodes[node_idx].font_h
#endif /* RETROGXC_PRESENT */
   );

   mdata_vector_unlock( &(parser->tags) );

   retval = retrohtr_tree_dump(
      tree, parser, tree->nodes[node_idx].first_child, d + 1 );
   maug_cleanup_if_not_ok();

   retval = retrohtr_tree_dump(
      tree, parser, tree->nodes[node_idx].next_sibling, d );
   maug_cleanup_if_not_ok();

cleanup:

   return retval;
}

void retrohtr_tree_free( struct RETROHTR_RENDER_TREE* tree ) {

   debug_printf( RETROHTR_TRACE_LVL, "freeing render nodes..." );

   /* TODO: Free bitmaps from img! */

   /* TODO: Free node->font_h! */

   /* Free GUI if present. */
   if(
      RETROHTR_TREE_FLAG_GUI_ACTIVE ==
         (tree->flags & RETROHTR_TREE_FLAG_GUI_ACTIVE)
   ) {
      retrogui_free( &(tree->gui) );
   }

   /* Unlock nodes before trying to free them. */
   retrohtr_tree_unlock( tree );

   if( NULL != tree->nodes_h ) {
      maug_mfree( tree->nodes_h );
   }
}

MERROR_RETVAL retrohtr_tree_init( struct RETROHTR_RENDER_TREE* tree ) {
   MERROR_RETVAL retval = MERROR_OK;

   maug_mzero( tree, sizeof( struct RETROHTR_RENDER_TREE ) );

   /* Perform initial node allocation. */
   tree->nodes_sz_max = MHTML_PARSER_TAGS_INIT_SZ;
   debug_printf( RETROHTR_TRACE_LVL,
      "allocating " SIZE_T_FMT " nodes...", tree->nodes_sz_max );
   tree->nodes_h = maug_malloc(
      tree->nodes_sz_max, sizeof( struct RETROHTR_RENDER_NODE ) );
   maug_cleanup_if_null_alloc( struct RETROHTR_RENDER_NODE*, tree->nodes_h );

   /* XXX
   r.w_max = retroflat_screen_w();
   r.h_max = retroflat_screen_h(); */

cleanup:

   return retval;  
}

#endif /* RETROHTR_C */

#endif /* !RETROHTR_H */

