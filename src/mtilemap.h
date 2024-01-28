
#ifndef MTILEMAP_H
#define MTILEMAP_H

#include <mparser.h>
#include <mjson.h>
/* TODO: mtilemap should be renamed retrotm. m should never depend on retro. */
/* TODO: Also retrofil should be mfil. Oh dear. */
#include <retrofil.h>

/**
 * \addtogroup maug_tilemap Tilemap API
 * \brief Functions and structures for working with tilemaps/grids.
 * \{
 * \file mtilemap.h
 */

#ifndef MTILEMAP_NAME_SZ_MAX
#  define MTILEMAP_NAME_SZ_MAX 10
#endif /* !MTILEMAP_NAME_SZ_MAX */

#ifndef MTILEMAP_TILES_SZ_INIT
#  define MTILEMAP_TILES_SZ_INIT 10
#endif /* !MTILEMAP_TILES_SZ_INIT */

#ifndef MTILEMAP_TILESET_IMAGE_STR_SZ_MAX
#  define MTILEMAP_TILESET_IMAGE_STR_SZ_MAX 48
#endif /* !MTILEMAP_TILESET_IMAGE_STR_SZ_MAX */

#ifndef MTILEMAP_TILE_SCALE
#  define MTILEMAP_TILE_SCALE 1.0f
#endif /* !MTILEMAP_TILE_SCALE */

#ifndef MTILEMAP_CPROP_STR_SZ_MAX
#  define MTILEMAP_CPROP_STR_SZ_MAX 32
#endif /* !MTILEMAP_CPROP_STR_SZ_MAX */

#ifndef MTILEMAP_TRACE_LVL
#  define MTILEMAP_TRACE_LVL 1
#endif /* !MTILEMAP_TRACE_LVL */

#define MTILEMAP_FLAG_ACTIVE 0x1

/**
 * \addtogroup \maug_tilemap_defs Tilemap Tile Definitions
 * \{
 */

/**
 * \addtogroup \maug_tilemap_defs_types Tilemap Custom Property Types
 * \{
 */

#define MTILEMAP_CPROP_TYPE_NONE    0
#define MTILEMAP_CPROP_TYPE_INT     1
#define MTILEMAP_CPROP_TYPE_FLOAT   2
#define MTILEMAP_CPROP_TYPE_STR     3
#define MTILEMAP_CPROP_TYPE_BOOL    4

/*! \} */ /* maug_tilemap_defs_types */

/*! \brief Data store for MTILEMAP_CPROP::value. */
union MTILEMAP_CPROP_VAL {
   uint32_t u32;
   int32_t i32;
   float f32;
   char str[MTILEMAP_CPROP_STR_SZ_MAX];
   uint8_t bool;
};

/*! \brief Custom property key-value storage type. */
struct MTILEMAP_CPROP {
   /*! \brief \ref maug_tilemap_defs_types for MTILEMAP_CPROP::value. */
   uint8_t type;
   char key[MTILEMAP_CPROP_STR_SZ_MAX];
   union MTILEMAP_CPROP_VAL value;
};

struct MTILEMAP_TILE_DEF {
   uint8_t flags;
#ifdef MAUG_NO_MMEM
   struct MTILEMAP_CPROP* cprops;
#else
   MAUG_MHANDLE cprops;
#endif /* MAUG_NO_MMEM */
   size_t cprops_sz;
   char image_path[MTILEMAP_TILESET_IMAGE_STR_SZ_MAX];
};

/*! \} */ /* maug_tilemap_defs */

struct MTILEMAP_LAYER {
   size_t tiles_sz;
   size_t tiles_sz_max;
#ifdef MAUG_NO_MMEM
   size_t* tiles;
#else
   MAUG_MHANDLE tiles;
#endif /* MAUG_NO_MMEM */
};

/* TODO: Rework this based on perpix grid to reduce lockable handles needed. */

struct MTILEMAP {
   char name[MTILEMAP_NAME_SZ_MAX];
#ifdef MAUG_NO_MMEM
   struct MTILEMAP_CPROP* cprops;
   struct MTILEMAP_TILE_DEF* tile_defs;
   struct MTILEMAP_LAYER* layers;
#else
   MAUG_MHANDLE cprops;
   MAUG_MHANDLE tile_defs;
   MAUG_MHANDLE layers;
#endif /* MAUG_NO_MMEM */
   size_t tile_defs_sz;
   size_t cprops_sz;
   size_t layers_sz;
   size_t tileset_fgid;
   uint8_t tiles_h;
   uint8_t tiles_w;
};

/**
 * \addtogroup maug_tilemap_parser Tilemap Parser
 * \{
 */

typedef MERROR_RETVAL
(*mtilemap_tj_parse_cb)( const char* filename, struct MTILEMAP* t );

struct MTILEMAP_PARSER {
   uint8_t mstate;
   uint8_t mstate_prev;
   size_t tileset_id_cur;
   struct MTILEMAP* t;
   mtilemap_tj_parse_cb tj_parse_cb;
   struct MJSON_PARSER jparser;
};

#define MTILEMAP_PARSER_MSTATE_TABLE( f ) \
   f( MTILEMAP_MSTATE_NONE,         0, "", 0 ) \
   f( MTILEMAP_MSTATE_HEIGHT,       1, "height",   0 ) \
   f( MTILEMAP_MSTATE_WIDTH,        2, "width",    0 ) \
   f( MTILEMAP_MSTATE_LAYERS,       3, "layers",   0 ) \
   f( MTILEMAP_MSTATE_LAYER_DATA,   4, "data",     3 /* MSTATE_LAYERS */ ) \
   f( MTILEMAP_MSTATE_LAYER_NAME,   5, "name",     3 /* MSTATE_LAYERS */ ) \
   f( MTILEMAP_MSTATE_TILES,        6, "tiles",    0 ) \
   f( MTILEMAP_MSTATE_TILES_ID,     7, "id",       6 /* MSTATE_TILES */ ) \
   f( MTILEMAP_MSTATE_TILES_IMAGE,  8, "image",    6 /* MSTATE_TILES */ ) \
   f( MTILEMAP_MSTATE_TILESETS,     9, "tilesets", 0 ) \
   f( MTILEMAP_MSTATE_TILESETS_SRC, 10, "source",  9 /* MSTATE_TILESETS */ ) \
   f( MTILEMAP_MSTATE_TILESETS_FGID,11, "firstgid",9 /* MSTATE_TILESETS */ ) \
   f( MTILEMAP_MSTATE_TILESETS_PROP,12, "firstgid",9 /* MSTATE_TILESETS */ ) \
   f( MTILEMAP_MSTATE_GRID,         13, "grid",    0 ) \
   f( MTILEMAP_MSTATE_TILES_PROP,   14, "properties",6 /* MSTATE_TILES */ ) \
   f( MTILEMAP_MSTATE_TILES_PROP_N, 15, "name",14 /* M_TILES_PROP */ ) \
   f( MTILEMAP_MSTATE_TILES_PROP_T, 16, "type",14 /* M_TILES_PROP */ ) \
   f( MTILEMAP_MSTATE_TILES_PROP_V, 17, "value",14 /* M_TILES_PROP */ ) \

MERROR_RETVAL
mtilemap_parse_json_c( struct MTILEMAP_PARSER* parser, char c );

MERROR_RETVAL
mtilemap_parse_json_file( const char* filename, struct MTILEMAP* m );

/*! \} */ /* maug_tilemap_parser */

MERROR_RETVAL
mtilemap_alloc_tile_defs( struct MTILEMAP* t, size_t ndefs );

MERROR_RETVAL
mtilemap_alloc_layers( struct MTILEMAP* t, size_t ndefs );

MERROR_RETVAL
mtilemap_alloc_tiles( struct MTILEMAP_LAYER* layer, size_t ndefs );

MERROR_RETVAL
mtilemap_free( struct MTILEMAP* t );

#ifdef MTILEMAP_C

#  include <stdio.h>

#  include <mparser.h>

/* TODO: Function names should be verb_noun! */

/* TODO: Better swap logic. */
#define mtilemap_parser_mstate( parser, new_mstate ) \
   if( new_mstate != parser->mstate_prev ) { \
      parser->mstate_prev = parser->mstate; \
   } \
   parser->mstate = new_mstate; \
   debug_printf( \
      MTILEMAP_TRACE_LVL, "parser mstate: %s (prev: %s)", \
         gc_mtilemap_mstate_names[parser->mstate], \
         gc_mtilemap_mstate_names[parser->mstate_prev] );

#  define MTILEMAP_PARSER_MSTATE_TABLE_CONST( name, idx, token, parent ) \
      static MAUG_CONST uint8_t name = idx;

MTILEMAP_PARSER_MSTATE_TABLE( MTILEMAP_PARSER_MSTATE_TABLE_CONST )

#  define MTILEMAP_PARSER_MSTATE_TABLE_NAME( name, idx, token, parent ) \
      #name,

static MAUG_CONST char* gc_mtilemap_mstate_names[] = {
   MTILEMAP_PARSER_MSTATE_TABLE( MTILEMAP_PARSER_MSTATE_TABLE_NAME )
   ""
};

#  define MTILEMAP_PARSER_MSTATE_TABLE_TOKEN( name, idx, token, parent ) \
      token,

static MAUG_CONST char* gc_mtilemap_mstate_tokens[] = {
   MTILEMAP_PARSER_MSTATE_TABLE( MTILEMAP_PARSER_MSTATE_TABLE_TOKEN )
   ""
};

#  define MTILEMAP_PARSER_MSTATE_TABLE_PARENT( name, idx, token, parent ) \
      parent,

static MAUG_CONST uint8_t gc_mtilemap_mstate_parents[] = {
   MTILEMAP_PARSER_MSTATE_TABLE( MTILEMAP_PARSER_MSTATE_TABLE_PARENT )
   0
};

/* === */

MERROR_RETVAL mtilemap_parser_tile_set_cprop_name(
   size_t tile_idx, struct MTILEMAP_TILE_DEF* tile_def, const char* name
) {
   MAUG_MHANDLE cprops_new_h = (MAUG_MHANDLE)NULL;
   struct MTILEMAP_CPROP* cprops = NULL;
   MERROR_RETVAL retval = MERROR_OK;

   /* Allocate new cprop, as name is assumed to always be the first field. */
   if( 0 == tile_def->cprops_sz ) {
      assert( (MAUG_MHANDLE)NULL == tile_def->cprops );
      tile_def->cprops = maug_malloc( 1, sizeof( struct MTILEMAP_CPROP ) );
      maug_cleanup_if_null_alloc( MAUG_MHANDLE, tile_def->cprops );

      /* Lock and zero. */
      maug_mlock( tile_def->cprops, cprops );
      maug_cleanup_if_null_alloc( struct MTILEMAP_CPROP*, cprops );
      maug_mzero( cprops, sizeof( struct MTILEMAP_CPROP ) );

   } else {
      maug_mrealloc_test(
         cprops_new_h, tile_def->cprops, 
         tile_def->cprops_sz + 1, sizeof( struct MTILEMAP_CPROP ) );

      /* Lock and zero newly allocated part. */
      maug_mlock( tile_def->cprops, cprops );
      maug_cleanup_if_null_alloc( struct MTILEMAP_CPROP*, cprops );
      maug_mzero(
         &(cprops[tile_def->cprops_sz]), sizeof( struct MTILEMAP_CPROP ) );
   }

   strncpy(
      cprops[tile_def->cprops_sz].key,
      name,
      MTILEMAP_CPROP_STR_SZ_MAX );

   debug_printf( MTILEMAP_TRACE_LVL,
      "set tile #" SIZE_T_FMT " cprop #" SIZE_T_FMT " key: %s",
      tile_idx, tile_def->cprops_sz, cprops[tile_def->cprops_sz].key );

   tile_def->cprops_sz++;

cleanup:

   if( NULL != cprops ) {
      maug_munlock( tile_def->cprops, cprops );
   }

   return retval;
}

MERROR_RETVAL mtilemap_parser_tile_set_cprop_type(
   struct MTILEMAP_TILE_DEF* tile_def, const char* type
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MTILEMAP_CPROP* cprops = NULL;

   maug_mlock( tile_def->cprops, cprops );
   maug_cleanup_if_null_alloc( struct MTILEMAP_CPROP*, cprops );

   /* Parse type. */
   if( 0 == strncmp( type, "int", 3 ) ) {
      cprops[tile_def->cprops_sz - 1].type = MTILEMAP_CPROP_TYPE_INT;

   } else if( 0 == strncmp( type, "float", 5 ) ) {
      cprops[tile_def->cprops_sz - 1].type = MTILEMAP_CPROP_TYPE_FLOAT;

   } else if( 0 == strncmp( type, "str", 3 ) ) {
      cprops[tile_def->cprops_sz - 1].type = MTILEMAP_CPROP_TYPE_STR;

   } else if( 0 == strncmp( type, "bool", 4 ) ) {
      cprops[tile_def->cprops_sz - 1].type = MTILEMAP_CPROP_TYPE_BOOL;

   }

   debug_printf( MTILEMAP_TRACE_LVL, "set cprop #" SIZE_T_FMT " type: %d",
      tile_def->cprops_sz - 1,
      cprops[tile_def->cprops_sz - 1].type );

cleanup:

   if( NULL != cprops ) {
      maug_munlock( tile_def->cprops, cprops );
   }

   return retval;
}

MERROR_RETVAL mtilemap_parser_tile_set_cprop_value(
   struct MTILEMAP_TILE_DEF* tile_def, const char* val
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MTILEMAP_CPROP* cprops = NULL;

   maug_mlock( tile_def->cprops, cprops );
   maug_cleanup_if_null_alloc( struct MTILEMAP_CPROP*, cprops );

   /* TODO: Parse value. */
   switch( cprops[tile_def->cprops_sz - 1].type ) {
   case MTILEMAP_CPROP_TYPE_INT:
      cprops[tile_def->cprops_sz - 1].value.i32 = atoi( val );
      debug_printf( MTILEMAP_TRACE_LVL, "set cprop #" SIZE_T_FMT " value: %d",
         tile_def->cprops_sz - 1,
         cprops[tile_def->cprops_sz - 1].value.i32 );
      break;
      
   case MTILEMAP_CPROP_TYPE_FLOAT:
      cprops[tile_def->cprops_sz - 1].value.f32 = atof( val );
      debug_printf( MTILEMAP_TRACE_LVL, "set cprop #" SIZE_T_FMT " value: %f",
         tile_def->cprops_sz - 1,
         cprops[tile_def->cprops_sz - 1].value.f32 );
      break;

   case MTILEMAP_CPROP_TYPE_STR:
      strncpy(
         cprops[tile_def->cprops_sz - 1].value.str,
         val,
         MTILEMAP_CPROP_STR_SZ_MAX );
      debug_printf( MTILEMAP_TRACE_LVL, "set cprop #" SIZE_T_FMT " value: %s",
         tile_def->cprops_sz - 1,
         cprops[tile_def->cprops_sz - 1].value.str );
      break;

   case MTILEMAP_CPROP_TYPE_BOOL:
      cprops[tile_def->cprops_sz - 1].value.bool =
         (0 == strncmp( "true", val, 4 ) ? 1 : 0);
      debug_printf( MTILEMAP_TRACE_LVL, "set cprop #" SIZE_T_FMT " value: %s",
         tile_def->cprops_sz - 1,
         cprops[tile_def->cprops_sz - 1].value.bool ? "true" : "false" );
      break;

   default:
      error_printf( "invalid cprop value!" );
      retval = 1;
      break;
   }

cleanup:

   if( NULL != cprops ) {
      maug_munlock( tile_def->cprops, cprops );
   }

   return retval;
}

MERROR_RETVAL mtilemap_parser_parse_token(
   const char* token, size_t token_sz, void* parser_arg
) {
   size_t j = 1;
   MERROR_RETVAL retval = MERROR_OK;
   struct MTILEMAP_TILE_DEF* tile_defs = NULL;
   struct MTILEMAP_LAYER* layers = NULL;
   struct MTILEMAP_LAYER* tiles_layer = NULL;
   struct MTILEMAP_PARSER* parser = (struct MTILEMAP_PARSER*)parser_arg;
   size_t* tiles = NULL;

   if( 0 < parser->t->tile_defs_sz ) {
      maug_mlock( parser->t->tile_defs, tile_defs );
      maug_cleanup_if_null_alloc( struct MTILEMAP_TILE_DEF*, tile_defs );
   }

   if( MTILEMAP_PSTATE_LIST == mjson_parser_pstate( &(parser->jparser) ) ) {

      if( MTILEMAP_MSTATE_LAYER_DATA == parser->mstate ) {

         /* TODO: Can we read tiles ahead of time to know and allocate that
          *       many in advance? */

         maug_mlock( parser->t->layers, layers );
         maug_cleanup_if_null_alloc( struct MTILEMAP_LAYER*, layers );
         tiles_layer = &(layers[parser->t->layers_sz - 1]);

         if( tiles_layer->tiles_sz + 1 >= tiles_layer->tiles_sz_max ) {
            retval = mtilemap_alloc_tiles(
               tiles_layer,
               0 == tiles_layer->tiles_sz_max ? MTILEMAP_TILES_SZ_INIT : 
                  tiles_layer->tiles_sz_max * 2 );
            maug_cleanup_if_not_ok();
         }

         maug_mlock( tiles_layer->tiles, tiles );
         maug_cleanup_if_null_alloc( size_t*, tiles );

         /* Parse tilemap tile. */
         tiles[tiles_layer->tiles_sz] = atoi( token );
         tiles_layer->tiles_sz++;

      }
      goto cleanup;

   } else if(
      MTILEMAP_PSTATE_OBJECT_VAL == 
         mjson_parser_pstate( &(parser->jparser) )
   ) {
      if( MTILEMAP_MSTATE_TILESETS_FGID == parser->mstate ) {
         parser->t->tileset_fgid = atoi( token );
         debug_printf(
            MTILEMAP_TRACE_LVL, "tileset FGID set to: " SIZE_T_FMT,
            parser->t->tileset_fgid );
         mtilemap_parser_mstate( parser, MTILEMAP_MSTATE_TILESETS );

      } else if( MTILEMAP_MSTATE_TILESETS_SRC == parser->mstate ) {
         debug_printf( MTILEMAP_TRACE_LVL, "parsing %s...", token );
         parser->tj_parse_cb( token, parser->t );
         mtilemap_parser_mstate( parser, MTILEMAP_MSTATE_TILESETS );

      } else if( MTILEMAP_MSTATE_TILES_ID == parser->mstate ) {
         /* Parse tile ID. */
         parser->tileset_id_cur = atoi( token );
         debug_printf(
            MTILEMAP_TRACE_LVL,
            "select tile ID: " SIZE_T_FMT, parser->tileset_id_cur );
         mtilemap_parser_mstate( parser, MTILEMAP_MSTATE_TILES );

         /* Make sure tile ID is available. */
         if( parser->tileset_id_cur >= parser->t->tile_defs_sz ) {
            maug_munlock( parser->t->tile_defs, tile_defs );
            retval = mtilemap_alloc_tile_defs(
               parser->t, parser->tileset_id_cur + 1 );
            maug_cleanup_if_not_ok();
         }

      } else if( MTILEMAP_MSTATE_TILES_IMAGE == parser->mstate ) {
         debug_printf(
            MTILEMAP_TRACE_LVL, "setting tile ID " SIZE_T_FMT "...",
            parser->tileset_id_cur );

         /* Parse tile image. */
         strncpy(
            tile_defs[parser->tileset_id_cur].image_path,
            token,
            MTILEMAP_TILESET_IMAGE_STR_SZ_MAX );

         debug_printf(
            MTILEMAP_TRACE_LVL, "set tile ID " SIZE_T_FMT " to: %s",
            parser->tileset_id_cur,
            tile_defs[parser->tileset_id_cur].image_path );
         mtilemap_parser_mstate( parser, MTILEMAP_MSTATE_TILES );

      } else if( MTILEMAP_MSTATE_TILES_PROP == parser->mstate ) {
         mtilemap_parser_mstate( parser, MTILEMAP_MSTATE_TILES );

      } else if( MTILEMAP_MSTATE_TILES_PROP_N == parser->mstate ) {
         retval = mtilemap_parser_tile_set_cprop_name(
            parser->tileset_id_cur,
            &(tile_defs[parser->tileset_id_cur]), token );
         maug_cleanup_if_not_ok();
         mtilemap_parser_mstate( parser, MTILEMAP_MSTATE_TILES_PROP );

      } else if( MTILEMAP_MSTATE_TILES_PROP_T == parser->mstate ) {
         retval = mtilemap_parser_tile_set_cprop_type(
            &(tile_defs[parser->tileset_id_cur]), token );
         maug_cleanup_if_not_ok();
         mtilemap_parser_mstate( parser, MTILEMAP_MSTATE_TILES_PROP );

      } else if( MTILEMAP_MSTATE_TILES_PROP_V == parser->mstate ) {
         retval = mtilemap_parser_tile_set_cprop_value(
            &(tile_defs[parser->tileset_id_cur]), token );
         maug_cleanup_if_not_ok();
         mtilemap_parser_mstate( parser, MTILEMAP_MSTATE_TILES_PROP );

      } else if( MTILEMAP_MSTATE_HEIGHT == parser->mstate ) {
         parser->t->tiles_h = atoi( token );
         debug_printf(
            MTILEMAP_TRACE_LVL, "tilemap height: %d", parser->t->tiles_h );
         mtilemap_parser_mstate( parser, MTILEMAP_MSTATE_NONE );

      } else if( MTILEMAP_MSTATE_WIDTH == parser->mstate ) {
         parser->t->tiles_w = atoi( token );
         debug_printf(
            MTILEMAP_TRACE_LVL, "tilemap width: %d", parser->t->tiles_h );
         mtilemap_parser_mstate( parser, MTILEMAP_MSTATE_NONE );

      } else if( MTILEMAP_MSTATE_LAYER_NAME == parser->mstate ) {
         /* TODO: Store */
         mtilemap_parser_mstate( parser, MTILEMAP_MSTATE_LAYERS );
         
      } else if( MTILEMAP_MSTATE_GRID == parser->mstate ) {
         /* This only has an mstate to differentiate its height/width children
            * from those in the tileset root.
            */
         mtilemap_parser_mstate( parser, MTILEMAP_MSTATE_NONE );

      }
      goto cleanup;
   }

   /* Figure out what the key is for. */
   while( '\0' != gc_mtilemap_mstate_tokens[j][0] ) {
      if(
         /* This state can only be
          * reached THROUGH that parent state. This allows us to have
          * keys with the same name but different parents!
          */
         parser->mstate == gc_mtilemap_mstate_parents[j] &&
         /* Make sure tokens match. */
         strlen( gc_mtilemap_mstate_tokens[j] ) == token_sz &&
         0 == strncmp(
            token, gc_mtilemap_mstate_tokens[j], token_sz
         )
      ) {
         /* Found it! */
         mtilemap_parser_mstate( parser, j );
         break;
      }
      j++;
   }

   /* Special prep work. */

   if( MTILEMAP_MSTATE_LAYER_DATA == parser->mstate ) {

      if( NULL != layers ) {
         maug_munlock( parser->t->layers, layers );
         maug_cleanup_if_null_alloc(
            struct MTILEMAP_LAYER*, parser->t->layers );
      }

      retval = mtilemap_alloc_layers( parser->t, parser->t->layers_sz + 1 );
      maug_cleanup_if_not_ok();
   }

cleanup:

   if( NULL != tile_defs ) {
      maug_munlock( parser->t->tile_defs, tile_defs );
   }

   if( NULL != tiles ) {
      maug_munlock( layers[parser->t->layers_sz - 1].tiles, tiles );
   }

   if( NULL != layers ) {
      maug_munlock( parser->t->layers, layers );
   }

   return retval;
}

/* === */

MERROR_RETVAL mtilemap_json_close_list( void* parg ) {
   struct MTILEMAP_PARSER* parser = (struct MTILEMAP_PARSER*)parg;

   if( MTILEMAP_MSTATE_LAYER_DATA == parser->mstate ) {
      mtilemap_parser_mstate( parser, MTILEMAP_MSTATE_LAYERS );
   } else if( MTILEMAP_MSTATE_LAYERS == parser->mstate ) {
      mtilemap_parser_mstate( parser, MTILEMAP_MSTATE_NONE );
   } else if( MTILEMAP_MSTATE_TILESETS == parser->mstate ) {
      mtilemap_parser_mstate( parser, MTILEMAP_MSTATE_NONE );
   }

   return MERROR_OK;
}

/* === */

MERROR_RETVAL
mtilemap_parse_json_file( const char* filename, struct MTILEMAP* t ) {
   MERROR_RETVAL retval = MERROR_OK;
   MAUG_MHANDLE parser_h = (MAUG_MHANDLE)NULL;
   struct MTILEMAP_PARSER* parser = NULL;
   char filename_path[RETROFLAT_PATH_MAX];
   MAUG_MHANDLE bytes_h = (MAUG_MHANDLE)NULL;
   size_t bytes_sz = 0;
   uint8_t* bytes = NULL;
   size_t i = 0;

   /* Initialize parser. */
   parser_h = maug_malloc( 1, sizeof( struct MTILEMAP_PARSER ) );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, parser_h );

   maug_mlock( parser_h, parser );
   maug_cleanup_if_null_alloc( struct MTILEMAP_PARSER*, parser );
   maug_mzero( parser, sizeof( struct MTILEMAP_PARSER ) );

   parser->t = t;
   parser->tj_parse_cb = mtilemap_parse_json_file;
   parser->jparser.token_parser = mtilemap_parser_parse_token;
   parser->jparser.token_parser_arg = parser;
   parser->jparser.close_list = mtilemap_json_close_list;
   parser->jparser.close_list_arg = parser;

   /* Setup filename path. */
   memset( filename_path, '\0', RETROFLAT_PATH_MAX );
   /* TODO: Configurable path. */
   maug_snprintf( filename_path, RETROFLAT_PATH_MAX, "mapsrc/%s", filename );

   /* TODO: fread chars one by one from disk using retrofil_cread. */
   retval = retrofil_open_mread( filename_path, &bytes_h, &bytes_sz );
   maug_cleanup_if_not_ok();

   maug_mlock( bytes_h, bytes );
   maug_cleanup_if_null_alloc( uint8_t*, bytes );

   /* Parse JSON and react to state. */
   for( i = 0 ; bytes_sz > i ; i++ ) {
      retval = mjson_parse_c( &(parser->jparser), bytes[i] );
      if( MERROR_OK != retval ) {
         error_printf( "error parsing JSON!" );
         goto cleanup;
      }
   }

cleanup:

   if( NULL != bytes ) {
      maug_munlock( bytes_h, bytes );
   }

   if( NULL != bytes_h ) {
      retrofil_close_mread( &bytes_h, bytes_sz );
   }

   if( NULL != parser ) {
      maug_munlock( parser_h, parser );
   }

   if( NULL != parser_h ) {
      maug_mfree( parser_h );
   }

   return retval;
}

/* === */

MERROR_RETVAL mtilemap_alloc_tile_defs( struct MTILEMAP* t, size_t ndefs ) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MTILEMAP_TILE_DEF* tile_defs_new_h = NULL;
   struct MTILEMAP_TILE_DEF* tile_defs = NULL;
   size_t zero_sz = 0;

   if( 0 == t->tile_defs_sz ) {
      debug_printf( MTILEMAP_TRACE_LVL,
         "allocating " SIZE_T_FMT " tile_defs...", ndefs );
      t->tile_defs = maug_malloc( ndefs, sizeof( struct MTILEMAP_TILE_DEF ) );
      maug_cleanup_if_null_alloc( MAUG_MHANDLE, t->tile_defs );

   } else {
      debug_printf( MTILEMAP_TRACE_LVL,
         "reallocating " SIZE_T_FMT " tile_defs...", ndefs );
      maug_mrealloc_test(
         tile_defs_new_h, t->tile_defs, 
         ndefs, sizeof( struct MTILEMAP_TILE_DEF ) );

   }

   /* Zero new allocs. */
   maug_mlock( t->tile_defs, tile_defs );
   maug_cleanup_if_null_alloc( struct MTILEMAP_TILE_DEF*, tile_defs );
   zero_sz = ndefs - t->tile_defs_sz;
   debug_printf( MTILEMAP_TRACE_LVL,
      "zeroing " SIZE_T_FMT " new defs...", zero_sz );
   maug_mzero(
      &(tile_defs[t->tile_defs_sz]),
      zero_sz * sizeof( struct MTILEMAP_TILE_DEF ) );

   /* Set new sz. */
   t->tile_defs_sz = ndefs;

cleanup:

   if( NULL != tile_defs ) {
      maug_munlock( t->tile_defs, tile_defs );
   }

   return retval;
}

/* === */

MERROR_RETVAL mtilemap_alloc_layers( struct MTILEMAP* t, size_t ndefs ) {
   struct MTILEMAP_LAYER* layers = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   size_t zero_sz = 0;
   MAUG_MHANDLE layers_new_h = NULL;

   /* TODO: Use layers_sz_max */

   if( 0 == t->layers_sz ) {
      debug_printf( MTILEMAP_TRACE_LVL,
         "alloc for layer #" SIZE_T_FMT " (" SIZE_T_FMT " bytes)...",
         ndefs, sizeof( struct MTILEMAP_LAYER ) );
      t->layers = maug_malloc( ndefs, sizeof( struct MTILEMAP_LAYER ) );
      maug_cleanup_if_null_alloc( MAUG_MHANDLE, t->layers );

   } else {
      debug_printf( MTILEMAP_TRACE_LVL,
         "realloc for layer #" SIZE_T_FMT " (" SIZE_T_FMT " bytes)...",
         ndefs, sizeof( struct MTILEMAP_LAYER ) );
      maug_mrealloc_test(
         layers_new_h, t->layers, ndefs, sizeof( struct MTILEMAP_LAYER ) );
   }

   /* Zero new allocs. */
   maug_mlock( t->layers, layers );
   maug_cleanup_if_null_alloc( struct MTILEMAP_LAYER*, layers );
   zero_sz = ndefs - t->layers_sz;
   debug_printf( MTILEMAP_TRACE_LVL,
      "zeroing " SIZE_T_FMT " new layers...", zero_sz );
   maug_mzero(
      &(layers[t->layers_sz]),
      zero_sz * sizeof( struct MTILEMAP_LAYER ) );

   t->layers_sz++;

cleanup:

   if( NULL != layers ) {
      maug_munlock( t->layers, layers );
   }

   return retval;
}

/* === */

MERROR_RETVAL mtilemap_alloc_tiles(
   struct MTILEMAP_LAYER* layer, size_t ndefs
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t zero_sz = 0;
   size_t* tiles = NULL;
   MAUG_MHANDLE tiles_new_h = NULL;

   if( 0 == layer->tiles_sz_max ) {
      debug_printf( MTILEMAP_TRACE_LVL,
         "allocating %d tiles...",
         MTILEMAP_TILES_SZ_INIT );
      layer->tiles = maug_malloc( ndefs, sizeof( size_t ) );
      maug_cleanup_if_null_alloc( size_t*, layer->tiles );

   } else {
      /* Double tiles size if more space needed. */
      debug_printf( MTILEMAP_TRACE_LVL,
         "reallocating " SIZE_T_FMT " tiles...", ndefs );
      maug_mrealloc_test(
         tiles_new_h, layer->tiles, ndefs, sizeof( size_t ) );
   }

   /* Zero new allocs. */
   maug_mlock( layer->tiles, tiles );
   maug_cleanup_if_null_alloc( size_t*, tiles );
   zero_sz = ndefs - layer->tiles_sz_max;
   debug_printf( MTILEMAP_TRACE_LVL,
      "zeroing " SIZE_T_FMT " new tiles...", zero_sz );
   maug_mzero( &(tiles[layer->tiles_sz]), zero_sz * sizeof( size_t ) );

   layer->tiles_sz_max = ndefs;

cleanup:

   if( NULL != tiles ) {
      maug_munlock( layer->tiles, tiles );
   }

   return retval;
}

/* === */

MERROR_RETVAL mtilemap_free( struct MTILEMAP* t ) {
   size_t i = 0;
   struct MTILEMAP_TILE_DEF* tile_defs = NULL;
   struct MTILEMAP_LAYER* layers = NULL;
   MERROR_RETVAL retval = MERROR_OK;

   maug_mlock( t->tile_defs, tile_defs );
   if( NULL == tile_defs ) {
      goto free_layers;
   }

   for( i = 0 ; t->tile_defs_sz > i ; i++ ) {
      if( NULL != tile_defs[i].cprops ) {
         maug_mfree( tile_defs[i].cprops );
         tile_defs[i].cprops_sz = 0;
      }
   }
   
   maug_munlock( t->tile_defs, tile_defs );
   maug_mfree( t->tile_defs );

free_layers:

   maug_mlock( t->layers, layers );
   maug_cleanup_if_null_alloc( struct MTILEMAP_LAYER*, layers );

   for( i = 0 ; t->layers_sz > i ; i++ ) {
      if( NULL != layers[i].tiles ) {
         maug_mfree( layers[i].tiles );
         layers[i].tiles_sz = 0;
      }
   }
 
   maug_munlock( t->layers, layers );
   maug_mfree( t->layers );

cleanup:

   return retval;
}

#endif /* MTILEMAP_C */

/*! \} */ /* maug_tilemap */

#endif /* !MTILEMAP_H */

