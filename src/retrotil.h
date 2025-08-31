
#ifndef RETROTIL_H
#define RETROTIL_H

#include <mjson.h>
#include <mfile.h>

/**
 * \addtogroup maug_retroflt
 * \{
 * \addtogroup retrotile RetroTile API
 * \brief Functions and structures for working with tilemaps/grids.
 * \{
 * \file retrotil.h
 */

#define RETROTILE_PRESENT

typedef int16_t retrotile_coord_t;

#ifndef RETROTILE_TRACE_CHARS
#  define RETROTILE_TRACE_CHARS 0
#endif /* !RETROTILE_TRACE_TOKENS */

#ifndef RETROTILE_NAME_SZ_MAX
/*! \brief Maximum number of chars in a RETROTILE::name. */
#  define RETROTILE_NAME_SZ_MAX 10
#endif /* !RETROTILE_NAME_SZ_MAX */

#ifndef RETROTILE_PROP_NAME_SZ_MAX
/*! \brief Maximum number of chars in a parsed property name. */
#  define RETROTILE_PROP_NAME_SZ_MAX 10
#endif /* !RETROTILE_PROP_NAME_SZ_MAX */

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

#ifndef RETROTILE_PARSER_FLAG_LITERAL_PATHS
/**
 * \relates RETROTILE_PARSER
 * \brief Flag for RETROTILE_PARSER::flags indicating to use literal image
 *        asset paths.
 */
#  define RETROTILE_PARSER_FLAG_LITERAL_PATHS 0x02
#endif /* !RETROTILE_PARSER_FLAG_LITERAL_PATHS */

/**
 * \relates RETROTILE_PARSER
 * \brief Value for RETROTILE_PARSER::mode indicating the parser is currently
 *        parsing a tilemap.
 */
#define RETROTILE_PARSER_MODE_MAP    0

/**
 * \relates RETROTILE_PARSER
 * \brief Value for RETROTILE_PARSER::mode indicating the parser is currently
 *        parsing tile definitions.
 */
#define RETROTILE_PARSER_MODE_DEFS   1

#define RETROTILE_CLASS_TABLE( f ) \
   f( TILE,    tile,    0 ) \
   f( MOBILE,  mobile,  1 ) \
   f( WARP,    warp,    2 ) \
   f( ITEM,    item,    3 ) \
   f( CROP,    crop,    4 )

/**
 * \addtogroup \retrotile_defs RetroTile Tile Definitions
 * \{
 */

/**
 * \addtogroup \retrotile_defs_types RetroTile Custom Property Types
 * \{
 */

/**
 * \relates RETROTILE_TILE_DEF
 * \brief Indicates tile should block movement.
 */
#define RETROTILE_TILE_FLAG_BLOCK      0x01

/**
 * \relates RETROTILE_TILE_DEF
 * \brief Not currently used, but may be in the future.
 */
#define RETROTILE_TILE_FLAG_RESERVED1  0x02

/**
 * \relates RETROTILE_TILE_DEF
 * \brief Not currently used, but may be in the future.
 */
#define RETROTILE_TILE_FLAG_RESERVED2  0x04

/**
 * \relates RETROTILE_TILE_DEF
 * \brief Not currently used, but may be in the future.
 */
#define RETROTILE_TILE_FLAG_RESERVED3  0x08

/*! \} */ /* retrotile_defs_types */

/**
 * \relates RETROTILE_PARSER
 * \brief Value for RETROTILE_PARSER::last_prop_type indicating other type.
 */
#define RETROTILE_PROP_TYPE_OTHER 0

/**
 * \relates RETROTILE_PARSER
 * \brief Value for RETROTILE_PARSER::last_prop_type indicating string.
 */
#define RETROTILE_PROP_TYPE_STRING 1

/**
 * \relates RETROTILE_PARSER
 * \brief Value for RETROTILE_PARSER::last_prop_type indicating file path.
 */
#define RETROTILE_PROP_TYPE_FILE 2

/**
 * \relates RETROTILE_PARSER
 * \brief Value for RETROTILE_PARSER::last_prop_type indicating integer.
 */
#define RETROTILE_PROP_TYPE_INT 3

/**
 * \brief Flag for retrotile_gen_diamond_square_iter() indicating that passed
 *        ::RETROTILE_DATA_DS object should be initialized (first pass).
 *
 * This should only be used if calling retrotile_gen_diamond_square_iter()
 * from outside retrotil.h with a ::RETROTILE_DATA_DS object, so as to return
 * statistics about the generated terrain. Otherwise, a NULL can be passed to
 * retrotile_gen_diamond_square_iter() so that it will just use an internal
 * object which will always be initialized.
 */
#define RETROTILE_DS_FLAG_INIT_DATA    0x02

/**
 * \brief Conversion specifier to use with ::retroflat_tile_t.
 */
#define RETROTILE_IDX_FMT "%u"

struct RETROTILE_TILE_DEF {
   /*! \brief Size of this struct (useful for serializing). */
   size_t sz;
   uint8_t flags;
   retroflat_asset_path image_path;
   size_t x;
   size_t y;
   uint16_t tile_class;
   char warp_dest[RETROTILE_NAME_SZ_MAX + 1];
   /**
    * \brief Field indicating how many degrees the tile should always be
    *        rotated before drawin on-screen. Useful for engine drawing
    *        routines.
    */
   mfix_t static_rotation;
   retrotile_coord_t warp_x;
   retrotile_coord_t warp_y;
#ifdef RETROFLAT_3D
   /* TODO: Work this into retrogxc? */
   struct RETRO3DP_MODEL model;
#endif /* RETROFLAT_3D */
#ifdef RETROGXC_PRESENT
   ssize_t image_cache_id;
#else
   struct RETROFLAT_BITMAP image;
#endif /* RETROGXC_PRESENT */
};

/*! \} */ /* retrotile_defs */

struct RETROTILE_LAYER {
   /*! \brief Size of this struct (useful for serializing). */
   size_t sz;
   /*! \brief Size of the layer in bytes (including this struct header). */
   size_t total_sz;
   uint16_t layer_class;
};

/**
 * \brief This is not currently used for anything, but is provided as a
 *        a convenience for game logic.
 */
struct RETROTILE_COORDS {
   /* \brief X position in tilemap tiles. */
   retrotile_coord_t x;
   /* \brief Y position in tilemap tiles. */
   retrotile_coord_t y;
};

/**
 * \brief A struct representing a tilemap.
 * \warning Only single-tileset tilemaps are supported!
 * \warning Only tile layers are supported!
 */
struct RETROTILE {
   /*! \brief Size of this struct (useful for serializing). */
   size_t sz;
   char name[RETROTILE_NAME_SZ_MAX + 1];
   char tileset[RETROTILE_NAME_SZ_MAX + 1];
   /*! \brief Size of the tilemap in bytes (including this struct header). */
   uint32_t total_sz;
   /*! \brief Number of tile layers in this tilemap. */
   uint32_t layers_count;
   /*! \brief First GID in the accompanying tileset. */
   size_t tileset_fgid;
   /*! \brief Height of all layers of the tilemap in tiles. */
   size_t tiles_h;
   /*! \brief Width of all layers of the tilemap in tiles. */
   size_t tiles_w;
   /**
    * \brief Amount by which to scale tiles (convenience property).
    * \todo Replace this with a hashtable of custom properties.
    * */
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
   retroflat_tile_t highest_generated;
   retroflat_tile_t lowest_generated;
};

struct RETROTILE_DATA_BORDER {
   int16_t tiles_changed;
   retroflat_tile_t center;
   retroflat_tile_t outside;
   /*! \brief If the center and outside match, use this mod-to. */
   retroflat_tile_t mod_to[8];
};

#define retrotile_get_tile( tilemap, layer, x, y ) \
   (retrotile_get_tiles_p( layer )[((y) * (tilemap)->tiles_w) + (x)])

#define retrotile_set_tile( tilemap, layer, x, y, new_val ) \
   (retrotile_get_tiles_p( layer )[((y) * (tilemap)->tiles_w) + (x)])

#define retrotile_get_tiles_p( layer ) \
   ((retroflat_tile_t*)(((uint8_t*)(layer)) + \
   sizeof( struct RETROTILE_LAYER )))

#ifdef MAUG_NO_STDLIB
/* Loop is slower, but doesn't use memset. */
/* TODO: This kinda assumes retroflat_tile_t is 8 bits... Fix that! */
#  define retrotile_clear_tiles( t, layer, i ) \
      assert( 1 == sizeof( retroflat_tile_t ) ); \
      for( \
         i = 0 ; \
         (t)->tiles_w * (t)->tiles_h * sizeof( retroflat_tile_t ) > i ; \
         i++ \
      ) { \
         retrotile_get_tiles_p( layer )[i] = -1; \
      }
#else
#  define retrotile_clear_tiles( t, layer, i ) \
      memset( retrotile_get_tiles_p( layer ), -1, \
         (t)->tiles_w * (t)->tiles_h * sizeof( retroflat_tile_t ) )
#endif /* MAUG_NO_STDLIB */

/**
 * \addtogroup retrotile_parser RetroTile Parser
 * \{
 */

typedef MERROR_RETVAL (*retrotile_tj_parse_cb)(
   const char* dirname, const char* filename, MAUG_MHANDLE* p_tm_h,
   struct MDATA_VECTOR* p_td, mparser_wait_cb_t wait_cb, void* wait_data,
   mparser_parse_token_cb token_cb, void* token_cb_data, uint8_t passes,
   uint8_t flags );

struct RETROTILE_PARSER {
   uint8_t mstate;
   uint16_t flags;
   /* TODO: Use flags and combine these. */
   uint8_t pass;
   uint8_t passes_max;
   /*! \brief Value indicating the current type of object being parsed into. */
   uint8_t mode;
   mparser_wait_cb_t wait_cb;
   void* wait_data;
   retroflat_ms_t wait_last;
   size_t layer_tile_iter;
   /*! \brief The name of the last property key/value pair parsed. */
   char last_prop_name[RETROTILE_PROP_NAME_SZ_MAX + 1];
   int last_prop_type;
   size_t last_prop_name_sz;
   /*! \brief The name to give to the new tilemap. */
   char tilemap_name[RETROTILE_NAME_SZ_MAX + 1];
   char tileset_name[RETROTILE_NAME_SZ_MAX + 1];
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
   /**
    * \brief Callback to parse engine-specific custom tokens from the tilemap
    *        JSON. Should return ::MERROR_PREEMPT if parsing is successful and
    *        should override standard parsing.
    */
   mparser_parse_token_cb custom_token_cb;
   void* custom_token_cb_data;
   struct MJSON_PARSER jparser;
   struct MDATA_VECTOR* p_tile_defs;
   char dirname[MAUG_PATH_SZ_MAX + 1];
   uint16_t layer_class;
};

/*    State                        Idx JSONKeyWord    Parent       ParseMode */
#define RETROTILE_PARSER_MSTATE_TABLE( f ) \
   f( MTILESTATE_NONE,              0, "", 0, 0 ) \
   f( MTILESTATE_HEIGHT,            1, "height",      0                 , 0 ) \
   f( MTILESTATE_WIDTH,             2, "width",       0                 , 0 ) \
   f( MTILESTATE_LAYERS,            3, "layers",      0                 , 0 ) \
   f( MTILESTATE_LAYER_DATA,        4, "data",        15 /* LAYER */    , 0 ) \
   f( MTILESTATE_LAYER_NAME,        5, "name",        15 /* LAYER */    , 0 ) \
   f( MTILESTATE_TILES,             6, "tiles",       0                 , 1 ) \
   f( MTILESTATE_TILES_ID,          7, "id",          6 /* TILES */     , 1 ) \
   f( MTILESTATE_TILES_IMAGE,       8, "image",       6 /* TILES */     , 1 ) \
   f( MTILESTATE_TILESETS,          9, "tilesets",    0                 , 0 ) \
   f( MTILESTATE_TILESETS_SRC,      10, "source",     9 /* TILESETS */  , 0 ) \
   f( MTILESTATE_TILESETS_FGID,     11, "firstgid",   9 /* TILESETS */  , 0 ) \
   f( MTILESTATE_TILESETS_PROP,     12, "firstgid",   9 /* TILESETS */  , 0 ) \
   f( MTILESTATE_GRID,              13, "grid",       0                 , 1 ) \
   f( MTILESTATE_TILES_PROP,        14, "properties", 6 /* TILES */     , 1 ) \
   f( MTILESTATE_LAYER,             15, "layers", /* [sic] */ 3         , 0 ) \
   f( MTILESTATE_TILES_PROP_NAME,   16, "name",       14 /* TIL_PROP */ , 1 ) \
   f( MTILESTATE_TILES_PROP_TYPE,   17, "type",       14 /* TIL_PROP */ , 1 ) \
   f( MTILESTATE_TILES_PROP_VAL,    18, "value",      14 /* TIL_PROP */ , 1 ) \
   f( MTILESTATE_PROP,              19, "properties", 0  /* NONE */     , 0 ) \
   f( MTILESTATE_PROP_NAME,         20, "name",       19 /* PROP */     , 0 ) \
   f( MTILESTATE_PROP_TYPE,         21, "type",       19 /* PROP */     , 0 ) \
   f( MTILESTATE_PROP_VAL,          22, "value",      19 /* PROP */     , 0 ) \
   f( MTILESTATE_LAYER_CLASS,       23, "class",      15 /* LAYER */    , 0 ) \
   f( MTILESTATE_TILES_CLASS,       24, "type",       6  /* TILES */    , 1 ) \
   f( MTILESTATE_NAME,              25, "name",       0                 , 1 ) \
   f( MTILESTATE_WANGSETS,          26, "wangsets",   0                 , 1 ) \
   f( MTILESTATE_TPROP,             27, "properties", 0  /* NONE */     , 1 ) \
   f( MTILESTATE_TPROP_NAME,        28, "name",       27 /* PROP */     , 1 ) \
   f( MTILESTATE_TPROP_TYPE,        29, "type",       27 /* PROP */     , 1 ) \
   f( MTILESTATE_TPROP_VAL,         30, "value",      27 /* PROP */     , 1 )

/* TODO: Mine wangsets for slowdown values, etc. */

MERROR_RETVAL
retrotile_parse_json_c( struct RETROTILE_PARSER* parser, char c );

/**
 * \brief Convert a Tiled "type" field to an integer suitable for use with
 *        RETROTILE_PARSER::last_prop_type.
 */
int retrotile_parse_prop_type( const char* token, size_t token_sz );

/**
 * \brief Convert a less-or-equal-to-two-character string to a direction in
 *        degrees.
 *
 * This function is useful for converting a prerendered isometric tileset to
 * modifications for a realtime-rendered ::RETRO3DP_MODEL.
 */
mfix_t retrotile_static_rotation_from_dir( const char* dir );

/**
 * \brief Parse the JSON file at the given path into a heap-allocated tilemap
 *        with a ::RETROTILE struct header.
 * \param dirname Base directory from which to load the tilemap and its
 *                accompanying data files. This includes external tileset
 *                files and tile graphics.
 * \param filename Name of tilemap JSON file to parse, inside of dirname.
 * \param p_tilemap_h Pointer to a ::MAUG_MHANDLE to allocate and load the
 *                    tilemap into. The handle targeted should be NULL and,
 *                    if this is successful, will no longer be NULL afterward.
 * \param p_tile_defs Pointer to a ::MDATA_VECTOR to be loaded with
 *                    ::RETROTILE_TILE_DEF structs from the given tilemap.
 * \param wait_cb Callback to call every so often during loading (e.g. to
 *                update a loading animation or similar).
 * \param wait_data Arbitrary data to pass to wait_cb when it is called.
 * \param token_cb Callback to parse engine-specific custom tokens from the
 *                 tilemap JSON. Should return MERROR_PREEMPT if parsing is
 *                 successful and should override standard parsing.
 * \param passes Number of passes to make on parsed tilemap. This can be
 *               combined with token_cb for more advanced operations. The
 *               minimum is 2.
 * \return MERROR_OK if successful or other MERROR_RETVAL otherwise.
 * \note This function ignores RETROFLAT_STATE::assets_path.
 */
MERROR_RETVAL retrotile_parse_json_file(
   const char* dirname, const char* filename, MAUG_MHANDLE* p_tilemap_h,
   struct MDATA_VECTOR* p_tile_defs,
   mparser_wait_cb_t wait_cb, void* wait_data,
   mparser_parse_token_cb token_cb, void* token_cb_data, uint8_t passes,
   uint8_t flags );

/*! \} */ /* retrotile_parser */

/**
 * \addtogroup retrotile_gen RetroTile Generators
 * \brief Tools for procedurally generating tilemaps.
 * \{
 */

typedef MERROR_RETVAL (*retrotile_ani_cb)(
   void* animation_cb_data, int16_t iter );

typedef MERROR_RETVAL (*retrotile_gen_cb)(
   struct RETROTILE* t, retroflat_tile_t min_z, retroflat_tile_t max_z,
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
   struct RETROTILE* t, retroflat_tile_t min_z, retroflat_tile_t max_z,
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
   struct RETROTILE* t, retroflat_tile_t min_z, retroflat_tile_t max_z,
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
   struct RETROTILE* t, retroflat_tile_t min_z, retroflat_tile_t max_z,
   uint32_t tuning, size_t layer_idx, uint8_t flags, void* data,
   retrotile_ani_cb animation_cb, void* animation_cb_data );

/**
 * \brief Given a list of ::RETROTILE_DATA_BORDER structs, this will
 *        search for occurrences of RETROTILE_DATA_BORDER::center next to
 *        RETROTILE_DATA_BORDER::outside and replace with the appropriate
 *        border between the two!
 */
MERROR_RETVAL retrotile_gen_borders_iter(
   struct RETROTILE* t, retroflat_tile_t min_z, retroflat_tile_t max_z,
   uint32_t tuning, size_t layer_idx, uint8_t flags, void* data,
   retrotile_ani_cb animation_cb, void* animation_cb_data );

/*! \} */ /* retrotile_gen */

struct RETROTILE_LAYER* retrotile_get_layer_p(
   struct RETROTILE* tilemap, uint32_t layer_idx );

MERROR_RETVAL retrotile_alloc(
   MAUG_MHANDLE* p_tilemap_h, size_t w, size_t h, size_t layers_count,
   const char* tilemap_name, const char* tileset_name );

void retrotile_format_asset_path(
   retroflat_asset_path path_out, const char* afile,
   struct RETROTILE_PARSER* parser );

MERROR_RETVAL retrotile_clear_refresh( retroflat_pxxy_t y_max );

MERROR_RETVAL retrotile_topdown_draw(
   struct RETROFLAT_BITMAP* target,
   struct RETROTILE* t, struct MDATA_VECTOR* t_defs );

#ifdef RETROTIL_C

#  include <mparser.h>

/* TODO: Function names should be verb_noun! */

#if RETROTILE_TRACE_LVL > 0
#  define retrotile_parser_mstate( parser, new_mstate ) \
      parser->mstate = new_mstate; \
      debug_printf( \
         RETROTILE_TRACE_LVL, "parser mstate: %d", parser->mstate );
#else
#  define retrotile_parser_mstate( parser, new_mstate ) \
      parser->mstate = new_mstate;
#endif /* RETROTILE_TRACE_LVL */

#  define RETROTILE_PARSER_MSTATE_TABLE_CONST( name, idx, tokn, parent, m ) \
      MAUG_CONST uint8_t SEG_MCONST name = idx;

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

#  define RETROTILE_CLASS_TABLE_CONSTS( A, a, i ) \
      MAUG_CONST uint8_t SEG_MCONST RETROTILE_CLASS_ ## A = i;

RETROTILE_CLASS_TABLE( RETROTILE_CLASS_TABLE_CONSTS )

/* === */

static void retrotile_parser_match_token(
   const char* token, size_t token_sz, struct RETROTILE_PARSER* parser
) {
   size_t j = 1;

   /* Figure out what the key is for. */
   while( '\0' != gc_retrotile_mstate_tokens[j][0] ) {
      if(
         /* Make sure tokens match. */
         maug_strlen( gc_retrotile_mstate_tokens[j] ) != token_sz ||
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
#ifdef RETROTILE_TRACE_TOKENS
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
#endif /* !RETROTILE_TRACE_TOKENS */
         j++;
         continue;

      } else if( parser->mode != gc_retrotile_mstate_modes[j] ) {
#ifdef RETROTILE_TRACE_TOKENS
         debug_printf(
            RETROTILE_TRACE_LVL, "found token %s but incorrect mode %u!",
            token,
            gc_retrotile_mstate_modes[j] );
#endif /* !RETROTILE_TRACE_TOKENS */
         j++;
         continue;

      } else {
         /* Found it! */
#ifdef RETROTILE_TRACE_TOKENS
         debug_printf(
            RETROTILE_TRACE_LVL,
            "found token \"%s\" "
#ifdef MPARSER_TRACE_NAMES
               "under correct parent %s (%d)!",
#else
               "under correct parent %d!",
#endif /* MPARSER_TRACE_NAMES */
            token,
#ifdef MPARSER_TRACE_NAMES
            retrotile_mstate_name( parser->mstate ),
            parser->mstate
#else
            parser->mstate
#endif /* MPARSER_TRACE_NAMES */
         );
#endif /* !RETROTILE_TRACE_TOKENS */

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
   struct RETROTILE_TILE_DEF* tile_def = NULL;
   struct RETROTILE_PARSER* parser = (struct RETROTILE_PARSER*)parser_arg;
   size_t tileset_id_parsed = 0;

   /* We don't lock the vector right away, since we might reallocate tile defs
    * later on, below.
    */

   /* Try the custom parser. */
   if(
      NULL != parser->custom_token_cb &&
      /* MERROR_PREEMPT is dealt with in cleanup. */
      MERROR_OK != (retval = parser->custom_token_cb(
         token, token_sz, parser_arg ))
   ) {
      goto cleanup;
   }

   if( MJSON_PSTATE_OBJECT_VAL != mjson_parser_pstate( &(parser->jparser) ) ) {
      /* Not a token we have to worry about! */
      retrotile_parser_match_token( token, token_sz, parser );
      goto cleanup;
   }

   if( MTILESTATE_TILES_ID == parser->mstate ) {
      retrotile_parser_mstate( parser, MTILESTATE_TILES );
      if( 0 == parser->pass ) {
         /* Parse tile ID. */
         tileset_id_parsed = maug_atou32( token, token_sz, 10 );
         if( tileset_id_parsed > parser->tileset_id_cur ) {
            parser->tileset_id_cur = tileset_id_parsed;
#if RETROTILE_TRACE_LVL > 0
            debug_printf(
               RETROTILE_TRACE_LVL,
               "new highest tile ID: " SIZE_T_FMT, parser->tileset_id_cur );
#endif /* RETROTILE_TRACE_LVL */
         }
      } else {
         assert( 0 < mdata_vector_ct( parser->p_tile_defs ) );
         parser->tileset_id_cur = maug_atou32( token, token_sz, 10 );
#if RETROTILE_TRACE_LVL > 0
         debug_printf(
            RETROTILE_TRACE_LVL,
            "next tile ID: " SIZE_T_FMT, parser->tileset_id_cur );
#endif /* RETROTILE_TRACE_LVL */
      }
      retrotile_parser_mstate( parser, MTILESTATE_TILES );

   } else if( MTILESTATE_TILES_IMAGE == parser->mstate ) {
      if( 1 == parser->pass ) {
         /* Need this pass 1 so tile_defs vector is allocated. */
         mdata_vector_lock( parser->p_tile_defs );
         tile_def = mdata_vector_get(
            parser->p_tile_defs, parser->tileset_id_cur,
            struct RETROTILE_TILE_DEF );
         assert( NULL != tile_def );

         /* Parse tile image. */
         if(
            RETROTILE_PARSER_FLAG_LITERAL_PATHS ==
            (RETROTILE_PARSER_FLAG_LITERAL_PATHS & parser->flags)
         ) {
            mfile_assign_path( tile_def->image_path, token, 0 );
         } else {
            mfile_assign_path(
               tile_def->image_path, token, MFILE_ASSIGN_FLAG_TRIM_EXT );
         }

         /* Setup tile_def sz for serializing. */
         tile_def->sz = sizeof( struct RETROTILE_TILE_DEF );

#if RETROTILE_TRACE_LVL > 0
         debug_printf(
            RETROTILE_TRACE_LVL, "set tile ID " SIZE_T_FMT " to: %s",
            parser->tileset_id_cur, tile_def->image_path );
#endif /* RETROTILE_TRACE_LVL */
      }
      retrotile_parser_mstate( parser, MTILESTATE_TILES );

   } else if( MTILESTATE_TILES_CLASS == parser->mstate ) {
      if( 1 == parser->pass ) {
         /* Set the class from the pass 1 so the custom function can use
          * it on pass 2 even if it's alphabetically out of order.
          * (Tiled calls it "type" and puts it near the bottom!)
          *
          * We can't do this on pass 0 since tiles aren't allocated yet!
          */
         mdata_vector_lock( parser->p_tile_defs );
         tile_def = mdata_vector_get(
            parser->p_tile_defs, parser->tileset_id_cur,
            struct RETROTILE_TILE_DEF );
         assert( NULL != tile_def );
         assert( 0 == tile_def->tile_class );

#if RETROTILE_TRACE_LVL > 0
         #define RETROTILE_CLASS_TABLE_SET( A, a, i ) \
            } else if( 0 == strncmp( #a, token, maug_strlen( #a ) + 1 ) ) { \
               tile_def->tile_class = RETROTILE_CLASS_ ## A; \
               debug_printf( RETROTILE_TRACE_LVL, \
                  "set tile " SIZE_T_FMT " type: " #a " (%u)", \
                  parser->tileset_id_cur, tile_def->tile_class );
#else
         #define RETROTILE_CLASS_TABLE_SET( A, a, i ) \
            } else if( 0 == strncmp( #a, token, maug_strlen( #a ) + 1 ) ) { \
               tile_def->tile_class = RETROTILE_CLASS_ ## A;
#endif /* RETROTILE_TRACE_LVL */

         if( 0 ) {
         RETROTILE_CLASS_TABLE( RETROTILE_CLASS_TABLE_SET )
         } else {
            tile_def->tile_class = RETROTILE_CLASS_TILE;
#if RETROTILE_TRACE_LVL > 0
            debug_printf( RETROTILE_TRACE_LVL,
               "set tile " SIZE_T_FMT " type: tile (%u)",
               parser->tileset_id_cur, tile_def->tile_class );
#endif /* RETROTILE_TRACE_LVL */
         }
      }
      retrotile_parser_mstate( parser, MTILESTATE_TILES );

   } else if( MTILESTATE_TILES_PROP_NAME == parser->mstate ) {
#if RETROTILE_TRACE_LVL > 0
      debug_printf( RETROTILE_TRACE_LVL, "parsing property: %s", token );
#endif /* RETROTILE_TRACE_LVL */
      maug_mzero( parser->last_prop_name, RETROTILE_PROP_NAME_SZ_MAX + 1 );
      maug_strncpy( parser->last_prop_name, token, RETROTILE_PROP_NAME_SZ_MAX );
      retrotile_parser_mstate( parser, MTILESTATE_TILES_PROP );

   } else if( MTILESTATE_TILES_PROP_TYPE == parser->mstate ) {
#if RETROTILE_TRACE_LVL > 0
      debug_printf( RETROTILE_TRACE_LVL, "property %s is type: %s",
         parser->last_prop_name, token );
#endif /* RETROTILE_TRACE_LVL */
      parser->last_prop_type = retrotile_parse_prop_type( token, token_sz );
      retrotile_parser_mstate( parser, MTILESTATE_TILES_PROP );

   } else if( MTILESTATE_TILES_PROP_VAL == parser->mstate ) {
      /* This would be ideal to be handled in the custom token parser callback.
       */

      if( 1 == parser->pass ) {
         mdata_vector_lock( parser->p_tile_defs );
         tile_def = mdata_vector_get(
            parser->p_tile_defs, parser->tileset_id_cur,
            struct RETROTILE_TILE_DEF );
         assert( NULL != tile_def );

         if( 0 == strncmp( "warp_dest", parser->last_prop_name, 10 ) ) {
            maug_mzero( tile_def->warp_dest, RETROTILE_NAME_SZ_MAX );
            maug_strncpy( tile_def->warp_dest, token, RETROTILE_NAME_SZ_MAX );
#if RETROTILE_TRACE_LVL > 0
            debug_printf(
               RETROTILE_TRACE_LVL, "set tile " SIZE_T_FMT " warp_dest: %s",
               parser->tileset_id_cur, tile_def->warp_dest );
#endif /* RETROTILE_TRACE_LVL */

         } else if( 0 == strncmp( "warp_x", parser->last_prop_name, 7 ) ) {
            tile_def->warp_x = maug_atos32( token, token_sz );
#if RETROTILE_TRACE_LVL > 0
            debug_printf(
               RETROTILE_TRACE_LVL, "set tile " SIZE_T_FMT " warp_x: %d",
               parser->tileset_id_cur, tile_def->warp_x );
#endif /* RETROTILE_TRACE_LVL */

         } else if( 0 == strncmp( "warp_y", parser->last_prop_name, 7 ) ) {
            tile_def->warp_y = maug_atos32( token, token_sz );
#if RETROTILE_TRACE_LVL > 0
            debug_printf(
               RETROTILE_TRACE_LVL, "set tile " SIZE_T_FMT " warp_y: %d",
               parser->tileset_id_cur, tile_def->warp_y );
#endif /* RETROTILE_TRACE_LVL */

         }
      }

      maug_mzero( parser->last_prop_name, RETROTILE_PROP_NAME_SZ_MAX + 1 );
      retrotile_parser_mstate( parser, MTILESTATE_TILES_PROP );

   } else if( MTILESTATE_NAME == parser->mstate ) {
      maug_strncpy( parser->tileset_name, token, RETROTILE_NAME_SZ_MAX );
#if RETROTILE_TRACE_LVL > 0
      debug_printf(
         RETROTILE_TRACE_LVL, "tileset name: %s", parser->tileset_name );
#endif /* RETROTILE_TRACE_LVL */

      retrotile_parser_mstate( parser, 0 );

   } else if( MTILESTATE_TPROP_NAME == parser->mstate ) {
      maug_mzero( parser->last_prop_name, RETROTILE_PROP_NAME_SZ_MAX + 1 );
      maug_strncpy(
         parser->last_prop_name, token, RETROTILE_PROP_NAME_SZ_MAX );
      parser->last_prop_name_sz = token_sz;
      retrotile_parser_mstate( parser, MTILESTATE_TPROP );

   } else if( MTILESTATE_TPROP_TYPE == parser->mstate ) {
#if RETROTILE_TRACE_LVL > 0
      debug_printf( RETROTILE_TRACE_LVL, "property %s is type: %s",
         parser->last_prop_name, token );
#endif /* RETROTILE_TRACE_LVL */
      parser->last_prop_type = retrotile_parse_prop_type( token, token_sz );
      retrotile_parser_mstate( parser, MTILESTATE_TPROP );

   } else if( MTILESTATE_TPROP_VAL == parser->mstate ) {
      /* This should be handled in the custom_cb above! */
      maug_mzero( parser->last_prop_name, RETROTILE_PROP_NAME_SZ_MAX + 1 );
      retrotile_parser_mstate( parser, MTILESTATE_TPROP );
   }

cleanup:

   mdata_vector_unlock( parser->p_tile_defs );

   if( MERROR_PREEMPT == retval ) {
      /* Reset custom callback retval. */
      retval = MERROR_OK;
   }

   return retval;
}

/* === */

MERROR_RETVAL retrotile_parser_parse_token(
   const char* token, size_t token_sz, void* parser_arg
) {
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROTILE_LAYER* tiles_layer = NULL;
   struct RETROTILE_PARSER* parser = (struct RETROTILE_PARSER*)parser_arg;
   retroflat_tile_t* tiles = NULL;

   /* Try the custom parser. */
   if(
      NULL != parser->custom_token_cb &&
      /* MERROR_PREEMPT is dealt with in cleanup. */
      MERROR_OK != (retval = parser->custom_token_cb(
         token, token_sz, parser_arg ))
   ) {
      goto cleanup;
   }

   if( MJSON_PSTATE_LIST == mjson_parser_pstate( &(parser->jparser) ) ) {
      /* We're parsing a list. What lists do we care about? */

      /* Please note that this is for new tokens encountered inside of a list.
       * For dealing with the closing of a list, see
       * retrotile_json_close_list()!
       */

      if(
         1 == parser->pass &&
         MTILESTATE_LAYER_DATA == parser->mstate
      ) {
         /* This is a list of layer data... AKA tiles! */

         /*
         assert( NULL != *(parser->p_tilemap_h) );
         maug_mlock( *(parser->p_tilemap_h), tilemap );
         maug_cleanup_if_null_alloc( struct RETROTILE*, tilemap );
         */

         /*
         debug_printf( RETROTILE_TRACE_LVL,
            "selecting layer " SIZE_T_FMT "...",
            parser->pass_layer_iter );
         */
         assert( NULL != parser->t );
         tiles_layer = retrotile_get_layer_p(
            parser->t, parser->pass_layer_iter );
         assert( NULL != tiles_layer );

         /* Apply the class parsed previously. */
         tiles_layer->layer_class = parser->layer_class;

         tiles = retrotile_get_tiles_p( tiles_layer );

         /* Parse tilemap tile. */
         /*
         debug_printf( RETROTILE_TRACE_LVL,
            "layer " SIZE_T_FMT " tile: " SIZE_T_FMT " (tiles: %p)", 
            parser->pass_layer_iter, parser->layer_tile_iter,
            tiles );
         */
         assert( NULL != token );
         assert( NULL != parser );
         assert( NULL != tiles );

         if(
            parser->layer_tile_iter >=
               parser->t->tiles_w * parser->t->tiles_h
         ) {
            /* Tile is outside of layer! */
            error_printf(
               "tile " SIZE_T_FMT " outside of layer tile buffer size "
                  SIZE_T_FMT "!",
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

      /* Pay attention to the retrotile_parser_mstate() calls here. We don't
       * have a stack of states, and we really only track a few we're
       * interested in. Even if we don't process their data here, we still
       * use retrotile_parser_mstate() to move the parser mstate back up
       * to the parent mstate when we're done with it. This kind of "fakes" a
       * stack (in a well-formed file, of course).
       */

      if( MTILESTATE_TILESETS_FGID == parser->mstate ) {
         if( 1 == parser->pass ) {
            parser->t->tileset_fgid = maug_atou32( token, token_sz, 10 );
#if RETROTILE_TRACE_LVL > 0
            debug_printf(
               RETROTILE_TRACE_LVL, "tileset FGID set to: " SIZE_T_FMT,
               parser->t->tileset_fgid );
#endif /* RETROTILE_TRACE_LVL */
         }
         retrotile_parser_mstate( parser, MTILESTATE_TILESETS );

      } else if( MTILESTATE_TILESETS_SRC == parser->mstate ) {
         if( 1 == parser->pass ) {
#if RETROTILE_TRACE_LVL > 0
            debug_printf( RETROTILE_TRACE_LVL, "parsing %s...", token );
#endif /* RETROTILE_TRACE_LVL */
            parser->tj_parse_cb(
               parser->dirname, token, NULL, parser->p_tile_defs,
               parser->wait_cb, parser->wait_data,
               parser->custom_token_cb, parser->custom_token_cb_data,
               parser->passes_max, parser->flags );
         }
         retrotile_parser_mstate( parser, MTILESTATE_TILESETS );

      } else if( MTILESTATE_HEIGHT == parser->mstate ) {
         if( 0 == parser->pass ) {
            /* Need this to allocate on pass 1. */
            parser->tiles_h = atoi( token );
#if RETROTILE_TRACE_LVL > 0
            debug_printf(
               RETROTILE_TRACE_LVL, "tilemap height: " SIZE_T_FMT,
               parser->tiles_h );
#endif /* RETROTILE_TRACE_LVL */
         }
         retrotile_parser_mstate( parser, MTILESTATE_NONE );

      } else if( MTILESTATE_WIDTH == parser->mstate ) {
         if( 0 == parser->pass ) {
            /* Need this to allocate on pass 1. */
            parser->tiles_w = atoi( token );
#if RETROTILE_TRACE_LVL > 0
            debug_printf(
               RETROTILE_TRACE_LVL, "tilemap width: " SIZE_T_FMT,
               parser->tiles_w );
#endif /* RETROTILE_TRACE_LVL */
         }
         retrotile_parser_mstate( parser, MTILESTATE_NONE );

      } else if( MTILESTATE_LAYER_NAME == parser->mstate ) {
         /* TODO: Store */
         retrotile_parser_mstate( parser, MTILESTATE_LAYER );

      } else if( MTILESTATE_LAYER_CLASS == parser->mstate ) {
         /* TODO: Use the class table to create layers for e.g. crops, items. */
         if( 0 == strncmp( "mobile", token, 7 ) ) {
#if RETROTILE_TRACE_LVL > 0
            debug_printf( RETROTILE_TRACE_LVL,
               "layer " SIZE_T_FMT " type: mobile",
               parser->pass_layer_iter );
#endif /* RETROTILE_TRACE_LVL */
            parser->layer_class = RETROTILE_CLASS_MOBILE;
         } else {
#if RETROTILE_TRACE_LVL > 0
            debug_printf( RETROTILE_TRACE_LVL,
               "layer " SIZE_T_FMT " type: tile",
               parser->pass_layer_iter );
#endif /* RETROTILE_TRACE_LVL */
            parser->layer_class = RETROTILE_CLASS_TILE;
         }
         retrotile_parser_mstate( parser, MTILESTATE_LAYER );
         
      } else if( MTILESTATE_PROP_NAME == parser->mstate ) {
         maug_mzero( parser->last_prop_name, RETROTILE_PROP_NAME_SZ_MAX + 1 );
         maug_strncpy(
            parser->last_prop_name, token, RETROTILE_PROP_NAME_SZ_MAX );
         parser->last_prop_name_sz = token_sz;
         retrotile_parser_mstate( parser, MTILESTATE_PROP );

      } else if( MTILESTATE_PROP_TYPE == parser->mstate ) {
#if RETROTILE_TRACE_LVL > 0
         debug_printf( RETROTILE_TRACE_LVL, "property %s is type: %s",
            parser->last_prop_name, token );
#endif /* RETROTILE_TRACE_LVL */
         parser->last_prop_type = retrotile_parse_prop_type( token, token_sz );
         retrotile_parser_mstate( parser, MTILESTATE_PROP );

      } else if( MTILESTATE_PROP_VAL == parser->mstate ) {
         /* We're dealing with properties of the tilemap. */
         if( 0 == strncmp( parser->last_prop_name, "name", 5 ) ) {
#if RETROTILE_TRACE_LVL > 0
            debug_printf( RETROTILE_TRACE_LVL, "tilemap name: %s", token );
#endif /* RETROTILE_TRACE_LVL */
            maug_strncpy( parser->tilemap_name, token, RETROTILE_NAME_SZ_MAX );
         }

         maug_mzero( parser->last_prop_name, RETROTILE_PROP_NAME_SZ_MAX + 1 );
         retrotile_parser_mstate( parser, MTILESTATE_PROP );
      }
      goto cleanup;
   }

   retrotile_parser_match_token( token, token_sz, parser );

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retrotile_json_close_list( void* parg ) {
   struct RETROTILE_PARSER* parser = (struct RETROTILE_PARSER*)parg;

   if( MTILESTATE_LAYER_DATA == parser->mstate ) {
      assert( RETROTILE_PARSER_MODE_MAP == parser->mode );
#if RETROTILE_TRACE_LVL > 0
      debug_printf( RETROTILE_TRACE_LVL, "parsed " SIZE_T_FMT " tiles!",
         parser->layer_tile_iter );
#endif /* RETROTILE_TRACE_LVL */
      assert( parser->layer_tile_iter > 0 );
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

   } else if( MTILESTATE_TILES == parser->mstate ) {
      assert( RETROTILE_PARSER_MODE_DEFS == parser->mode );
      retrotile_parser_mstate( parser, MTILESTATE_NONE );

   } else if( MTILESTATE_PROP == parser->mstate ) {
      retrotile_parser_mstate( parser, MTILESTATE_NONE );

   } else if( MTILESTATE_TPROP == parser->mstate ) {
      retrotile_parser_mstate( parser, MTILESTATE_NONE );
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
#if RETROTILE_TRACE_LVL > 0
      debug_printf( RETROTILE_TRACE_LVL,
         "incrementing pass layer to " SIZE_T_FMT " after " SIZE_T_FMT
            " tiles...",
         parser->pass_layer_iter + 1, parser->layer_tile_iter );
#endif /* RETROTILE_TRACE_LVL */
      parser->pass_layer_iter++;
      retrotile_parser_mstate( parser, MTILESTATE_LAYERS );

   } else if( MTILESTATE_GRID == parser->mstate ) {
      retrotile_parser_mstate( parser, MTILESTATE_NONE );
   }

   return MERROR_OK;
}

/* === */

int retrotile_parse_prop_type( const char* token, size_t token_sz ) {
   int out = RETROTILE_PROP_TYPE_OTHER;

   if( 0 == strncmp( "string", token, 7 ) ) {
      out = RETROTILE_PROP_TYPE_STRING;
   } else if( 0 == strncmp( "file", token, 5 ) ) {
      out = RETROTILE_PROP_TYPE_FILE;
   } else if( 0 == strncmp( "int", token, 4 ) ) {
      out = RETROTILE_PROP_TYPE_INT;
   }

   return out;
}

/* === */

mfix_t retrotile_static_rotation_from_dir( const char* dir ) {
   mfix_t static_rotate_out = 0;

   if( NULL == dir ) {
      return 0;
   }

   /* Translate dir into rotation value. */
   if( 0 == strncmp( dir, "NW", 2 ) ) {
      static_rotate_out = mfix_from_f( 90.0f );
   } else if( 0 == strncmp( dir, "SW", 2 ) ) {
      static_rotate_out = mfix_from_f( 180.0f ); 
   } else if( 0 == strncmp( dir, "SE", 2 ) ) {
      static_rotate_out = mfix_from_f( 270.0f );

   } else if( 0 == strncmp( dir, "W", 1 ) ) {
      static_rotate_out = mfix_from_f( 90.0f );
   } else if( 0 == strncmp( dir, "S", 1 ) ) {
      static_rotate_out = mfix_from_f( 180.0f );
   } else if( 0 == strncmp( dir, "E", 1 ) ) {
      static_rotate_out = mfix_from_f( 270.0f );

   } else {
      static_rotate_out = 0;
   }

   return static_rotate_out;
}

/* === */

MERROR_RETVAL retrotile_parse_json_file(
   const char* dirname, const char* filename, MAUG_MHANDLE* p_tilemap_h,
   struct MDATA_VECTOR* p_tile_defs, mparser_wait_cb_t wait_cb, void* wait_data,
   mparser_parse_token_cb token_cb, void* token_cb_data, uint8_t passes,
   uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   MAUG_MHANDLE parser_h = (MAUG_MHANDLE)NULL;
   struct RETROTILE_PARSER* parser = NULL;
   char filename_path[MAUG_PATH_SZ_MAX];
   mfile_t tile_file;
   char c;
   char* filename_ext = NULL;

   /* Initialize parser. */
   parser_h = maug_malloc( 1, sizeof( struct RETROTILE_PARSER ) );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, parser_h );

   maug_mlock( parser_h, parser );
   maug_cleanup_if_null_alloc( struct RETROTILE_PARSER*, parser );
   maug_mzero( parser, sizeof( struct RETROTILE_PARSER ) );

   parser->flags = flags;
   parser->tj_parse_cb = retrotile_parse_json_file;
   parser->custom_token_cb = token_cb;
   parser->custom_token_cb_data = token_cb_data;

   maug_strncpy( parser->dirname, dirname, MAUG_PATH_SZ_MAX );

   if( 2 > passes ) {
#if RETROTILE_TRACE_LVL > 0
      debug_printf( RETROTILE_TRACE_LVL,
         "increasing parse passes to minimum, 2!" );
#endif /* RETROTILE_TRACE_LVL */
      passes = 2;
   }

   parser->passes_max = passes;

   /* Setup filename path. */
   maug_mzero( filename_path, MAUG_PATH_SZ_MAX );
   /* TODO: Configurable path. */
   maug_snprintf(
      filename_path, MAUG_PATH_SZ_MAX, "%s/%s", dirname, filename );

#if RETROTILE_TRACE_LVL > 0
   debug_printf( RETROTILE_TRACE_LVL, "opening %s...", filename_path );
#endif /* RETROTILE_TRACE_LVL */

   retval = mfile_open_read( filename_path, &tile_file );
   maug_cleanup_if_not_ok();

   /* Parse JSON and react to state. */
   for( parser->pass = 0 ; passes > parser->pass ; parser->pass++ ) {
#if RETROTILE_TRACE_LVL > 0
      debug_printf( RETROTILE_TRACE_LVL, "beginning pass #%u...",
         parser->pass );
#endif /* RETROTILE_TRACE_LVL */

      /* Reset tilemap parser. */
      parser->mstate = 0;

      /* Reset JSON parser. */
      maug_mzero( &(parser->jparser.base), sizeof( struct MJSON_PARSER ) );

      parser->wait_cb = wait_cb;
      parser->wait_data = wait_data;
      parser->jparser.base.wait_cb = wait_cb;
      parser->jparser.base.wait_data = wait_data;

      /* Figure out if we're parsing a .tmj or .tsj. */
      filename_ext = maug_strrchr( filename, '.' );
      if( NULL == filename_ext ) {
         error_printf( "could not parse filename extension!" );
         retval = MERROR_FILE;
         goto cleanup;
      }
      if( 's' == filename_ext[2] ) {
#if RETROTILE_TRACE_LVL > 0
         debug_printf( RETROTILE_TRACE_LVL,
            "(tile_defs pass %u)", parser->pass );
#endif /* RETROTILE_TRACE_LVL */
         parser->mode = RETROTILE_PARSER_MODE_DEFS;
         parser->jparser.token_parser = retrotile_parser_parse_tiledef_token;
         parser->jparser.token_parser_arg = parser;
         parser->jparser.close_list = retrotile_json_close_list;
         parser->jparser.close_list_arg = parser;
         parser->jparser.close_obj = retrotile_json_close_obj;
         parser->jparser.close_obj_arg = parser;
         /*
         parser->jparser.base.close_val = retrotile_json_close_val;
         parser->jparser.base.close_val_arg = parser;
         */
         parser->p_tile_defs = p_tile_defs;

         assert( NULL != p_tile_defs );
         if( 1 == parser->pass ) {
            /* Allocate tile defs based on highest tile ID found on
             * first pass.
             */
            assert( 0 < parser->tileset_id_cur );
#if RETROTILE_TRACE_LVL > 0
            debug_printf(
               RETROTILE_TRACE_LVL, "allocating " SIZE_T_FMT " tile defs...",
               parser->tileset_id_cur + 1 );
#endif /* RETROTILE_TRACE_LVL */
            mdata_vector_fill(
               parser->p_tile_defs, parser->tileset_id_cur + 1,
               sizeof( struct RETROTILE_TILE_DEF ) );
         }
      } else {
#if RETROTILE_TRACE_LVL > 0
         debug_printf( RETROTILE_TRACE_LVL, "(tilemap pass %u)", parser->pass );
#endif /* RETROTILE_TRACE_LVL */
         parser->mode = RETROTILE_PARSER_MODE_MAP;

         parser->jparser.close_list = retrotile_json_close_list;
         parser->jparser.close_list_arg = parser;
         parser->jparser.open_obj = retrotile_json_open_obj;
         parser->jparser.open_obj_arg = parser;
         parser->jparser.close_obj = retrotile_json_close_obj;
         parser->jparser.close_obj_arg = parser;
         parser->jparser.token_parser = retrotile_parser_parse_token;
         parser->jparser.token_parser_arg = parser;
         parser->p_tile_defs = p_tile_defs;

         assert( NULL != p_tilemap_h );
         assert( NULL != p_tile_defs );
         if( 1 == parser->pass ) {
            /* Allocate tiles for the new layers. */
            retval = retrotile_alloc(
               p_tilemap_h, parser->tiles_w, parser->tiles_h,
               parser->pass_layer_iter, parser->tilemap_name,
               parser->tileset_name );
            maug_cleanup_if_not_ok();
            maug_mlock( *p_tilemap_h, parser->t );
         }
         parser->pass_layer_iter = 0;
      }

      while( tile_file.has_bytes( &tile_file ) ) {
         tile_file.read_int( &tile_file, (uint8_t*)&c, 1, 0 );
#if RETROTILE_TRACE_CHARS > 0
         debug_printf( RETROTILE_TRACE_CHARS, "%c", c );
#endif /* RETROTILE_TRACE_CHARS */
         retval = mjson_parse_c( &(parser->jparser), c );
         if( MERROR_OK != retval ) {
            error_printf( "error parsing JSON!" );
            goto cleanup;
         }
      }

      tile_file.seek( &tile_file, 0 );

      filename_ext = maug_strrchr( filename, '.' );
      if( NULL == filename_ext ) {
         error_printf( "could not parse filename extension!" );
         retval = MERROR_FILE;
         goto cleanup;
      }
      if( 's' != filename_ext[2] ) {
#if RETROTILE_TRACE_LVL > 0
         debug_printf( RETROTILE_TRACE_LVL,
            "pass %u found " SIZE_T_FMT " layers",
            parser->pass, parser->pass_layer_iter );
#endif /* RETROTILE_TRACE_LVL */
      }
   }

#if RETROTILE_TRACE_LVL > 0
   debug_printf(
      RETROTILE_TRACE_LVL, "finished parsing %s, retval: %d",
      filename_path, retval );
#endif /* RETROTILE_TRACE_LVL */

cleanup:

   if( NULL != parser ) {
      if( NULL != parser->t ) {
         maug_munlock( *p_tilemap_h, parser->t );
      }
      maug_munlock( parser_h, parser );
   }

   if( (MAUG_MHANDLE)NULL != parser_h ) {
      maug_mfree( parser_h );
   }

   return retval;
}

/* === */

static retroflat_tile_t retrotile_gen_diamond_square_rand(
   retroflat_tile_t min_z, retroflat_tile_t max_z, uint32_t tuning,
   retroflat_tile_t top_left_z
) {
   retroflat_tile_t avg = top_left_z;

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
   retroflat_tile_t min_z, retroflat_tile_t max_z,
   uint32_t tuning, struct RETROTILE_DATA_DS* data_ds,
   struct RETROTILE_LAYER* layer, struct RETROTILE* t
) {
   int16_t iter_x = 0,
      iter_y = 0;
   retroflat_tile_t* tile_iter = NULL;
   retroflat_tile_t top_left_z = 0;

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
#if RETROTILE_TRACE_LVL > 0
            debug_printf(
               RETROTILE_TRACE_LVL, "corner coord %d x %d present: %d",
               corners_x[iter_x][iter_y], corners_y[iter_x][iter_y],
               retrotile_get_tile(
                  t, layer,
                  corners_x[iter_x][iter_y],
                  corners_y[iter_x][iter_y] ) );
#endif /* RETROTILE_TRACE_LVL */
            continue;
         }

         /* Generate a new value for this corner. */
         *tile_iter = retrotile_gen_diamond_square_rand(
            min_z, max_z, tuning, top_left_z );

#if RETROTILE_TRACE_LVL > 0
         debug_printf( RETROTILE_TRACE_LVL,
            "missing corner coord %d x %d: %d",
            corners_x[iter_x][iter_y], corners_y[iter_x][iter_y],
            *tile_iter );
#endif /* RETROTILE_TRACE_LVL */
      }
   }
}

/* === */

static retroflat_tile_t retrotile_gen_diamond_square_avg(
   int16_t corners_x[2][2], int16_t corners_y[2][2],
   struct RETROTILE* t, struct RETROTILE_LAYER* layer
) {
   retroflat_tile_t* tile_iter = NULL;
   int16_t iter_x = 0,
      iter_y = 0;
   retroflat_tile_t avg = 0;

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
   struct RETROTILE* t, retroflat_tile_t min_z, retroflat_tile_t max_z,
   uint32_t tuning, size_t layer_idx, uint8_t flags, void* data,
   retrotile_ani_cb animation_cb, void* animation_cb_data
) {
   int16_t iter_x = 0,
      iter_y = 0
#if RETROTILE_TRACE_LVL > 0
      , iter_depth = 0;
#else
      ;
#endif /* RETROTILE_TRACE_LVL */
   int16_t corners_x[2][2];
   int16_t corners_y[2][2];
   int32_t avg = 0;
   /* size_t tile_idx = 0; */
   struct RETROTILE_DATA_DS data_ds_sub;
   MAUG_MHANDLE data_ds_h = (MAUG_MHANDLE)NULL;
   struct RETROTILE_DATA_DS* data_ds = NULL;
   /* retroflat_tile_t* tiles = NULL; */
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROTILE_LAYER* layer = NULL;
   retroflat_tile_t* tile_iter = NULL;
   uint8_t free_ds_data = 0;
#ifdef MAUG_NO_STDLIB
   size_t i = 0;
#endif /* MAUG_NO_STDLIB */

   /*
   maug_mlock( t->tiles, tiles );
   maug_cleanup_if_null_alloc( struct GRIDCITY_TILE*, tiles );
   */

   #define _retrotile_ds_update_statistics( data, tile ) \
      /* Gather statistics. */ \
      if( (data)->highest_generated < (tile) && 32767 > (tile) ) { \
         (data)->highest_generated = (tile); \
      } \
      if( (data)->lowest_generated > (tile) && 0 < (tile) ) { \
         (data)->lowest_generated = (tile); \
      }

   layer = retrotile_get_layer_p( t, layer_idx );

   if(
      NULL == data ||
      RETROTILE_DS_FLAG_INIT_DATA == (RETROTILE_DS_FLAG_INIT_DATA & flags)
   ) {
      /* This must be the first call, so initialize or allocate a new struct.
       */
      if( NULL == data ) {
         /* An internal struct needs to be allocated before initialization. */
         data_ds_h = maug_malloc( 1, sizeof( struct RETROTILE_DATA_DS ) );
         maug_cleanup_if_null_alloc( MAUG_MHANDLE, data_ds_h );
         free_ds_data = 1;
         maug_mlock( data_ds_h, data_ds );
         maug_cleanup_if_null_alloc( struct RETROTILE_DATA_DS*, data_ds );
      } else {
         data_ds = (struct RETROTILE_DATA_DS*)data;
      }

      /* Initialize passed tilemap while we're handling first call stuff. */
      retrotile_clear_tiles( t, layer, i );

      /* Initialize DS struct from tilemap properties. */
      maug_mzero( data_ds, sizeof( struct RETROTILE_DATA_DS ) );
      data_ds->sect_w = t->tiles_w;
      data_ds->sect_h = t->tiles_h;
      data_ds->sect_w_half = data_ds->sect_w >> 1;
      data_ds->sect_h_half = data_ds->sect_h >> 1;
      data_ds->lowest_generated = 32767;

      /* Disable this flag for subsequent calls. */
      flags &= ~RETROTILE_DS_FLAG_INIT_DATA;
   } else {
      data_ds = (struct RETROTILE_DATA_DS*)data;
   }
   assert( NULL != data_ds );

   /* Trivial case; end recursion. */
   if( 0 == data_ds->sect_w ) {
#if RETROTILE_TRACE_LVL > 0
      debug_printf(
         RETROTILE_TRACE_LVL, "%d return: null sector", iter_depth );
#endif /* RETROTILE_TRACE_LVL */
      goto cleanup;
   }

   if(
      data_ds->sect_x + data_ds->sect_w > t->tiles_w ||
      data_ds->sect_y + data_ds->sect_h > t->tiles_h
   ) {
#if RETROTILE_TRACE_LVL > 0
      debug_printf(
         RETROTILE_TRACE_LVL, "%d return: overflow sector", iter_depth );
#endif /* RETROTILE_TRACE_LVL */
      goto cleanup;
   }

#if RETROTILE_TRACE_LVL > 0
   iter_depth = t->tiles_w / data_ds->sect_w;
#endif /* RETROTILE_TRACE_LVL */

   /* Generate/grab corners before averaging them! */
   retrotile_gen_diamond_square_corners(
      corners_x, corners_y, min_z, max_z, tuning, data_ds, layer, t );

   if( 2 == data_ds->sect_w || 2 == data_ds->sect_h ) {
      /* Nothing to average, this sector is just corners! */
#if RETROTILE_TRACE_LVL > 0
      debug_printf(
         RETROTILE_TRACE_LVL,
         "%d return: reached innermost point", iter_depth );
#endif /* RETROTILE_TRACE_LVL */
      goto cleanup; /* Skip further descent regardless. */
   }
   
   avg = 
      retrotile_gen_diamond_square_avg( corners_x, corners_y, t, layer );

#if RETROTILE_TRACE_LVL > 0
   debug_printf( RETROTILE_TRACE_LVL, "avg: " S32_FMT, avg );
#endif /* RETROTILE_TRACE_LVL */

   tile_iter = &(retrotile_get_tile(
      t, layer,
      data_ds->sect_x + data_ds->sect_w_half,
      data_ds->sect_y + data_ds->sect_h_half ));
   if( -1 != *tile_iter ) {
#if RETROTILE_TRACE_LVL > 0
      debug_printf( RETROTILE_TRACE_LVL, "avg already present at %d x %d!",
         data_ds->sect_x + data_ds->sect_w_half,
         data_ds->sect_y + data_ds->sect_h_half );
#endif /* RETROTILE_TRACE_LVL */
      goto cleanup;
   }
   *tile_iter = avg;
   _retrotile_ds_update_statistics( data_ds, avg );

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
         data_ds_sub.lowest_generated = 32767;
         data_ds_sub.highest_generated = 0;

#if RETROTILE_TRACE_LVL > 0
         debug_printf(
            RETROTILE_TRACE_LVL, "%d: child sector at %d x %d, %d wide",
            iter_depth,
            data_ds_sub.sect_x, data_ds_sub.sect_y, data_ds_sub.sect_w );
#endif /* RETROTILE_TRACE_LVL */

         retval = retrotile_gen_diamond_square_iter(
            t, min_z, max_z, tuning, layer_idx, flags, &data_ds_sub,
            animation_cb, animation_cb_data );
         maug_cleanup_if_not_ok();

         _retrotile_ds_update_statistics(
            data_ds, data_ds_sub.highest_generated );
         _retrotile_ds_update_statistics(
            data_ds, data_ds_sub.lowest_generated );
      }
   }

   if(
      data_ds->sect_w == (t->tiles_w >> 1) &&
      NULL != animation_cb
   ) {
      retval = animation_cb( animation_cb_data, iter_y );
      maug_cleanup_if_not_ok();
   }

#if RETROTILE_TRACE_LVL > 0
   debug_printf(
      RETROTILE_TRACE_LVL, "%d return: all sectors complete", iter_depth );
#endif /* RETROTILE_TRACE_LVL */

cleanup:

   if( free_ds_data && NULL != data_ds ) {
      maug_munlock( data_ds_h, data_ds );
   }

   if( free_ds_data && (MAUG_MHANDLE)NULL != data_ds_h ) {
      maug_mfree( data_ds_h );
   }

   return retval;
}

/* === */

MERROR_RETVAL retrotile_gen_voronoi_iter(
   struct RETROTILE* t, retroflat_tile_t min_z, retroflat_tile_t max_z,
   uint32_t tuning, size_t layer_idx, uint8_t flags, void* data,
   retrotile_ani_cb animation_cb, void* animation_cb_data
) {
   size_t x = 0,
      y = 0;
   int16_t offset_x = 0,
      offset_y = 0,
      finished = 0;
   MERROR_RETVAL retval = MERROR_OK;
   struct RETROTILE_LAYER* layer = NULL;
   int16_t spb = RETROTILE_VORONOI_DEFAULT_SPB;
   int16_t drift = RETROTILE_VORONOI_DEFAULT_DRIFT;
   MAUG_MHANDLE temp_grid_h = (MAUG_MHANDLE)NULL;
   retroflat_tile_t* temp_grid = NULL;
   retroflat_tile_t* tiles = NULL;
   /* Only use 4 cardinal directions. */
   int8_t side_iter = 0;
#ifdef MAUG_NO_STDLIB
   size_t i = 0;
#endif /* MAUG_NO_STDLIB */

   layer = retrotile_get_layer_p( t, 0 );

   tiles = retrotile_get_tiles_p( layer );

   /* Initialize grid to empty. */
   retrotile_clear_tiles( t, layer, i );

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
      sizeof( retroflat_tile_t ), t->tiles_w * t->tiles_h );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, temp_grid_h );

   maug_mlock( temp_grid_h, temp_grid );
   maug_cleanup_if_null_alloc( retroflat_tile_t*, temp_grid );

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
         sizeof( retroflat_tile_t ) * t->tiles_w * t->tiles_h );

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
#if RETROTILE_TRACE_LVL > 0
               debug_printf( RETROTILE_TRACE_LVL,
                  SIZE_T_FMT " (%d), " SIZE_T_FMT " (%d) ("
                  SIZE_T_FMT ", " SIZE_T_FMT ")", 
                  x,
                  gc_retroflat_offsets4_x[side_iter],
                  y,
                  gc_retroflat_offsets4_y[side_iter],
                  t->tiles_w, t->tiles_h );
#endif /* RETROTILE_TRACE_LVL */
            
               /* Iterate through directions to expand. */
               /* TODO: Add tuning to select directional probability. */
               if(
                  t->tiles_w > x + gc_retroflat_offsets4_x[side_iter] &&
                  t->tiles_h > y + gc_retroflat_offsets4_y[side_iter] &&
                  -1 == temp_grid[
                     ((y + gc_retroflat_offsets4_y[side_iter]) *
                        t->tiles_w) +
                           (x + gc_retroflat_offsets4_x[side_iter])]
               ) {
                  /* Copy center tile to this direction. */
                  retrotile_get_tile( t, layer,
                     x + gc_retroflat_offsets4_x[side_iter],
                     y + gc_retroflat_offsets4_y[side_iter] ) =
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

   if( (MAUG_MHANDLE)NULL != temp_grid_h ) {
      maug_mfree( temp_grid_h );
   }

   return retval;
}

/* === */

MERROR_RETVAL retrotile_gen_smooth_iter(
   struct RETROTILE* t, retroflat_tile_t min_z, retroflat_tile_t max_z,
   uint32_t tuning, size_t layer_idx, uint8_t flags, void* data,
   retrotile_ani_cb animation_cb, void* animation_cb_data
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t x = 0,
      y = 0;
   int16_t side_iter = 0,
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
               t->tiles_w <= x + gc_retroflat_offsets8_x[side_iter] ||
               t->tiles_h <= y + gc_retroflat_offsets8_y[side_iter]
            ) {
               continue;
            }

            sides_avail++;
#if RETROTILE_TRACE_LVL > 0
            debug_printf(
               RETROTILE_TRACE_LVL,
               "si %d: x, y: " SIZE_T_FMT " (+%d), " SIZE_T_FMT
                  " (+%d) idx: " SIZE_T_FMT,
               side_iter,
               x + gc_retroflat_offsets8_x[side_iter],
               gc_retroflat_offsets8_x[side_iter],
               y + gc_retroflat_offsets8_y[side_iter],
               gc_retroflat_offsets8_y[side_iter],
               ((y + gc_retroflat_offsets8_y[side_iter]) * t->tiles_w) +
                  x + gc_retroflat_offsets8_x[side_iter] );
#endif /* RETROTILE_TRACE_LVL */
            sides_sum += retrotile_get_tile(
               t, layer,
               x + gc_retroflat_offsets8_x[side_iter],
               y + gc_retroflat_offsets8_y[side_iter] );
         }

         retrotile_get_tile( t, layer, x, y ) = sides_sum / sides_avail;
      }
   }

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retrotile_gen_borders_iter(
   struct RETROTILE* t, retroflat_tile_t min_z, retroflat_tile_t max_z,
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

#if RETROTILE_TRACE_LVL > 0
   debug_printf( RETROTILE_TRACE_LVL, "adding borders..." );
#endif /* RETROTILE_TRACE_LVL */

   for( y = 0 ; t->tiles_h > y ; y++ ) {
      for( x = 0 ; t->tiles_w > x ; x++ ) {
         i = 0;
         while( 0 <= borders[i].center ) {
            /* Compare/grab current center tile. */
            ctr_iter = retrotile_get_tile( t, layer, x, y );
#if RETROTILE_TRACE_LVL > 0
            debug_printf( RETROTILE_TRACE_LVL,
               "x: " SIZE_T_FMT ", y: " SIZE_T_FMT ", 0x%04x vs 0x%04x",
               x, y, ctr_iter, borders[i].center );
#endif /* RETROTILE_TRACE_LVL */
            if( ctr_iter != borders[i].center ) {
               i++;
               continue;
            }

#if RETROTILE_TRACE_LVL > 0
            debug_printf( RETROTILE_TRACE_LVL, "comparing sides..." );
#endif /* RETROTILE_TRACE_LVL */

            /* Zeroth pass: look for stick-outs. */
            for( side = 0 ; 8 > side ; side += 2 ) {
               if(
                  x + gc_retroflat_offsets8_x[side] > t->tiles_w ||
                  y + gc_retroflat_offsets8_y[side] > t->tiles_h
               ) {
                  /* Skip out-of-bounds. */
                  continue;
               }
               /* Get the outside tile on this side. */
               outside_iter = retrotile_get_tile( t, layer,
                  x + gc_retroflat_offsets8_x[side],
                  y + gc_retroflat_offsets8_y[side] );

               /* Get the outside tile next two clock-steps from this one.
                */
               if( side + 4 < 8 ) {
                  x_plus_1 = x + gc_retroflat_offsets8_x[side + 4];
                  y_plus_1 = y + gc_retroflat_offsets8_y[side + 4];
               } else {
                  x_plus_1 = x + gc_retroflat_offsets8_x[side - 4];
                  y_plus_1 = y + gc_retroflat_offsets8_y[side - 4];
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
                  x + gc_retroflat_offsets8_x[side] > t->tiles_w ||
                  y + gc_retroflat_offsets8_y[side] > t->tiles_h
               ) {
                  /* Skip out-of-bounds. */
                  continue;
               }
               /* Get the outside tile on this side. */
               outside_iter = retrotile_get_tile( t, layer,
                  x + gc_retroflat_offsets8_x[side],
                  y + gc_retroflat_offsets8_y[side] );

               /* Get the outside tile next two clock-steps from this one.
                */
               if( side + 2 < 8 ) {
                  x_plus_1 = x + gc_retroflat_offsets8_x[side + 2];
                  y_plus_1 = y + gc_retroflat_offsets8_y[side + 2];
               } else {
                  x_plus_1 = x + gc_retroflat_offsets8_x[0];
                  y_plus_1 = y + gc_retroflat_offsets8_y[0];
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
                  x + gc_retroflat_offsets8_x[side] > t->tiles_w ||
                  y + gc_retroflat_offsets8_y[side] > t->tiles_h
               ) {
                  /* Skip out-of-bounds. */
                  continue;
               }
               /* Get the outside tile on this side. */
               outside_iter = retrotile_get_tile( t, layer,
                  x + gc_retroflat_offsets8_x[side],
                  y + gc_retroflat_offsets8_y[side] );

               if( outside_iter == borders[i].outside ) {
                  /* It only matches on this side. */
#if RETROTILE_TRACE_LVL > 0
                  debug_printf( RETROTILE_TRACE_LVL, "replacing..." );
#endif /* RETROTILE_TRACE_LVL */
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
      error_printf( "invalid layer " U32_FMT
         " requested (of " U32_FMT ")!",
         layer_idx, tilemap->layers_count );
      return NULL;
   }

   /* Advance to first grid. */
   tilemap_buf += sizeof( struct RETROTILE );
   layer_iter = (struct RETROTILE_LAYER*)tilemap_buf;
   while( layer_idx > 0 ) {
      tilemap_buf += layer_iter->total_sz;
      layer_iter = (struct RETROTILE_LAYER*)tilemap_buf;
      layer_idx--;
   }

   return layer_iter;
}

/* === */

MERROR_RETVAL retrotile_alloc(
   MAUG_MHANDLE* p_tilemap_h, size_t w, size_t h, size_t layers_count,
   const char* tilemap_name, const char* tileset_name
) {
   struct RETROTILE_LAYER* layer_iter = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   size_t tilemap_sz = 0;
   struct RETROTILE* tilemap = NULL;
   size_t i = 0;

   tilemap_sz = sizeof( struct RETROTILE ) +
      (layers_count * sizeof( struct RETROTILE_LAYER )) +
      (layers_count * (w * h * sizeof( retroflat_tile_t ) ));

#if RETROTILE_TRACE_LVL > 0
   debug_printf(
      RETROTILE_TRACE_LVL, "allocating new tilemap " SIZE_T_FMT "x" SIZE_T_FMT
      " tiles, " SIZE_T_FMT " layers (" SIZE_T_FMT " bytes)...",
      w, h, layers_count, tilemap_sz );
#endif /* RETROTILE_TRACE_LVL */

   *p_tilemap_h = maug_malloc( 1, tilemap_sz );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, *p_tilemap_h );

   maug_mlock( *p_tilemap_h, tilemap );
   maug_cleanup_if_null_alloc( struct RETROTILE*, tilemap );

   maug_mzero( tilemap, tilemap_sz );
   tilemap->total_sz = tilemap_sz;
   tilemap->layers_count = layers_count;
   tilemap->tiles_w = w;
   tilemap->tiles_h = h;
   tilemap->tile_scale = RETROTILE_TILE_SCALE_DEFAULT;
   tilemap->sz = sizeof( struct RETROTILE );

   maug_strncpy( tilemap->name, tilemap_name, RETROTILE_NAME_SZ_MAX );

   maug_strncpy( tilemap->tileset, tileset_name, RETROTILE_NAME_SZ_MAX );

   for( i = 0 ; layers_count > i ; i++ ) {
      layer_iter = retrotile_get_layer_p( tilemap, i );
      assert( NULL != layer_iter );
      layer_iter->total_sz = sizeof( struct RETROTILE_LAYER ) +
         (w * h * sizeof( retroflat_tile_t ));
      maug_cleanup_if_not_ok();
      layer_iter->sz = sizeof( struct RETROTILE_LAYER );
   }

cleanup:

   if( NULL != tilemap ) {
      maug_munlock( *p_tilemap_h, tilemap );
   }

   return retval;
}

/* === */

void retrotile_format_asset_path(
   retroflat_asset_path path_out, const char* afile,
   struct RETROTILE_PARSER* parser
) {
   /* Load the portrait. */
   maug_mzero( path_out, MAUG_PATH_SZ_MAX + 1 );
   maug_snprintf( path_out, MAUG_PATH_SZ_MAX, "%s/%s",
      parser->dirname, afile );
}

/* === */

MERROR_RETVAL retrotile_clear_refresh( retroflat_pxxy_t y_max ) {
   MERROR_RETVAL retval = MERROR_OK;
#ifndef RETROFLAT_NO_VIEWPORT_REFRESH
   int16_t x = 0,
      y = 0;

#if RETROTILE_TRACE_LVL > 0
   debug_printf( RETROTILE_TRACE_LVL,
      "clearing " SIZE_T_FMT " vertical viewport pixels (" SIZE_T_FMT
         " rows)...",
      y_max, y_max / RETROFLAT_TILE_H );
#endif /* RETROTILE_TRACE_LVL */

   retroflat_viewport_lock_refresh();
   for( y = 0 ; y_max > y ; y += RETROFLAT_TILE_H ) {
      for( x = 0 ; retroflat_viewport_screen_w() > x ; x += RETROFLAT_TILE_W ) {
         retroflat_viewport_set_refresh( x, y, -1 );
      }
   }

cleanup:

   retroflat_viewport_unlock_refresh();

#endif /* !RETROFLAT_NO_VIEWPORT_REFRESH */

   return retval;
}

/* === */

MERROR_RETVAL retrotile_topdown_draw(
   struct RETROFLAT_BITMAP* target,
   struct RETROTILE* t, struct MDATA_VECTOR* t_defs
) {
   int16_t x = 0,
      y = 0,
      x_tile = 0,
      y_tile = 0;
   retroflat_tile_t tile_id = 0;
   struct RETROTILE_LAYER* layer = NULL;
   struct RETROTILE_TILE_DEF* t_def = NULL;
   MERROR_RETVAL retval = MERROR_OK;

   layer = retrotile_get_layer_p( t, 0 );

   mdata_vector_lock( t_defs );
   /* TODO: Rework this so it uses viewport tile indexes and then calculates
    *       screen pixel X/Y from those? For performance?
    */
   for(
      y = ((retroflat_viewport_world_y() >>
         RETROFLAT_TILE_H_BITS) << RETROFLAT_TILE_H_BITS) ;
      y < (int)retroflat_viewport_world_y() +
         (int)retroflat_viewport_screen_h() ;
      y += RETROFLAT_TILE_H
   ) {
      for(
         x = ((retroflat_viewport_world_x() >>
            RETROFLAT_TILE_W_BITS) << RETROFLAT_TILE_W_BITS) ;
         x < (int)retroflat_viewport_world_x() +
            (int)retroflat_viewport_screen_w() ;
         x += RETROFLAT_TILE_W
      ) {
         /* Limit to tiles that exist in the world. */
         if(
            -1 > x || -1 > y ||
            (int)retroflat_viewport_world_w() <= x ||
            (int)retroflat_viewport_world_h() <= y
         ) {
            continue;
         }

         /* Divide by tile width (16), or shift by 2^4. */
         x_tile = x >> RETROFLAT_TILE_W_BITS;
         y_tile = y >> RETROFLAT_TILE_H_BITS;

         tile_id = retrotile_get_tile( t, layer, x_tile, y_tile );
         t_def = mdata_vector_get(
            t_defs, tile_id - t->tileset_fgid, struct RETROTILE_TILE_DEF );
         if( NULL == t_def ) {
            error_printf(
               "invalid tile ID: %d (- " SIZE_T_FMT " = " SIZE_T_FMT ")",
               tile_id, t->tileset_fgid, tile_id - t->tileset_fgid );
            continue;
         }
         assert( NULL != t_def );

#ifndef RETROFLAT_NO_VIEWPORT_REFRESH
         /* Check tile refresh buffer. */
         retroflat_viewport_lock_refresh();
         if( !retroflat_viewport_tile_is_stale(
            x - retroflat_viewport_world_x(),
            y - retroflat_viewport_world_y(), tile_id
         ) ) {
            retroflat_viewport_unlock_refresh();
            continue;
         }
         /* Noisy! */
         /*
         debug_printf( RETROTILE_TRACE_LVL, "redrawing tile: %u, %u",
            x - retroflat_viewport_world_x(),
            y - retroflat_viewport_world_y() );
         */
         retroflat_viewport_set_refresh(
            x - retroflat_viewport_world_x(),
            y - retroflat_viewport_world_y(), tile_id );
         retroflat_viewport_unlock_refresh();
#endif /* !RETROFLAT_NO_VIEWPORT_REFRESH */

#ifdef RETROGXC_PRESENT
         retrogxc_blit_bitmap( target, t_def->image_cache_id,
            t_def->x, t_def->y,
            retroflat_viewport_screen_x( x ),
            retroflat_viewport_screen_y( y ),
            RETROFLAT_TILE_W, RETROFLAT_TILE_H,
            retroflat_instance_tile( tile_id ) );
#else
         retroflat_blit_bitmap( target, &(t_def->image),
            t_def->x, t_def->y,
            retroflat_viewport_screen_x( x ),
            retroflat_viewport_screen_y( y ),
            RETROFLAT_TILE_W, RETROFLAT_TILE_H,
            retroflat_instance_tile( tile_id ) );
#endif /* RETROGXC_PRESENT */
      }
   }

cleanup:

   mdata_vector_unlock( t_defs );

   return retval;
}

#else

/* This is defined externally so custom token callbacks can reference it. */

#  define RETROTILE_PARSER_MSTATE_TABLE_CONST( name, idx, tokn, parent, m ) \
      extern MAUG_CONST uint8_t SEG_MCONST name;

RETROTILE_PARSER_MSTATE_TABLE( RETROTILE_PARSER_MSTATE_TABLE_CONST )

#  define RETROTILE_CLASS_TABLE_CONSTS( A, a, i ) \
      extern MAUG_CONST uint8_t SEG_MCONST RETROTILE_CLASS_ ## A;

RETROTILE_CLASS_TABLE( RETROTILE_CLASS_TABLE_CONSTS )


#endif /* RETROTIL_C */

/*! \} */ /* retrotile */

/*! \} */ /* maug_retroflt */

#endif /* !RETROTIL_H */

