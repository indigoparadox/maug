
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAUG_NO_RETRO
#define MAUG_C
#include "maug.h"
#include "mvmd.h"
#include "mvmc.h"

/* === */

int main( int argc, char* argv[] ) {
   int retval = 0;
   FILE* src_f = NULL;
   size_t src_sz = 0;
   uint8_t* src_buf = NULL;
   struct MVMC_PARSER parser;
   size_t i = 0;
   size_t src_read = 0;
   char* symbol_dbg = NULL;

   memset( &parser, '\0', sizeof( struct MVMC_PARSER ) );

   /* Load the source file into memory. */
   src_f = fopen( argv[1], "r" );
   if( NULL == src_f ) {
      retval = 1;
      goto cleanup;
   }
   fseek( src_f, 0, SEEK_END );
   src_sz = ftell( src_f );
   fseek( src_f, 0, SEEK_SET );
   src_buf = calloc( 1, src_sz );
   if( NULL == src_buf ) {
      retval = 1;
      goto cleanup;
   }
   src_read = fread( src_buf, 1, src_sz, src_f );
   if( src_read < src_sz ) {
      fprintf( stderr, "insufficient read bytes: %ld\n", src_read );
      retval = 1;
      goto cleanup;
   }

   /* Parse the source file. */
   for( i = 0 ; src_sz > i ; i++ ) {
      retval = mvmc_lex( &parser, src_buf[i] );
      if( retval ) {
         goto cleanup;
      }
   }

   for( i = 0 ; parser.tokens_ct > i ; i++ ) {
      switch( parser.tokens[i].type ) {
      case MVMC_TOK_VAR:
         symbol_dbg = "VAR";
         break;
      case MVMC_TOK_LCBRACE:
         symbol_dbg = "LCBRACE";
         break;
      case MVMC_TOK_RCBRACE:
         symbol_dbg = "RCBRACE";
         break;
      case MVMC_TOK_LPAREN:
         symbol_dbg = "LPAREN";
         break;
      case MVMC_TOK_RPAREN:
         symbol_dbg = "RPAREN";
         break;
      case MVMC_TOK_SEMICOLON:
         symbol_dbg = "SEMICOLON";
         break;
      case MVMC_TOK_INT:
         symbol_dbg = "INT";
         break;
      case MVMC_TOK_STR:
         symbol_dbg = "STR";
         break;
      case MVMC_TOK_ASSIGN:
         symbol_dbg = "ASSIGN";
         break;
      case MVMC_TOK_DIV:
         symbol_dbg = "DIV";
         break;
      case MVMC_TOK_MUL:
         symbol_dbg = "MUL";
         break;
      case MVMC_TOK_ADD:
         symbol_dbg = "ADD";
         break;
      case MVMC_TOK_SUB:
         symbol_dbg = "SUB";
         break;
      case MVMC_TOK_LT:
         symbol_dbg = "LT";
         break;
      case MVMC_TOK_GT:
         symbol_dbg = "GT";
         break;
      case MVMC_TOK_RETURN:
         symbol_dbg = "RETURN";
         break;
      case MVMC_TOK_IF:
         symbol_dbg = "IF";
         break;
      case MVMC_TOK_WHILE:
         symbol_dbg = "WHILE";
         break;
      case MVMC_TOK_EOF:
         symbol_dbg = "EOF";
         break;

      default:
         symbol_dbg = "UNKNOWN";
         break;
      }

      if( MVMC_TOK_STR == parser.tokens[i].type ) {
         mdata_strpool_lock( &(parser.names) );
         debug_printf( 1, "tok: name: %s",
            mdata_strpool_get( &(parser.names), parser.tokens[i].value ) );
         mdata_strpool_unlock( &(parser.names) );
      } else {
         debug_printf( 1, "tok: %s: %d", symbol_dbg, parser.tokens[i].value );
      }
   }

cleanup:

   return retval;
}

