
#ifndef MHTML_H
#define MHTML_H

#ifndef MHTML_PARSER_TOKEN_SZ_MAX
#  define MHTML_PARSER_TOKEN_SZ_MAX 1024
#endif /* !MHTML_PARSER_TOKEN_SZ_MAX */

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

#include <mparser.h>
#include <mcss.h>

#define MHTML_ATTRIB_TABLE( f ) \
   f( NONE, 0 ) \
   f( STYLE, 1 ) \
   f( CLASS, 2 ) \
   f( ID, 3 ) \
   f( NAME, 4 ) \
   f( SRC, 5 )

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
   f( 10, IMG, char src[MHTML_SRC_HREF_SZ_MAX + 1]; size_t src_sz;, BLOCK )

#define MHTML_PARSER_PSTATE_TABLE( f ) \
   f( MHTML_PSTATE_NONE, 0 ) \
   f( MHTML_PSTATE_ELEMENT, 1 ) \
   f( MHTML_PSTATE_ATTRIB_KEY, 2 ) \
   f( MHTML_PSTATE_ATTRIB_VAL, 3 ) \
   f( MHTML_PSTATE_END_ELEMENT, 4 ) \
   f( MHTML_PSTATE_STRING, 5 ) \
   f( MHTML_PSTATE_STYLE, 6 )

/* TODO: Function names should be verb_noun! */

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

#define mhtml_parser_pstate( parser ) \
   mparser_pstate( parser )

#define mhtml_parser_pstate_push( parser, new_pstate ) \
   mparser_pstate_push( mhtml, parser, new_pstate )

#define mhtml_parser_pstate_pop( parser ) \
   mparser_pstate_pop( mhtml, parser )

#define mhtml_parser_invalid_c( parser, c, retval ) \
   mparser_invalid_c( mhtml, parser, c, retval )

#define mhtml_parser_reset_token( parser ) \
   mparser_reset_token( mhtml, parser )

#define mhtml_parser_append_token( parser, c ) \
   mparser_append_token( mhtml, parser, c, MHTML_PARSER_TOKEN_SZ_MAX )

#define mhtml_parser_lock( parser ) \
   if( NULL == (parser)->tags ) { \
      maug_mlock( (parser)->tags_h, (parser)->tags ); \
      maug_cleanup_if_null_alloc( union MHTML_TAG*, (parser)->tags ); \
   } \
   mcss_parser_lock( &((parser)->styler) );

#define mhtml_parser_unlock( parser ) \
   if( NULL != (parser)->tags ) { \
      maug_munlock( (parser)->tags_h, (parser)->tags ); \
   } \
   mcss_parser_unlock( &((parser)->styler) );

#define mhtml_parser_is_locked( parser ) (NULL != (parser)->tags)

struct MHTML_TAG_BASE {
   uint16_t type;
   uint8_t flags;
   ssize_t parent;
   ssize_t first_child;
   ssize_t next_sibling;
   ssize_t style;
   char classes[MCSS_CLASS_SZ_MAX + 1];
   size_t classes_sz;
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
   uint16_t pstate[MPARSER_STACK_SZ_MAX];
   size_t pstate_sz;
   uint16_t attrib_key;
   char token[MHTML_PARSER_TOKEN_SZ_MAX];
   size_t token_sz;
   size_t i;
   MAUG_MHANDLE tags_h;
   union MHTML_TAG* tags;
   size_t tags_sz;
   size_t tags_sz_max;
   ssize_t tag_iter;
   /**
    * \brief Flags to be pushed to MHTML_TAG_BASE::flags on next 
    *        mhtml_push_tag().
    */
   uint8_t tag_flags;
   struct MCSS_PARSER styler;
   ssize_t body_idx;
};

MERROR_RETVAL mhtml_parser_free( struct MHTML_PARSER* parser );

MERROR_RETVAL mhtml_pop_tag( struct MHTML_PARSER* parser );

MERROR_RETVAL mhtml_parse_c( struct MHTML_PARSER* parser, char c );

MERROR_RETVAL mhtml_parser_init( struct MHTML_PARSER* parser );

void mhtml_dump_tree( struct MHTML_PARSER* parser, ssize_t iter, size_t d );

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

#define MHTML_PSTATE_TABLE_NAME( name, idx ) \
   #name,

static MAUG_CONST char* SEG_MCONST gc_mhtml_attrib_names[] = {
   MHTML_ATTRIB_TABLE( MHTML_PSTATE_TABLE_NAME )
   ""
};

#define MHTML_ATTRIB_TABLE_NAME( attrib_name, attrib_id ) \
   MAUG_CONST uint16_t SEG_MCONST MHTML_ATTRIB_KEY_ ## attrib_name = attrib_id;

MHTML_ATTRIB_TABLE( MHTML_ATTRIB_TABLE_NAME )

ssize_t mhtml_get_next_free_tag( struct MHTML_PARSER* parser ) {
   uint8_t auto_unlocked = 0;
   ssize_t retidx = -1;
   MAUG_MHANDLE new_tags_h = (MAUG_MHANDLE)NULL;

   if( NULL != parser->tags ) {
      debug_printf( MHTML_TRACE_LVL, "auto-unlocking tags..." );
      maug_munlock( parser->tags_h, parser->tags );
      auto_unlocked = 1;
   }

   assert( 0 < parser->tags_sz_max );
   assert( NULL == parser->tags );
   assert( (MAUG_MHANDLE)NULL != parser->tags_h );
   if( parser->tags_sz_max <= parser->tags_sz + 1 ) {
      /* We've run out of tags, so double the available number. */
      /* TODO: Check for sz overflow. */
      new_tags_h = maug_mrealloc(
         parser->tags_h, parser->tags_sz_max * 2, sizeof( union MHTML_TAG ) );
      if( (MAUG_MHANDLE)NULL == new_tags_h ) {
         error_printf(
            "unable to reallocate " SIZE_T_FMT " tags!",
            parser->tags_sz_max * 2 );
         goto cleanup;
      }
      parser->tags_h = new_tags_h;
      parser->tags_sz_max *= 2;
   }

   /* Assume handle is unlocked. */
   assert( NULL == parser->tags );
   maug_mlock( parser->tags_h, parser->tags );
   if( NULL == parser->tags ) {
      error_printf( "unable to lock tags!" );
      goto cleanup;
   }

   /* Zero out the last tag, add it to the list, and return its index. */
   debug_printf( MHTML_TRACE_LVL,
      "zeroing tag " SIZE_T_FMT " (of " SIZE_T_FMT ")...",
      parser->tags_sz, parser->tags_sz_max );
   maug_mzero( &(parser->tags[parser->tags_sz]), sizeof( union MHTML_TAG ) );
   retidx = parser->tags_sz;
   parser->tags_sz++;

   /* Compensate for cleanup below. */
   maug_munlock( parser->tags_h, parser->tags );

cleanup:

   if( auto_unlocked ) {
      maug_mlock( parser->tags_h, parser->tags );
   }

   return retidx;
}

MERROR_RETVAL mhtml_parser_free( struct MHTML_PARSER* parser ) {
   size_t i = 0;
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( MHTML_TRACE_LVL, "freeing HTML parser..." );

   mhtml_parser_lock( parser );

   for( i = 0 ; parser->tags_sz > i ; i++ ) {
      if(
         (
            MHTML_TAG_TYPE_TEXT == parser->tags[i].base.type ||
            MHTML_TAG_TYPE_STYLE == parser->tags[i].base.type
         ) &&
         (MAUG_MHANDLE)NULL != parser->tags[i].TEXT.content
      ) {
         maug_mfree( parser->tags[i].TEXT.content );
      }
   }

cleanup:

   mcss_parser_free( &(parser->styler) );

   if( NULL != parser->tags ) {
      maug_munlock( parser->tags_h, parser->tags );
   }

   if( NULL != parser->tags_h ) {
      maug_mfree( parser->tags_h );
   }

   return retval;
}

MERROR_RETVAL mhtml_pop_tag( struct MHTML_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;

   /* Move up from current tag. */
   mhtml_parser_lock( parser );
   assert( parser->tag_iter >= 0 );
   parser->tag_iter = parser->tags[parser->tag_iter].base.parent;

   if( 0 <= parser->tag_iter ) {
      debug_printf( MHTML_TRACE_LVL,
         "moved iter back to tag %s (" SIZE_T_FMT ")",
         gc_mhtml_tag_names[parser->tags[parser->tag_iter].base.type],
         parser->tag_iter );
   } else {
      debug_printf( MHTML_TRACE_LVL, "moved iter back to root (-1)" );
   }

cleanup:

   return retval;
}

MERROR_RETVAL mhtml_push_tag( struct MHTML_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t new_tag_idx = -1,
      tag_child_idx = 0;

   new_tag_idx = mhtml_get_next_free_tag( parser );
   if( 0 > new_tag_idx ) {
      /* Allocating new tag failed! */
      retval = MERROR_ALLOC;
      goto cleanup;
   }

   mhtml_parser_lock( parser );

   parser->tags[new_tag_idx].base.parent = -1;
   parser->tags[new_tag_idx].base.first_child = -1;
   parser->tags[new_tag_idx].base.next_sibling = -1;
   parser->tags[new_tag_idx].base.style = -1;
   
   parser->tags[new_tag_idx].base.flags = parser->tag_flags;
   parser->tag_flags = 0;

   if( 0 <= parser->tag_iter ) {
      /* Set new tag parent to current tag. */
      parser->tags[new_tag_idx].base.parent = parser->tag_iter;

      /* Add new tag to current tag's children. */
      if( 0 > parser->tags[parser->tag_iter].base.first_child ) {
         parser->tags[parser->tag_iter].base.first_child = new_tag_idx;
      } else {
         /* Find the last sibling child. */
         tag_child_idx = parser->tags[parser->tag_iter].base.first_child;
         debug_printf( MHTML_TRACE_LVL,
            "tci: " SSIZE_T_FMT " ns: " SSIZE_T_FMT,
            tag_child_idx, parser->tags[tag_child_idx].base.next_sibling );
         while(
            0 <= tag_child_idx &&
            0 <= parser->tags[tag_child_idx].base.next_sibling
         ) {
            tag_child_idx = parser->tags[tag_child_idx].base.next_sibling;
            debug_printf( MHTML_TRACE_LVL,
               "tci: " SSIZE_T_FMT, tag_child_idx );
         }
         assert( 0 <= tag_child_idx );
         parser->tags[tag_child_idx].base.next_sibling = new_tag_idx;
      }
   }

   parser->tag_iter = new_tag_idx;

cleanup:

   return retval;
}

MERROR_RETVAL mhtml_push_element_tag( struct MHTML_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   mparser_token_upper( parser, i );

   if( 0 == strncmp( "STYLE", parser->token, 6 ) ) {
      /* Special case: style tag. Don't push a new tag here, but set a flag for
       * the text tag next created by mhtml_push_tag() so the contents are
       * directly attached to the style tag.
       */
      parser->tag_flags |= MHTML_TAG_FLAG_STYLE;
      goto cleanup;
   }

   retval = mhtml_push_tag( parser );
   maug_cleanup_if_not_ok();

   /* Figure out tag type. */
   i = 0;
   while( '\0' != gc_mhtml_tag_names[i][0] ) {
      if(
         parser->token_sz == strlen( gc_mhtml_tag_names[i] ) &&
         0 == strncmp(
            gc_mhtml_tag_names[i], parser->token, parser->token_sz )
      ) {
         debug_printf( MHTML_TRACE_LVL,
            "new tag (" SSIZE_T_FMT ") type: %s",
            parser->tag_iter, gc_mhtml_tag_names[i] );
         parser->tags[parser->tag_iter].base.type = i;

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

   return retval;
}

MERROR_RETVAL mhtml_push_text_tag( struct MHTML_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   char* tag_content = NULL;
   size_t i = 0;

   retval = mhtml_push_tag( parser );
   maug_cleanup_if_not_ok();

   if(
      MHTML_TAG_FLAG_STYLE == (MHTML_TAG_FLAG_STYLE & 
         parser->tags[parser->tag_iter].base.flags)
   ) {
      parser->tags[parser->tag_iter].base.type = MHTML_TAG_TYPE_STYLE;
   } else {
      parser->tags[parser->tag_iter].base.type = MHTML_TAG_TYPE_TEXT;
   }

   /* Allocate text memory. */
   parser->tags[parser->tag_iter].TEXT.content =
      maug_malloc( parser->token_sz + 1, 1 );
   maug_cleanup_if_null_alloc(
      MAUG_MHANDLE, parser->tags[parser->tag_iter].TEXT.content );
   maug_mlock( parser->tags[parser->tag_iter].TEXT.content, tag_content );
   maug_cleanup_if_null_alloc( char*, tag_content );

   if( MHTML_TAG_TYPE_STYLE == parser->tags[parser->tag_iter].base.type ) {
      /* TODO: If it's the last character and there's still a token, process it! */
      debug_printf( MHTML_TRACE_LVL, "parsing STYLE tag..." );
      for( ; parser->token_sz > i ; i++ ) {
         retval = mcss_parse_c( &(parser->styler), parser->token[i] );
         maug_cleanup_if_not_ok();
      }
      debug_printf( 1, "out of style characters..." );
      mcss_parser_flush( &(parser->styler) );
      mcss_parser_reset( &(parser->styler) );
   } else {
      /* Copy token to tag text. */
      strncpy( tag_content, parser->token, parser->token_sz );
      tag_content[parser->token_sz] = '\0';
      parser->tags[parser->tag_iter].TEXT.content_sz = parser->token_sz;
   }

   debug_printf( 1, "done processing tag contents..." );

   maug_munlock( parser->tags[parser->tag_iter].TEXT.content, tag_content );

cleanup:

   return retval;
}

MERROR_RETVAL mhtml_push_attrib_key( struct MHTML_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   debug_printf( MHTML_TRACE_LVL, "attrib: %s", parser->token );

   mparser_token_upper( parser, i );

   /* Figure out attrib type. */
   i = 0;
   while( '\0' != gc_mhtml_attrib_names[i][0] ) {
      if(
         parser->token_sz == strlen( gc_mhtml_attrib_names[i] ) &&
         0 == strncmp(
            gc_mhtml_attrib_names[i], parser->token, parser->token_sz )
      ) {
         debug_printf( MHTML_TRACE_LVL, "new attrib type: %s", gc_mhtml_attrib_names[i] );
         parser->attrib_key = i;
         goto cleanup;
      }
      i++;
   }

   error_printf( "unknown attrib: %s", parser->token );

cleanup:

   return retval;
}

MERROR_RETVAL mhtml_push_attrib_val( struct MHTML_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0;

   /* TODO: Equip styler to manage its own locking. */
   mhtml_parser_lock( parser );

   if( MHTML_ATTRIB_KEY_STYLE == parser->attrib_key ) {
      debug_printf( MHTML_TRACE_LVL, "style: %s", parser->token );
      /* TODO: Parse and attach style. */

      retval = mcss_push_style( &(parser->styler) );
      maug_cleanup_if_not_ok();
      
      /* Set the new style as this tag's explicit style. */
      parser->tags[parser->tag_iter].base.style = parser->styler.styles_sz - 1;

      for( ; parser->token_sz > i ; i++ ) {
         retval = mcss_parse_c( &(parser->styler), parser->token[i] );
         maug_cleanup_if_not_ok();
      }
      debug_printf( 1, "out of style characters..." );
      mcss_parser_flush( &(parser->styler) );

      goto cleanup;

   } else if( MHTML_ATTRIB_KEY_CLASS == parser->attrib_key ) {
      strncpy(
         parser->tags[parser->tag_iter].base.classes,
         parser->token,
         MCSS_CLASS_SZ_MAX );
      parser->tags[parser->tag_iter].base.classes_sz = parser->token_sz;

   } else if( MHTML_ATTRIB_KEY_ID == parser->attrib_key ) {
      strncpy(
         parser->tags[parser->tag_iter].base.id,
         parser->token,
         MCSS_ID_SZ_MAX );
      parser->tags[parser->tag_iter].base.id_sz = parser->token_sz;

   } else if( MHTML_ATTRIB_KEY_SRC == parser->attrib_key ) {
      strncpy(
         parser->tags[parser->tag_iter].IMG.src,
         parser->token,
         MHTML_SRC_HREF_SZ_MAX );
      parser->tags[parser->tag_iter].IMG.src_sz = parser->token_sz;
   }

cleanup:

   return retval;
}

MERROR_RETVAL mhtml_parse_c( struct MHTML_PARSER* parser, char c ) {
   MERROR_RETVAL retval = MERROR_OK;

   switch( c ) {
   case '<':
      if( MHTML_PSTATE_NONE == mhtml_parser_pstate( parser ) ) {
         if( 0 < parser->token_sz ) {
            retval = mhtml_push_text_tag( parser );
            maug_cleanup_if_not_ok();

            if(
               /* See special exception in mhtml_push_tag(). Style tags don't
                * push their subordinate text, so popping here would be
                * uneven!
                */
               MHTML_TAG_TYPE_STYLE !=
                  parser->tags[parser->tag_iter].base.type
            ) {
               /* Pop out of text so next tag isn't a child of it. */
               retval = mhtml_pop_tag( parser );
               maug_cleanup_if_not_ok();
            }
         }
         mhtml_parser_pstate_push( parser, MHTML_PSTATE_ELEMENT )
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
         mhtml_parser_append_token( parser, c );

      } else if( MHTML_PSTATE_NONE == mhtml_parser_pstate( parser ) ) {
         mhtml_parser_append_token( parser, c );

      } else {
         mhtml_parser_invalid_c( parser, c, retval );
      }
      break;

   case '/':
      if(
         MHTML_PSTATE_ELEMENT == mhtml_parser_pstate( parser ) &&
         0 == parser->token_sz
      ) {
         /* Start of a close tag. */
         mhtml_parser_pstate_push( parser, MHTML_PSTATE_END_ELEMENT );

      } else if( MHTML_PSTATE_ATTRIB_KEY == mhtml_parser_pstate( parser ) ) {
         /* Close of a self-closing tag. */
         mhtml_parser_pstate_push( parser, MHTML_PSTATE_END_ELEMENT );

      } else if( MHTML_PSTATE_STRING == mhtml_parser_pstate( parser ) ) {
         mhtml_parser_append_token( parser, c );

      } else if( MHTML_PSTATE_NONE == mhtml_parser_pstate( parser ) ) {
         mhtml_parser_append_token( parser, c );

      } else {
         mhtml_parser_invalid_c( parser, c, retval );
      }
      break;

   case '=':
      if( MHTML_PSTATE_ATTRIB_KEY == mhtml_parser_pstate( parser ) ) {
         retval = mhtml_push_attrib_key( parser );
         maug_cleanup_if_not_ok();
         mhtml_parser_pstate_push( parser, MHTML_PSTATE_ATTRIB_VAL );
         mhtml_parser_reset_token( parser );

      } else if( MHTML_PSTATE_ATTRIB_VAL == mhtml_parser_pstate( parser ) ) {
         mhtml_parser_append_token( parser, c );

      } else if( MHTML_PSTATE_NONE == mhtml_parser_pstate( parser ) ) {
         mhtml_parser_append_token( parser, c );

      } else {
         mhtml_parser_invalid_c( parser, '_', retval );
      }
      break;

   case '"':
      if( MHTML_PSTATE_ATTRIB_VAL == mhtml_parser_pstate( parser ) ) {
         mhtml_parser_pstate_push( parser, MHTML_PSTATE_STRING );
         mhtml_parser_reset_token( parser );

      } else if( MHTML_PSTATE_STRING == mhtml_parser_pstate( parser ) ) {
         retval = mhtml_push_attrib_val( parser );
         maug_cleanup_if_not_ok();
         mhtml_parser_pstate_pop( parser );
         assert( MHTML_PSTATE_ATTRIB_VAL == mhtml_parser_pstate( parser ) );
         mhtml_parser_pstate_pop( parser );
         mhtml_parser_reset_token( parser );

      } else if( MHTML_PSTATE_NONE == mhtml_parser_pstate( parser ) ) {
         mhtml_parser_append_token( parser, c );

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
         mhtml_parser_pstate_push( parser, MHTML_PSTATE_ATTRIB_KEY );
         mhtml_parser_reset_token( parser );

      } else if( MHTML_PSTATE_STRING == mhtml_parser_pstate( parser ) ) {
         mhtml_parser_append_token( parser, c );

      } else if( MHTML_PSTATE_ATTRIB_KEY == mhtml_parser_pstate( parser ) ) {
         /* Do nothing. */

      } else if( MHTML_PSTATE_NONE == mhtml_parser_pstate( parser ) ) {
         /* Avoid a token that's only whitespace. */
         if(
            0 < parser->token_sz &&
            ' ' != parser->token[parser->token_sz - 1]
         ) {
            mhtml_parser_append_token( parser, ' ' );
         }

      } else {
         mhtml_parser_invalid_c( parser, '_', retval );
      }
      break;

   default:
      mhtml_parser_append_token( parser, c );
      break;
   }

   parser->i++;

cleanup:

   mhtml_parser_unlock( parser );

   return retval;
}

MERROR_RETVAL mhtml_parser_init( struct MHTML_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( 0 == parser->tags_sz );
   assert( (MAUG_MHANDLE)NULL == parser->tags );

   /* Perform initial tag allocation. */
   parser->tags_sz_max = MHTML_PARSER_TAGS_INIT_SZ;
   debug_printf( MHTML_TRACE_LVL, "allocating " SIZE_T_FMT " tags...",
      parser->tags_sz_max );
   parser->tags_h = maug_malloc(
      parser->tags_sz_max, sizeof( union MHTML_TAG ) );
   assert( (MAUG_MHANDLE)NULL != parser->tags_h );
   if( NULL == parser->tags_h ) {
      error_printf( "unable to allocate " SIZE_T_FMT " tags!",
         parser->tags_sz_max );
      goto cleanup;
   }

   parser->tag_iter = -1;
   parser->body_idx = -1;

   retval = mcss_parser_init( &(parser->styler) );
   maug_cleanup_if_not_ok();

cleanup:

   return retval;
}

void mhtml_dump_tree(
   struct MHTML_PARSER* parser, ssize_t iter, size_t d
) {
   size_t i = 0;
   char* tag_content = NULL;
   char dump_line[MHTML_DUMP_LINE_SZ + 1];

   if( 0 > iter ) {
      return;
   }

   for( i = 0 ; d > i ; i++ ) {
      maug_snprintf( dump_line, MHTML_DUMP_LINE_SZ, "   " );
   }
   if( MHTML_TAG_TYPE_TEXT == parser->tags[iter].base.type ) {
      maug_mlock( parser->tags[iter].TEXT.content, tag_content );
      if( NULL == tag_content ) {
         error_printf( "could not lock tag content!" );
         return;
      }

      if(
         strlen( dump_line ) + 7 /* ("TEXT: \n") */
            + strlen( tag_content ) < MHTML_DUMP_LINE_SZ
      ) {
         maug_snprintf( &(dump_line[strlen( dump_line )]),
            MHTML_DUMP_LINE_SZ - strlen( dump_line ),
            "TEXT: %s\n", tag_content );
      }

      maug_munlock( parser->tags[iter].TEXT.content, tag_content );

   } else {
      if(
         strlen( dump_line ) +
         strlen( gc_mhtml_tag_names[parser->tags[iter].base.type] ) <
         MHTML_DUMP_LINE_SZ
      ) {
         strcat( dump_line,
            gc_mhtml_tag_names[parser->tags[iter].base.type] );
      }

      if(
         0 <= parser->tags[iter].base.style &&
         strlen( dump_line ) + 9 /* (styled) */ < MHTML_DUMP_LINE_SZ
      ) {
         strcat( dump_line, " (styled)" );
      }

      if(
         0 < parser->tags[iter].base.id_sz &&
         strlen( dump_line ) + 7 /* (id: ) */
            + strlen( parser->tags[iter].base.id ) < MHTML_DUMP_LINE_SZ
      ) {
         maug_snprintf( &(dump_line[strlen( dump_line )]),
            MHTML_DUMP_LINE_SZ - strlen( dump_line ),
            " (id: %s)", parser->tags[iter].base.id );
      }

      if(
         0 < parser->tags[iter].base.classes_sz &&
         strlen( dump_line ) + 12 /* (classes: ) */
            + strlen( parser->tags[iter].base.id ) < MHTML_DUMP_LINE_SZ
      ) {
         maug_snprintf( &(dump_line[strlen( dump_line )]),
            MHTML_DUMP_LINE_SZ - strlen( dump_line ),
            " (classes: %s)", parser->tags[iter].base.classes );
      }

      if(
         0 < parser->tags[iter].IMG.src_sz &&
         strlen( dump_line ) + 13 /* (src/href: ) */
            + strlen( parser->tags[iter].IMG.src ) < MHTML_DUMP_LINE_SZ
      ) {
         maug_snprintf( &(dump_line[strlen( dump_line )]),
            MHTML_DUMP_LINE_SZ - strlen( dump_line ),
            " (src/href: %s)", parser->tags[iter].IMG.src );
      }

   }

   debug_printf( 1, "%s", dump_line );

   mhtml_dump_tree( parser, parser->tags[iter].base.first_child, d + 1 );

   mhtml_dump_tree( parser, parser->tags[iter].base.next_sibling, d );
}

#else

#define MHTML_TAG_TABLE_CONST( tag_id, tag_name, fields, disp ) \
   extern MAUG_CONST uint16_t SEG_MCONST MHTML_TAG_TYPE_ ## tag_name;

MHTML_TAG_TABLE( MHTML_TAG_TABLE_CONST )

extern MAUG_CONST char* SEG_MCONST gc_mhtml_tag_names[];

#endif /* MHTML_C */

#endif /* !MHTML_H */

