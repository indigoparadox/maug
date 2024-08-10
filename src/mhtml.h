
#ifndef MHTML_H
#define MHTML_H

#ifndef MHTML_PARSER_TAGS_INIT_SZ
#  define MHTML_PARSER_TAGS_INIT_SZ 10
#endif /* !MHTML_PARSER_TAGS_INIT_SZ */

#ifdef MHTML_C
#  define MCSS_C
#endif /* MHTML_C */

#ifndef MHTML_DUMP_LINE_SZ
#  define MHTML_DUMP_LINE_SZ 255
#endif /* !MHTML_DUMP_LINE_SZ */

#ifndef MHTML_SRC_HREF_SZ_MAX
#  define MHTML_SRC_HREF_SZ_MAX 128
#endif /* !MHTML_SRC_HREF_SZ_MAX */

#ifndef MHTML_TRACE_LVL
#  define MHTML_TRACE_LVL 0
#endif /* !MHTML_TRACE_LVL */

/*! \brief Indicates a text tag contains CSS information to be parsed. */
#define MHTML_TAG_FLAG_STYLE  0x02

#define MHTML_INPUT_TYPE_BUTTON  0x01

#include <mparser.h>
#include <mcss.h>

#define MHTML_ATTRIB_TABLE( f ) \
   f( NONE, 0 ) \
   f( STYLE, 1 ) \
   f( CLASS, 2 ) \
   f( ID, 3 ) \
   f( NAME, 4 ) \
   f( SRC, 5 ) \
   f( TYPE, 6 ) \
   f( VALUE, 7 )

#define MHTML_TAG_TABLE( f ) \
   f(  0, NONE, void* none;, NONE ) \
   f(  1, BODY, void* none;, BLOCK ) \
   f(  2, DIV, void* none;, BLOCK ) \
   f(  3, HEAD, void* none;, NONE ) \
   f(  4, HTML, void* none;, BLOCK ) \
   f(  5, TEXT, MAUG_MHANDLE content; size_t content_sz;, INLINE ) \
   f(  6, TITLE, MAUG_MHANDLE content; size_t content_sz;, NONE ) \
   f(  7, SPAN, void* none;, INLINE ) \
   f(  8, BR, void* none;, BLOCK ) \
   f(  9, STYLE, void* none;, NONE ) \
   f( 10, IMG, char src[MHTML_SRC_HREF_SZ_MAX + 1]; size_t src_sz;, BLOCK ) \
   f( 11, INPUT, uint8_t input_type; char name[MCSS_ID_SZ_MAX + 1]; size_t name_sz; char value[MCSS_ID_SZ_MAX + 1]; size_t value_sz;, INLINE )

#define MHTML_PARSER_PSTATE_TABLE( f ) \
   f( MHTML_PSTATE_NONE, 0 ) \
   f( MHTML_PSTATE_ELEMENT, 1 ) \
   f( MHTML_PSTATE_ATTRIB_KEY, 2 ) \
   f( MHTML_PSTATE_ATTRIB_VAL, 3 ) \
   f( MHTML_PSTATE_END_ELEMENT, 4 ) \
   f( MHTML_PSTATE_STRING, 5 ) \
   f( MHTML_PSTATE_STYLE, 6 )

/* TODO: Function names should be verb_noun! */

#if 0
#define mhtml_tag( parser, idx ) (&((parser)->tags[idx]))

#define mhtml_tag_parent( parser, idx ) \
   (0 <= (parser)->tags[idx].parent ? \
      (&((parser)->tags[(parser)->tags[idx].parent]])) : NULL)

#define mhtml_tag_child( parser, idx ) \
   (0 <= (parser)->tags[idx].first_child ? \
      (&((parser)->tags[(parser)->tags[idx].first_child]])) : NULL)

#define mhtml_tag_sibling( parser, idx ) \
   (0 <= (parser)->tags[idx].next_sibling ? \
      (&((parser)->tags[(parser)->tags[idx].next_sibling]])) : NULL)
#endif

#define mhtml_parser_pstate( parser ) \
   mparser_pstate( &((parser)->base) )

#ifdef MPARSER_TRACE_NAMES
#  define mhtml_parser_pstate_push( parser, new_pstate ) \
      mparser_pstate_push( \
         "mhtml", &((parser)->base), new_pstate, gc_mhtml_pstate_names )

#  define mhtml_parser_pstate_pop( parser ) \
      mparser_pstate_pop( \
         "mhtml", &((parser)->base), gc_mhtml_pstate_names )
#else
#  define mhtml_parser_pstate_push( parser, new_pstate ) \
      mparser_pstate_push( "mhtml", &((parser)->base), new_pstate )

#  define mhtml_parser_pstate_pop( parser ) \
      mparser_pstate_pop( "mhtml", &((parser)->base) )
#endif /* MPARSER_TRACE_NAMES */

#define mhtml_parser_invalid_c( parser, c, retval ) \
   mparser_invalid_c( mhtml, &((parser)->base), c, retval )

#define mhtml_parser_reset_token( parser ) \
   mparser_reset_token( "mhtml", &((parser)->base) )

#define mhtml_parser_append_token( parser, c ) \
   mparser_append_token( "mhtml", &((parser)->base), c )

#define mhtml_parser_lock( parser ) \
   mdata_vector_lock( &((parser)->tags) );

#define mhtml_parser_unlock( parser ) \
   mdata_vector_unlock( &((parser)->tags) );

#define mhtml_parser_set_tag_iter( parser, iter ) \
   debug_printf( MHTML_TRACE_LVL, "setting tag_iter to: " SSIZE_T_FMT \
      " (previously: " SSIZE_T_FMT ")", (ssize_t)iter, (parser)->tag_iter ); \
   (parser)->tag_iter = iter;

#define mhtml_parser_is_locked( parser ) (NULL != (parser)->tags)

struct MHTML_TAG_BASE {
   uint16_t type;
   uint8_t flags;
   ssize_t parent;
   ssize_t first_child;
   ssize_t next_sibling;
   ssize_t style;
   /* TODO: Use str_stable for classes. */
   char classes[MCSS_CLASS_SZ_MAX + 1];
   size_t classes_sz;
   /* TODO: Use str_stable for id. */
   char id[MCSS_ID_SZ_MAX + 1];
   size_t id_sz;
};

#define MHTML_TAG_TABLE_STRUCT( tag_id, tag_name, fields, disp ) \
   struct MHTML_TAG_ ## tag_name { \
      struct MHTML_TAG_BASE base; \
      fields \
   };

MHTML_TAG_TABLE( MHTML_TAG_TABLE_STRUCT )

#define MHTML_TAG_TABLE_UNION_FIELD( tag_id, tag_name, fields, disp ) \
   struct MHTML_TAG_ ## tag_name tag_name;

union MHTML_TAG {
   struct MHTML_TAG_BASE base; /* Should line up w/ 1st "base" in all types. */
   MHTML_TAG_TABLE( MHTML_TAG_TABLE_UNION_FIELD )
};

struct MHTML_PARSER {
   struct MPARSER base;
   uint16_t attrib_key;
   ssize_t tag_iter;
   /**
    * \brief Flags to be pushed to MHTML_TAG_BASE::flags on next 
    *        mhtml_push_tag().
    */
   uint8_t tag_flags;
   struct MCSS_PARSER styler;
   struct MDATA_STRPOOL strpool;
   struct MDATA_VECTOR tags;
   ssize_t body_idx;
};

MERROR_RETVAL mhtml_parser_free( struct MHTML_PARSER* parser );

MERROR_RETVAL mhtml_pop_tag( struct MHTML_PARSER* parser );

MERROR_RETVAL mhtml_parse_c( struct MHTML_PARSER* parser, char c );

MERROR_RETVAL mhtml_parser_init( struct MHTML_PARSER* parser );

MERROR_RETVAL mhtml_dump_tree(
   struct MHTML_PARSER* parser, ssize_t iter, size_t d );

#ifdef MHTML_C

#define MHTML_PSTATE_TABLE_CONST( name, idx ) \
   MAUG_CONST uint8_t SEG_MCONST name = idx;

MHTML_PARSER_PSTATE_TABLE( MHTML_PSTATE_TABLE_CONST )

MPARSER_PSTATE_NAMES( MHTML_PARSER_PSTATE_TABLE, mhtml )

#define MHTML_TAG_TABLE_CONST( tag_id, tag_name, fields, disp ) \
   MAUG_CONST uint16_t SEG_MCONST MHTML_TAG_TYPE_ ## tag_name = tag_id;

MHTML_TAG_TABLE( MHTML_TAG_TABLE_CONST )

#define MHTML_TAG_TABLE_NAMES( tag_id, tag_name, fields, disp ) \
   #tag_name,

MAUG_CONST char* SEG_MCONST gc_mhtml_tag_names[] = {
   MHTML_TAG_TABLE( MHTML_TAG_TABLE_NAMES )
   ""
};

#define MHTML_ATTRIB_TABLE_NAME( name, idx ) \
   #name,

static MAUG_CONST char* SEG_MCONST gc_mhtml_attrib_names[] = {
   MHTML_ATTRIB_TABLE( MHTML_ATTRIB_TABLE_NAME )
   ""
};

#define MHTML_ATTRIB_TABLE_NAME_CONST( attrib_name, attrib_id ) \
   MAUG_CONST uint16_t SEG_MCONST MHTML_ATTRIB_KEY_ ## attrib_name = attrib_id;

MHTML_ATTRIB_TABLE( MHTML_ATTRIB_TABLE_NAME_CONST )

MERROR_RETVAL mhtml_parser_free( struct MHTML_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   union MHTML_TAG* tag_iter = NULL;

   debug_printf( MHTML_TRACE_LVL, "freeing HTML parser..." );

   mhtml_parser_lock( parser );

   while( 0 < mdata_vector_ct( &(parser->tags) ) ) {
      tag_iter = mdata_vector_get( &(parser->tags), 0, union MHTML_TAG );
      assert( NULL != tag_iter );
      if(
         (
            MHTML_TAG_TYPE_TEXT == tag_iter->base.type ||
            MHTML_TAG_TYPE_STYLE == tag_iter->base.type
         ) &&
         (MAUG_MHANDLE)NULL != tag_iter->TEXT.content
      ) {
         maug_mfree( tag_iter->TEXT.content );
      }

      mdata_vector_unlock( &(parser->tags) );
      mdata_vector_remove( &(parser->tags), 0 );
      mdata_vector_lock( &(parser->tags) );
   }

cleanup:

   mcss_parser_free( &(parser->styler) );

   if( mdata_vector_is_locked( &(parser->tags) ) ) {
      mdata_vector_unlock( &(parser->tags) );
   }

   mdata_vector_free( &(parser->tags) );

   return retval;
}

MERROR_RETVAL mhtml_pop_tag( struct MHTML_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   union MHTML_TAG* tag_iter = NULL;

   /* Move up from current tag. */
   assert( parser->tag_iter >= 0 );
   mdata_vector_lock( &(parser->tags) );
   tag_iter = mdata_vector_get(
      &(parser->tags), parser->tag_iter, union MHTML_TAG );
   assert( NULL != tag_iter );

   mhtml_parser_set_tag_iter( parser, tag_iter->base.parent );

   if( 0 <= parser->tag_iter ) {
      debug_printf( MHTML_TRACE_LVL,
         "moved iter back to tag %s (" SIZE_T_FMT ")",
         gc_mhtml_tag_names[tag_iter->base.type], parser->tag_iter );
   } else {
      debug_printf( MHTML_TRACE_LVL, "moved iter back to root (-1)" );
   }

cleanup:

   mdata_vector_unlock( &(parser->tags) );

   return retval;
}

MERROR_RETVAL mhtml_push_tag( struct MHTML_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t new_tag_idx = -1;
   ssize_t next_sibling_idx = -1;
   union MHTML_TAG tag_new;
   union MHTML_TAG* p_tag_new = NULL;
   union MHTML_TAG* p_tag_iter = NULL;

   maug_mzero( &tag_new, sizeof( union MHTML_TAG ) );
   tag_new.base.parent = -1;
   tag_new.base.first_child = -1;
   tag_new.base.next_sibling = -1;
   tag_new.base.style = -1;
   tag_new.base.flags = parser->tag_flags;
   parser->tag_flags = 0;

   new_tag_idx = mdata_vector_append(
      &(parser->tags), &tag_new, sizeof( union MHTML_TAG ) );
   if( 0 > new_tag_idx ) {
      retval = mdata_retval( new_tag_idx );
      goto cleanup;
   }

   mdata_vector_lock( &(parser->tags) );
   p_tag_new = mdata_vector_get(
      &(parser->tags), new_tag_idx, union MHTML_TAG );
   assert( NULL != p_tag_new );

   if( 0 > parser->tag_iter ) {
      mhtml_parser_set_tag_iter( parser, new_tag_idx );
      goto cleanup;
   }

   /* Get the current tag_iter. */
   p_tag_iter = mdata_vector_get(
      &(parser->tags), parser->tag_iter, union MHTML_TAG );
   assert( NULL != p_tag_iter );

   /* Set new tag parent to current tag. */
   p_tag_new->base.parent = parser->tag_iter;

   /* Add new tag to current tag's children. */
   if( 0 > p_tag_iter->base.first_child ) {
      debug_printf( MHTML_TRACE_LVL,
         "zxzx attached " SSIZE_T_FMT " as first child to  "
         SSIZE_T_FMT, new_tag_idx, parser->tag_iter );
      p_tag_iter->base.first_child = new_tag_idx;
   } else {
      /* Find the last sibling child. */
      next_sibling_idx = p_tag_iter->base.first_child;
      p_tag_iter = mdata_vector_get(
         &(parser->tags), next_sibling_idx, union MHTML_TAG );
      while( NULL != p_tag_iter && 0 <= p_tag_iter->base.next_sibling ) {
         next_sibling_idx = p_tag_iter->base.next_sibling;
         p_tag_iter = mdata_vector_get(
            &(parser->tags), next_sibling_idx, union MHTML_TAG );
      }
      assert( NULL != p_tag_iter );
      p_tag_iter->base.next_sibling = new_tag_idx;
      debug_printf( MHTML_TRACE_LVL,
         "attached " SSIZE_T_FMT " as next sibling to  "
         SSIZE_T_FMT, new_tag_idx, next_sibling_idx );
   }

   debug_printf( MHTML_TRACE_LVL,
      "pushed new tag " SSIZE_T_FMT " under " SSIZE_T_FMT,
      new_tag_idx, p_tag_new->base.parent );

   mhtml_parser_set_tag_iter( parser, new_tag_idx );

cleanup:

   mdata_vector_unlock( &(parser->tags) );

   return retval;
}

MERROR_RETVAL mhtml_push_element_tag( struct MHTML_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   union MHTML_TAG* p_tag_iter = NULL;

   mparser_token_upper( &((parser)->base), i );

   if( 0 == strncmp( "STYLE", parser->base.token, 6 ) ) {
      /* Special case: style tag. Don't push a new tag here, but set a flag for
       * the text tag next created by mhtml_push_tag() so the contents are
       * directly attached to the style tag.
       */
      parser->tag_flags |= MHTML_TAG_FLAG_STYLE;
      goto cleanup;
   }

   retval = mhtml_push_tag( parser );
   maug_cleanup_if_not_ok();

   mdata_vector_lock( &(parser->tags) );

   p_tag_iter = mdata_vector_get(
      &(parser->tags), parser->tag_iter, union MHTML_TAG );
   assert( NULL != p_tag_iter );

   /* Figure out tag type. */
   i = 0;
   while( '\0' != gc_mhtml_tag_names[i][0] ) {
      if(
         parser->base.token_sz == maug_strlen( gc_mhtml_tag_names[i] ) &&
         0 == strncmp(
            gc_mhtml_tag_names[i], parser->base.token, parser->base.token_sz )
      ) {
         debug_printf( MHTML_TRACE_LVL,
            "new tag (" SSIZE_T_FMT ") type: %s",
            parser->tag_iter, gc_mhtml_tag_names[i] );
         p_tag_iter->base.type = i;

         if( MHTML_TAG_TYPE_BODY == i ) {
            /* Special case: body tag. Keep track of it for later so it can
             * be passed to the renderer.
             */
            assert( -1 == parser->body_idx );
            parser->body_idx = parser->tag_iter;
            debug_printf( MHTML_TRACE_LVL,
               "set body index to: " SSIZE_T_FMT,
               parser->body_idx );
         }

         goto cleanup;
      }
      i++;
   }

   error_printf( "could not find type for new tag (" SSIZE_T_FMT ")",
      parser->tag_iter );

cleanup:

   if( mdata_vector_is_locked( &(parser->tags) ) ) {
      mdata_vector_unlock( &(parser->tags) );
   }

   return retval;
}

MERROR_RETVAL mhtml_push_text_tag( struct MHTML_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   char* tag_content = NULL;
   size_t i = 0;
   union MHTML_TAG* p_tag_iter = NULL;

   /* TODO: Move text to strpool. */

   retval = mhtml_push_tag( parser );
   maug_cleanup_if_not_ok();
    
   mdata_vector_lock( &(parser->tags) );

   p_tag_iter = mdata_vector_get(
      &(parser->tags), parser->tag_iter, union MHTML_TAG );
   assert( NULL != p_tag_iter );

   if(
      MHTML_TAG_FLAG_STYLE == (MHTML_TAG_FLAG_STYLE & 
         p_tag_iter->base.flags)
   ) {
      p_tag_iter->base.type = MHTML_TAG_TYPE_STYLE;
   } else {
      p_tag_iter->base.type = MHTML_TAG_TYPE_TEXT;
   }

   /* Allocate text memory. */
   /* TODO: Switch to strpool. */
   p_tag_iter->TEXT.content =
      maug_malloc( parser->base.token_sz + 1, 1 );
   maug_cleanup_if_null_alloc(
      MAUG_MHANDLE, p_tag_iter->TEXT.content );
   maug_mlock( p_tag_iter->TEXT.content, tag_content );
   maug_cleanup_if_null_alloc( char*, tag_content );

   if( MHTML_TAG_TYPE_STYLE == p_tag_iter->base.type ) {
      /* TODO: If it's the last character and there's still a token, process it! */
      debug_printf( MHTML_TRACE_LVL, "parsing STYLE tag..." );
      for( ; parser->base.token_sz > i ; i++ ) {
         retval = mcss_parse_c( &(parser->styler), parser->base.token[i] );
         maug_cleanup_if_not_ok();
      }
      debug_printf( 1, "out of style characters..." );
      mcss_parser_flush( &(parser->styler) );
      mcss_parser_reset( &(parser->styler) );
   } else {
      /* Eliminate trailing spaces. */
      while( ' ' == parser->base.token[parser->base.token_sz - 1] ) {
         parser->base.token_sz--;
      }

      /* Copy token to tag text. */
      maug_strncpy( tag_content, parser->base.token, parser->base.token_sz );
      tag_content[parser->base.token_sz] = '\0';
      p_tag_iter->TEXT.content_sz = parser->base.token_sz;
   }

   debug_printf( 1, "done processing tag contents..." );

   maug_munlock( p_tag_iter->TEXT.content, tag_content );

cleanup:

   if( mdata_vector_is_locked( &(parser->tags) ) ) {
      mdata_vector_unlock( &(parser->tags) );
   }

   return retval;
}

MERROR_RETVAL mhtml_push_attrib_key( struct MHTML_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   debug_printf( MHTML_TRACE_LVL, "attrib: %s", parser->base.token );

   mparser_token_upper( &((parser)->base), i );

   /* Figure out attrib type. */
   i = 0;
   while( '\0' != gc_mhtml_attrib_names[i][0] ) {
      if(
         parser->base.token_sz == maug_strlen( gc_mhtml_attrib_names[i] ) &&
         0 == strncmp(
            gc_mhtml_attrib_names[i], parser->base.token, parser->base.token_sz )
      ) {
         debug_printf(
            MHTML_TRACE_LVL, "new attrib type: %s", gc_mhtml_attrib_names[i] );
         parser->attrib_key = i;
         goto cleanup;
      }
      i++;
   }

   error_printf( "unknown attrib: %s", parser->base.token );

cleanup:

   return retval;
}

static MERROR_RETVAL _mhtml_set_attrib_val( struct MHTML_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;
   union MHTML_TAG* p_tag_iter = NULL;

   mdata_vector_lock( &(parser->tags) );

   p_tag_iter = mdata_vector_get(
      &(parser->tags), parser->tag_iter, union MHTML_TAG );
   assert( NULL != p_tag_iter );

   if( MHTML_ATTRIB_KEY_STYLE == parser->attrib_key ) {
      debug_printf( MHTML_TRACE_LVL, "style: %s", parser->base.token );
      /* TODO: Parse and attach style. */

      /* Create an empty new style. */
      mhtml_parser_unlock( parser );
      retval = mcss_push_style( &(parser->styler), MCSS_SELECT_NONE, NULL, 0 );
      maug_cleanup_if_not_ok();
      mhtml_parser_lock( parser );
      
      /* Set the new style as this tag's explicit style. */
      p_tag_iter->base.style =
         mdata_vector_ct( &(parser->styler.styles) ) - 1;

      for( ; parser->base.token_sz > i ; i++ ) {
         retval = mcss_parse_c( &(parser->styler), parser->base.token[i] );
         maug_cleanup_if_not_ok();
      }

      debug_printf( 1, "out of style characters..." );
      mcss_parser_flush( &(parser->styler) );

      goto cleanup;

   } else if( MHTML_ATTRIB_KEY_CLASS == parser->attrib_key ) {
      maug_strncpy(
         p_tag_iter->base.classes,
         parser->base.token,
         MCSS_CLASS_SZ_MAX );
      p_tag_iter->base.classes_sz = parser->base.token_sz;

   } else if( MHTML_ATTRIB_KEY_ID == parser->attrib_key ) {
      maug_strncpy(
         p_tag_iter->base.id,
         parser->base.token,
         MCSS_ID_SZ_MAX );
      p_tag_iter->base.id_sz = parser->base.token_sz;

   } else if( MHTML_ATTRIB_KEY_SRC == parser->attrib_key ) {
      /* TODO: Validate tag type. */
      maug_strncpy(
         p_tag_iter->IMG.src,
         parser->base.token,
         MHTML_SRC_HREF_SZ_MAX );
      p_tag_iter->IMG.src_sz = parser->base.token_sz;

   } else if( MHTML_ATTRIB_KEY_TYPE == parser->attrib_key ) {
      /* TODO: Validate tag type. */

      if( 0 == maug_strncpy( parser->base.token, "button", 7 ) ) {
         p_tag_iter->INPUT.input_type =
            MHTML_INPUT_TYPE_BUTTON;
      }

   } else if( MHTML_ATTRIB_KEY_NAME == parser->attrib_key ) {
      /* TODO: Validate tag type. */
      maug_strncpy(
         p_tag_iter->INPUT.name,
         parser->base.token,
         MCSS_ID_SZ_MAX );
      p_tag_iter->INPUT.name_sz = parser->base.token_sz;

   } else if( MHTML_ATTRIB_KEY_VALUE == parser->attrib_key ) {
      /* TODO: Validate tag type. */
      maug_strncpy(
         p_tag_iter->INPUT.value,
         parser->base.token,
         MCSS_ID_SZ_MAX );
      p_tag_iter->INPUT.value_sz = parser->base.token_sz;
   }

cleanup:

   if( mdata_vector_is_locked( &(parser->tags) ) ) {
      mdata_vector_unlock( &(parser->tags) );
   }

   return retval;
}

MERROR_RETVAL mhtml_parse_c( struct MHTML_PARSER* parser, char c ) {
   MERROR_RETVAL retval = MERROR_OK;
   union MHTML_TAG* p_tag_iter = NULL;
   size_t tag_iter_type = 0;

   switch( c ) {
   case '<':
      if( MHTML_PSTATE_NONE == mhtml_parser_pstate( parser ) ) {
         if( 0 < parser->base.token_sz ) {
            retval = mhtml_push_text_tag( parser );
            maug_cleanup_if_not_ok();

            /* Grab the current tag to check its type below. */
            mdata_vector_lock( &(parser->tags) );
            p_tag_iter = mdata_vector_get(
               &(parser->tags), parser->tag_iter, union MHTML_TAG );
            assert( NULL != p_tag_iter );
            tag_iter_type = p_tag_iter->base.type;
            mdata_vector_unlock( &(parser->tags) );

            if(
               /* See special exception in mhtml_push_tag(). Style tags don't
                * push their subordinate text, so popping here would be
                * uneven!
                */
               MHTML_TAG_TYPE_STYLE != tag_iter_type
            ) {
               /* Pop out of text so next tag isn't a child of it. */
               retval = mhtml_pop_tag( parser );
               maug_cleanup_if_not_ok();
            }
         }
         retval = mhtml_parser_pstate_push( parser, MHTML_PSTATE_ELEMENT );
         maug_cleanup_if_not_ok();
         mhtml_parser_reset_token( parser );

      } else {
         mhtml_parser_invalid_c( parser, c, retval );
      }
      break;     

   case '>':
      if( MHTML_PSTATE_ELEMENT == mhtml_parser_pstate( parser ) ) {
         retval = mhtml_push_element_tag( parser );
         maug_cleanup_if_not_ok();
         mhtml_parser_pstate_pop( parser );
         mhtml_parser_reset_token( parser );

      } else if( MHTML_PSTATE_ATTRIB_KEY == mhtml_parser_pstate( parser ) ) {
         mhtml_parser_pstate_pop( parser );
         assert( MHTML_PSTATE_ELEMENT == mhtml_parser_pstate( parser ) );
         mhtml_parser_pstate_pop( parser ); /* Pop element. */
         mhtml_parser_reset_token( parser );

      } else if( MHTML_PSTATE_END_ELEMENT == mhtml_parser_pstate( parser ) ) {

         retval = mhtml_pop_tag( parser );
         maug_cleanup_if_not_ok();
         
         mhtml_parser_pstate_pop( parser );
         if( MHTML_PSTATE_ATTRIB_KEY == mhtml_parser_pstate( parser ) ) {
            mhtml_parser_pstate_pop( parser );
         }
         assert( MHTML_PSTATE_ELEMENT == mhtml_parser_pstate( parser ) );
         mhtml_parser_pstate_pop( parser ); /* Pop element. */
         mhtml_parser_reset_token( parser );

      } else if( MHTML_PSTATE_STRING == mhtml_parser_pstate( parser ) ) {
         retval = mhtml_parser_append_token( parser, c );
         maug_cleanup_if_not_ok();

      } else if( MHTML_PSTATE_NONE == mhtml_parser_pstate( parser ) ) {
         retval = mhtml_parser_append_token( parser, c );
         maug_cleanup_if_not_ok();

      } else {
         mhtml_parser_invalid_c( parser, c, retval );
      }
      break;

   case '/':
      if(
         MHTML_PSTATE_ELEMENT == mhtml_parser_pstate( parser ) &&
         0 == parser->base.token_sz
      ) {
         /* Start of a close tag. */
         retval = mhtml_parser_pstate_push( parser, MHTML_PSTATE_END_ELEMENT );
         maug_cleanup_if_not_ok();

      } else if( MHTML_PSTATE_ATTRIB_KEY == mhtml_parser_pstate( parser ) ) {
         /* Close of a self-closing tag. */
         retval = mhtml_parser_pstate_push( parser, MHTML_PSTATE_END_ELEMENT );
         maug_cleanup_if_not_ok();

      } else if( MHTML_PSTATE_STRING == mhtml_parser_pstate( parser ) ) {
         retval = mhtml_parser_append_token( parser, c );
         maug_cleanup_if_not_ok();

      } else if( MHTML_PSTATE_NONE == mhtml_parser_pstate( parser ) ) {
         retval = mhtml_parser_append_token( parser, c );
         maug_cleanup_if_not_ok();

      } else {
         mhtml_parser_invalid_c( parser, c, retval );
      }
      break;

   case '=':
      if( MHTML_PSTATE_ATTRIB_KEY == mhtml_parser_pstate( parser ) ) {
         retval = mhtml_push_attrib_key( parser );
         maug_cleanup_if_not_ok();
         retval = mhtml_parser_pstate_push( parser, MHTML_PSTATE_ATTRIB_VAL );
         maug_cleanup_if_not_ok();
         mhtml_parser_reset_token( parser );

      } else if( MHTML_PSTATE_ATTRIB_VAL == mhtml_parser_pstate( parser ) ) {
         retval = mhtml_parser_append_token( parser, c );
         maug_cleanup_if_not_ok();

      } else if( MHTML_PSTATE_NONE == mhtml_parser_pstate( parser ) ) {
         retval = mhtml_parser_append_token( parser, c );
         maug_cleanup_if_not_ok();

      } else {
         mhtml_parser_invalid_c( parser, '_', retval );
      }
      break;

   case '"':
      if( MHTML_PSTATE_ATTRIB_VAL == mhtml_parser_pstate( parser ) ) {
         retval = mhtml_parser_pstate_push( parser, MHTML_PSTATE_STRING );
         maug_cleanup_if_not_ok();
         mhtml_parser_reset_token( parser );

      } else if( MHTML_PSTATE_STRING == mhtml_parser_pstate( parser ) ) {
         retval = _mhtml_set_attrib_val( parser );
         maug_cleanup_if_not_ok();
         mhtml_parser_pstate_pop( parser );
         assert( MHTML_PSTATE_ATTRIB_VAL == mhtml_parser_pstate( parser ) );
         mhtml_parser_pstate_pop( parser );
         mhtml_parser_reset_token( parser );

      } else if( MHTML_PSTATE_NONE == mhtml_parser_pstate( parser ) ) {
         retval = mhtml_parser_append_token( parser, c );
         maug_cleanup_if_not_ok();

      } else {
         mhtml_parser_invalid_c( parser, '_', retval );
      }
      break;

   case '\r':
   case '\n':
   case '\t':
      break;

   case ' ':
      if( MHTML_PSTATE_ELEMENT == mhtml_parser_pstate( parser ) ) {
         retval = mhtml_push_element_tag( parser );
         maug_cleanup_if_not_ok();
         retval = mhtml_parser_pstate_push( parser, MHTML_PSTATE_ATTRIB_KEY );
         maug_cleanup_if_not_ok();
         mhtml_parser_reset_token( parser );

      } else if( MHTML_PSTATE_STRING == mhtml_parser_pstate( parser ) ) {
         retval = mhtml_parser_append_token( parser, c );
         maug_cleanup_if_not_ok();

      } else if( MHTML_PSTATE_ATTRIB_KEY == mhtml_parser_pstate( parser ) ) {
         /* Do nothing. */

      } else if( MHTML_PSTATE_NONE == mhtml_parser_pstate( parser ) ) {
         /* Avoid a token that's only whitespace. */
         if(
            0 < parser->base.token_sz &&
            ' ' != parser->base.token[parser->base.token_sz - 1]
         ) {
            retval = mhtml_parser_append_token( parser, ' ' );
            maug_cleanup_if_not_ok();
         }

      } else {
         mhtml_parser_invalid_c( parser, '_', retval );
      }
      break;

   default:
      retval = mhtml_parser_append_token( parser, c );
      maug_cleanup_if_not_ok();
      break;
   }

   parser->base.i++;

   mparser_wait( &((parser)->base) );

cleanup:

   parser->base.last_c = c;

   if( mdata_vector_is_locked( &(parser->tags) ) ) {
      mdata_vector_unlock( &(parser->tags) );
   }

   return retval;
}

MERROR_RETVAL mhtml_parser_init( struct MHTML_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;

   /* Perform initial tag allocation. */
   mhtml_parser_set_tag_iter( parser, -1 );
   parser->body_idx = -1;

   retval = mcss_parser_init( &(parser->styler) );
   maug_cleanup_if_not_ok();

cleanup:

   return retval;
}

MERROR_RETVAL mhtml_dump_tree(
   struct MHTML_PARSER* parser, ssize_t iter, size_t d
) {
   size_t i = 0;
   char* tag_content = NULL;
   char dump_line[MHTML_DUMP_LINE_SZ + 1];
   union MHTML_TAG* p_tag_iter = NULL;
   ssize_t first_child = -1;
   ssize_t next_sibling = -1;
   MERROR_RETVAL retval = MERROR_OK;

   if( 0 > iter ) {
      return retval;
   }

   mdata_vector_lock( &(parser->tags) );

   p_tag_iter = mdata_vector_get( &(parser->tags), iter, union MHTML_TAG );
   assert( NULL != p_tag_iter );

   maug_mzero( dump_line, MHTML_DUMP_LINE_SZ + 1 );

   for( i = 0 ; d > i ; i++ ) {
      assert( i < MHTML_DUMP_LINE_SZ );
      strcat( dump_line, " " );
   }
   if( MHTML_TAG_TYPE_TEXT == p_tag_iter->base.type ) {
      maug_mlock( p_tag_iter->TEXT.content, tag_content );
      if( NULL == tag_content ) {
         error_printf( "could not lock tag content!" );
         goto cleanup;
      }

      if(
         maug_strlen( dump_line ) + 7 /* ("TEXT: \n") */
            + maug_strlen( tag_content ) < MHTML_DUMP_LINE_SZ
      ) {
         maug_snprintf( &(dump_line[maug_strlen( dump_line )]),
            MHTML_DUMP_LINE_SZ - maug_strlen( dump_line ),
            "TEXT: %s\n", tag_content );
      }

      maug_munlock( p_tag_iter->TEXT.content, tag_content );

   } else {
      if(
         maug_strlen( dump_line ) +
         maug_strlen( gc_mhtml_tag_names[p_tag_iter->base.type] ) <
         MHTML_DUMP_LINE_SZ
      ) {
         strcat( dump_line,
            gc_mhtml_tag_names[p_tag_iter->base.type] );
      }

      if(
         0 <= p_tag_iter->base.style &&
         maug_strlen( dump_line ) + 9 /* (styled) */ < MHTML_DUMP_LINE_SZ
      ) {
         strcat( dump_line, " (styled)" );
      }

      if(
         0 < p_tag_iter->base.id_sz &&
         maug_strlen( dump_line ) + 7 /* (id: ) */
            + maug_strlen( p_tag_iter->base.id ) < MHTML_DUMP_LINE_SZ
      ) {
         maug_snprintf( &(dump_line[maug_strlen( dump_line )]),
            MHTML_DUMP_LINE_SZ - maug_strlen( dump_line ),
            " (id: %s)", p_tag_iter->base.id );
      }

      if(
         0 < p_tag_iter->base.classes_sz &&
         maug_strlen( dump_line ) + 12 /* (classes: ) */
            + maug_strlen( p_tag_iter->base.id ) < MHTML_DUMP_LINE_SZ
      ) {
         maug_snprintf( &(dump_line[maug_strlen( dump_line )]),
            MHTML_DUMP_LINE_SZ - maug_strlen( dump_line ),
            " (classes: %s)", p_tag_iter->base.classes );
      }

      if(
         MHTML_TAG_TYPE_IMG == p_tag_iter->base.type &&
         0 < p_tag_iter->IMG.src_sz &&
         maug_strlen( dump_line ) + 8 /* (src: ) */
            + maug_strlen( p_tag_iter->IMG.src ) < MHTML_DUMP_LINE_SZ
      ) {
         maug_snprintf( &(dump_line[maug_strlen( dump_line )]),
            MHTML_DUMP_LINE_SZ - maug_strlen( dump_line ),
            " (src: %s)", p_tag_iter->IMG.src );
      }

      if(
         MHTML_TAG_TYPE_INPUT == p_tag_iter->base.type &&
         0 < p_tag_iter->INPUT.value_sz &&
         maug_strlen( dump_line ) + 10 /* (value: ) */
            + maug_strlen( p_tag_iter->INPUT.value ) < MHTML_DUMP_LINE_SZ
      ) {
         maug_snprintf( &(dump_line[maug_strlen( dump_line )]),
            MHTML_DUMP_LINE_SZ - maug_strlen( dump_line ),
            " (value: %s)", p_tag_iter->INPUT.value );
      }

   }

   debug_printf( 1, "%s", dump_line );

   first_child = p_tag_iter->base.first_child;
   next_sibling = p_tag_iter->base.next_sibling;

   mdata_vector_unlock( &(parser->tags) );

   retval = mhtml_dump_tree( parser, first_child, d + 1 );
   maug_cleanup_if_not_ok();

   retval = mhtml_dump_tree( parser, next_sibling, d );
   maug_cleanup_if_not_ok();

cleanup:

   if( mdata_vector_is_locked( &(parser->tags) ) ) {
      mdata_vector_unlock( &(parser->tags) );
   }

   return retval;
}

#else

#define MHTML_TAG_TABLE_CONST( tag_id, tag_name, fields, disp ) \
   extern MAUG_CONST uint16_t SEG_MCONST MHTML_TAG_TYPE_ ## tag_name;

MHTML_TAG_TABLE( MHTML_TAG_TABLE_CONST )

extern MAUG_CONST char* SEG_MCONST gc_mhtml_tag_names[];

#endif /* MHTML_C */

#endif /* !MHTML_H */

