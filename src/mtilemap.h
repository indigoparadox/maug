
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

#define MTILEMAP_PARSER_MODE_MAP    0
#define MTILEMAP_PARSER_MODE_DEFS   1

#define MTILEMAP_TILE_FLAG_BLOCK       1
#define MTILEMAP_TILE_FLAG_ROT_X       2
#define MTILEMAP_TILE_FLAG_ROT_Z       4
#define MTILEMAP_TILE_FLAG_PRTCL_FIRE  8

/*! \} */ /* maug_tilemap_defs_types */

typedef uint16_t mtilemap_tile_t;

#define MTILEMAP_IDX_FMT "%u"

struct MTILEMAP_TILE_DEF {
   uint8_t flags;
   char image_path[MTILEMAP_TILESET_IMAGE_STR_SZ_MAX];
};

/*! \} */ /* maug_tilemap_defs */

struct MTILEMAP_LAYER {
   size_t sz;
#ifdef MAUG_NO_MMEM
   mtilemap_tile_t* tiles_h;
#else
   MAUG_MHANDLE tiles_h;
#endif /* MAUG_NO_MMEM */
};

/* TODO: Rework this based on perpix grid to reduce lockable handles needed. */

struct MTILEMAP {
   uint32_t sz;
   uint32_t layers_count;
   uint32_t layers_offset;
   char name[MTILEMAP_NAME_SZ_MAX];
   size_t tileset_fgid;
   uint8_t tiles_h;
   uint8_t tiles_w;
};

#define mtilemap_get_first_layer( t ) \
   (struct MTILEMAP_LAYER*)(((uint8_t*)(t)) + (t)->layers_offset)

#define mtilemap_get_next_layer( t, l ) \
   (struct MTILEMAP_LAYER*)(((uint8_t*)(l)) + (l)->layer_sz)

/**
 * \addtogroup maug_tilemap_parser Tilemap Parser
 * \{
 */

typedef MERROR_RETVAL
(*mtilemap_tj_parse_cb)(
   const char* filename, MAUG_MHANDLE* p_tm_h,
   MAUG_MHANDLE* p_td_h, size_t* p_td_c );

struct MTILEMAP_PARSER {
   uint8_t mstate;
   /* TODO: Use flags and combine these. */
   uint8_t pass;
   uint8_t mode;
   size_t layer_tile_iter;
   size_t pass_layer_iter;
   /**
    * \brief Highest tileset ID on first pass and next ID to be assigned
    *        on second.
    */
   size_t tileset_id_cur;
   size_t tiles_w;
   size_t tiles_h;
   struct MTILEMAP* t;
   mtilemap_tj_parse_cb tj_parse_cb;
   struct MJSON_PARSER jparser;
   MAUG_MHANDLE* p_tile_defs_h;
   size_t* p_tile_defs_count;
};

#define MTILEMAP_PARSER_MSTATE_TABLE( f ) \
   f( MTILESTATE_NONE,              0, "", 0, 0 ) \
   f( MTILESTATE_HEIGHT,            1, "height",   0              , 0 ) \
   f( MTILESTATE_WIDTH,             2, "width",    0              , 0 ) \
   f( MTILESTATE_LAYERS,            3, "layers",   0              , 0 ) \
   f( MTILESTATE_LAYER_DATA,        4, "data",     15 /* LAYER */, 0 ) \
   f( MTILESTATE_LAYER_NAME,        5, "name",     15 /* LAYER */, 0 ) \
   f( MTILESTATE_TILES,             6, "tiles",    0              , 1 ) \
   f( MTILESTATE_TILES_ID,          7, "id",       6 /* TILES */  , 1 ) \
   f( MTILESTATE_TILES_IMAGE,       8, "image",    6 /* TILES */  , 1 ) \
   f( MTILESTATE_TILESETS,          9, "tilesets", 0              , 0 ) \
   f( MTILESTATE_TILESETS_SRC,      10, "source",  9 /* TILESETS */, 0 ) \
   f( MTILESTATE_TILESETS_FGID,     11, "firstgid",9 /* TILESETS */, 0 ) \
   f( MTILESTATE_TILESETS_PROP,     12, "firstgid",9 /* TILESETS */, 0 ) \
   f( MTILESTATE_GRID,              13, "grid",    0               , 1 ) \
   f( MTILESTATE_TILES_PROP,        14, "properties",6 /* TILES */ , 1 ) \
   f( MTILESTATE_LAYER,             15, "layers", /* [sic] */ 3    , 0 ) \
   f( MTILESTATE_TILES_PROP_NAME,   16, "name",    14 /* TILES_PROP */ , 1 ) \
   f( MTILESTATE_TILES_PROP_VAL,    17, "value",   14 /* TILES_PROP */ , 1 )

MERROR_RETVAL
mtilemap_parse_json_c( struct MTILEMAP_PARSER* parser, char c );

MERROR_RETVAL mtilemap_parse_json_file(
   const char* filename, MAUG_MHANDLE* p_tilemap_h,
   MAUG_MHANDLE* p_tile_defs_h, size_t* p_tile_defs_count );

/*! \} */ /* maug_tilemap_parser */

struct MTILEMAP_LAYER* mtilemap_get_layer_p(
   struct MTILEMAP* tilemap, uint32_t layer_idx );

MERROR_RETVAL mtilemap_alloc_tile_defs(
   MAUG_MHANDLE* p_tile_defs_h, size_t* p_tile_defs_count, size_t ndefs );

MERROR_RETVAL
mtilemap_alloc(
   MAUG_MHANDLE* p_tilemap_h, size_t w, size_t h, size_t layers_count );

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
   parser->mstate = new_mstate; \
   debug_printf( \
      MTILEMAP_TRACE_LVL, "parser mstate: %s", \
         gc_mtilemap_mstate_names[parser->mstate] );

#  define MTILEMAP_PARSER_MSTATE_TABLE_CONST( name, idx, tokn, parent, m ) \
      static MAUG_CONST uint8_t name = idx;

MTILEMAP_PARSER_MSTATE_TABLE( MTILEMAP_PARSER_MSTATE_TABLE_CONST )

#  define MTILEMAP_PARSER_MSTATE_TABLE_NAME( name, idx, tokn, parent, m ) \
      #name,

static MAUG_CONST char* gc_mtilemap_mstate_names[] = {
   MTILEMAP_PARSER_MSTATE_TABLE( MTILEMAP_PARSER_MSTATE_TABLE_NAME )
   ""
};

#  define MTILEMAP_PARSER_MSTATE_TABLE_TOKEN( name, idx, tokn, parent, m ) \
      tokn,

static MAUG_CONST char* gc_mtilemap_mstate_tokens[] = {
   MTILEMAP_PARSER_MSTATE_TABLE( MTILEMAP_PARSER_MSTATE_TABLE_TOKEN )
   ""
};

#  define MTILEMAP_PARSER_MSTATE_TABLE_PARNT( name, idx, tokn, parent, m ) \
      parent,

static MAUG_CONST uint8_t gc_mtilemap_mstate_parents[] = {
   MTILEMAP_PARSER_MSTATE_TABLE( MTILEMAP_PARSER_MSTATE_TABLE_PARNT )
   0
};

#  define MTILEMAP_PARSER_MSTATE_TABLE_MODE( name, idx, tokn, parent, m ) \
      m,

static MAUG_CONST uint8_t gc_mtilemap_mstate_modes[] = {
   MTILEMAP_PARSER_MSTATE_TABLE( MTILEMAP_PARSER_MSTATE_TABLE_MODE )
   0
};

/* === */

static void mtilemap_parser_match_token(
   const char* token, size_t token_sz, struct MTILEMAP_PARSER* parser
) {
   size_t j = 1;

   /* Figure out what the key is for. */
   while( '\0' != gc_mtilemap_mstate_tokens[j][0] ) {
      if(
         /* Make sure tokens match. */
         strlen( gc_mtilemap_mstate_tokens[j] ) != token_sz ||
         0 != strncmp(
            token, gc_mtilemap_mstate_tokens[j], token_sz
         )
      ) {
         j++;
         continue;

      } else if(
         /* This state can only be
          * reached THROUGH that parent state. This allows us to have
          * keys with the same name but different parents!
          */
         parser->mstate != gc_mtilemap_mstate_parents[j]
      ) {
         debug_printf(
            MTILEMAP_TRACE_LVL,
            "found token \"%s\" but incorrect parent %s (%d) "
               "(needs %s (%d))!",
            token,
            gc_mtilemap_mstate_names[parser->mstate],
            parser->mstate,
            gc_mtilemap_mstate_names[gc_mtilemap_mstate_parents[j]],
            gc_mtilemap_mstate_parents[j] );
         j++;
         continue;

      } else if(
         /* None works in all modes! */
         /* MTILESTATE_NONE != parser->mstate && */
         parser->mode != gc_mtilemap_mstate_modes[j]
      ) {
         debug_printf(
            MTILEMAP_TRACE_LVL, "found token %s but incorrect mode %u!",
            token,
            gc_mtilemap_mstate_modes[j] );
         j++;
         continue;

      } else {
         /* Found it! */
         mtilemap_parser_mstate( parser, j );
         return;
      }
   }
}

/* === */

MERROR_RETVAL mtilemap_parser_parse_tiledef_token(
   const char* token, size_t token_sz, void* parser_arg
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MTILEMAP_TILE_DEF* tile_defs = NULL;
   struct MTILEMAP_PARSER* parser = (struct MTILEMAP_PARSER*)parser_arg;
   size_t tileset_id_parsed = 0;

   if( 0 < *(parser->p_tile_defs_count) ) {
      maug_mlock( *(parser->p_tile_defs_h), tile_defs );
      maug_cleanup_if_null_alloc( struct MTILEMAP_TILE_DEF*, tile_defs );
   }

   if(
      MTILEMAP_PSTATE_OBJECT_VAL == 
         mjson_parser_pstate( &(parser->jparser) )
   ) {
      if( MTILESTATE_TILES_ID == parser->mstate ) {
         mtilemap_parser_mstate( parser, MTILESTATE_TILES );
         if( 0 == parser->pass ) {
            /* Parse tile ID. */
            tileset_id_parsed = atoi( token );
            if( tileset_id_parsed > parser->tileset_id_cur ) {
               parser->tileset_id_cur = tileset_id_parsed;
               debug_printf(
                  MTILEMAP_TRACE_LVL,
                  "new highest tile ID: " SIZE_T_FMT,
                  parser->tileset_id_cur );
            }
         } else {
            /* TODO: atoi or atoul? */
            parser->tileset_id_cur = atoi( token );
            debug_printf(
               MTILEMAP_TRACE_LVL,
               "next tile ID: " SIZE_T_FMT,
               parser->tileset_id_cur );
         }

      } else if( MTILESTATE_TILES_IMAGE == parser->mstate ) {
         if( 1 == parser->pass ) {
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
         }
         mtilemap_parser_mstate( parser, MTILESTATE_TILES );

      } else if( MTILESTATE_TILES_PROP_NAME == parser->mstate ) {

         if( 1 == parser->pass ) {
            if( 0 == strncmp(
               parser->jparser.token,
               "rotate_x",
               parser->jparser.token_sz
            ) ) {
               /* Found flag: rotate X! */
               /* TODO: Read boolean value. */
               assert( parser->tileset_id_cur < 
                  *(parser->p_tile_defs_count) );
               tile_defs[parser->tileset_id_cur].flags |= 
                  MTILEMAP_TILE_FLAG_ROT_X;
            }

            /* TODO: Read boolean Z and fire particles prop/flag. */
         }

         mtilemap_parser_mstate( parser, MTILESTATE_TILES_PROP );

      } else if( MTILESTATE_TILES_PROP_VAL == parser->mstate ) {

         mtilemap_parser_mstate( parser, MTILESTATE_TILES_PROP );
      }
      goto cleanup;
   }

   mtilemap_parser_match_token( token, token_sz, parser );

cleanup:

   if( NULL != tile_defs ) {
      maug_munlock( *(parser->p_tile_defs_h), tile_defs );
   }

   return retval;
}

MERROR_RETVAL mtilemap_parser_parse_token(
   const char* token, size_t token_sz, void* parser_arg
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MTILEMAP_TILE_DEF* tile_defs = NULL;
   struct MTILEMAP_LAYER* tiles_layer = NULL;
   struct MTILEMAP_PARSER* parser = (struct MTILEMAP_PARSER*)parser_arg;
   mtilemap_tile_t* tiles = NULL;

   if( 0 < *(parser->p_tile_defs_count) ) {
      maug_mlock( *(parser->p_tile_defs_h), tile_defs );
      maug_cleanup_if_null_alloc( struct MTILEMAP_TILE_DEF*, tile_defs );
   }

   if( MTILEMAP_PSTATE_LIST == mjson_parser_pstate( &(parser->jparser) ) ) {
      if(
         1 == parser->pass &&
         MTILESTATE_LAYER_DATA == parser->mstate
      ) {
         /*
         assert( NULL != *(parser->p_tilemap_h) );
         maug_mlock( *(parser->p_tilemap_h), tilemap );
         maug_cleanup_if_null_alloc( struct MTILEMAP*, tilemap );
         */

         debug_printf( MTILEMAP_TRACE_LVL,
            "selecting layer " SIZE_T_FMT "...",
            parser->pass_layer_iter );
         assert( NULL != parser->t );
         tiles_layer = mtilemap_get_layer_p(
            parser->t, parser->pass_layer_iter );
         assert( NULL != tiles_layer );

         assert( NULL == tiles );
         assert( NULL != tiles_layer->tiles_h );
         maug_mlock( tiles_layer->tiles_h, tiles );
         maug_cleanup_if_null_alloc( mtilemap_tile_t*, tiles );

         /* Parse tilemap tile. */
         debug_printf( MTILEMAP_TRACE_LVL,
            "layer " SIZE_T_FMT " tile: " SIZE_T_FMT " (tiles: %p)", 
            parser->pass_layer_iter, parser->layer_tile_iter,
            tiles );
         assert( NULL != token );
         assert( NULL != parser );
         assert( NULL != tiles );
         assert( parser->layer_tile_iter <
            parser->t->tiles_w * parser->t->tiles_h );
         assert( 0 == tiles[parser->layer_tile_iter] );
         tiles[parser->layer_tile_iter] = atoi( token );
      }
      parser->layer_tile_iter++;
      goto cleanup;

   } else if(
      MTILEMAP_PSTATE_OBJECT_VAL == 
         mjson_parser_pstate( &(parser->jparser) )
   ) {
      if( MTILESTATE_TILESETS_FGID == parser->mstate ) {
         if( 1 == parser->pass ) {
            parser->t->tileset_fgid = atoi( token );
            debug_printf(
               MTILEMAP_TRACE_LVL, "tileset FGID set to: " SIZE_T_FMT,
               parser->t->tileset_fgid );
         }
         mtilemap_parser_mstate( parser, MTILESTATE_TILESETS );

      } else if( MTILESTATE_TILESETS_SRC == parser->mstate ) {
         if( 1 == parser->pass ) {
            debug_printf( MTILEMAP_TRACE_LVL, "parsing %s...", token );
            parser->tj_parse_cb(
               token, NULL, parser->p_tile_defs_h,
               parser->p_tile_defs_count );
         }
         mtilemap_parser_mstate( parser, MTILESTATE_TILESETS );

      } else if( MTILESTATE_HEIGHT == parser->mstate ) {
         if( 0 == parser->pass ) {
            parser->tiles_h = atoi( token );
            debug_printf(
               MTILEMAP_TRACE_LVL, "tilemap height: " SIZE_T_FMT,
               parser->tiles_h );
         }
         mtilemap_parser_mstate( parser, MTILESTATE_NONE );

      } else if( MTILESTATE_WIDTH == parser->mstate ) {
         if( 0 == parser->pass ) {
            parser->tiles_w = atoi( token );
            debug_printf(
               MTILEMAP_TRACE_LVL, "tilemap width: " SIZE_T_FMT,
               parser->tiles_w );
         }
         mtilemap_parser_mstate( parser, MTILESTATE_NONE );

      } else if( MTILESTATE_LAYER_NAME == parser->mstate ) {
         /* TODO: Store */
         mtilemap_parser_mstate( parser, MTILESTATE_LAYER );
         
      }
      goto cleanup;
   }

   mtilemap_parser_match_token( token, token_sz, parser );

cleanup:

   if( NULL != tile_defs ) {
      maug_munlock( *(parser->p_tile_defs_h), tile_defs );
   }

   if( NULL != tiles ) {
      maug_munlock( tiles_layer->tiles_h, tiles );
   }

   return retval;
}

/* === */

MERROR_RETVAL mtilemap_json_close_list( void* parg ) {
   struct MTILEMAP_PARSER* parser = (struct MTILEMAP_PARSER*)parg;

   if( MTILESTATE_LAYER_DATA == parser->mstate ) {
      assert( MTILEMAP_PARSER_MODE_MAP == parser->mode );
      assert( parser->layer_tile_iter == 1600 );
      mtilemap_parser_mstate( parser, MTILESTATE_LAYER );

   } else if( MTILESTATE_LAYERS == parser->mstate ) {
      assert( MTILEMAP_PARSER_MODE_MAP == parser->mode );
      mtilemap_parser_mstate( parser, MTILESTATE_NONE );

   } else if( MTILESTATE_TILESETS == parser->mstate ) {
      assert( MTILEMAP_PARSER_MODE_MAP == parser->mode );
      mtilemap_parser_mstate( parser, MTILESTATE_NONE );

   } else if( MTILESTATE_TILES_PROP == parser->mstate ) {
      assert( MTILEMAP_PARSER_MODE_DEFS == parser->mode );
      mtilemap_parser_mstate( parser, MTILESTATE_TILES );
   }

   return MERROR_OK;
}

/* === */

MERROR_RETVAL mtilemap_json_open_obj( void* parg ) {
   struct MTILEMAP_PARSER* parser = (struct MTILEMAP_PARSER*)parg;

   if( MTILESTATE_LAYERS == parser->mstate ) {
      assert( MTILEMAP_PARSER_MODE_MAP == parser->mode );
      /* Reset on open so count is retained for allocating after first 
       * pass. */
      parser->layer_tile_iter = 0;
      mtilemap_parser_mstate( parser, MTILESTATE_LAYER );
   }

   return MERROR_OK;
}

/* === */

MERROR_RETVAL mtilemap_json_close_obj( void* parg ) {
   struct MTILEMAP_PARSER* parser = (struct MTILEMAP_PARSER*)parg;

   if( MTILESTATE_LAYER == parser->mstate ) {
      assert( MTILEMAP_PARSER_MODE_MAP == parser->mode );
      assert( parser->layer_tile_iter == 1600 );
      debug_printf( MTILEMAP_TRACE_LVL,
         "incrementing pass layer to " SIZE_T_FMT " after " SIZE_T_FMT
            " tiles...",
         parser->pass_layer_iter + 1, parser->layer_tile_iter );
      parser->pass_layer_iter++;
      mtilemap_parser_mstate( parser, MTILESTATE_LAYERS );

   } else if( MTILESTATE_GRID == parser->mstate ) {
      mtilemap_parser_mstate( parser, MTILESTATE_NONE );
   }

   return MERROR_OK;
}

/* === */

#if 0
MERROR_RETVAL mtilemap_json_close_val( void* parg ) {
   struct MTILEMAP_PARSER* parser = (struct MTILEMAP_PARSER*)parg;

   if(
      MTILESTATE_TILES_PROP == parser->mstate
   ) {
      assert( MTILEMAP_PARSER_MODE_DEFS == parser->mode );
   
      debug_printf( 1, "prop: %s", parser->jparser.token );

      /* mtilemap_parser_mstate( parser, MTILESTATE_TILES_PROP_ROT_X ); */
   }

   return MERROR_OK;
}
#endif

/* === */

MERROR_RETVAL mtilemap_parse_json_file(
   const char* filename, MAUG_MHANDLE* p_tilemap_h,
   MAUG_MHANDLE* p_tile_defs_h, size_t* p_tile_defs_count
) {
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

   parser->tj_parse_cb = mtilemap_parse_json_file;

   /* Setup filename path. */
   memset( filename_path, '\0', RETROFLAT_PATH_MAX );
   /* TODO: Configurable path. */
   maug_snprintf(
      filename_path, RETROFLAT_PATH_MAX, "mapsrc/%s", filename );

   debug_printf( MTILEMAP_TRACE_LVL, "opening %s...", filename_path );

   /* TODO: fread chars one by one from disk using retrofil_cread. */
   retval = retrofil_open_mread( filename_path, &bytes_h, &bytes_sz );
   maug_cleanup_if_not_ok();

   maug_mlock( bytes_h, bytes );
   maug_cleanup_if_null_alloc( uint8_t*, bytes );

   assert( NULL != p_tile_defs_count );

   /* Parse JSON and react to state. */
   for( parser->pass = 0 ; 2 > parser->pass ; parser->pass++ ) {
      debug_printf( MTILEMAP_TRACE_LVL, "beginning pass #%u...",
         parser->pass );

      /* Reset tilemap parser. */
      parser->mstate = 0;

      /* Reset JSON parser. */
      maug_mzero( &(parser->jparser), sizeof( struct MJSON_PARSER ) );

      /* Figure out if we're parsing a .tmj or .tsj. */
      if( 's' == strrchr( filename, '.' )[2] ) {
         debug_printf( MTILEMAP_TRACE_LVL, "(tile_defs mode)" );
         parser->mode = MTILEMAP_PARSER_MODE_DEFS;
         parser->jparser.token_parser = mtilemap_parser_parse_tiledef_token;
         parser->jparser.token_parser_arg = parser;
         parser->jparser.close_list = mtilemap_json_close_list;
         parser->jparser.close_list_arg = parser;
         parser->jparser.close_obj = mtilemap_json_close_obj;
         parser->jparser.close_obj_arg = parser;
         /*
         parser->jparser.close_val = mtilemap_json_close_val;
         parser->jparser.close_val_arg = parser;
         */
         parser->p_tile_defs_h = p_tile_defs_h;
         parser->p_tile_defs_count = p_tile_defs_count;

         assert( NULL != p_tile_defs_h );
         if( 1 == parser->pass ) {
            /* Allocate tile defs based on highest tile ID found on
             * first pass.
             */
            retval = mtilemap_alloc_tile_defs(
               p_tile_defs_h, p_tile_defs_count,
               parser->tileset_id_cur + 1 );
            maug_cleanup_if_not_ok();
         }
      } else {
         debug_printf( MTILEMAP_TRACE_LVL, "(tilemap mode)" );
         parser->mode = MTILEMAP_PARSER_MODE_MAP;

         parser->jparser.close_list = mtilemap_json_close_list;
         parser->jparser.close_list_arg = parser;
         parser->jparser.open_obj = mtilemap_json_open_obj;
         parser->jparser.open_obj_arg = parser;
         parser->jparser.close_obj = mtilemap_json_close_obj;
         parser->jparser.close_obj_arg = parser;
         parser->jparser.token_parser = mtilemap_parser_parse_token;
         parser->jparser.token_parser_arg = parser;
         parser->p_tile_defs_h = p_tile_defs_h;
         parser->p_tile_defs_count = p_tile_defs_count;

         assert( NULL != p_tilemap_h );
         if( 1 == parser->pass ) {
            /* Allocate tiles for the new layers. */
            retval = mtilemap_alloc(
               p_tilemap_h, parser->tiles_w, parser->tiles_h,
               parser->pass_layer_iter );
            maug_cleanup_if_not_ok();

            maug_mlock( *p_tilemap_h, parser->t );
         }
         parser->pass_layer_iter = 0;
      }

      for( i = 0 ; bytes_sz > i ; i++ ) {
         retval = mjson_parse_c( &(parser->jparser), bytes[i] );
         if( MERROR_OK != retval ) {
            error_printf( "error parsing JSON!" );
            goto cleanup;
         }
      }

      if( 's' != strrchr( filename, '.' )[2] ) {
         debug_printf( MTILEMAP_TRACE_LVL,
            "pass %u found " SIZE_T_FMT " layers",
            parser->pass, parser->pass_layer_iter );
      }
   }

   debug_printf(
      MTILEMAP_TRACE_LVL, "finished parsing %s...", filename_path );

cleanup:

   if( NULL != bytes ) {
      maug_munlock( bytes_h, bytes );
   }

   if( NULL != bytes_h ) {
      retrofil_close_mread( &bytes_h, bytes_sz );
   }

   if( NULL != parser ) {
      if( NULL != parser->t ) {
         maug_munlock( *p_tilemap_h, parser->t );
      }
      maug_munlock( parser_h, parser );
   }

   if( NULL != parser_h ) {
      maug_mfree( parser_h );
   }

   return retval;
}

/* === */

struct MTILEMAP_LAYER* mtilemap_get_layer_p(
   struct MTILEMAP* tilemap, uint32_t layer_idx
) {
   struct MTILEMAP_LAYER* layer_iter = NULL;
   uint8_t* tilemap_buf = (uint8_t*)tilemap;

   if( 0 == tilemap->layers_count || layer_idx >= tilemap->layers_count ) {
      error_printf( "invalid layer " UPRINTF_U32_FMT
         " requested (of " UPRINTF_U32_FMT ")!",
         layer_idx, tilemap->layers_count );
      return NULL;
   }

   /* Advance to first grid. */
   tilemap_buf += sizeof( struct MTILEMAP );
   layer_iter = (struct MTILEMAP_LAYER*)tilemap_buf;
   while( layer_idx > 0 ) {
      tilemap_buf += layer_iter->sz;
      layer_iter = (struct MTILEMAP_LAYER*)tilemap_buf;
      layer_idx--;
   }

   return layer_iter;
}


/* === */

MERROR_RETVAL mtilemap_alloc_tile_defs(
   MAUG_MHANDLE* p_tile_defs_h, size_t* p_tile_defs_count, size_t ndefs
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct MTILEMAP_TILE_DEF* tile_defs = NULL;

   assert( 0 == *p_tile_defs_count );
   assert( NULL == *p_tile_defs_h );
   debug_printf( MTILEMAP_TRACE_LVL,
      "allocating " SIZE_T_FMT " tile_defs...", ndefs );
   *p_tile_defs_h =
      maug_malloc( ndefs, sizeof( struct MTILEMAP_TILE_DEF ) );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, *p_tile_defs_h );
   *p_tile_defs_count = ndefs;

   /* Zero new allocs. */
   maug_mlock( *p_tile_defs_h, tile_defs );
   maug_cleanup_if_null_alloc( struct MTILEMAP_TILE_DEF*, tile_defs );
   maug_mzero( tile_defs, ndefs * sizeof( struct MTILEMAP_TILE_DEF ) );

cleanup:

   if( NULL != tile_defs ) {
      maug_munlock( *p_tile_defs_h, tile_defs );
   }

   return retval;
}

/* === */

MERROR_RETVAL mtilemap_alloc(
   MAUG_MHANDLE* p_tilemap_h, size_t w, size_t h, size_t layers_count
) {
   struct MTILEMAP_LAYER* layer_iter = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   size_t tilemap_sz = 0;
   struct MTILEMAP* tilemap = NULL;
   size_t i = 0;

   tilemap_sz = sizeof( struct MTILEMAP ) +
      (layers_count * sizeof( struct MTILEMAP_LAYER )) +
      (layers_count * (w * h * sizeof( mtilemap_tile_t ) ));

   debug_printf( 1, "allocating new tilemap " SIZE_T_FMT "x" SIZE_T_FMT
      " tiles, " SIZE_T_FMT "layers (" SIZE_T_FMT " bytes)...",
      w, h, layers_count, tilemap_sz );

   *p_tilemap_h = maug_malloc( 1, tilemap_sz );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, *p_tilemap_h );

   maug_mlock( *p_tilemap_h, tilemap );
   maug_cleanup_if_null_alloc( struct MTILEMAP*, tilemap );

   maug_mzero( tilemap, tilemap_sz );
   tilemap->sz = tilemap_sz;
   tilemap->layers_count = layers_count;
   tilemap->tiles_w = w;
   tilemap->tiles_h = h;

   for( i = 0 ; layers_count > i ; i++ ) {
      layer_iter = mtilemap_get_layer_p( tilemap, i );
      assert( NULL != layer_iter );
      layer_iter->sz = sizeof( struct MTILEMAP_LAYER ) +
         (w * h * sizeof( mtilemap_tile_t ));

      retval = mtilemap_alloc_tiles( layer_iter,
         tilemap->tiles_w * tilemap->tiles_h );
      maug_cleanup_if_not_ok();
   }

cleanup:

   if( NULL != tilemap ) {
      maug_munlock( *p_tilemap_h, tilemap );
   }

   return retval;
}

/* === */

MERROR_RETVAL mtilemap_alloc_tiles(
   struct MTILEMAP_LAYER* layer, size_t ndefs
) {
   MERROR_RETVAL retval = MERROR_OK;
   mtilemap_tile_t* tiles = NULL;

   debug_printf( MTILEMAP_TRACE_LVL,
      "allocating %d tiles...",
      MTILEMAP_TILES_SZ_INIT );
   layer->tiles_h = maug_malloc( ndefs, sizeof( mtilemap_tile_t ) );
   maug_cleanup_if_null_alloc( mtilemap_tile_t*, layer->tiles_h );

   /* Zero new allocs. */
   maug_mlock( layer->tiles_h, tiles );
   maug_cleanup_if_null_alloc( mtilemap_tile_t*, tiles );
   maug_mzero( tiles, ndefs * sizeof( mtilemap_tile_t ) );

cleanup:

   if( NULL != tiles ) {
      maug_munlock( layer->tiles_h, tiles );
   }

   return retval;
}

/* === */

MERROR_RETVAL mtilemap_free( struct MTILEMAP* t ) {
   size_t i = 0;
   struct MTILEMAP_LAYER* layer = NULL;
   MERROR_RETVAL retval = MERROR_OK;

   for( i = 0 ; t->layers_count > i ; i++ ) {
      layer = mtilemap_get_layer_p( t, i );
      if( NULL != layer->tiles_h ) {
         maug_mfree( layer->tiles_h );
      }
   }
 
   return retval;
}

#endif /* MTILEMAP_C */

/*! \} */ /* maug_tilemap */

#endif /* !MTILEMAP_H */

