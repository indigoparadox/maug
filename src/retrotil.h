
#ifndef RETROTIL_H
#define RETROTIL_H

#include <mparser.h>
#include <mjson.h>
#include <mfile.h>

/**
 * \addtogroup maug_tilemap Tilemap API
 * \brief Functions and structures for working with tilemaps/grids.
 * \{
 * \file retrotile.h
 */

#ifndef RETROTILE_NAME_SZ_MAX
/*! \brief Maximum number of chars in a RETROTILE::name. */
#  define RETROTILE_NAME_SZ_MAX 10
#endif /* !RETROTILE_NAME_SZ_MAX */

#ifndef RETROTILE_TILESET_IMAGE_STR_SZ_MAX
/*! \brief Maximum number of chars in a RETROTILE_TILE_DEF::image_path. */
#  define RETROTILE_TILESET_IMAGE_STR_SZ_MAX 48
#endif /* !RETROTILE_TILESET_IMAGE_STR_SZ_MAX */

#ifndef RETROTILE_TILE_SCALE_DEFAULT
/*! \brief Default value for RETROTILE::tile_scale. */
#  define RETROTILE_TILE_SCALE_DEFAULT 1.0f
#endif /* !RETROTILE_TILE_SCALE_DEFAULT */

#ifndef RETROTILE_TRACE_LVL
/*! \brief If defined, bring debug printf statements up to this level. */
#  define RETROTILE_TRACE_LVL 0
#endif /* !RETROTILE_TRACE_LVL */

/**
 * \addtogroup \maug_tilemap_defs Tilemap Tile Definitions
 * \{
 */

/**
 * \addtogroup \maug_tilemap_defs_types Tilemap Custom Property Types
 * \{
 */

#define RETROTILE_PARSER_MODE_MAP    0
#define RETROTILE_PARSER_MODE_DEFS   1

#define RETROTILE_TILE_FLAG_BLOCK       1
#define RETROTILE_TILE_FLAG_ROT_X       2
#define RETROTILE_TILE_FLAG_ROT_Z       4
#define RETROTILE_TILE_FLAG_PRTCL_FIRE  8

/*! \} */ /* maug_tilemap_defs_types */

typedef uint16_t retrotile_tile_t;

#define RETROTILE_IDX_FMT "%u"

struct RETROTILE_TILE_DEF {
   uint8_t flags;
   char image_path[RETROTILE_TILESET_IMAGE_STR_SZ_MAX];
};

/*! \} */ /* maug_tilemap_defs */

struct RETROTILE_LAYER {
   size_t sz;
};

struct RETROTILE {
   uint32_t sz;
   uint32_t layers_count;
   uint32_t layers_offset;
   char name[RETROTILE_NAME_SZ_MAX];
   size_t tileset_fgid;
   uint8_t tiles_h;
   uint8_t tiles_w;
   float tile_scale;
};

#define retrotile_get_tile( tilemap, layer, x, y ) \
   (retrotile_get_tiles_p( layer )[(y * tilemap->tiles_w) + x])

#define retrotile_get_tiles_p( layer ) \
   ((retrotile_tile_t*)(((uint8_t*)(layer)) + \
   sizeof( struct RETROTILE_LAYER )))

/**
 * \addtogroup maug_tilemap_parser Tilemap Parser
 * \{
 */

typedef MERROR_RETVAL
(*retrotile_tj_parse_cb)(
   const char* filename, MAUG_MHANDLE* p_tm_h,
   MAUG_MHANDLE* p_td_h, size_t* p_td_c );

struct RETROTILE_PARSER {
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
   struct RETROTILE* t;
   retrotile_tj_parse_cb tj_parse_cb;
   struct MJSON_PARSER jparser;
   MAUG_MHANDLE* p_tile_defs_h;
   size_t* p_tile_defs_count;
};

#define RETROTILE_PARSER_MSTATE_TABLE( f ) \
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
retrotile_parse_json_c( struct RETROTILE_PARSER* parser, char c );

MERROR_RETVAL retrotile_parse_json_file(
   const char* filename, MAUG_MHANDLE* p_tilemap_h,
   MAUG_MHANDLE* p_tile_defs_h, size_t* p_tile_defs_count );

/*! \} */ /* maug_tilemap_parser */

struct RETROTILE_LAYER* retrotile_get_layer_p(
   struct RETROTILE* tilemap, uint32_t layer_idx );

MERROR_RETVAL retrotile_alloc_tile_defs(
   MAUG_MHANDLE* p_tile_defs_h, size_t* p_tile_defs_count, size_t ndefs );

MERROR_RETVAL
retrotile_alloc(
   MAUG_MHANDLE* p_tilemap_h, size_t w, size_t h, size_t layers_count );

MERROR_RETVAL
retrotile_alloc_tiles( struct RETROTILE_LAYER* layer, size_t ndefs );

MERROR_RETVAL
retrotile_free( struct RETROTILE* t );

#ifdef RETROTIL_C

#  include <stdio.h>

#  include <mparser.h>

/* TODO: Function names should be verb_noun! */

/* TODO: Better swap logic. */
#define retrotile_parser_mstate( parser, new_mstate ) \
   parser->mstate = new_mstate; \
   debug_printf( \
      RETROTILE_TRACE_LVL, "parser mstate: %s", \
         gc_retrotile_mstate_names[parser->mstate] );

#  define RETROTILE_PARSER_MSTATE_TABLE_CONST( name, idx, tokn, parent, m ) \
      static MAUG_CONST uint8_t name = idx;

RETROTILE_PARSER_MSTATE_TABLE( RETROTILE_PARSER_MSTATE_TABLE_CONST )

#  define RETROTILE_PARSER_MSTATE_TABLE_NAME( name, idx, tokn, parent, m ) \
      #name,

static MAUG_CONST char* gc_retrotile_mstate_names[] = {
   RETROTILE_PARSER_MSTATE_TABLE( RETROTILE_PARSER_MSTATE_TABLE_NAME )
   ""
};

#  define RETROTILE_PARSER_MSTATE_TABLE_TOKEN( name, idx, tokn, parent, m ) \
      tokn,

static MAUG_CONST char* gc_retrotile_mstate_tokens[] = {
   RETROTILE_PARSER_MSTATE_TABLE( RETROTILE_PARSER_MSTATE_TABLE_TOKEN )
   ""
};

#  define RETROTILE_PARSER_MSTATE_TABLE_PARNT( name, idx, tokn, parent, m ) \
      parent,

static MAUG_CONST uint8_t gc_retrotile_mstate_parents[] = {
   RETROTILE_PARSER_MSTATE_TABLE( RETROTILE_PARSER_MSTATE_TABLE_PARNT )
   0
};

#  define RETROTILE_PARSER_MSTATE_TABLE_MODE( name, idx, tokn, parent, m ) \
      m,

static MAUG_CONST uint8_t gc_retrotile_mstate_modes[] = {
   RETROTILE_PARSER_MSTATE_TABLE( RETROTILE_PARSER_MSTATE_TABLE_MODE )
   0
};

/* === */

static void retrotile_parser_match_token(
   const char* token, size_t token_sz, struct RETROTILE_PARSER* parser
) {
   size_t j = 1;

   /* Figure out what the key is for. */
   while( '\0' != gc_retrotile_mstate_tokens[j][0] ) {
      if(
         /* Make sure tokens match. */
         strlen( gc_retrotile_mstate_tokens[j] ) != token_sz ||
         0 != strncmp(
            token, gc_retrotile_mstate_tokens[j], token_sz
         )
      ) {
         j++;
         continue;

      } else if(
         /* This state can only be
          * reached THROUGH that parent state. This allows us to have
          * keys with the same name but different parents!
          */
         parser->mstate != gc_retrotile_mstate_parents[j]
      ) {
         debug_printf(
            RETROTILE_TRACE_LVL,
            "found token \"%s\" but incorrect parent %s (%d) "
               "(needs %s (%d))!",
            token,
            gc_retrotile_mstate_names[parser->mstate],
            parser->mstate,
            gc_retrotile_mstate_names[gc_retrotile_mstate_parents[j]],
            gc_retrotile_mstate_parents[j] );
         j++;
         continue;

      } else if(
         /* None works in all modes! */
         /* MTILESTATE_NONE != parser->mstate && */
         parser->mode != gc_retrotile_mstate_modes[j]
      ) {
         debug_printf(
            RETROTILE_TRACE_LVL, "found token %s but incorrect mode %u!",
            token,
            gc_retrotile_mstate_modes[j] );
         j++;
         continue;

      } else {
         /* Found it! */
         retrotile_parser_mstate( parser, j );
         return;
      }
   }
}

/* === */

MERROR_RETVAL retrotile_parser_parse_tiledef_token(
   const char* token, size_t token_sz, void* parser_arg
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROTILE_TILE_DEF* tile_defs = NULL;
   struct RETROTILE_PARSER* parser = (struct RETROTILE_PARSER*)parser_arg;
   size_t tileset_id_parsed = 0;

   if( 0 < *(parser->p_tile_defs_count) ) {
      maug_mlock( *(parser->p_tile_defs_h), tile_defs );
      maug_cleanup_if_null_alloc( struct RETROTILE_TILE_DEF*, tile_defs );
   }

   if(
      MJSON_PSTATE_OBJECT_VAL == 
         mjson_parser_pstate( &(parser->jparser) )
   ) {
      if( MTILESTATE_TILES_ID == parser->mstate ) {
         retrotile_parser_mstate( parser, MTILESTATE_TILES );
         if( 0 == parser->pass ) {
            /* Parse tile ID. */
            tileset_id_parsed = atoi( token );
            if( tileset_id_parsed > parser->tileset_id_cur ) {
               parser->tileset_id_cur = tileset_id_parsed;
               debug_printf(
                  RETROTILE_TRACE_LVL,
                  "new highest tile ID: " SIZE_T_FMT,
                  parser->tileset_id_cur );
            }
         } else {
            /* TODO: atoi or atoul? */
            parser->tileset_id_cur = atoi( token );
            debug_printf(
               RETROTILE_TRACE_LVL,
               "next tile ID: " SIZE_T_FMT,
               parser->tileset_id_cur );
         }

      } else if( MTILESTATE_TILES_IMAGE == parser->mstate ) {
         if( 1 == parser->pass ) {
            debug_printf(
               RETROTILE_TRACE_LVL, "setting tile ID " SIZE_T_FMT "...",
               parser->tileset_id_cur );

            /* Parse tile image. */
            strncpy(
               tile_defs[parser->tileset_id_cur].image_path,
               token,
               RETROTILE_TILESET_IMAGE_STR_SZ_MAX );

            debug_printf(
               RETROTILE_TRACE_LVL, "set tile ID " SIZE_T_FMT " to: %s",
               parser->tileset_id_cur,
               tile_defs[parser->tileset_id_cur].image_path );
         }
         retrotile_parser_mstate( parser, MTILESTATE_TILES );

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
                  RETROTILE_TILE_FLAG_ROT_X;
            }

            /* TODO: Read boolean Z and fire particles prop/flag. */
         }

         retrotile_parser_mstate( parser, MTILESTATE_TILES_PROP );

      } else if( MTILESTATE_TILES_PROP_VAL == parser->mstate ) {

         retrotile_parser_mstate( parser, MTILESTATE_TILES_PROP );
      }
      goto cleanup;
   }

   retrotile_parser_match_token( token, token_sz, parser );

cleanup:

   if( NULL != tile_defs ) {
      maug_munlock( *(parser->p_tile_defs_h), tile_defs );
   }

   return retval;
}

MERROR_RETVAL retrotile_parser_parse_token(
   const char* token, size_t token_sz, void* parser_arg
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROTILE_TILE_DEF* tile_defs = NULL;
   struct RETROTILE_LAYER* tiles_layer = NULL;
   struct RETROTILE_PARSER* parser = (struct RETROTILE_PARSER*)parser_arg;
   retrotile_tile_t* tiles = NULL;

   if( 0 < *(parser->p_tile_defs_count) ) {
      maug_mlock( *(parser->p_tile_defs_h), tile_defs );
      maug_cleanup_if_null_alloc( struct RETROTILE_TILE_DEF*, tile_defs );
   }

   if( MJSON_PSTATE_LIST == mjson_parser_pstate( &(parser->jparser) ) ) {
      if(
         1 == parser->pass &&
         MTILESTATE_LAYER_DATA == parser->mstate
      ) {
         /*
         assert( NULL != *(parser->p_tilemap_h) );
         maug_mlock( *(parser->p_tilemap_h), tilemap );
         maug_cleanup_if_null_alloc( struct RETROTILE*, tilemap );
         */

         debug_printf( RETROTILE_TRACE_LVL,
            "selecting layer " SIZE_T_FMT "...",
            parser->pass_layer_iter );
         assert( NULL != parser->t );
         tiles_layer = retrotile_get_layer_p(
            parser->t, parser->pass_layer_iter );
         assert( NULL != tiles_layer );

         tiles = retrotile_get_tiles_p( tiles_layer );

         /* Parse tilemap tile. */
         debug_printf( RETROTILE_TRACE_LVL,
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
      MJSON_PSTATE_OBJECT_VAL == 
         mjson_parser_pstate( &(parser->jparser) )
   ) {
      if( MTILESTATE_TILESETS_FGID == parser->mstate ) {
         if( 1 == parser->pass ) {
            parser->t->tileset_fgid = atoi( token );
            debug_printf(
               RETROTILE_TRACE_LVL, "tileset FGID set to: " SIZE_T_FMT,
               parser->t->tileset_fgid );
         }
         retrotile_parser_mstate( parser, MTILESTATE_TILESETS );

      } else if( MTILESTATE_TILESETS_SRC == parser->mstate ) {
         if( 1 == parser->pass ) {
            debug_printf( RETROTILE_TRACE_LVL, "parsing %s...", token );
            parser->tj_parse_cb(
               token, NULL, parser->p_tile_defs_h,
               parser->p_tile_defs_count );
         }
         retrotile_parser_mstate( parser, MTILESTATE_TILESETS );

      } else if( MTILESTATE_HEIGHT == parser->mstate ) {
         if( 0 == parser->pass ) {
            parser->tiles_h = atoi( token );
            debug_printf(
               RETROTILE_TRACE_LVL, "tilemap height: " SIZE_T_FMT,
               parser->tiles_h );
         }
         retrotile_parser_mstate( parser, MTILESTATE_NONE );

      } else if( MTILESTATE_WIDTH == parser->mstate ) {
         if( 0 == parser->pass ) {
            parser->tiles_w = atoi( token );
            debug_printf(
               RETROTILE_TRACE_LVL, "tilemap width: " SIZE_T_FMT,
               parser->tiles_w );
         }
         retrotile_parser_mstate( parser, MTILESTATE_NONE );

      } else if( MTILESTATE_LAYER_NAME == parser->mstate ) {
         /* TODO: Store */
         retrotile_parser_mstate( parser, MTILESTATE_LAYER );
         
      }
      goto cleanup;
   }

   retrotile_parser_match_token( token, token_sz, parser );

cleanup:

   if( NULL != tile_defs ) {
      maug_munlock( *(parser->p_tile_defs_h), tile_defs );
   }

   return retval;
}

/* === */

MERROR_RETVAL retrotile_json_close_list( void* parg ) {
   struct RETROTILE_PARSER* parser = (struct RETROTILE_PARSER*)parg;

   if( MTILESTATE_LAYER_DATA == parser->mstate ) {
      assert( RETROTILE_PARSER_MODE_MAP == parser->mode );
      assert( parser->layer_tile_iter == 1600 );
      retrotile_parser_mstate( parser, MTILESTATE_LAYER );

   } else if( MTILESTATE_LAYERS == parser->mstate ) {
      assert( RETROTILE_PARSER_MODE_MAP == parser->mode );
      retrotile_parser_mstate( parser, MTILESTATE_NONE );

   } else if( MTILESTATE_TILESETS == parser->mstate ) {
      assert( RETROTILE_PARSER_MODE_MAP == parser->mode );
      retrotile_parser_mstate( parser, MTILESTATE_NONE );

   } else if( MTILESTATE_TILES_PROP == parser->mstate ) {
      assert( RETROTILE_PARSER_MODE_DEFS == parser->mode );
      retrotile_parser_mstate( parser, MTILESTATE_TILES );
   }

   return MERROR_OK;
}

/* === */

MERROR_RETVAL retrotile_json_open_obj( void* parg ) {
   struct RETROTILE_PARSER* parser = (struct RETROTILE_PARSER*)parg;

   if( MTILESTATE_LAYERS == parser->mstate ) {
      assert( RETROTILE_PARSER_MODE_MAP == parser->mode );
      /* Reset on open so count is retained for allocating after first 
       * pass. */
      parser->layer_tile_iter = 0;
      retrotile_parser_mstate( parser, MTILESTATE_LAYER );
   }

   return MERROR_OK;
}

/* === */

MERROR_RETVAL retrotile_json_close_obj( void* parg ) {
   struct RETROTILE_PARSER* parser = (struct RETROTILE_PARSER*)parg;

   if( MTILESTATE_LAYER == parser->mstate ) {
      assert( RETROTILE_PARSER_MODE_MAP == parser->mode );
      assert( parser->layer_tile_iter == 1600 );
      debug_printf( RETROTILE_TRACE_LVL,
         "incrementing pass layer to " SIZE_T_FMT " after " SIZE_T_FMT
            " tiles...",
         parser->pass_layer_iter + 1, parser->layer_tile_iter );
      parser->pass_layer_iter++;
      retrotile_parser_mstate( parser, MTILESTATE_LAYERS );

   } else if( MTILESTATE_GRID == parser->mstate ) {
      retrotile_parser_mstate( parser, MTILESTATE_NONE );
   }

   return MERROR_OK;
}

/* === */

#if 0
MERROR_RETVAL retrotile_json_close_val( void* parg ) {
   struct RETROTILE_PARSER* parser = (struct RETROTILE_PARSER*)parg;

   if(
      MTILESTATE_TILES_PROP == parser->mstate
   ) {
      assert( RETROTILE_PARSER_MODE_DEFS == parser->mode );
   
      debug_printf( 1, "prop: %s", parser->jparser.token );

      /* retrotile_parser_mstate( parser, MTILESTATE_TILES_PROP_ROT_X ); */
   }

   return MERROR_OK;
}
#endif

/* === */

MERROR_RETVAL retrotile_parse_json_file(
   const char* filename, MAUG_MHANDLE* p_tilemap_h,
   MAUG_MHANDLE* p_tile_defs_h, size_t* p_tile_defs_count
) {
   MERROR_RETVAL retval = MERROR_OK;
   MAUG_MHANDLE parser_h = (MAUG_MHANDLE)NULL;
   struct RETROTILE_PARSER* parser = NULL;
   char filename_path[RETROFLAT_PATH_MAX];
   size_t i = 0;
   mfile_t buffer;
   char c;

   /* Initialize parser. */
   parser_h = maug_malloc( 1, sizeof( struct RETROTILE_PARSER ) );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, parser_h );

   maug_mlock( parser_h, parser );
   maug_cleanup_if_null_alloc( struct RETROTILE_PARSER*, parser );
   maug_mzero( parser, sizeof( struct RETROTILE_PARSER ) );

   parser->tj_parse_cb = retrotile_parse_json_file;

   /* Setup filename path. */
   memset( filename_path, '\0', RETROFLAT_PATH_MAX );
   /* TODO: Configurable path. */
   maug_snprintf(
      filename_path, RETROFLAT_PATH_MAX, "mapsrc/%s", filename );

   debug_printf( RETROTILE_TRACE_LVL, "opening %s...", filename_path );

   /* TODO: fread chars one by one from disk using retrofil_cread. */
   retval = mfile_open_read( filename_path, &buffer );
   maug_cleanup_if_not_ok();

   assert( NULL != p_tile_defs_count );

   /* Parse JSON and react to state. */
   for( parser->pass = 0 ; 2 > parser->pass ; parser->pass++ ) {
      debug_printf( RETROTILE_TRACE_LVL, "beginning pass #%u...",
         parser->pass );

      /* Reset tilemap parser. */
      parser->mstate = 0;

      /* Reset JSON parser. */
      maug_mzero( &(parser->jparser), sizeof( struct MJSON_PARSER ) );

      /* Figure out if we're parsing a .tmj or .tsj. */
      if( 's' == strrchr( filename, '.' )[2] ) {
         debug_printf( RETROTILE_TRACE_LVL, "(tile_defs mode)" );
         parser->mode = RETROTILE_PARSER_MODE_DEFS;
         parser->jparser.token_parser = retrotile_parser_parse_tiledef_token;
         parser->jparser.token_parser_arg = parser;
         parser->jparser.close_list = retrotile_json_close_list;
         parser->jparser.close_list_arg = parser;
         parser->jparser.close_obj = retrotile_json_close_obj;
         parser->jparser.close_obj_arg = parser;
         /*
         parser->jparser.close_val = retrotile_json_close_val;
         parser->jparser.close_val_arg = parser;
         */
         parser->p_tile_defs_h = p_tile_defs_h;
         parser->p_tile_defs_count = p_tile_defs_count;

         assert( NULL != p_tile_defs_h );
         if( 1 == parser->pass ) {
            /* Allocate tile defs based on highest tile ID found on
             * first pass.
             */
            retval = retrotile_alloc_tile_defs(
               p_tile_defs_h, p_tile_defs_count,
               parser->tileset_id_cur + 1 );
            maug_cleanup_if_not_ok();
         }
      } else {
         debug_printf( RETROTILE_TRACE_LVL, "(tilemap mode)" );
         parser->mode = RETROTILE_PARSER_MODE_MAP;

         parser->jparser.close_list = retrotile_json_close_list;
         parser->jparser.close_list_arg = parser;
         parser->jparser.open_obj = retrotile_json_open_obj;
         parser->jparser.open_obj_arg = parser;
         parser->jparser.close_obj = retrotile_json_close_obj;
         parser->jparser.close_obj_arg = parser;
         parser->jparser.token_parser = retrotile_parser_parse_token;
         parser->jparser.token_parser_arg = parser;
         parser->p_tile_defs_h = p_tile_defs_h;
         parser->p_tile_defs_count = p_tile_defs_count;

         assert( NULL != p_tilemap_h );
         if( 1 == parser->pass ) {
            /* Allocate tiles for the new layers. */
            retval = retrotile_alloc(
               p_tilemap_h, parser->tiles_w, parser->tiles_h,
               parser->pass_layer_iter );
            maug_cleanup_if_not_ok();

            maug_mlock( *p_tilemap_h, parser->t );
         }
         parser->pass_layer_iter = 0;
      }

      while( mfile_has_bytes( &buffer ) ) {
         mfile_cread( &buffer, c );
         retval = mjson_parse_c( &(parser->jparser), c );
         if( MERROR_OK != retval ) {
            error_printf( "error parsing JSON!" );
            goto cleanup;
         }
      }

      mfile_reset( &buffer );

      if( 's' != strrchr( filename, '.' )[2] ) {
         debug_printf( RETROTILE_TRACE_LVL,
            "pass %u found " SIZE_T_FMT " layers",
            parser->pass, parser->pass_layer_iter );
      }
   }

   debug_printf(
      RETROTILE_TRACE_LVL, "finished parsing %s...", filename_path );

cleanup:

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

struct RETROTILE_LAYER* retrotile_get_layer_p(
   struct RETROTILE* tilemap, uint32_t layer_idx
) {
   struct RETROTILE_LAYER* layer_iter = NULL;
   uint8_t* tilemap_buf = (uint8_t*)tilemap;

   if( 0 == tilemap->layers_count || layer_idx >= tilemap->layers_count ) {
      error_printf( "invalid layer " UPRINTF_U32_FMT
         " requested (of " UPRINTF_U32_FMT ")!",
         layer_idx, tilemap->layers_count );
      return NULL;
   }

   /* Advance to first grid. */
   tilemap_buf += sizeof( struct RETROTILE );
   layer_iter = (struct RETROTILE_LAYER*)tilemap_buf;
   while( layer_idx > 0 ) {
      tilemap_buf += layer_iter->sz;
      layer_iter = (struct RETROTILE_LAYER*)tilemap_buf;
      layer_idx--;
   }

   return layer_iter;
}


/* === */

MERROR_RETVAL retrotile_alloc_tile_defs(
   MAUG_MHANDLE* p_tile_defs_h, size_t* p_tile_defs_count, size_t ndefs
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROTILE_TILE_DEF* tile_defs = NULL;

   assert( 0 == *p_tile_defs_count );
   assert( NULL == *p_tile_defs_h );
   debug_printf( 1, "allocating " SIZE_T_FMT " tile definitions ("
      SIZE_T_FMT " bytes)...",
      ndefs, ndefs * sizeof( struct RETROTILE_TILE_DEF ) );
   *p_tile_defs_h =
      maug_malloc( ndefs, sizeof( struct RETROTILE_TILE_DEF ) );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, *p_tile_defs_h );
   *p_tile_defs_count = ndefs;

   /* Zero new allocs. */
   maug_mlock( *p_tile_defs_h, tile_defs );
   maug_cleanup_if_null_alloc( struct RETROTILE_TILE_DEF*, tile_defs );
   maug_mzero( tile_defs, ndefs * sizeof( struct RETROTILE_TILE_DEF ) );

cleanup:

   if( NULL != tile_defs ) {
      maug_munlock( *p_tile_defs_h, tile_defs );
   }

   return retval;
}

/* === */

MERROR_RETVAL retrotile_alloc(
   MAUG_MHANDLE* p_tilemap_h, size_t w, size_t h, size_t layers_count
) {
   struct RETROTILE_LAYER* layer_iter = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   size_t tilemap_sz = 0;
   struct RETROTILE* tilemap = NULL;
   size_t i = 0;

   tilemap_sz = sizeof( struct RETROTILE ) +
      (layers_count * sizeof( struct RETROTILE_LAYER )) +
      (layers_count * (w * h * sizeof( retrotile_tile_t ) ));

   debug_printf( 1, "allocating new tilemap " SIZE_T_FMT "x" SIZE_T_FMT
      " tiles, " SIZE_T_FMT "layers (" SIZE_T_FMT " bytes)...",
      w, h, layers_count, tilemap_sz );

   *p_tilemap_h = maug_malloc( 1, tilemap_sz );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, *p_tilemap_h );

   maug_mlock( *p_tilemap_h, tilemap );
   maug_cleanup_if_null_alloc( struct RETROTILE*, tilemap );

   maug_mzero( tilemap, tilemap_sz );
   tilemap->sz = tilemap_sz;
   tilemap->layers_count = layers_count;
   tilemap->tiles_w = w;
   tilemap->tiles_h = h;
   tilemap->tile_scale = RETROTILE_TILE_SCALE_DEFAULT;

   for( i = 0 ; layers_count > i ; i++ ) {
      layer_iter = retrotile_get_layer_p( tilemap, i );
      assert( NULL != layer_iter );
      layer_iter->sz = sizeof( struct RETROTILE_LAYER ) +
         (w * h * sizeof( retrotile_tile_t ));
      maug_cleanup_if_not_ok();
   }

cleanup:

   if( NULL != tilemap ) {
      maug_munlock( *p_tilemap_h, tilemap );
   }

   return retval;
}

#endif /* RETROTIL_C */

/*! \} */ /* maug_tilemap */

#endif /* !RETROTIL_H */

