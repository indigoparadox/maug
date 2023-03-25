
#ifndef MHTML_H
#define MHTML_H

#ifndef MHTML_PARSER_TOKEN_SZ_MAX
#  define MHTML_PARSER_TOKEN_SZ_MAX 1024
#endif /* !MHTML_PARSER_TOKEN_SZ_MAX */

#ifdef MHTML_C
#  define MCSS_C
#endif /* MHTML_C */

#include <mparser.h>
#include <mcss.h>

#define MHTML_PARSER_PSTATE_TABLE( f ) \
   f( MHTML_PSTATE_NONE, 0 ) \
   f( MHTML_PSTATE_ELEMENT, 1 ) \
   f( MHTML_PSTATE_ATTRIBUTE, 2 ) \
   f( MHTML_PSTATE_END_ELEMENT, 3 )

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

#define MHTML_TAG_TABLE( f ) \
   f( 0, NONE, void* none; ) \
   f( 1, BODY, void* none; ) \
   f( 2, DIV, void* none; ) \
   f( 3, HEAD, void* none; ) \
   f( 4, HTML, void* none; ) \
   f( 5, TEXT, MAUG_MHANDLE content; size_t content_sz; ) \
   f( 6, TITLE, MAUG_MHANDLE content; size_t content_sz; )

struct MHTML_TAG_BASE {
   uint16_t type;
   ssize_t parent;
   ssize_t first_child;
   ssize_t next_sibling;
};

#define MHTML_TAG_TABLE_STRUCT( tag_id, tag_name, fields ) \
   struct MHTML_TAG_ ## tag_name { \
      struct MHTML_TAG_BASE base; \
      fields \
   };

MHTML_TAG_TABLE( MHTML_TAG_TABLE_STRUCT )

#define MHTML_TAG_TABLE_UNION_FIELD( tag_id, tag_name, fields ) \
   struct MHTML_TAG_ ## tag_name tag_name;

union MHTML_TAG {
   struct MHTML_TAG_BASE base; /* Should line up w/ 1st "base" in all types. */
   MHTML_TAG_TABLE( MHTML_TAG_TABLE_UNION_FIELD )
};

struct MHTML_PARSER {
   uint16_t pstate[MPARSER_STACK_SZ_MAX];
   size_t pstate_sz;
   char token[MHTML_PARSER_TOKEN_SZ_MAX];
   size_t token_sz;
   size_t i;
   MAUG_MHANDLE tags_h;
   union MHTML_TAG* tags;
   size_t tags_sz;
   size_t tags_sz_max;
   ssize_t tag_iter;
   struct MCSS_PARSER styler;
};

void mhtml_unlock_parser( struct MHTML_PARSER* parser );

MERROR_RETVAL mhtml_lock_parser( struct MHTML_PARSER* parser );

void mhtml_free_parser( struct MHTML_PARSER* parser );

MERROR_RETVAL mhtml_pop_tag( struct MHTML_PARSER* parser );

MERROR_RETVAL mhtml_parse_c( struct MHTML_PARSER* parser, char c );

void mhtml_dump_tree( struct MHTML_PARSER* parser, ssize_t iter, size_t d );

#ifdef MHTML_C

#endif /* MHTML_C */

MHTML_PARSER_PSTATE_TABLE( MPARSER_PSTATE_TABLE_CONST )

static MAUG_CONST char* gc_mhtml_pstate_names[] = {
   MHTML_PARSER_PSTATE_TABLE( MPARSER_PSTATE_TABLE_NAME )
   ""
};

#define MHTML_TAG_TABLE_CONST( tag_id, tag_name, fields ) \
   MAUG_CONST uint16_t MHTML_TAG_TYPE_ ## tag_name = tag_id;

MHTML_TAG_TABLE( MHTML_TAG_TABLE_CONST )

#define MHTML_TAG_TABLE_NAMES( tag_id, tag_name, fields ) \
   #tag_name,

MAUG_CONST char* gc_mhtml_tag_names[] = {
   MHTML_TAG_TABLE( MHTML_TAG_TABLE_NAMES )
   ""
};

void mhtml_unlock_parser( struct MHTML_PARSER* parser ) {

   if( NULL != parser->tags ) {
      /* Parser already locked! */
      goto cleanup;
   }

   maug_munlock( parser->tags_h, parser->tags );

cleanup:
   return;
}

MERROR_RETVAL mhtml_lock_parser( struct MHTML_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;

   if( NULL != parser->tags ) {
      /* Parser already locked! */
      goto cleanup;
   }

   maug_mlock( parser->tags_h, parser->tags );
   maug_cleanup_if_null_alloc( union MHTML_TAG*, parser->tags );

cleanup:

   return retval;
}

ssize_t mhtml_get_next_free_tag( struct MHTML_PARSER* parser ) {
   uint8_t auto_unlocked = 0;
   ssize_t retidx = -1;
   MAUG_MHANDLE new_tags_h = (MAUG_MHANDLE)NULL;

   if( NULL != parser->tags ) {
      maug_munlock( parser->tags_h, parser->tags );
      auto_unlocked = 1;
   }

   if( 0 == parser->tags_sz ) {
      /* Perform initial tag allocation. */
      parser->tags_sz_max = 1;
      parser->tags_h = maug_malloc(
         parser->tags_sz_max, sizeof( union MHTML_TAG ) );
      if( NULL == parser->tags_h ) {
         error_printf( "unable to allocate " SIZE_T_FMT " tags!",
            parser->tags_sz_max );
         goto cleanup;
      }

   } else if( parser->tags_sz_max <= parser->tags_sz + 1 ) {
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
   maug_mlock( parser->tags_h, parser->tags );
   if( NULL == parser->tags ) {
      error_printf( "unable to lock tags!" );
      goto cleanup;
   }

   /* Zero out the last tag, add it to the list, and return its index. */
   debug_printf( 1, "zeroing tag " SIZE_T_FMT " (of " SIZE_T_FMT ")...",
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

void mhtml_free_parser( struct MHTML_PARSER* parser ) {
   size_t i = 0;

   mhtml_lock_parser( parser );

   for( i = 0 ; parser->tags_sz > i ; i++ ) {
      if(
         MHTML_TAG_TYPE_TEXT == parser->tags[i].base.type &&
         (MAUG_MHANDLE)NULL != parser->tags[i].TEXT.content
      ) {
         maug_mfree( parser->tags[i].TEXT.content );
      }
   }

   if( NULL != parser->tags ) {
      maug_munlock( parser->tags_h, parser->tags );
   }

   if( NULL != parser->tags_h ) {
      maug_mfree( parser->tags_h );
   }
}

MERROR_RETVAL mhtml_pop_tag( struct MHTML_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;

   /* Move up from current tag. */
   retval = mhtml_lock_parser( parser );
   maug_cleanup_if_not_ok();
   assert( parser->tag_iter >= 0 );
   parser->tag_iter = parser->tags[parser->tag_iter].base.parent;

   if( 0 <= parser->tag_iter ) {
      debug_printf( 1, "moved iter back to tag %s (" SIZE_T_FMT ")",
         gc_mhtml_tag_names[parser->tags[parser->tag_iter].base.type],
         parser->tag_iter );
   } else {
      debug_printf( 1, "moved iter back to root (-1)" );
   }

cleanup:

   return retval;
}

MERROR_RETVAL mhtml_push_blank_tag( struct MHTML_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   ssize_t new_tag_idx = -1,
      tag_child_idx = 0;

   new_tag_idx = mhtml_get_next_free_tag( parser );
   if( 0 > new_tag_idx ) {
      /* Allocating new tag failed! */
      retval = MERROR_ALLOC;
      goto cleanup;
   }

   retval = mhtml_lock_parser( parser );
   maug_cleanup_if_not_ok();

   parser->tags[new_tag_idx].base.parent = -1;
   parser->tags[new_tag_idx].base.first_child = -1;
   parser->tags[new_tag_idx].base.next_sibling = -1;

   if( 0 <= parser->tag_iter ) {
      /* Set new tag parent to current tag. */
      parser->tags[new_tag_idx].base.parent = parser->tag_iter;

      /* Add new tag to current tag's children. */
      if( 0 > parser->tags[parser->tag_iter].base.first_child ) {
         parser->tags[parser->tag_iter].base.first_child = new_tag_idx;
      } else {
         /* Find the last sibling child. */
         tag_child_idx = parser->tags[parser->tag_iter].base.first_child;
         debug_printf( 1, "tci: " SSIZE_T_FMT " ns: " SSIZE_T_FMT,
            tag_child_idx, parser->tags[tag_child_idx].base.next_sibling );
         while(
            0 <= tag_child_idx &&
            0 <= parser->tags[tag_child_idx].base.next_sibling
         ) {
            tag_child_idx = parser->tags[tag_child_idx].base.next_sibling;
            debug_printf( 1, "tci: " SSIZE_T_FMT, tag_child_idx );
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

   retval = mhtml_push_blank_tag( parser );
   maug_cleanup_if_not_ok();

   /* Normalize token case. */
   for( i = 0 ; parser->token_sz > i ; i++ ) {
      if( 0x61 <= parser->token[i] && 0x7a >= parser->token[i] ) {
         parser->token[i] -= 0x20;
      }
   }

   /* Figure out tag type. */

   # define MHTML_TAG_TABLE_ASSIGN( tag_id, tag_name, fields ) \
      } else if( \
         parser->token_sz == strlen( #tag_name ) && \
         0 == strncmp( parser->token, #tag_name, parser->token_sz ) \
      ) { \
         debug_printf( 1, "new tag (" SSIZE_T_FMT ") type: " #tag_name, \
            parser->tag_iter ); \
         parser->tags[parser->tag_iter].base.type = \
            MHTML_TAG_TYPE_ ## tag_name; \
   
   if( 0 ) {
   MHTML_TAG_TABLE( MHTML_TAG_TABLE_ASSIGN )
   } else {
      error_printf( "could not find type for new tag (" SSIZE_T_FMT ")",
         parser->tag_iter );
   }

cleanup:

   return retval;
}

MERROR_RETVAL mhtml_push_text_tag( struct MHTML_PARSER* parser ) {
   MERROR_RETVAL retval = MERROR_OK;
   char* tag_content = NULL;

   retval = mhtml_push_blank_tag( parser );
   maug_cleanup_if_not_ok();

   parser->tags[parser->tag_iter].base.type = MHTML_TAG_TYPE_TEXT;

   /* Allocate text memory. */
   parser->tags[parser->tag_iter].TEXT.content =
      maug_malloc( parser->token_sz + 1, 1 );
   maug_cleanup_if_null_alloc(
      MAUG_MHANDLE, parser->tags[parser->tag_iter].TEXT.content );
   maug_mlock( parser->tags[parser->tag_iter].TEXT.content, tag_content );
   maug_cleanup_if_null_alloc( char*, tag_content );

   /* Copy token to tag text. */
   strncpy( tag_content, parser->token, parser->token_sz );
   tag_content[parser->token_sz] = '\0';
   parser->tags[parser->tag_iter].TEXT.content_sz = parser->token_sz;

   maug_munlock( parser->tags[parser->tag_iter].TEXT.content, tag_content );

cleanup:

   return retval;
}

uint8_t mhtml_token_is_blank( struct MHTML_PARSER* parser ) {
   size_t i = 0;
   uint8_t is_blank = 1;

   for( i = 0 ; parser->token_sz > i ; i++ ) {
      if(
         ' ' != parser->token[i] &&
         '\t' != parser->token[i] &&
         '\r' != parser->token[i] &&
         '\n' != parser->token[i]
      ) {
         /* Non-whitespace character found! */
         is_blank = 0;
         break;
      }
   }

   return is_blank;
}

MERROR_RETVAL mhtml_parse_c( struct MHTML_PARSER* parser, char c ) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( 1, "%c", c );

   switch( c ) {
   case '<':
      if( MHTML_PSTATE_NONE == mhtml_parser_pstate( parser ) ) {
         if( 0 < parser->token_sz && !mhtml_token_is_blank( parser ) ) {
            mhtml_push_text_tag( parser );

            /* Pop out of text so next tag isn't a child of it. */
            retval = mhtml_pop_tag( parser );
            maug_cleanup_if_not_ok();
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

      } else if( MHTML_PSTATE_ATTRIBUTE == mhtml_parser_pstate( parser ) ) {

         /* TODO: Add attribute to element. */

         mhtml_parser_pstate_pop( parser );
         assert( MHTML_PSTATE_ELEMENT == mhtml_parser_pstate( parser ) );
         mhtml_parser_pstate_pop( parser ); /* Pop element. */

      } else if( MHTML_PSTATE_END_ELEMENT == mhtml_parser_pstate( parser ) ) {

         retval = mhtml_pop_tag( parser );
         maug_cleanup_if_not_ok();
         
         mhtml_parser_pstate_pop( parser );
         assert( MHTML_PSTATE_ELEMENT == mhtml_parser_pstate( parser ) );
         mhtml_parser_pstate_pop( parser ); /* Pop element. */
         mhtml_parser_reset_token( parser );

      } else {
         mhtml_parser_invalid_c( parser, c, retval );
      }
      break;

   case '/':
      if(
         MHTML_PSTATE_ELEMENT == mhtml_parser_pstate( parser ) &&
         0 == parser->token_sz
      ) {
         mhtml_parser_pstate_push( parser, MHTML_PSTATE_END_ELEMENT );

      } else if( MHTML_PSTATE_NONE == mhtml_parser_pstate( parser ) ) {
         mhtml_parser_append_token( parser, c );

      } else {
         mhtml_parser_invalid_c( parser, c, retval );
      }
      break;

   case ' ':
      if( MHTML_PSTATE_ELEMENT == mhtml_parser_pstate( parser ) ) {
         retval = mhtml_push_element_tag( parser );
         maug_cleanup_if_not_ok();
         mhtml_parser_pstate_push( parser, MHTML_PSTATE_ATTRIBUTE );
         mhtml_parser_reset_token( parser );

      } else if( MHTML_PSTATE_NONE == mhtml_parser_pstate( parser ) ) {
         mhtml_parser_append_token( parser, c );

      } else {
         mhtml_parser_invalid_c( parser, c, retval );
      }
      break;

   default:
      mhtml_parser_append_token( parser, c );
      break;
   }

   parser->i++;

cleanup:

   mhtml_unlock_parser( parser );

   return retval;
}

void mhtml_dump_tree( struct MHTML_PARSER* parser, ssize_t iter, size_t d ) {
   size_t i = 0;
   char* tag_content = NULL;

   if( 0 > iter ) {
      return;
   }

   for( i = 0 ; d > i ; i++ ) {
      printf( "   " );
   }
   if( MHTML_TAG_TYPE_TEXT == parser->tags[iter].base.type ) {
      maug_mlock( parser->tags[iter].TEXT.content, tag_content );
      if( NULL == tag_content ) {
         error_printf( "could not lock tag content!" );
         return;
      }

      printf( "TEXT: %s\n", tag_content );

      maug_munlock( parser->tags[iter].TEXT.content, tag_content );

   } else {
      printf( "%s\n", gc_mhtml_tag_names[parser->tags[iter].base.type] );
   }

   mhtml_dump_tree( parser, parser->tags[iter].base.first_child, d + 1 );

   mhtml_dump_tree( parser, parser->tags[iter].base.next_sibling, d );
}

#endif /* !MHTML_H */

