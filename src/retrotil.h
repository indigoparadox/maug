
#ifndef RETROTIL_H
#define RETROTIL_H

#include <mparser.h>
#include <mjson.h>
#include <mfile.h>

/**
 * \addtogroup retrotile RetroTile API
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

#ifndef RETROTILE_VORONOI_DEFAULT_SPB
#  define RETROTILE_VORONOI_DEFAULT_SPB 8
#endif /* !RETROTILE_VORONOI_DEFAULT_SPB */

#ifndef RETROTILE_VORONOI_DEFAULT_DRIFT
#  define RETROTILE_VORONOI_DEFAULT_DRIFT 4
#endif /* !RETROTILE_VORONOI_DEFAULT_DRIFT */

#ifdef MPARSER_TRACE_NAMES
#  define retrotile_mstate_name( state ) gc_retrotile_mstate_names[state]
#else
#  define retrotile_mstate_name( state ) state
#endif /* MPARSER_TRACE_NAMES */

/**
 * \addtogroup \retrotile_defs RetroTile Tile Definitions
 * \{
 */

/**
 * \addtogroup \retrotile_defs_types RetroTile Custom Property Types
 * \{
 */

#define RETROTILE_PARSER_MODE_MAP    0
#define RETROTILE_PARSER_MODE_DEFS   1

#define RETROTILE_TILE_FLAG_BLOCK      0x01
#define RETROTILE_TILE_FLAG_ROT_X      0x02
#define RETROTILE_TILE_FLAG_ROT_Z      0x04
#define RETROTILE_TILE_FLAG_PRTCL_FIRE 0x08

/*! \} */ /* retrotile_defs_types */

#define RETROTILE_DIR4_NORTH  0
#define RETROTILE_DIR4_EAST   1
#define RETROTILE_DIR4_SOUTH  2
#define RETROTILE_DIR4_WEST   3

#define RETROTILE_DIR8_NORTH  0
#define RETROTILE_DIR8_EAST   2
#define RETROTILE_DIR8_SOUTH  4
#define RETROTILE_DIR8_WEST   6

typedef int16_t retrotile_tile_t;

#define RETROTILE_IDX_FMT "%u"

struct RETROTILE_TILE_DEF {
   uint8_t flags;
   char image_path[RETROTILE_TILESET_IMAGE_STR_SZ_MAX];
};

/*! \} */ /* retrotile_defs */

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

/**
 * \brief Internal data structure used by
 *        retrotile_gen_diamond_square_iter().
 */
struct RETROTILE_DATA_DS {
   /*! \brief Starting X of subsector in a given iteration. */
   int16_t sect_x;
   /*! \brief Starting Y of subsector in a given iteration. */
   int16_t sect_y;
   /*! \brief Width of subsector in a given iteration. */
   int16_t sect_w;
   /*! \brief Height of subsector in a given iteration. */
   int16_t sect_h;
   /*! \brief Half of the width of subsector in a given iteration. */
   int16_t sect_w_half;
   /*! \brief Half of the height of subsector in a given iteration. */
   int16_t sect_h_half;
};

struct RETROTILE_DATA_BORDER {
   int16_t tiles_changed;
   retrotile_tile_t center;
   retrotile_tile_t outside;
   /*! \brief If the center and outside match, use this mod-to. */
   retrotile_tile_t mod_to[8];
};

#define retrotile_get_tile( tilemap, layer, x, y ) \
   (retrotile_get_tiles_p( layer )[((y) * (tilemap)->tiles_w) + (x)])

#define retrotile_set_tile( tilemap, layer, x, y, new_val ) \
   (retrotile_get_tiles_p( layer )[((y) * (tilemap)->tiles_w) + (x)])

#define retrotile_get_tiles_p( layer ) \
   ((retrotile_tile_t*)(((uint8_t*)(layer)) + \
   sizeof( struct RETROTILE_LAYER )))

/**
 * \addtogroup retrotile_parser RetroTile Parser
 * \{
 */

typedef MERROR_RETVAL (*retrotile_tj_parse_cb)(
   const char* filename, MAUG_MHANDLE* p_tm_h,
   MAUG_MHANDLE* p_td_h, size_t* p_td_c,
   mparser_wait_cb_t wait_cb, void* wait_data );

struct RETROTILE_PARSER {
   uint8_t mstate;
   /* TODO: Use flags and combine these. */
   uint8_t pass;
   uint8_t mode;
   mparser_wait_cb_t wait_cb;
   void* wait_data;
   retroflat_ms_t wait_last;
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
   MAUG_MHANDLE* p_tile_defs_h, size_t* p_tile_defs_count,
   mparser_wait_cb_t wait_cb, void* wait_data );

/*! \} */ /* retrotile_parser */

/**
 * \addtogroup retrotile_gen RetroTile Generators
 * \brief Tools for procedurally generating tilemaps.
 * \{
 */

typedef MERROR_RETVAL (*retrotile_ani_cb)(
   void* animation_cb_data, int16_t iter );

typedef MERROR_RETVAL (*retrotile_gen_cb)(
   struct RETROTILE* t, retrotile_tile_t min_z, retrotile_tile_t max_z,
   uint32_t tuning, size_t layer_idx, uint8_t flags, void* data,
   retrotile_ani_cb animation_cb, void* animation_cb_data );

/**
 * \brief Generate tilemap terrain using diamond square algorithm.
 *
 * See retrotile_gen_cb() for details.
 *
 * \warning This can be very slow!
 *
 * \warning This will generate a complete new tilemap!
 */
MERROR_RETVAL retrotile_gen_diamond_square_iter(
   struct RETROTILE* t, retrotile_tile_t min_z, retrotile_tile_t max_z,
   uint32_t tuning, size_t layer_idx, uint8_t flags, void* data,
   retrotile_ani_cb animation_cb, void* animation_cb_data );

/**
 * \brief Generate tilemap terrain using voronoi graph.
 *
 * See retrotile_gen_cb() for details.
 *
 * This is more suitable for purely 2D tilesets, as the changes between
 * indexes are very abrupt!
 *
 * \warning This will generate a complete new tilemap!
 */
MERROR_RETVAL retrotile_gen_voronoi_iter(
   struct RETROTILE* t, retrotile_tile_t min_z, retrotile_tile_t max_z,
   uint32_t tuning, size_t layer_idx, uint8_t flags, void* data,
   retrotile_ani_cb animation_cb, void* animation_cb_data );

/**
 * \brief Average the values in adjacent tiles over an already-generated
 *        tilemap.
 *
 * This is designed to be used with tilemaps that use their tile indexes
 * as Z values rather than indexes in a table of tile definitions.
 */
MERROR_RETVAL retrotile_gen_smooth_iter(
   struct RETROTILE* t, retrotile_tile_t min_z, retrotile_tile_t max_z,
   uint32_t tuning, size_t layer_idx, uint8_t flags, void* data,
   retrotile_ani_cb animation_cb, void* animation_cb_data );

/**
 * \brief Given a list of ::RETROTILE_DATA_BORDER structs, this will
 *        search for occurrences of RETROTILE_DATA_BORDER::center next to
 *        RETROTILE_DATA_BORDER::outside and replace with the appropriate
 *        border between the two!
 */
MERROR_RETVAL retrotile_gen_borders_iter(
   struct RETROTILE* t, retrotile_tile_t min_z, retrotile_tile_t max_z,
   uint32_t tuning, size_t layer_idx, uint8_t flags, void* data,
   retrotile_ani_cb animation_cb, void* animation_cb_data );

struct RETROTILE_LAYER* retrotile_get_layer_p(
   struct RETROTILE* tilemap, uint32_t layer_idx );

MERROR_RETVAL retrotile_alloc_tile_defs(
   MAUG_MHANDLE* p_tile_defs_h, size_t* p_tile_defs_count, size_t ndefs );

MERROR_RETVAL retrotile_alloc(
   MAUG_MHANDLE* p_tilemap_h, size_t w, size_t h, size_t layers_count );

/*! \} */ /* retrotile_gen */

#ifdef RETROTIL_C

#  include <stdio.h>

#  include <mparser.h>

MAUG_CONST int16_t SEG_MCONST gc_retrotile_offsets8_x[8] =
   {  0,  1, 1, 1, 0, -1, -1, -1 };
MAUG_CONST int16_t SEG_MCONST gc_retrotile_offsets8_y[8] =
   { -1, -1, 0, 1, 1,  1,  0, -1 };

MAUG_CONST int16_t SEG_MCONST gc_retrotile_offsets4_x[4] =
   {  0, 1, 0, -1 };
MAUG_CONST int16_t SEG_MCONST gc_retrotile_offsets4_y[4] =
   { -1, 0, 1,  0 };

/* TODO: Function names should be verb_noun! */

#define retrotile_parser_mstate( parser, new_mstate ) \
   parser->mstate = new_mstate;

#if 0
   debug_printf( \
      RETROTILE_TRACE_LVL, "parser mstate: %s", \
         retrotile_mstate_name( parser->mstate ) );
#endif

#  define RETROTILE_PARSER_MSTATE_TABLE_CONST( name, idx, tokn, parent, m ) \
      static MAUG_CONST uint8_t SEG_MCONST name = idx;

RETROTILE_PARSER_MSTATE_TABLE( RETROTILE_PARSER_MSTATE_TABLE_CONST )

#ifdef MPARSER_TRACE_NAMES
#  define RETROTILE_PARSER_MSTATE_TABLE_NAME( name, idx, tokn, parent, m ) \
      #name,

static MAUG_CONST char* SEG_MCONST gc_retrotile_mstate_names[] = {
   RETROTILE_PARSER_MSTATE_TABLE( RETROTILE_PARSER_MSTATE_TABLE_NAME )
   ""
};
#endif /* MPARSER_TRACE_NAMES */

#  define RETROTILE_PARSER_MSTATE_TABLE_TOKEN( name, idx, tokn, parent, m ) \
      tokn,

static MAUG_CONST char* SEG_MCONST gc_retrotile_mstate_tokens[] = {
   RETROTILE_PARSER_MSTATE_TABLE( RETROTILE_PARSER_MSTATE_TABLE_TOKEN )
   ""
};

#  define RETROTILE_PARSER_MSTATE_TABLE_PARNT( name, idx, tokn, parent, m ) \
      parent,

static MAUG_CONST uint8_t SEG_MCONST gc_retrotile_mstate_parents[] = {
   RETROTILE_PARSER_MSTATE_TABLE( RETROTILE_PARSER_MSTATE_TABLE_PARNT )
   0
};

#  define RETROTILE_PARSER_MSTATE_TABLE_MODE( name, idx, tokn, parent, m ) \
      m,

static MAUG_CONST uint8_t SEG_MCONST gc_retrotile_mstate_modes[] = {
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
            "found token \"%s\" "
#ifdef MPARSER_TRACE_NAMES
               "but incorrect parent %s (%d) (needs %s (%d))!",
#else
               "but incorrect parent %d (needs %d)!",
#endif /* MPARSER_TRACE_NAMES */
            token,
#ifdef MPARSER_TRACE_NAMES
            retrotile_mstate_name( parser->mstate ),
            parser->mstate,
            retrotile_mstate_name( gc_retrotile_mstate_parents[j] ),
            gc_retrotile_mstate_parents[j]
#else
            parser->mstate,
            gc_retrotile_mstate_parents[j]
#endif /* MPARSER_TRACE_NAMES */
         );
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
               if( parser->tileset_id_cur >= *(parser->p_tile_defs_count) ) {
                  error_printf(
                     "tileset ID " SIZE_T_FMT
                     " outside of tile defs count " SIZE_T_FMT "!",
                     parser->tileset_id_cur, *(parser->p_tile_defs_count) );
                  retval = MERROR_OVERFLOW;
                  goto cleanup;
               }
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

         if(
            parser->layer_tile_iter >=
               parser->t->tiles_w * parser->t->tiles_h
         ) {
            error_printf(
               "tile " SIZE_T_FMT " outside of layer tile buffer size %d!",
               parser->layer_tile_iter,
               parser->t->tiles_w * parser->t->tiles_h );
            retval = MERROR_OVERFLOW;
            goto cleanup;
         }

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
               parser->p_tile_defs_count,
               parser->wait_cb, parser->wait_data );
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

MERROR_RETVAL retrotile_parse_json_file(
   const char* filename, MAUG_MHANDLE* p_tilemap_h,
   MAUG_MHANDLE* p_tile_defs_h, size_t* p_tile_defs_count,
   mparser_wait_cb_t wait_cb, void* wait_data
) {
   MERROR_RETVAL retval = MERROR_OK;
   MAUG_MHANDLE parser_h = (MAUG_MHANDLE)NULL;
   struct RETROTILE_PARSER* parser = NULL;
   char filename_path[RETROFLAT_PATH_MAX];
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

      parser->jparser.wait_cb = wait_cb;
      parser->jparser.wait_data = wait_data;

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
         buffer.read_int( &buffer, (uint8_t*)&c, 1, 0 );
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

static retrotile_tile_t retrotile_gen_diamond_square_rand(
   retrotile_tile_t min_z, retrotile_tile_t max_z, uint32_t tuning,
   retrotile_tile_t top_left_z
) {
   retrotile_tile_t avg = top_left_z;

   if( 8 > rand() % 10 ) {
      /* avg = min_z + (rand() % (max_z - min_z)); */
      avg -= (min_z / tuning) + (rand() % (max_z / tuning));
   /* } else {
      avg += (min_z / 10) + (rand() % (max_z / 10)); */
   }

   /* Clamp the result. */

   if( min_z > avg ) {
      avg = min_z;
   }

   if( max_z < avg ) {
      avg = max_z;
   }

   return avg;
}

/* === */

static void retrotile_gen_diamond_square_corners(
   int16_t corners_x[2][2], int16_t corners_y[2][2],
   retrotile_tile_t min_z, retrotile_tile_t max_z,
   uint32_t tuning, struct RETROTILE_DATA_DS* data_ds,
   struct RETROTILE_LAYER* layer, struct RETROTILE* t
) {
   int16_t iter_x = 0,
      iter_y = 0;
   retrotile_tile_t* tile_iter = NULL;
   retrotile_tile_t top_left_z = 0;

   /* Generate missing corner data. Loop through X/Y coords stored in
    * corners_x/corners_y convenience arrays.
    */
   for( iter_y = 0 ; iter_y < 2 ; iter_y++ ) {
      for( iter_x = 0 ; iter_x < 2 ; iter_x++ ) {

         /* Make sure corner X is in bounds. */
         corners_x[iter_x][iter_y] =
            (data_ds->sect_x - 1) + (iter_x * data_ds->sect_w);
         if( 0 > corners_x[iter_x][iter_y] ) {
            corners_x[iter_x][iter_y] += 1;
         }

         /* Make sure corner Y is in bounds. */
         corners_y[iter_x][iter_y] =
            (data_ds->sect_y - 1) + (iter_y * data_ds->sect_h);
         if( 0 > corners_y[iter_x][iter_y] ) {
            corners_y[iter_x][iter_y] += 1;
         }
      }
   }

   /* Should be handled by the check above. */
   assert( 0 <= corners_x[0][0] );
   assert( 0 <= corners_y[0][0] );
   assert( t->tiles_w > corners_x[0][0] );
   assert( t->tiles_h > corners_y[0][0] );

   /* Grab the top-left Z-value to anchor generated corners to. */
   top_left_z = retrotile_get_tile(
      t, layer,
      corners_x[0][0],
      corners_y[0][0] );

   if( 0 > top_left_z ) {
      retrotile_get_tile(
         t, layer,
         corners_x[0][0] >= 0 ? corners_x[0][0] : 0,
         corners_y[0][0] >= 0 ? corners_y[0][0] : 0 ) = max_z;
      top_left_z = max_z;
   }

   /* Fill in missing corners. */
   for( iter_y = 0 ; iter_y < 2 ; iter_y++ ) {
      for( iter_x = 0 ; iter_x < 2 ; iter_x++ ) {
         /* Grab a pointer to the corner so we can modify it easily. */
         tile_iter = &(retrotile_get_tile(
            t, layer,
            corners_x[iter_x][iter_y],
            corners_y[iter_x][iter_y] ));

         /* Check if corner is already filled in. */
         if( -1 != *tile_iter ) {
            debug_printf(
               RETROTILE_TRACE_LVL, "corner coord %d x %d present: %d",
               corners_x[iter_x][iter_y], corners_y[iter_x][iter_y],
               retrotile_get_tile(
                  t, layer,
                  corners_x[iter_x][iter_y],
                  corners_y[iter_x][iter_y] ) );
            continue;
         }

         /* Generate a new value for this corner. */
         *tile_iter = retrotile_gen_diamond_square_rand(
            min_z, max_z, tuning, top_left_z );

         debug_printf( RETROTILE_TRACE_LVL,
            "missing corner coord %d x %d: %d",
            corners_x[iter_x][iter_y], corners_y[iter_x][iter_y],
            *tile_iter );
      }
   }
}

/* === */

static retrotile_tile_t retrotile_gen_diamond_square_avg(
   int16_t corners_x[2][2], int16_t corners_y[2][2],
   struct RETROTILE* t, struct RETROTILE_LAYER* layer
) {
   retrotile_tile_t* tile_iter = NULL;
   int16_t iter_x = 0,
      iter_y = 0;
   retrotile_tile_t avg = 0;

   /* Average corner data. */
   for( iter_y = 0 ; 2 > iter_y ; iter_y++ ) {
      for( iter_x = 0 ; 2 > iter_x ; iter_x++ ) {
         tile_iter = &(retrotile_get_tile(
            t, layer,
            corners_x[iter_x][iter_y],
            corners_y[iter_x][iter_y] ));
         assert( -1 != *tile_iter );
         /*
         debug_printf(
            RETROTILE_TRACE_LVL, "%d: adding from coords %d x %d: %d",
            iter_depth,
            corners_x[iter_x][iter_y], corners_y[iter_x][iter_y],
            *tile_iter ); */
         avg += *tile_iter;
      }
   }

   /* TODO: Use right shift? */
   avg /= 4;

   return avg;
}

/* === */
 
MERROR_RETVAL retrotile_gen_diamond_square_iter(
   struct RETROTILE* t, retrotile_tile_t min_z, retrotile_tile_t max_z,
   uint32_t tuning, size_t layer_idx, uint8_t flags, void* data,
   retrotile_ani_cb animation_cb, void* animation_cb_data
) {
   int16_t iter_x = 0,
      iter_y = 0,
      iter_depth = 0;
   int16_t corners_x[2][2];
   int16_t corners_y[2][2];
   int32_t avg = 0;
   /* size_t tile_idx = 0; */
   struct RETROTILE_DATA_DS data_ds_sub;
   MAUG_MHANDLE data_ds_h = NULL;
   struct RETROTILE_DATA_DS* data_ds = NULL;
   /* retrotile_tile_t* tiles = NULL; */
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROTILE_LAYER* layer = NULL;
   retrotile_tile_t* tile_iter = NULL;
   uint8_t free_ds_data = 0;

   /*
   maug_mlock( t->tiles, tiles );
   maug_cleanup_if_null_alloc( struct GRIDCITY_TILE*, tiles );
   */

   layer = retrotile_get_layer_p( t, layer_idx );

   if( NULL == data ) {
      data_ds_h = maug_malloc( 1, sizeof( struct RETROTILE_DATA_DS ) );
      maug_cleanup_if_null_alloc( MAUG_MHANDLE, data_ds_h );
      free_ds_data = 1;
      maug_mlock( data_ds_h, data_ds );
      maug_cleanup_if_null_alloc( struct RETROTILE_DATA_DS*, data_ds );
      maug_mzero( data_ds, sizeof( struct RETROTILE_DATA_DS ) );

      memset( retrotile_get_tiles_p( layer ), -1,
         t->tiles_w * t->tiles_h * sizeof( retrotile_tile_t ) );

      data_ds->sect_w = t->tiles_w;
      data_ds->sect_h = t->tiles_h;
      data_ds->sect_w_half = data_ds->sect_w >> 1;
      data_ds->sect_h_half = data_ds->sect_h >> 1;
   } else {
      data_ds = (struct RETROTILE_DATA_DS*)data;
   }
   assert( NULL != data_ds );

   /* Trivial case; end recursion. */
   if( 0 == data_ds->sect_w ) {
      debug_printf(
         RETROTILE_TRACE_LVL, "%d return: null sector", iter_depth );
      goto cleanup;
   }

   if(
      data_ds->sect_x + data_ds->sect_w > t->tiles_w ||
      data_ds->sect_y + data_ds->sect_h > t->tiles_h
   ) {
      debug_printf(
         RETROTILE_TRACE_LVL, "%d return: overflow sector", iter_depth );
      goto cleanup;
   }

   iter_depth = t->tiles_w / data_ds->sect_w;

   /* Generate/grab corners before averaging them! */
   retrotile_gen_diamond_square_corners(
      corners_x, corners_y, min_z, max_z, tuning, data_ds, layer, t );

   if( 2 == data_ds->sect_w || 2 == data_ds->sect_h ) {
      /* Nothing to average, this sector is just corners! */
      debug_printf(
         RETROTILE_TRACE_LVL,
         "%d return: reached innermost point", iter_depth );
      goto cleanup; /* Skip further descent regardless. */
   }
   
   avg = 
      retrotile_gen_diamond_square_avg( corners_x, corners_y, t, layer );

   tile_iter = &(retrotile_get_tile(
      t, layer,
      data_ds->sect_x + data_ds->sect_w_half,
      data_ds->sect_y + data_ds->sect_h_half ));
   if( -1 != *tile_iter ) {
      debug_printf( RETROTILE_TRACE_LVL, "avg already present at %d x %d!",
         data_ds->sect_x + data_ds->sect_w_half,
         data_ds->sect_y + data_ds->sect_h_half );
      goto cleanup;
   }
   *tile_iter = avg;

   /* assert( 0 <= tiles[tile_idx].terrain );

   maug_munlock( city->tiles, tiles );
   tiles = NULL; */

   /* Recurse into subsectors. */
   for(
      iter_y = data_ds->sect_y ;
      iter_y < (data_ds->sect_y + data_ds->sect_h) ;
      iter_y++
   ) {
      for(
         iter_x = data_ds->sect_x ;
         iter_x < (data_ds->sect_x + data_ds->sect_w) ;
         iter_x++
      ) {
         data_ds_sub.sect_x = data_ds->sect_x + iter_x;

         data_ds_sub.sect_y = data_ds->sect_y + iter_y;

         data_ds_sub.sect_w = data_ds->sect_w_half;
         data_ds_sub.sect_h = data_ds->sect_h_half;
         data_ds_sub.sect_w_half = data_ds_sub.sect_w >> 1;
         data_ds_sub.sect_h_half = data_ds_sub.sect_h >> 1;

         debug_printf(
            RETROTILE_TRACE_LVL, "%d: child sector at %d x %d, %d wide",
            iter_depth,
            data_ds_sub.sect_x, data_ds_sub.sect_y, data_ds_sub.sect_w );

         retval = retrotile_gen_diamond_square_iter(
            t, min_z, max_z, tuning, layer_idx, flags, &data_ds_sub,
            animation_cb, animation_cb_data );
         maug_cleanup_if_not_ok();
      }
   }

   if(
      data_ds->sect_w == t->tiles_w >> 1 &&
      NULL != animation_cb
   ) {
      retval = animation_cb( animation_cb_data, iter_y );
      maug_cleanup_if_not_ok();
   }

   debug_printf(
      RETROTILE_TRACE_LVL, "%d return: all sectors complete", iter_depth );

cleanup:

   if( free_ds_data && NULL != data_ds ) {
      maug_munlock( data_ds_h, data_ds );
   }

   if( free_ds_data && NULL != data_ds_h ) {
      maug_mfree( data_ds_h );
   }

   return retval;
}

/* === */

MERROR_RETVAL retrotile_gen_voronoi_iter(
   struct RETROTILE* t, retrotile_tile_t min_z, retrotile_tile_t max_z,
   uint32_t tuning, size_t layer_idx, uint8_t flags, void* data,
   retrotile_ani_cb animation_cb, void* animation_cb_data
) {
   int16_t x = 0,
      y = 0,
      offset_x = 0,
      offset_y = 0,
      finished = 0;
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROTILE_LAYER* layer = NULL;
   int16_t spb = RETROTILE_VORONOI_DEFAULT_SPB;
   int16_t drift = RETROTILE_VORONOI_DEFAULT_DRIFT;
   MAUG_MHANDLE temp_grid_h = (MAUG_MHANDLE)NULL;
   retrotile_tile_t* temp_grid = NULL;
   retrotile_tile_t* tiles = NULL;
   /* Only use 4 cardinal directions. */
   int8_t side_iter = 0;

   layer = retrotile_get_layer_p( t, 0 );

   tiles = retrotile_get_tiles_p( layer );

   /* Initialize grid to empty. */
   memset( tiles, -1,
      t->tiles_w * t->tiles_h * sizeof( retrotile_tile_t ) );

   /* Generate the initial sector starting points. */
   for( y = 0 ; t->tiles_w > y ; y += spb ) {
      for( x = 0 ; t->tiles_w > x ; x += spb ) {
         offset_x = x + ((drift * -1) + (rand() % drift));
         offset_y = y + ((drift * -1) + (rand() % drift));

         /* Clamp sector offsets onto map borders. */
         if( 0 > offset_x ) {
            offset_x = 0;
         }
         if( offset_x >= t->tiles_w ) {
            offset_x = t->tiles_w - 1;
         }
         if( 0 > offset_y ) {
            offset_y = 0;
         }
         if( offset_y >= t->tiles_h ) {
            offset_y = t->tiles_h - 1;
         }

         retrotile_get_tile( t, layer, offset_x, offset_y ) =
            min_z + (rand() % max_z);
      }
   }

   temp_grid_h = maug_malloc(
      sizeof( retrotile_tile_t ), t->tiles_w * t->tiles_h );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, temp_grid_h );

   maug_mlock( temp_grid_h, temp_grid );
   maug_cleanup_if_null_alloc( retrotile_tile_t*, temp_grid );

   /* Grow the sector starting points. */
   while( !finished ) {
      if( NULL != animation_cb ) {
         retval = animation_cb( animation_cb_data, -1 );
         maug_cleanup_if_not_ok();
      }

      /* Prepare sampling grid so we're working from unexpanded sections
       * below.
       */
      memcpy(
         temp_grid, tiles,
         sizeof( retrotile_tile_t ) * t->tiles_w * t->tiles_h );

      /* Starting another pass, assume finished until proven otherwise. */
      finished = 1;
      for( y = 0 ; t->tiles_h > y ; y++ ) {
         for( x = 0 ; t->tiles_w > x ; x++ ) {
            if( -1 == retrotile_get_tile( t, layer, x, y ) ) {
               /* If there are still unfilled tiles, we're not finished
                * yet!
                */
               finished = 0;

               /* Skip filled tile. */
               continue;
            }


            for( side_iter = 0 ; 4 > side_iter ; side_iter++ ) {
               debug_printf( RETROTILE_TRACE_LVL,
                  "%d (%d), %d (%d) (%d, %d)", 
                  x,
                  gc_retrotile_offsets4_x[side_iter],
                  y,
                  gc_retrotile_offsets4_y[side_iter],
                  t->tiles_w, t->tiles_h );
            
               /* Iterate through directions to expand. */
               /* TODO: Add tuning to select directional probability. */
               if(
                  0 <= x + gc_retrotile_offsets4_x[side_iter] &&
                  0 <= y + gc_retrotile_offsets4_y[side_iter] &&
                  t->tiles_w > x + gc_retrotile_offsets4_x[side_iter] &&
                  t->tiles_h > y + gc_retrotile_offsets4_y[side_iter] &&
                  -1 == temp_grid[
                     ((y + gc_retrotile_offsets4_y[side_iter]) *
                        t->tiles_w) +
                           (x + gc_retrotile_offsets4_x[side_iter])]
               ) {
                  /* Copy center tile to this direction. */
                  retrotile_get_tile( t, layer,
                     x + gc_retrotile_offsets4_x[side_iter],
                     y + gc_retrotile_offsets4_y[side_iter] ) =
                        retrotile_get_tile( t, layer, x, y );
                  break;
               }
            }
         }
      }
   }

cleanup:

   if( NULL != temp_grid ) {
      maug_munlock( temp_grid_h, temp_grid );
   }

   if( NULL != temp_grid_h ) {
      maug_mfree( temp_grid_h );
   }

   return retval;
}

/* === */

MERROR_RETVAL retrotile_gen_smooth_iter(
   struct RETROTILE* t, retrotile_tile_t min_z, retrotile_tile_t max_z,
   uint32_t tuning, size_t layer_idx, uint8_t flags, void* data,
   retrotile_ani_cb animation_cb, void* animation_cb_data
) {
   MERROR_RETVAL retval = MERROR_OK;
   int16_t x = 0,
      y = 0,
      side_iter = 0,
      sides_avail = 0,
      sides_sum = 0;
   /* Sides start from 12 on the clock (up). */
   struct RETROTILE_LAYER* layer = NULL;

   assert( NULL != t );
   layer = retrotile_get_layer_p( t, layer_idx );
   assert( NULL != layer );
   
   for( y = 0 ; t->tiles_h > y ; y++ ) {
      if( NULL != animation_cb ) {
         retval = animation_cb( animation_cb_data, y );
         maug_cleanup_if_not_ok();
      }
      for( x = 0 ; t->tiles_w > x ; x++ ) {
         /* Reset average. */
         sides_avail = 0;
         sides_sum = 0;

         /* Grab values for available sides. */
         for( side_iter = 0 ; 8 > side_iter ; side_iter++ ) {
            if(
               0 > x + gc_retrotile_offsets8_x[side_iter] ||
               0 > y + gc_retrotile_offsets8_y[side_iter] ||
               t->tiles_w <= x + gc_retrotile_offsets8_x[side_iter] ||
               t->tiles_h <= y + gc_retrotile_offsets8_y[side_iter]
            ) {
               continue;
            }

            sides_avail++;
            debug_printf(
               RETROTILE_TRACE_LVL,
               "si %d: x, y: %d (+%d), %d (+%d) idx: %d",
               side_iter,
               x + gc_retrotile_offsets8_x[side_iter],
               gc_retrotile_offsets8_x[side_iter],
               y + gc_retrotile_offsets8_y[side_iter],
               gc_retrotile_offsets8_y[side_iter],
               ((y + gc_retrotile_offsets8_y[side_iter]) * t->tiles_w) +
                  x + gc_retrotile_offsets8_x[side_iter] );
            sides_sum += retrotile_get_tile(
               t, layer,
               x + gc_retrotile_offsets8_x[side_iter],
               y + gc_retrotile_offsets8_y[side_iter] );
         }

         retrotile_get_tile( t, layer, x, y ) = sides_sum / sides_avail;
      }
   }

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retrotile_gen_borders_iter(
   struct RETROTILE* t, retrotile_tile_t min_z, retrotile_tile_t max_z,
   uint32_t tuning, size_t layer_idx, uint8_t flags, void* data,
   retrotile_ani_cb animation_cb, void* animation_cb_data
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROTILE_DATA_BORDER* borders =
      (struct RETROTILE_DATA_BORDER*)data;
   size_t i = 0,
      x = 0,
      y = 0,
      x_plus_1 = 0,
      y_plus_1 = 0,
      side = 0;
   int16_t ctr_iter = 0,
      outside_iter = 0;
   struct RETROTILE_LAYER* layer = NULL;

   assert( NULL != t );
   layer = retrotile_get_layer_p( t, layer_idx );
   assert( NULL != layer );

   /* Reset tile counter for all defined borders. */
   for( i = 0 ; 0 <= borders[i].center ; i++ ) {
      borders[i].tiles_changed = 0;
   }

   debug_printf( 1, "adding borders..." );

   for( y = 0 ; t->tiles_h > y ; y++ ) {
      for( x = 0 ; t->tiles_w > x ; x++ ) {
         i = 0;
         while( 0 <= borders[i].center ) {
            /* Compare/grab current center tile. */
            ctr_iter = retrotile_get_tile( t, layer, x, y );
            debug_printf( RETROTILE_TRACE_LVL,
               "x: " SIZE_T_FMT ", y: " SIZE_T_FMT ", 0x%04x vs 0x%04x",
               x, y, ctr_iter, borders[i].center );
            if( ctr_iter != borders[i].center ) {
               i++;
               continue;
            }

            debug_printf( RETROTILE_TRACE_LVL, "comparing sides..." );

            /* Zeroth pass: look for stick-outs. */
            for( side = 0 ; 8 > side ; side += 2 ) {
               if(
                  x + gc_retrotile_offsets8_x[side] > t->tiles_w ||
                  y + gc_retrotile_offsets8_y[side] > t->tiles_h
               ) {
                  /* Skip out-of-bounds. */
                  continue;
               }
               /* Get the outside tile on this side. */
               outside_iter = retrotile_get_tile( t, layer,
                  x + gc_retrotile_offsets8_x[side],
                  y + gc_retrotile_offsets8_y[side] );

               /* Get the outside tile next two clock-steps from this one.
                */
               if( side + 4 < 8 ) {
                  x_plus_1 = x + gc_retrotile_offsets8_x[side + 4];
                  y_plus_1 = y + gc_retrotile_offsets8_y[side + 4];
               } else {
                  x_plus_1 = x + gc_retrotile_offsets8_x[side - 4];
                  y_plus_1 = y + gc_retrotile_offsets8_y[side - 4];
               }

               if(
                  x_plus_1 < t->tiles_w && y_plus_1 < t->tiles_h &&
                  outside_iter == borders[i].outside &&
                  outside_iter == retrotile_get_tile( t, layer,
                     x_plus_1, y_plus_1 )
               ) {
                  /* This has the outside on two opposing sides, so just
                   * erase it and use the outside. */
                  retrotile_get_tile( t, layer, x, y ) =
                     borders[i].outside;
                  borders[i].tiles_changed++;
                  goto tile_done;
               }
            }

 
            /* First pass: look for corners. */
            for( side = 0 ; 8 > side ; side += 2 ) {
               if(
                  x + gc_retrotile_offsets8_x[side] > t->tiles_w ||
                  y + gc_retrotile_offsets8_y[side] > t->tiles_h
               ) {
                  /* Skip out-of-bounds. */
                  continue;
               }
               /* Get the outside tile on this side. */
               outside_iter = retrotile_get_tile( t, layer,
                  x + gc_retrotile_offsets8_x[side],
                  y + gc_retrotile_offsets8_y[side] );

               /* Get the outside tile next two clock-steps from this one.
                */
               if( side + 2 < 8 ) {
                  x_plus_1 = x + gc_retrotile_offsets8_x[side + 2];
                  y_plus_1 = y + gc_retrotile_offsets8_y[side + 2];
               } else {
                  x_plus_1 = x + gc_retrotile_offsets8_x[0];
                  y_plus_1 = y + gc_retrotile_offsets8_y[0];
               }

               if(
                  x_plus_1 < t->tiles_w && y_plus_1 < t->tiles_h &&
                  outside_iter == borders[i].outside &&
                  outside_iter == retrotile_get_tile( t, layer,
                     x_plus_1, y_plus_1 )
               ) {
                  /* This has the outside on two sides, so use a corner. */
                  retrotile_get_tile( t, layer, x, y ) =
                     borders[i].mod_to[side + 1 < 8 ? side + 1 : 0];
                  borders[i].tiles_changed++;
                  goto tile_done;
               }
            }

            /* Second pass (if first pass fails): look for edges. */
            for( side = 0 ; 8 > side ; side += 2 ) {
               if(
                  x + gc_retrotile_offsets8_x[side] > t->tiles_w ||
                  y + gc_retrotile_offsets8_y[side] > t->tiles_h
               ) {
                  /* Skip out-of-bounds. */
                  continue;
               }
               /* Get the outside tile on this side. */
               outside_iter = retrotile_get_tile( t, layer,
                  x + gc_retrotile_offsets8_x[side],
                  y + gc_retrotile_offsets8_y[side] );

               if( outside_iter == borders[i].outside ) {
                  /* It only matches on this side. */
                  debug_printf( RETROTILE_TRACE_LVL, "replacing..." );
                  retrotile_get_tile( t, layer, x, y ) =
                     borders[i].mod_to[side];
                  borders[i].tiles_changed++;
                  goto tile_done;
               }
            }

tile_done:
            /* Tile replaced or not replaceable. */
            break;
         }
      }
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
      " tiles, " SIZE_T_FMT " layers (" SIZE_T_FMT " bytes)...",
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

#else

extern MAUG_CONST int16_t SEG_MCONST gc_retrotile_offsets8_x[8];
extern MAUG_CONST int16_t SEG_MCONST gc_retrotile_offsets8_y[8];
extern MAUG_CONST int16_t SEG_MCONST gc_retrotile_offsets4_x[4];
extern MAUG_CONST int16_t SEG_MCONST gc_retrotile_offsets4_y[4];

#endif /* RETROTIL_C */

/*! \} */ /* retrotile */

#endif /* !RETROTIL_H */

