
#ifndef RETROGUI_H
#define RETROGUI_H

/**
 * \addtogroup maug_retroflt
 * \{
 * \addtogroup maug_retrogui RetroGUI API
 * \brief Tools for drawing interactive GUI elements in a RetroFlat program.
 *
 * This library works well with the \ref maug_retrowin.
 * \{
 * \file retrogui.h
 * \page maug_retrogui_example_page RetroGUI Example
 *
 * Here is a brief example of how to create a RETROGUI with a button, a
 * LABEL, and a FILLBAR.
 *
 * This example presumes that gui_p EITHER points to a separate ::RETROGUI
 * that has been created on the stack and had retrogui_init() called on it, OR
 * it is set to win->gui_p from the \ref maug_retrowin_example_page.
 *
 *       / * These are arbitrary; they only must be unique! * /
 *       # define EXAMPLE_IDC_LABEL    10
 *       # define EXAMPLE_IDC_BUTTON   20
 *       # define EXAMPLE_IDC_FILLBAR  30
 *
 *       / * Insert the label control. * /
 *
 *       retrogui_init_ctl( &ctl, RETROGUI_CTL_TYPE_LABEL, EXAMPLE_IDC_LABEL );
 *
 *       ctl.base.x = 10;
 *       ctl.base.y = 10;
 *       ctl.base.w = 100;
 *       ctl.base.h = 20;
 *       ctl.base.fg_color = RETROFLAT_COLOR_WHITE;
 *       ctl.BUTTON.label = "Example\nLabel";
 *       ctl.BUTTON.label_sz = maug_strlen( ctl.BUTTON.label );
 *
 *       retval = retrogui_push_ctl( gui_p, &ctl );
 *       maug_cleanup_if_not_ok();
 *
 *       / * Insert the button control. * /
 *
 *       retrogui_init_ctl(
 *          &ctl, RETROGUI_CTL_TYPE_BUTTON, EXAMPLE_IDC_BUTTON );
 *
 *       ctl.base.x = 10;
 *       ctl.base.y = 40;
 *       ctl.base.w = 60;
 *       ctl.base.h = 20;
 *       ctl.BUTTON.label = "Test";
 *       ctl.BUTTON.label_sz = maug_strlen( ctl.BUTTON.label );
 *
 *       retval = retrogui_push_ctl( gui_p, &ctl );
 *       maug_cleanup_if_not_ok();
 *
 *       / * Insert the fillbar control. * /
 *
 *       retrogui_init_ctl(
 *          &ctl, RETROGUI_CTL_TYPE_FILLBAR, EXAMPLE_IDC_FILLBAR );
 *
 *       ctl.base.x = 10;
 *       ctl.base.y = 70;
 *       ctl.base.w = 100;
 *       ctl.base.h = 10;
 *       ctl.base.bg_color = RETROGUI_COLOR_BORDER;
 *       ctl.base.fg_color = RETROFLAT_COLOR_RED;
 *
 *       retval = retrogui_push_ctl( gui_p, &ctl );
 *       maug_cleanup_if_not_ok();
 *       
 *       / * Set the level of the inserted fillbar to 50%. * /
 *       retval = retrogui_set_ctl_level(
 *          gui_p, EXAMPLE_IDC_FILLBAR, 50, 100, 0 );
 *
 */

#ifndef RETROFONT_PRESENT
#  error "retrofont not present!"
#endif /* !RETROFONT_PRESENT */

/* TODO: Maug log unified API to reference. */

#ifndef RETROGUI_TRACE_LVL
#  define RETROGUI_TRACE_LVL 0
#endif /* !RETROGUI_TRACE_LVL */

#ifndef RETROGUI_COLOR_BORDER
/**
 * \brief RetroGUI will try to use this color on non-monochrome systems instead
 *        of black to draw things like borders, in order to coexist with
 *        window transparency (which uses black).
 */
#  define RETROGUI_COLOR_BORDER RETROFLAT_COLOR_DARKBLUE
#endif /* !RETROGUI_COLOR_BORDER */

#ifndef RETROGUI_CTL_TEXT_SZ_MAX
/**
 * \addtogroup maug_retrogui_cfg RetroGUI Overrideable Config
 * \brief Options to configure RetroGUI behavior defined at compile time.
 * \{
 */

#ifndef RETROGUI_KEY_ACTIVATE
/**
 * \brief Overrideable constant defining the keyboard key (RETROFLAT_KEY_*)
 *        that will activate the RETROGUI_CTL currently referenced by
 *        RETROGUI::focus.
 */
#  define RETROGUI_KEY_ACTIVATE RETROFLAT_KEY_SPACE
#endif /* !RETROGUI_KEY_ACTIVATE */

#ifndef RETROGUI_KEY_NEXT
/**
 * \brief Overrideable constant defining the keyboard key (RETROFLAT_KEY_*)
 *        that will select the next activateable RETROGUI_CTL currently
 *        referenced by RETROGUI::focus.
 */
#  define RETROGUI_KEY_NEXT RETROFLAT_KEY_DOWN
#endif /* !RETROGUI_KEY_NEXT */

#ifndef RETROGUI_KEY_PREV
/**
 * \brief Overrideable constant defining the keyboard key (RETROFLAT_KEY_*)
 *        that will select the previous activateable RETROGUI_CTL currently
 *        referenced by RETROGUI::focus.
 */
#  define RETROGUI_KEY_PREV RETROFLAT_KEY_UP
#endif /* !RETROGUI_KEY_PREV */

#ifndef RETROGUI_KEY_SEL_NEXT
/**
 * \brief Overrideable constant defining the keyboard key (RETROFLAT_KEY_*)
 *        that will select the next sub-item of the current control (e.g. a
 *        listbox.
 */
#  define RETROGUI_KEY_SEL_NEXT RETROFLAT_KEY_RIGHT
#endif /* !RETROGUI_KEY_SEL_NEXT */

#ifndef RETROGUI_KEY_SEL_PREV
/**
 * \brief Overrideable constant defining the keyboard key (RETROFLAT_KEY_*)
 *        that will select the previous sub-item of the current control (e.g. a
 *        listbox.
 */
#  define RETROGUI_KEY_SEL_PREV RETROFLAT_KEY_LEFT
#endif /* !RETROGUI_KEY_SEL_PREV */

#ifndef RETROGUI_PAD_ACTIVATE
/**
 * \brief Overrideable constant defining the gamepad button (RETROFLAT_PAD_*)
 *        that will activate the RETROGUI_CTL currently referenced by
 *        RETROGUI::focus.
 */
#  define RETROGUI_PAD_ACTIVATE RETROFLAT_PAD_A
#endif /* !RETROGUI_PAD_ACTIVATE */

#ifndef RETROGUI_PAD_NEXT
/**
 * \brief Overrideable constant defining the gamepad button (RETROFLAT_PAD_*)
 *        that will select the next activateable RETROGUI_CTL currently
 *        referenced by RETROGUI::focus.
 */
#  define RETROGUI_PAD_NEXT RETROFLAT_PAD_DOWN
#endif /* !RETROGUI_PAD_NEXT */

#ifndef RETROGUI_PAD_PREV
/**
 * \brief Overrideable constant defining the gamepad button (RETROFLAT_PAD_*)
 *        that will select the previous activateable RETROGUI_CTL currently
 *        referenced by RETROGUI::focus.
 */
#  define RETROGUI_PAD_PREV RETROFLAT_PAD_UP
#endif /* !RETROGUI_PAD_PREV */

#ifndef RETROGUI_PAD_SEL_NEXT
/**
 * \brief Overrideable constant defining the gamepad button (RETROGUI_PAD_*)
 *        that will select the next sub-item of the current control (e.g. a
 *        listbox.
 */
#  define RETROGUI_PAD_SEL_NEXT RETROFLAT_PAD_RIGHT
#endif /* !RETROGUI_PAD_SEL_NEXT */

#ifndef RETROGUI_PAD_SEL_PREV
/**
 * \brief Overrideable constant defining the gamepad button (RETROGUI_PAD_*)
 *        that will select the previous sub-item of the current control (e.g. a
 *        listbox.
 */
#  define RETROGUI_PAD_SEL_PREV RETROFLAT_PAD_LEFT
#endif /* !RETROGUI_PAD_SEL_PREV */

#  define RETROGUI_CTL_TEXT_SZ_MAX 128
#endif /* !RETROGUI_CTL_TEXT_SZ_MAX */

#ifndef RETROGUI_CTL_SZ_MAX_INIT
#  define RETROGUI_CTL_SZ_MAX_INIT 20
#endif /* !RETROGUI_CTL_SZ_MAX_INIT */

#ifndef RETROGUI_PADDING
/**
 * \brief Overrideable constant defining the padding for text inside of
 *        controls in pixels.
 */
#  define RETROGUI_PADDING 5
#endif /* !RETROGUI_PADDING */

#ifndef RETROGUI_BTN_LBL_SZ_MAX
#  define RETROGUI_BTN_LBL_SZ_MAX 64
#endif /* !RETROGUI_BTN_LBL_SZ_MAX */

#ifndef RETROGUI_BTN_LBL_PADDED_X
#  define RETROGUI_BTN_LBL_PADDED_X 8
#endif /* !RETROGUI_BTN_LBL_PADDED_X */

#ifndef RETROGUI_BTN_LBL_PADDED_Y
#  define RETROGUI_BTN_LBL_PADDED_Y 8
#endif /* !RETROGUI_BTN_LBL_PADDED_Y */

#ifndef RETROGUI_CTL_TEXT_BLINK_FRAMES
#  define RETROGUI_CTL_TEXT_BLINK_FRAMES 15
#endif /* !RETROGUI_CTL_TEXT_BLINK_FRAMES */

#ifndef RETROGUI_CTL_LISTBOX_CURSOR_RADIUS
#  define RETROGUI_CTL_LISTBOX_CURSOR_RADIUS 8
#endif /* !RETROGUI_CTL_LISTBOX_CURSOR_RADIUS */

/*! \} */ /* maug_retrogui_cfg */

/**
 * \relates RETROGUI
 * \brief RETROGUI::flags indicating controls should be redrawn.
 */
#define RETROGUI_FLAGS_DIRTY 0x01

/**
 * \relates RETROGUI
 * \brief RETROGUI::flags indicating GUI font is owned by this RETROGUI and
 *        can/will be freed by retrogui_destroy().
 */
#define RETROGUI_FLAGS_FONT_OWNED 0x02

#define RETROGUI_FILLBAR_FLAG_SHOWNUM 0x02

#define _retrogui_copy_str( field, src_str, dest_ctl, str_tmp, str_sz ) \
   /* Sanity checking. */ \
   assert( NULL != src_str ); \
   debug_printf( RETROGUI_TRACE_LVL, \
      "copying string \"%s\" to " #dest_ctl, src_str ); \
   if( 0 == str_sz ) { \
      str_sz = maug_strlen( src_str ); \
      debug_printf( RETROGUI_TRACE_LVL, \
         "determined str sz of \"%s\": " SIZE_T_FMT, src_str, str_sz ); \
   } \
   if( (MAUG_MHANDLE)NULL != dest_ctl. field ## _h ) { \
      /* Free the existing string. */ \
      maug_mfree( dest_ctl. field ## _h ); \
   } \
   \
   /* Allocate new string space. */ \
   dest_ctl. field ## _h = maug_malloc( str_sz + 1, 1 ); \
   debug_printf( RETROGUI_TRACE_LVL, \
      "allocated str sz for \"%s\": " SIZE_T_FMT, src_str, str_sz + 1 ); \
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, dest_ctl. field ## _h ); \
   maug_mlock( dest_ctl. field ## _h, str_tmp ); \
   maug_cleanup_if_null_lock( char*, str_tmp ); \
   \
   /* Copy the string over. */ \
   assert( NULL != str_tmp ); \
   maug_mzero( str_tmp, str_sz + 1 ); \
   debug_printf( RETROGUI_TRACE_LVL, \
      "zeroed str sz for \"%s\": " SIZE_T_FMT, src_str, str_sz + 1 ); \
   maug_strncpy( str_tmp, src_str, str_sz ); \
   debug_printf( RETROGUI_TRACE_LVL, "copied str as: \"%s\"", str_tmp ); \
   maug_munlock( dest_ctl. field ## _h, str_tmp );

/*! \brief Unique identifying constant number for controls. */
typedef int16_t retrogui_idc_t;

#define RETROGUI_IDC_FMT "%d"

#define RETROGUI_IDC_NONE -1

/**
 * \brief Value for retrogui_set_ctl_color() color_key indicating background.
 */
#define RETROGUI_COLOR_BG 1

/**
 * \brief Value for retrogui_set_ctl_color() color_key indicating foreground.
 */
#define RETROGUI_COLOR_FG 2

/**
 * \brief Value for retrogui_set_ctl_color() color_key indicating selection
 *        background.
 */
#define RETROGUI_COLOR_SEL_BG 3

/**
 * \brief Value for retrogui_set_ctl_color() color_key indicating selection
 *        foreground.
 */
#define RETROGUI_COLOR_SEL_FG 4

/**
 * \addtogroup maug_retrogui_ctl RetroGUI Controls
 * \{
 */

/**
 * \brief Table defining all control types and their specific fields in
 *        ::RETROGUI_CTL.
 *
 * This table takes a macro as an argument, itself with the argumentS:
 *
 * \param type_idx Index passed to RETROGUI_CTL_BASE::type.
 * \param struct Name of the struct under the ::RETROGUI_CTL union.
 * \param fields Semicolon-separated list of fields for the type struct.
 *
 * Each such macro defines a new control type.
 *
 * All entries in this table have corresponding retrogui_redraw_*,
 * retrogui_poll_*, retrogui_init_*, and retrogui_push_* functions which are
 * called internally from retrogui_redraw_ctls(), retrogui_poll_ctls(),
 * retrogui_init_ctl(), and retrogui_push_ctl(), respectively.
 */
#define RETROGUI_CTL_TABLE_BASE( f ) \
   f( 0, NONE, void* none; ) \
   f( 1, LISTBOX, MAUG_MHANDLE list_h; char* list; size_t list_sz; size_t list_sz_max; size_t sel_idx; ) \
   f( 2, BUTTON, MAUG_MHANDLE label_h; char* label; size_t label_sz; int16_t push_frames; uint8_t font_flags; ) \
   f( 3, LABEL, MAUG_MHANDLE label_h; char* label; size_t label_sz; uint8_t font_flags; ) \
   f( 4, IMAGE, retroflat_blit_t image; ssize_t image_cache_id; int16_t instance; retroflat_pxxy_t src_x; retroflat_pxxy_t src_y; ) \
   f( 5, FILLBAR, uint8_t flags; uint16_t cur; uint16_t max; )

#ifdef RETROGUI_NO_TEXTBOX
#  define RETROGUI_CTL_TABLE( f ) RETROGUI_CTL_TABLE_BASE( f )
#else
#  define RETROGUI_CTL_TABLE( f ) RETROGUI_CTL_TABLE_BASE( f ) \
   f( 6, TEXTBOX, MAUG_MHANDLE text_h; char* text; size_t text_sz; size_t text_sz_max; size_t text_cur; int16_t blink_frames; )
#endif /* RETROGUI_NO_TEXTBOX */

#if 0
   f( 6, SCROLLBAR, size_t min; size_t max; size_t value; )
#endif

#ifdef RETROGUI_NO_TEXTBOX
#  define retrogui_can_focus_ctl( ctl ) \
      (RETROGUI_CTL_TYPE_BUTTON == (ctl)->base.type || \
      RETROGUI_CTL_TYPE_LISTBOX == (ctl)->base.type)
#else
/**
 * \brief Determine if a RETROGUI_CTL can hold RETROGUI::focus.
 */
#  define retrogui_can_focus_ctl( ctl ) \
      (RETROGUI_CTL_TYPE_BUTTON == (ctl)->base.type || \
      RETROGUI_CTL_TYPE_TEXTBOX == (ctl)->base.type || \
      RETROGUI_CTL_TYPE_LISTBOX == (ctl)->base.type)
#endif /* RETROGUI_NO_TEXTBOX */

/*! \brief Fields common to ALL ::RETROGUI_CTL types. */
struct RETROGUI_CTL_BASE {
   uint8_t type;
   retrogui_idc_t idc;
   retroflat_pxxy_t x;
   retroflat_pxxy_t y;
   retroflat_pxxy_t w;
   retroflat_pxxy_t h;
   RETROFLAT_COLOR bg_color;
   RETROFLAT_COLOR fg_color;
   RETROFLAT_COLOR sel_fg;
   RETROFLAT_COLOR sel_bg;
#if defined( RETROGUI_NATIVE_WIN )
   HWND hwnd;
#endif
};

/**
 * \brief Creates the corresponding RETROGUI_* structs from
 *        ::RETROGUI_CTL_TABLE that populate union ::RETROGUI_CTL.
 */
#define RETROGUI_CTL_TABLE_FIELDS( idx, c_name, c_fields ) \
   struct RETROGUI_CTL_ ## c_name { \
      struct RETROGUI_CTL_BASE base; \
      c_fields \
   };

RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_FIELDS )

/**
 * \brief Adds the structs created by ::RETROGUI_CTL_TABLE_FIELDS to
 *        union ::RETROGUI_CTL.
 */
#define RETROGUI_CTL_TABLE_TYPES( idx, c_name, c_fields ) \
   struct RETROGUI_CTL_ ## c_name c_name;

union RETROGUI_CTL {
   struct RETROGUI_CTL_BASE base;
   RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_TYPES )
};

/*! \} */ /* maug_retrogui_ctl */

typedef void (*retrogui_xy_cb)(
   retroflat_pxxy_t* x, retroflat_pxxy_t* y, void* data );

/*
 * \note It is possible to have multiple GUI controllers in a program. For
 *       example, a web browser might have a controller for its address bar and
 *       a controller for form elements on pages.
 *
 */
struct RETROGUI {
   uint8_t flags;
   retroflat_pxxy_t x;
   retroflat_pxxy_t y;
   retroflat_pxxy_t w;
   retroflat_pxxy_t h;
   RETROFLAT_COLOR bg_color;
   retrogui_idc_t idc_prev;
   struct MDATA_VECTOR ctls;
   /*! \brief Unique identifying index for current highlighted RETROGUI_CTL. */
   retrogui_idc_t focus;
   retroflat_blit_t* draw_bmp;
#ifdef RETROGXC_PRESENT
   ssize_t font_idx;
#else
   /**
    * \brief Font used to draw any attached RETROGUI_CTL.
    *
    * This should be set with retrogui_set_font(), and will be automatically
    * freed by retrogui_destroy() if it is. If it is set by manual assignment,
    * then it will *not* be freed by retrogui_destroy().
    *
    * \warning This field is not present if \ref maug_retrogxc is loaded!
    */
   MAUG_MHANDLE font_h;
#endif /* RETROGXC_PRESENT */
};

MERROR_RETVAL retrogui_push_listbox_item(
   struct RETROGUI* gui, retrogui_idc_t idc, const char* item, size_t item_sz );

/**
 * \brief Poll for the last clicked control and maintain listboxes
 *        and menus.
 * \param input Input integer returned from retroflat_poll_input().
 * \param input_evt ::RETROFLAT_INPUT initialized by retroflat_poll_input().
 * \return IDC of clicked control or ::RETROGUI_IDC_NONE if none clicked
 *         since last poll.
 */
retrogui_idc_t retrogui_poll_ctls(
   struct RETROGUI* gui, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt );

MERROR_RETVAL retrogui_redraw_ctls( struct RETROGUI* gui );

MERROR_RETVAL retrogui_sz_ctl(
   struct RETROGUI* gui, retrogui_idc_t idc,
   retroflat_pxxy_t* p_w, retroflat_pxxy_t* p_h,
   retroflat_pxxy_t max_w, retroflat_pxxy_t max_h );

MERROR_RETVAL retrogui_pos_ctl(
   struct RETROGUI* gui, retrogui_idc_t idc,
   retroflat_pxxy_t x, retroflat_pxxy_t y,
   retroflat_pxxy_t w, retroflat_pxxy_t h );

MERROR_RETVAL retrogui_push_ctl(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl );

/**
 * \brief Load the \ref retrofont for the given ::RETROGUI to draw its controls
 *        with. Use \ref maug_retrogxc if available.
 *
 * Fonts loaded with this function will be automatically freed by
 * calling retrogui_destroy() on this ::RETROGUI.
 */
MERROR_RETVAL retrogui_set_font(
   struct RETROGUI* gui, const char* font_path );

#ifndef RETROGUI_NO_TEXTBOX

MERROR_RETVAL retrogui_get_ctl_text(
   struct RETROGUI* gui, retrogui_idc_t idc, char* buffer, size_t buffer_sz );

#endif /* !RETROGUI_NO_TEXTBOX */

ssize_t retrogui_get_ctl_sel_idx( struct RETROGUI* gui, retrogui_idc_t idc );

MERROR_RETVAL retrogui_set_ctl_color(
   struct RETROGUI* gui, retrogui_idc_t idc, uint8_t color_key,
   RETROFLAT_COLOR color_val );

MERROR_RETVAL retrogui_set_ctl_text(
   struct RETROGUI* gui, retrogui_idc_t idc, size_t buffer_sz,
   const char* fmt, ... );

/**
 * \brief Set the image displayed by an IMAGE-type RETROGUI_CTL.
 * \param idc Unique identifier index of the control to adjust.
 * \param path Filesystem path to pass to retroflat_load_bitmap().
 * \param flags Flags from the \ref maug_retroflt_drawing to pass to
 *              retroflat_load_bitmap().
 * \return One of the \ref maug_error_retvals indicating operation result.
 */
MERROR_RETVAL retrogui_set_ctl_image(
   struct RETROGUI* gui, retrogui_idc_t idc, const char* path, uint8_t flags );

/**
 * \brief Set the current progress level displayed by a FILLBAR-type
 *        RETROGUI_CTL.
 * \param idc Unique identifier index of the control to adjust.
 * \param level Numeric value out of the max param (e.g. 50 for half if max is
 *              100).
 * \param max The maximum possible value.
 * \return One of the \ref maug_error_retvals indicating operation result.
 */
MERROR_RETVAL retrogui_set_ctl_level(
   struct RETROGUI* gui, retrogui_idc_t idc, uint16_t level, uint16_t max,
   uint8_t flags );

MERROR_RETVAL retrogui_init_ctl(
   union RETROGUI_CTL* ctl, uint8_t type, size_t idc );

/**
 * \relates RETROGUI
 * \brief Increment RETROGUI::focus, skipping elements that cannot hold focus.
 * \warning This function is designed to be used via the retrogui_focus_next()
 *          and retrogui_focus_prev() convenience functions!
 * \param start Value to start from, either 0 for the first element or
 *              ctl_count - 1 for the last.
 * \param incr 1 or -1, depending on whether to increment forwards or backwards.
 * \return The new value of RETROGUI::focus, or a negative value to be
 *         converted to an error code with merror_sz_to_retval() on failure.
 */
retrogui_idc_t retrogui_focus_iter(
   struct RETROGUI* gui, size_t start, ssize_t incr );

/**
 * \relates RETROGUI
 * \brief Prepare a ::RETROGUI controller for use.
 *
 * \note It is possible to have multiple GUI controllers in a program. For
 *       example, a web browser might have a controller for its address bar and
 *       a controller for form elements on pages.
 *
 * \warning This must be run before any other methods are used on the given GUI!
 * \param gui Pointer to a RETROGUI struct to initialize.
 * \return One of the \ref maug_error_retvals indicating operation result.
 */
MERROR_RETVAL retrogui_init( struct RETROGUI* gui );

/**
 * \relates RETROGUI
 * \brief Remove a control with the given unique identifier index from the
 *        given RETROGUI controller.
 * \param gui Pointer to a RETROGUI struct to remove a control from.
 * \param idc Unique identifier index of the control to remove.
 * \return One of the \ref maug_error_retvals indicating operation result.
 */
MERROR_RETVAL retrogui_remove_ctl( struct RETROGUI* gui, retrogui_idc_t idc );

/**
 * \relates RETROGUI
 * \brief Free memory held by a ::RETROGUI controller internally and clean up
 *        any subordinate controls.
 *
 * \warning This function does not free memory used for the RETROGUI struct
 *          directly! A RETROGUI may exist on the stack or be managed by a
 *          RETROWIN, after all!
 * \return One of the \ref maug_error_retvals indicating operation result.
 */
MERROR_RETVAL retrogui_destroy( struct RETROGUI* gui );

#define retrogui_focus_next( gui ) \
   retrogui_focus_iter( gui, 0, 1 )

#define retrogui_focus_prev( gui ) \
   retrogui_focus_iter( gui, mdata_vector_ct( &((gui)->ctls) ) - 1, -1 )

#ifdef RETROGUI_C

#define RETROGUI_CTL_TABLE_CONSTS( idx, c_name, c_fields ) \
   MAUG_CONST uint8_t SEG_MCONST RETROGUI_CTL_TYPE_ ## c_name = idx;

RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_CONSTS )

#ifdef RETROGUI_TRACE_TOKENS

#define RETROGUI_CTL_TABLE_NAMES( idx, c_name, f_fields ) \
   #c_name,

MAUG_CONST char* SEG_MCONST gc_retrogui_ctl_names[] = {
   RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_NAMES )
   ""
};

#endif /* RETROGUI_TRACE_TOKENS */

static union RETROGUI_CTL* _retrogui_get_ctl_by_idc(
   struct RETROGUI* gui, size_t idc );

/* === Control: NONE === */

static retrogui_idc_t retrogui_click_NONE( 
   struct RETROGUI* gui,
   union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   retrogui_idc_t idc_out = RETROGUI_IDC_NONE;

   return idc_out;
}

static retrogui_idc_t retrogui_key_NONE( 
   struct RETROGUI* gui, union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   retrogui_idc_t idc_out = RETROGUI_IDC_NONE;

   return idc_out;
}

void retrogui_redraw_NONE( struct RETROGUI* gui, union RETROGUI_CTL* ctl ) {
}

static MERROR_RETVAL retrogui_push_NONE( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_GUI;
  
   return retval;
}

static MERROR_RETVAL retrogui_sz_NONE(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   retroflat_pxxy_t* p_w, retroflat_pxxy_t* p_h,
   retroflat_pxxy_t max_w, retroflat_pxxy_t max_h
) {
   return MERROR_OK;
}

static MERROR_RETVAL retrogui_pos_NONE(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   retroflat_pxxy_t x, retroflat_pxxy_t y,
   retroflat_pxxy_t w, retroflat_pxxy_t h
) {
   return MERROR_OK;
}

static void retrogui_destroy_NONE( union RETROGUI_CTL* ctl ) {
}

static MERROR_RETVAL retrogui_init_NONE( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_GUI;

   return retval;
}

/* === Control: LISTBOX === */

static retrogui_idc_t retrogui_click_LISTBOX( 
   struct RETROGUI* gui,
   union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   retrogui_idc_t idc_out = RETROGUI_IDC_NONE;
   MERROR_RETVAL retval = MERROR_OK;
   size_t i = 0,
      j = 0;
   retroflat_pxxy_t w = 0,
      h = 0;

#  if defined( RETROGUI_NATIVE_WIN )
   /* Do nothing. */
#  else

   assert( NULL == ctl->LISTBOX.list );
   assert( (MAUG_MHANDLE)NULL != ctl->LISTBOX.list_h );
   maug_mlock( ctl->LISTBOX.list_h, ctl->LISTBOX.list );
   maug_cleanup_if_null_lock( char*, ctl->LISTBOX.list );

   /* Figure out the item clicked. */
   while( i < ctl->LISTBOX.list_sz ) {
#ifdef RETROGXC_PRESENT
      retrogxc_string_sz(
         gui->draw_bmp, &(ctl->LISTBOX.list[i]), 0, gui->font_idx,
         ctl->base.w, ctl->base.h, &w, &h, 0 );
#else
      retrofont_string_sz(
         gui->draw_bmp, &(ctl->LISTBOX.list[i]), 0, gui->font_h,
         ctl->base.w, ctl->base.h, &w, &h, 0 );
#endif /* RETROGXC_PRESENT */

      if(
         (retroflat_pxxy_t)(input_evt->mouse_y) < 
         ctl->base.y + ((j + 1) * (h + RETROGUI_PADDING))
      ) {
         ctl->LISTBOX.sel_idx = j;
         break;
      }

      /* Try next variable-length string. */
      i += maug_strlen( &(ctl->LISTBOX.list[i]) ) + 1;
      assert( i <= ctl->LISTBOX.list_sz );
      j++;
   }

cleanup:

   if( NULL != ctl->LISTBOX.list ) {
      maug_munlock( ctl->LISTBOX.list_h, ctl->LISTBOX.list );
   }

   if( MERROR_OK != retval ) {
      idc_out = RETROGUI_IDC_NONE;
   }

#endif

   return idc_out;
}

static retrogui_idc_t retrogui_key_LISTBOX( 
   struct RETROGUI* gui, union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   retrogui_idc_t idc_out = RETROGUI_IDC_NONE;

   switch( *p_input ) {
   retroflat_case_key( RETROGUI_KEY_SEL_NEXT, RETROGUI_PAD_SEL_NEXT )
      ctl->LISTBOX.sel_idx++;

      /* Redraw and preempt further processing of input. */
      gui->flags |= RETROGUI_FLAGS_DIRTY;
      *p_input = 0;
      break;

   retroflat_case_key( RETROGUI_KEY_SEL_PREV, RETROGUI_PAD_SEL_PREV )
      ctl->LISTBOX.sel_idx--;

      /* Redraw and preempt further processing of input. */
      gui->flags |= RETROGUI_FLAGS_DIRTY;
      *p_input = 0;
      break;
   }

   return idc_out;
}

static void retrogui_redraw_LISTBOX(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl
) {
   size_t i = 0,
      j = 0;
   retroflat_pxxy_t w = 0,
      h = 0,
      item_y = 0;
   RETROFLAT_COLOR fg_color;
   
   assert( NULL == ctl->LISTBOX.list );

#  if defined( RETROGUI_NATIVE_WIN )
   /* TODO: InvalidateRect()? */
#  else

   maug_mlock( ctl->LISTBOX.list_h, ctl->LISTBOX.list );
   if( NULL == ctl->LISTBOX.list ) {
      goto cleanup;
   }
   
   if( RETROFLAT_COLOR_BLACK != ctl->base.bg_color ) {
      retroflat_2d_rect( gui->draw_bmp, ctl->base.bg_color,
         gui->x + ctl->base.x, gui->y + ctl->base.y,
         ctl->base.w, ctl->base.h, RETROFLAT_FLAGS_FILL );
   }

   /* Parse out variable-length strings. */
   while( i < ctl->LISTBOX.list_sz ) {
#ifdef RETROGXC_PRESENT
      retrogxc_string_sz(
         gui->draw_bmp, &(ctl->LISTBOX.list[i]), 0, gui->font_idx,
         ctl->base.w, ctl->base.h, &w, &h, 0 );
#else
      retrofont_string_sz(
         gui->draw_bmp, &(ctl->LISTBOX.list[i]), 0, gui->font_h,
         ctl->base.w, ctl->base.h, &w, &h, 0 );
#endif /* RETROGXC_PRESENT */
      debug_printf( RETROGUI_TRACE_LVL,
         "str height for \"%s\": " SIZE_T_FMT, 
         &(ctl->LISTBOX.list[i]), h );
      if( j == ctl->LISTBOX.sel_idx ) {
         /* Draw selection colors. */
         retroflat_2d_rect( gui->draw_bmp, ctl->base.sel_bg,
            gui->x + ctl->base.x,
            gui->y + ctl->base.y + item_y,
            ctl->base.w, h, RETROFLAT_FLAGS_FILL );
         fg_color = ctl->base.sel_fg;

         retroflat_ellipse(
            gui->draw_bmp, ctl->base.sel_fg,
            gui->x + ctl->base.x,
            gui->y + ctl->base.y + item_y,
            RETROGUI_CTL_LISTBOX_CURSOR_RADIUS,
            RETROGUI_CTL_LISTBOX_CURSOR_RADIUS, 0 );

      } else {
         fg_color = ctl->base.fg_color;
      }

#ifdef RETROGXC_PRESENT
      retrogxc_string(
         gui->draw_bmp, fg_color, &(ctl->LISTBOX.list[i]), 0,
         gui->font_idx,
         gui->x + ctl->base.x +
            RETROGUI_CTL_LISTBOX_CURSOR_RADIUS + RETROGUI_PADDING,
         gui->y + ctl->base.y + item_y,
         0, 0, 0 );
#else
      retrofont_string(
         gui->draw_bmp, fg_color, &(ctl->LISTBOX.list[i]), 0,
         gui->font_h,
         gui->x + ctl->base.x +
            RETROGUI_CTL_LISTBOX_CURSOR_RADIUS + RETROGUI_PADDING,
         gui->y + ctl->base.y + item_y,
         0, 0, 0 );
#endif /* RETROGXC_PRESENT */

      /* Move to next variable-length string. */
      i += maug_strlen( &(ctl->LISTBOX.list[i]) ) + 1;
      assert( i <= ctl->LISTBOX.list_sz );
      /* Track the item high separately in case the item is multi-line. */
      item_y += h + RETROGUI_PADDING;
      j++;
   }

cleanup:

   if( NULL != ctl->LISTBOX.list ) {
      maug_munlock( ctl->LISTBOX.list_h, ctl->LISTBOX.list );
   }

#  endif

}

MERROR_RETVAL retrogui_select_listbox_item(
   union RETROGUI_CTL* ctl, size_t item_idx
) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( RETROGUI_NATIVE_WIN )

   /* Select sel_idx. */
   SendMessage( ctl->base.hwnd, LB_SETCURSEL, item_idx, 0 );

#  else

   ctl->LISTBOX.sel_idx = item_idx;

#  endif

   return retval;
}

MERROR_RETVAL retrogui_push_listbox_item(
   struct RETROGUI* gui, retrogui_idc_t idc, const char* item, size_t item_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   union RETROGUI_CTL* ctl = NULL;
   MAUG_MHANDLE listbox_h_new = (MAUG_MHANDLE)NULL;
   int autolock = 0;

   if( !mdata_vector_is_locked( &((gui)->ctls) ) ) {
      mdata_vector_lock( &(gui->ctls) );
      autolock = 1;
   }

   debug_printf( RETROGUI_TRACE_LVL,
      "pushing item \"%s\" to listbox " RETROGUI_IDC_FMT "...", item, idc );

   ctl = _retrogui_get_ctl_by_idc( gui, idc );
   if( NULL == ctl ) {
      /* Elaborate on error from _retrogui_get_ctl_by_idc(). */
      error_printf( "could not add item: %s", item );
      retval = MERROR_GUI;
      goto cleanup;
   }
   
#  if defined( RETROGUI_NATIVE_WIN )

   SendMessage( ctl->LISTBOX.base.hwnd, LB_ADDSTRING, 0, (LPARAM)item );

#  else

   if( 0 == ctl->LISTBOX.list_sz ) {
      ctl->LISTBOX.list_h = maug_malloc( 255, sizeof( char ) );
      maug_cleanup_if_null_alloc( MAUG_MHANDLE, ctl->LISTBOX.list_h );
      ctl->LISTBOX.list_sz_max = 255;
   }

   if( NULL != ctl->LISTBOX.list ) {
      maug_munlock( ctl->LISTBOX.list_h, ctl->LISTBOX.list );
   }

   while( ctl->LISTBOX.list_sz + item_sz + 1 >= ctl->LISTBOX.list_sz_max )  {
      debug_printf( RETROGUI_TRACE_LVL,
         "resizing listbox items to " SIZE_T_FMT "...",
         ctl->LISTBOX.list_sz );
      maug_mrealloc_test(
         listbox_h_new, ctl->LISTBOX.list_h,
         ctl->LISTBOX.list_sz_max * 2, sizeof( char ) );
      ctl->LISTBOX.list_sz_max *= 2;
   }

   maug_mlock( ctl->LISTBOX.list_h, ctl->LISTBOX.list );
   maug_cleanup_if_null_alloc( char*, ctl->LISTBOX.list );

   maug_strncpy( &(ctl->LISTBOX.list[ctl->LISTBOX.list_sz]), item, item_sz );
   ctl->LISTBOX.list[ctl->LISTBOX.list_sz + item_sz] = '\0';
   ctl->LISTBOX.list_sz += item_sz + 1;

#endif

   gui->flags |= RETROGUI_FLAGS_DIRTY;

cleanup:

   if( NULL != ctl->LISTBOX.list ) {
      maug_munlock( ctl->LISTBOX.list_h, ctl->LISTBOX.list );
   }

   if( autolock ) {
      mdata_vector_unlock( &(gui->ctls) );
   }

   return retval;
}

static MERROR_RETVAL retrogui_push_LISTBOX( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( RETROGUI_NATIVE_WIN )

   ctl->base.hwnd = CreateWindow(
      "LISTBOX", NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD,
      gui->x + ctl->base.x, gui->y + ctl->base.y, ctl->base.w, ctl->base.h,
      g_retroflat_state->window, (HMENU)(ctl->base.idc),
      g_retroflat_instance, NULL );
   debug_printf( RETROGUI_TRACE_LVL,
      "listbox hwnd: %p", ctl->LISTBOX.base.hwnd );
   if( (HWND)NULL == ctl->base.hwnd ) {
      error_printf( "could not create listbox" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   gui->flags |= RETROGUI_FLAGS_DIRTY;

cleanup:

#  else

   /* TODO? */

#  endif

   return retval;
}

static MERROR_RETVAL retrogui_sz_LISTBOX(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   retroflat_pxxy_t* p_w, retroflat_pxxy_t* p_h,
   retroflat_pxxy_t max_w, retroflat_pxxy_t max_h
) {
   MERROR_RETVAL retval = MERROR_GUI;
   /* TODO */
   return retval;
}

static MERROR_RETVAL retrogui_pos_LISTBOX(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   retroflat_pxxy_t x, retroflat_pxxy_t y,
   retroflat_pxxy_t w, retroflat_pxxy_t h
) {
   MERROR_RETVAL retval = MERROR_GUI;
   /* TODO */
   return retval;
}

static void retrogui_destroy_LISTBOX( union RETROGUI_CTL* ctl ) {
   assert( NULL == ctl->LISTBOX.list );
   maug_mfree( ctl->LISTBOX.list_h );
}

static MERROR_RETVAL retrogui_init_LISTBOX( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( RETROGUI_TRACE_LVL,
      "initializing listbox " RETROGUI_IDC_FMT "...", ctl->base.idc );

   ctl->base.bg_color = RETROFLAT_COLOR_WHITE;
   ctl->base.sel_fg = RETROFLAT_COLOR_WHITE;
   if( 2 < retroflat_screen_colors() ) {
      ctl->base.sel_bg = RETROFLAT_COLOR_BLUE;
      ctl->base.fg_color = RETROGUI_COLOR_BORDER;
   } else {
      ctl->base.sel_bg = RETROFLAT_COLOR_BLACK;
      ctl->base.fg_color = RETROFLAT_COLOR_BLACK;
   }

   return retval;
}

/* === Control: BUTTON === */

static retrogui_idc_t retrogui_click_BUTTON( 
   struct RETROGUI* gui,
   union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   retrogui_idc_t idc_out = RETROGUI_IDC_NONE;

   if( 0 < ctl->BUTTON.push_frames ) {
      goto cleanup;
   }

   /* Set the last button clicked. */
   idc_out = ctl->base.idc;

   /* Set the frames to show the pushed-in view. */
   /* TODO: Use a constant, here. */
   ctl->BUTTON.push_frames = 3;

cleanup:

   return idc_out;
}

static retrogui_idc_t retrogui_key_BUTTON( 
   struct RETROGUI* gui, union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   retrogui_idc_t idc_out = RETROGUI_IDC_NONE;

   /* Set the last button clicked. */
   /* TODO: Only set out on ENTER/SPACE. */
   /* idc_out = ctl->base.idc; */

   return idc_out;
}

static void retrogui_redraw_BUTTON(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl
) {
   retroflat_pxxy_t w = 0,
      h = 0,
      text_offset = 0;
   RETROFLAT_COLOR fg_color = ctl->base.fg_color;
   RETROFLAT_COLOR bg_color = ctl->base.bg_color;
   RETROFLAT_COLOR push_shadow_color = RETROFLAT_COLOR_DARKGRAY;
   RETROFLAT_COLOR border_color = RETROGUI_COLOR_BORDER;

   if( ctl->base.idc == gui->focus ) {
      /* Assign selected color if focused. */
      fg_color = ctl->base.sel_fg;
   }

   if( ctl->base.idc == gui->focus ) {
      /* Assign selected color if focused. */
      bg_color = ctl->base.sel_bg;
   }

   /* Figure out push shadow color for current color depth. */
   if( 2 >= retroflat_screen_colors() ) {
      push_shadow_color = RETROFLAT_COLOR_BLACK;
      border_color = RETROFLAT_COLOR_BLACK;
   }

   retroflat_2d_rect(
      gui->draw_bmp, bg_color, ctl->base.x, ctl->base.y,
      ctl->base.w, ctl->base.h, RETROFLAT_FLAGS_FILL );

   retroflat_2d_rect( gui->draw_bmp, border_color,
      gui->x + ctl->base.x, gui->y + ctl->base.y,
      ctl->base.w, ctl->base.h, 0 );

   /* Draw the push shadows on top/left or bottom/right, depending on pushed
    * status.
    */
   if( 0 < ctl->BUTTON.push_frames ) {
      retroflat_2d_line(
         gui->draw_bmp, push_shadow_color,
         gui->x + ctl->base.x + 1, gui->y + ctl->base.y + 1,
         gui->x + ctl->base.x + ctl->base.w - 2, gui->y + ctl->base.y + 1, 0 );
      retroflat_2d_line(
         gui->draw_bmp, push_shadow_color,
         gui->x + ctl->base.x + 1, gui->y + ctl->base.y + 2,
         gui->x + ctl->base.x + 1, gui->y + ctl->base.y + ctl->base.h - 3, 0 );

      gui->flags |= RETROGUI_FLAGS_DIRTY; /* Mark dirty for push animation. */
      ctl->BUTTON.push_frames--;
      text_offset = 1;
   } else {
      /* Button is not pushed. */
      retroflat_2d_line(
         gui->draw_bmp, RETROFLAT_COLOR_WHITE,
         gui->x + ctl->base.x + 1, gui->y + ctl->base.y + 1,
         gui->x + ctl->base.x + ctl->base.w - 2, gui->y + ctl->base.y + 1, 0 );
      retroflat_2d_line(
         gui->draw_bmp, RETROFLAT_COLOR_WHITE,
         gui->x + ctl->base.x + 1, gui->y + ctl->base.y + 2,
         gui->x + ctl->base.x + 1, gui->y + ctl->base.y + ctl->base.h - 3, 0 );
   }

   maug_mlock( ctl->BUTTON.label_h, ctl->BUTTON.label );
   if( NULL == ctl->BUTTON.label ) {
      error_printf( "could not lock BUTTON label!" );
      goto cleanup;
   }
      
   /* Grab the string size and use it to center the text in the control. */
#ifdef RETROGXC_PRESENT
   retrogxc_string_sz(
#else
   retrofont_string_sz(
#endif /* RETROGXC_PRESENT */
      gui->draw_bmp, ctl->BUTTON.label, 0,
#ifdef RETROGXC_PRESENT
      gui->font_idx,
#else
      gui->font_h,
#endif /* RETROGXC_PRESENT */
      /* TODO: Pad max client area. */
      ctl->base.w, ctl->base.h, &w, &h, ctl->BUTTON.font_flags );

#ifdef RETROGXC_PRESENT
   retrogxc_string(
#else
   retrofont_string(
#endif /* RETROGXC_PRESENT */
      gui->draw_bmp, fg_color, ctl->BUTTON.label, 0,
#ifdef RETROGXC_PRESENT
      gui->font_idx,
#else
      gui->font_h,
#endif /* RETROGXC_PRESENT */
      gui->x + ctl->base.x + ((ctl->base.w >> 1) - (w >> 1)) + text_offset,
      gui->y + ctl->base.y + ((ctl->base.h >> 1) - (h >> 1)) + text_offset,
      /* TODO: Pad max client area. */
      ctl->base.w, ctl->base.h, ctl->BUTTON.font_flags );

   maug_munlock( ctl->BUTTON.label_h, ctl->BUTTON.label );

cleanup:

   return;
}

static MERROR_RETVAL retrogui_push_BUTTON( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( RETROGUI_NATIVE_WIN )

   ctl->base.hwnd = CreateWindow(
      "BUTTON", ctl->BUTTON.label, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
      gui->x + ctl->base.x, gui->y + ctl->base.y, ctl->base.w, ctl->base.h,
      g_retroflat_state->window, (HMENU)(ctl->base.idc),
      g_retroflat_instance, NULL );
   if( (HWND)NULL == ctl->base.hwnd ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR, "Error",
         "Could not create button " RETROGUI_IDC_FMT ": %s",
         ctl->base.idc, ctl->BUTTON.label );
      retval = MERROR_GUI;
      goto cleanup;
   }

#  else
   size_t label_sz = 0;
   char* label_tmp = NULL;

   debug_printf( RETROGUI_TRACE_LVL, "pushing BUTTON control..." );

   _retrogui_copy_str(
      label, ctl->BUTTON.label, ctl->BUTTON, label_tmp, label_sz );
   ctl->BUTTON.label = NULL;
#  endif

cleanup:

   return retval;
}

static MERROR_RETVAL retrogui_sz_BUTTON(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   retroflat_pxxy_t* p_w, retroflat_pxxy_t* p_h,
   retroflat_pxxy_t max_w, retroflat_pxxy_t max_h
) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( NULL != ctl );
   assert( NULL == ctl->BUTTON.label );
   assert( (MAUG_MHANDLE)NULL != ctl->BUTTON.label_h );

   maug_mlock( ctl->BUTTON.label_h, ctl->BUTTON.label );
   maug_cleanup_if_null_lock( char*, ctl->BUTTON.label );

   /* Get the size of the text-based GUI item. */
#ifdef RETROGXC_PRESENT
   retrogxc_string_sz(
#else
   retrofont_string_sz(
#endif /* RETROGXC_PRESENT */
      NULL,
      ctl->BUTTON.label,
      0,
#ifdef RETROGXC_PRESENT
      gui->font_idx,
#else
      gui->font_h,
#endif /* RETROGXC_PRESENT */
      max_w - 8,
      max_h - 8,
      p_w,
      p_h, ctl->BUTTON.font_flags );

   /* Add space for borders and stuff. */
   *p_w += RETROGUI_BTN_LBL_PADDED_X;
   *p_h += RETROGUI_BTN_LBL_PADDED_Y;

cleanup:

   maug_munlock( ctl->BUTTON.label_h, ctl->BUTTON.label );

   return retval;
}

static MERROR_RETVAL retrogui_pos_BUTTON(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   retroflat_pxxy_t x, retroflat_pxxy_t y,
   retroflat_pxxy_t w, retroflat_pxxy_t h
) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( RETROGUI_NATIVE_WIN )
   /* TODO */
#  else
   assert( NULL != ctl );

   ctl->base.x = x;
   ctl->base.y = y;
   if( 0 < w ) {
      ctl->base.w = w;
   }
   if( 0 < h ) {
      ctl->base.h = h;
   }
#  endif /* RETROGUI_NATIVE_WIN */

   return retval;
}

static void retrogui_destroy_BUTTON( union RETROGUI_CTL* ctl ) {
   if( (MAUG_MHANDLE)NULL != ctl->BUTTON.label_h ) {
      maug_mfree( ctl->BUTTON.label_h );
   }
}

static MERROR_RETVAL retrogui_init_BUTTON( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( RETROGUI_TRACE_LVL,
      "initializing button " RETROGUI_IDC_FMT "...", ctl->base.idc );

   if( 2 < retroflat_screen_colors() ) {
      ctl->base.fg_color = RETROGUI_COLOR_BORDER;
      ctl->base.bg_color = RETROFLAT_COLOR_GRAY;
      ctl->base.sel_fg = RETROFLAT_COLOR_BLUE;
      ctl->base.sel_bg = RETROFLAT_COLOR_GRAY;
   } else {
      ctl->base.fg_color = RETROFLAT_COLOR_BLACK;
      ctl->base.bg_color = RETROFLAT_COLOR_WHITE;
      ctl->base.sel_fg = RETROFLAT_COLOR_WHITE;
      ctl->base.sel_bg = RETROFLAT_COLOR_BLACK;
   }

   return retval;
}

#ifndef RETROGUI_NO_TEXTBOX

/* === Control: TEXTBOX === */

static retrogui_idc_t retrogui_click_TEXTBOX(
   struct RETROGUI* gui,
   union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   retrogui_idc_t idc_out = RETROGUI_IDC_NONE;

   return idc_out;
}

static retrogui_idc_t retrogui_key_TEXTBOX(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   retrogui_idc_t idc_out = RETROGUI_IDC_NONE;
   char c = '\0';

#  if defined( RETROGUI_NATIVE_WIN )
   /* Do nothing. */
#  else

   c = retroflat_vk_to_ascii( *p_input, input_evt->key_flags );

   /* Ignore non-printable characters. */
   if(
      0 == c &&
      RETROFLAT_KEY_RIGHT != *p_input &&
      RETROFLAT_KEY_LEFT != *p_input
   ) {
      goto cleanup;
   }

   /* Lock text field. */
   assert( NULL == ctl->TEXTBOX.text );
   assert( (MAUG_MHANDLE)NULL != ctl->TEXTBOX.text_h );
   maug_mlock( ctl->TEXTBOX.text_h, ctl->TEXTBOX.text );
   if( NULL == ctl->TEXTBOX.text ) {
      error_printf( "could not lock TEXTBOX text handle!" );
      goto cleanup;
   }

   switch( *p_input ) {
   case RETROFLAT_KEY_BKSP:
      retroflat_buffer_bksp(
         ctl->TEXTBOX.text, ctl->TEXTBOX.text_cur, ctl->TEXTBOX.text_sz )
      break;

   case RETROFLAT_KEY_ENTER:
      idc_out = ctl->base.idc;
      break;

   case RETROFLAT_KEY_LEFT:
      if( 0 < ctl->TEXTBOX.text_cur ) {
         ctl->TEXTBOX.text_cur--;
      }
      break;

   case RETROFLAT_KEY_RIGHT:
      if( ctl->TEXTBOX.text_sz > ctl->TEXTBOX.text_cur ) {
         ctl->TEXTBOX.text_cur++;
      }
      break;

   default:
      assert( ctl->TEXTBOX.text_sz < ctl->TEXTBOX.text_sz_max );
      retroflat_buffer_insert( c, 
         ctl->TEXTBOX.text,
         ctl->TEXTBOX.text_cur,
         ctl->TEXTBOX.text_sz,
         ctl->TEXTBOX.text_sz_max );
      break;
   }

   /* TODO: Remove input from queue? */

cleanup:

   if( NULL != ctl->TEXTBOX.text ) {
      maug_munlock( ctl->TEXTBOX.text_h, ctl->TEXTBOX.text );
   }

#  endif

   return idc_out;
}

static void retrogui_redraw_TEXTBOX(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl
) {
   RETROFLAT_COLOR shadow_color = RETROFLAT_COLOR_DARKGRAY;
   RETROFLAT_COLOR border_color = RETROGUI_COLOR_BORDER;
   retroflat_pxxy_t cursor_x = 0;

   /* Adjust shadow colors for monochrome. */
   if( 2 >= retroflat_screen_colors() ) {
      shadow_color = RETROFLAT_COLOR_BLACK;
      border_color = RETROFLAT_COLOR_BLACK;
   }

#  if defined( RETROGUI_NATIVE_WIN )
   /* Do nothing. */
#  else

   retroflat_2d_rect( gui->draw_bmp, ctl->base.bg_color,
      gui->x + ctl->base.x, gui->y + ctl->base.y,
      ctl->base.w, ctl->base.h, RETROFLAT_FLAGS_FILL );

   /* Draw chiselled inset border. */

   retroflat_2d_rect( gui->draw_bmp, border_color,
      gui->x + ctl->base.x,
      gui->y + ctl->base.y, ctl->base.w, 2,
      RETROFLAT_FLAGS_FILL );

   retroflat_2d_rect( gui->draw_bmp, border_color,
      gui->x + ctl->base.x,
      gui->y + ctl->base.y, 2, ctl->base.h,
      RETROFLAT_FLAGS_FILL );

   retroflat_2d_rect( gui->draw_bmp, shadow_color,
      gui->x + ctl->base.x,
      gui->y + ctl->base.y + ctl->base.h - 1,
      ctl->base.w, 2,
      RETROFLAT_FLAGS_FILL );

   retroflat_2d_rect( gui->draw_bmp, shadow_color,
       gui->x + ctl->base.x + ctl->base.w - 1,
       gui->y + ctl->base.y, 2, ctl->base.h,
      RETROFLAT_FLAGS_FILL );

   /* Draw text. */

   assert( NULL == ctl->TEXTBOX.text );
   maug_mlock( ctl->TEXTBOX.text_h, ctl->TEXTBOX.text );
   if( NULL == ctl->TEXTBOX.text ) {
      goto cleanup;
   }

#ifdef RETROGXC_PRESENT
   retrogxc_string(
      gui->draw_bmp, ctl->base.fg_color, ctl->TEXTBOX.text, 0, gui->font_idx,
      gui->x + ctl->base.x + RETROGUI_PADDING,
      gui->y + ctl->base.y + RETROGUI_PADDING, ctl->base.w, ctl->base.h, 0 );

   retrogxc_string_sz(
      gui->draw_bmp, ctl->TEXTBOX.text, 0, gui->font_idx,
      ctl->base.w, ctl->base.h, &cursor_x, NULL, 0 );
#else
   retrofont_string(
      gui->draw_bmp, ctl->base.fg_color, ctl->TEXTBOX.text, 0, gui->font_h,
      gui->x + ctl->base.x + RETROGUI_PADDING,
      gui->y + ctl->base.y + RETROGUI_PADDING, ctl->base.w, ctl->base.h, 0 );

   retrofont_string_sz(
      gui->draw_bmp, ctl->TEXTBOX.text, 0, gui->font_h,
      ctl->base.w, ctl->base.h, &cursor_x, NULL, 0 );
#endif /* RETROGXC_PRESENT */

cleanup:

   if( NULL != ctl->TEXTBOX.text ) {
      maug_munlock( ctl->TEXTBOX.text_h, ctl->TEXTBOX.text );
   }

   /* TODO: Get cursor color from GUI. */
   /* Use same padding as font for cursor. */
   retroflat_2d_rect( gui->draw_bmp,
      ctl->base.sel_fg,
      gui->x + ctl->base.x + RETROGUI_PADDING + cursor_x,
      gui->y + ctl->base.y + RETROGUI_PADDING,
      8, 8,
      /* Draw blinking cursor. */
      /* TODO: Use a global timer to mark this field dirty. */
      gui->focus == ctl->base.idc &&
         0 < ctl->TEXTBOX.blink_frames ? RETROFLAT_FLAGS_FILL : 0 );

   if( (-1 * RETROGUI_CTL_TEXT_BLINK_FRAMES) > --(ctl->TEXTBOX.blink_frames) ) {
      ctl->TEXTBOX.blink_frames = RETROGUI_CTL_TEXT_BLINK_FRAMES;
   }
   
   gui->flags |= RETROGUI_FLAGS_DIRTY; /* Mark dirty for blink animation. */

#  endif

   return;
}

static MERROR_RETVAL retrogui_push_TEXTBOX( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( RETROGUI_NATIVE_WIN )

   ctl->base.hwnd = CreateWindow(
      "EDIT", 0, WS_CHILD | WS_VISIBLE | WS_BORDER,
      gui->x + ctl->base.x, gui->y + ctl->base.y, ctl->base.w, ctl->base.h,
      g_retroflat_state->window, (HMENU)(ctl->base.idc),
      g_retroflat_instance, NULL );
   if( (HWND)NULL == ctl->base.hwnd ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR, "Error",
         "Could not create textbox: " RETROGUI_IDC_FMT, ctl->base.idc );
      retval = MERROR_GUI;
      goto cleanup;
   }

#  else

   debug_printf( RETROGUI_TRACE_LVL,
      "clearing textbox " RETROGUI_IDC_FMT " buffer...", ctl->base.idc );
   assert( (MAUG_MHANDLE)NULL == ctl->TEXTBOX.text_h );
   ctl->TEXTBOX.text_h = maug_malloc( RETROGUI_CTL_TEXT_SZ_MAX + 1, 1 );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, ctl->TEXTBOX.text_h );
   ctl->TEXTBOX.text_sz_max = RETROGUI_CTL_TEXT_SZ_MAX;

   maug_mlock( ctl->TEXTBOX.text_h, ctl->TEXTBOX.text );
   maug_cleanup_if_null_alloc( char*, ctl->TEXTBOX.text );
   debug_printf( RETROGUI_TRACE_LVL,
      "clearing textbox " RETROGUI_IDC_FMT " buffer...", ctl->base.idc );
   maug_mzero( ctl->TEXTBOX.text, RETROGUI_CTL_TEXT_SZ_MAX + 1 );
   maug_munlock( ctl->TEXTBOX.text_h, ctl->TEXTBOX.text );

#  endif

cleanup:

   return retval;
}

static MERROR_RETVAL retrogui_sz_TEXTBOX(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   retroflat_pxxy_t* p_w, retroflat_pxxy_t* p_h,
   retroflat_pxxy_t max_w, retroflat_pxxy_t max_h
) {
   MERROR_RETVAL retval = MERROR_GUI;
   /* TODO */
   return retval;
}

static MERROR_RETVAL retrogui_pos_TEXTBOX(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   retroflat_pxxy_t x, retroflat_pxxy_t y,
   retroflat_pxxy_t w, retroflat_pxxy_t h
) {
   MERROR_RETVAL retval = MERROR_GUI;
   /* TODO */
   return retval;
}

static void retrogui_destroy_TEXTBOX( union RETROGUI_CTL* ctl ) {
   if( (MAUG_MHANDLE)NULL != ctl->TEXTBOX.text_h ) {
      maug_mfree( ctl->TEXTBOX.text_h );
   }
}

static MERROR_RETVAL retrogui_init_TEXTBOX( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( RETROGUI_TRACE_LVL,
      "initializing textbox " RETROGUI_IDC_FMT "...", ctl->base.idc );

   ctl->base.bg_color = RETROFLAT_COLOR_WHITE;
   ctl->base.sel_bg = RETROFLAT_COLOR_WHITE;
   if( 2 < retroflat_screen_colors() ) {
      ctl->base.sel_fg = RETROFLAT_COLOR_BLUE;
      ctl->base.fg_color = RETROGUI_COLOR_BORDER;
   } else {
      ctl->base.sel_fg = RETROFLAT_COLOR_BLACK;
      ctl->base.fg_color = RETROFLAT_COLOR_BLACK;
   }

   return retval;
}

#endif /* RETROGUI_NO_TEXTBOX */

/* === Control: LABEL === */

static retrogui_idc_t retrogui_click_LABEL(
   struct RETROGUI* gui,
   union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   return RETROGUI_IDC_NONE;
}

static retrogui_idc_t retrogui_key_LABEL(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   return RETROGUI_IDC_NONE;
}

static void retrogui_redraw_LABEL(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl
) {

#  if defined( RETROGUI_NATIVE_WIN )
   /* Do nothing. */
#  else

   /* Draw text. */

#ifdef RETROGXC_PRESENT
   assert( 0 <= gui->font_idx );
#else
   assert( (MAUG_MHANDLE)NULL != gui->font_h );
#endif /* RETROGXC_PRESENT */

   assert( NULL == ctl->LABEL.label );
   maug_mlock( ctl->LABEL.label_h, ctl->LABEL.label );
   if( NULL == ctl->LABEL.label ) {
      error_printf( "could not lock LABEL text!" );
      goto cleanup;
   }

#ifdef RETROGXC_PRESENT
   retrogxc_string(
#else
   retrofont_string(
#endif /* RETROGXC_PRESENT */
      gui->draw_bmp, ctl->base.fg_color, ctl->LABEL.label,
      ctl->LABEL.label_sz,
#ifdef RETROGXC_PRESENT
      gui->font_idx,
#else
      gui->font_h,
#endif /* RETROGXC_PRESENT */
      gui->x + ctl->base.x + RETROGUI_PADDING,
      gui->y + ctl->base.y + RETROGUI_PADDING, ctl->base.w, ctl->base.h,
      ctl->LABEL.font_flags );

cleanup:

   if( NULL != ctl->LABEL.label ) {
      maug_munlock( ctl->LABEL.label_h, ctl->LABEL.label );
   }

#  endif

   return;
}

static MERROR_RETVAL retrogui_push_LABEL( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( RETROGUI_NATIVE_WIN )

   /* TODO */

#  else
   size_t label_sz = 0;
   char* label_tmp = NULL;

   debug_printf( RETROGUI_TRACE_LVL, "pushing LABEL control..." );

   _retrogui_copy_str(
      label, ctl->LABEL.label, ctl->LABEL, label_tmp, label_sz );
   ctl->LABEL.label = NULL;
#  endif

cleanup:

   return retval;
}

static MERROR_RETVAL retrogui_sz_LABEL(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   retroflat_pxxy_t* p_w, retroflat_pxxy_t* p_h,
   retroflat_pxxy_t max_w, retroflat_pxxy_t max_h
) {
   MERROR_RETVAL retval = MERROR_GUI;
   /* TODO */
   return retval;
}

static MERROR_RETVAL retrogui_pos_LABEL(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   retroflat_pxxy_t x, retroflat_pxxy_t y,
   retroflat_pxxy_t w, retroflat_pxxy_t h
) {
   MERROR_RETVAL retval = MERROR_GUI;
   /* TODO */
   return retval;
}

static void retrogui_destroy_LABEL( union RETROGUI_CTL* ctl ) {
   if( (MAUG_MHANDLE)NULL != ctl->LABEL.label_h ) {
      maug_mfree( ctl->LABEL.label_h );
   }
}

static MERROR_RETVAL retrogui_init_LABEL( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( RETROGUI_TRACE_LVL,
      "initializing label " RETROGUI_IDC_FMT "...", ctl->base.idc );

   if( 2 < retroflat_screen_colors() ) {
      ctl->base.fg_color = RETROGUI_COLOR_BORDER;
   } else {
      ctl->base.fg_color = RETROFLAT_COLOR_BLACK;
   }
   ctl->base.bg_color = RETROFLAT_COLOR_WHITE;

   return retval;
}

/* === Control: IMAGE === */

static retrogui_idc_t retrogui_click_IMAGE(
   struct RETROGUI* gui,
   union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   return RETROGUI_IDC_NONE;
}

static retrogui_idc_t retrogui_key_IMAGE(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   return RETROGUI_IDC_NONE;
}

static void retrogui_redraw_IMAGE(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl
) {
#  if defined( RETROGUI_NATIVE_WIN )
   /* Do nothing. */
#  else

#     if defined( RETROGXC_PRESENT )
   if( 0 > ctl->IMAGE.image_cache_id ) {
      return;
   }
   debug_printf( RETROGUI_TRACE_LVL,
      "redrawing image ctl " RETROGUI_IDC_FMT ", cache ID " SSIZE_T_FMT "...",
      ctl->base.idc, ctl->IMAGE.image_cache_id );
   retrogxc_blit_bitmap(
      gui->draw_bmp,
      ctl->IMAGE.image_cache_id,
#     else
   if( !retroflat_2d_bitmap_ok( gui->draw_bmp ) ) {
      return;
   }
   retroflat_2d_blit_bitmap(
      gui->draw_bmp,
      &(ctl->IMAGE.image),
#     endif /* RETROGXC_PRESENT */
      ctl->IMAGE.src_x, ctl->IMAGE.src_y,
      gui->x + ctl->base.x, gui->y + ctl->base.y, ctl->base.w, ctl->base.h,
      ctl->IMAGE.instance );
#  endif

   return;
}

static MERROR_RETVAL retrogui_push_IMAGE( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( RETROGUI_NATIVE_WIN )

   /* TODO */

#  else

   debug_printf( RETROGUI_TRACE_LVL, "pushing IMAGE control..." );

   /* TODO: Copy non-cached image. */

   /* ctl->IMAGE.cache_idx = NULL; */
#  endif

   return retval;
}

static MERROR_RETVAL retrogui_sz_IMAGE(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   retroflat_pxxy_t* p_w, retroflat_pxxy_t* p_h,
   retroflat_pxxy_t max_w, retroflat_pxxy_t max_h
) {
   MERROR_RETVAL retval = MERROR_GUI;
#     ifdef RETROGXC_PRESENT
   retval = retrogxc_bitmap_wh( ctl->IMAGE.image_cache_id, p_w, p_h );
   maug_cleanup_if_not_ok();
#     else
   if( !retroflat_2d_bitmap_ok( &(ctl->IMAGE.image) ) ) {
      error_printf( "image not assigned!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   *p_w = retroflat_2d_bitmap_w( &(ctl->IMAGE.image) );
   *p_h = retroflat_2d_bitmap_h( &(ctl->IMAGE.image) );
#     endif /* RETROGXC_PRESENT */

cleanup:

   return retval;
}

static MERROR_RETVAL retrogui_pos_IMAGE(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   retroflat_pxxy_t x, retroflat_pxxy_t y,
   retroflat_pxxy_t w, retroflat_pxxy_t h
) {
   MERROR_RETVAL retval = MERROR_GUI;
   /* TODO */
   return retval;
}

static void retrogui_destroy_IMAGE( union RETROGUI_CTL* ctl ) {
#  ifndef RETROGXC_PRESENT
   retroflat_2d_destroy_bitmap( &(ctl->IMAGE.image) );
#  endif /* RETROGXC_PRESENT */
}

static MERROR_RETVAL retrogui_init_IMAGE( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( RETROGUI_TRACE_LVL,
      "initializing IMAGE " RETROGUI_IDC_FMT "...", ctl->base.idc );

   return retval;
}

/* === Control: FILLBAR === */

static retrogui_idc_t retrogui_click_FILLBAR( 
   struct RETROGUI* gui,
   union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   retrogui_idc_t idc_out = RETROGUI_IDC_NONE;

   return idc_out;
}

static retrogui_idc_t retrogui_key_FILLBAR( 
   struct RETROGUI* gui, union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   retrogui_idc_t idc_out = RETROGUI_IDC_NONE;

   return idc_out;
}

static void retrogui_redraw_FILLBAR(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl
) {
   retroflat_pxxy_t fill_w = 0;

   if( 0 == ctl->FILLBAR.cur ) {
      fill_w = 0;
   } else {
      fill_w = ctl->base.w * ctl->FILLBAR.cur / ctl->FILLBAR.max;
   }

   retroflat_2d_rect(
      gui->draw_bmp, ctl->base.bg_color, ctl->base.x + fill_w, ctl->base.y,
      ctl->base.w - fill_w, ctl->base.h, RETROFLAT_FLAGS_FILL );

   retroflat_2d_rect(
      gui->draw_bmp, ctl->base.fg_color, ctl->base.x, ctl->base.y,
      fill_w, ctl->base.h, RETROFLAT_FLAGS_FILL );

   return;
}

static MERROR_RETVAL retrogui_push_FILLBAR( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( RETROGUI_NATIVE_WIN )

   /* TODO: Native fillbar implementation. */

#  endif

   return retval;
}

static MERROR_RETVAL retrogui_sz_FILLBAR(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   retroflat_pxxy_t* p_w, retroflat_pxxy_t* p_h,
   retroflat_pxxy_t max_w, retroflat_pxxy_t max_h
) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( NULL != ctl );

   /* TODO? */

   return retval;
}

static MERROR_RETVAL retrogui_pos_FILLBAR(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   retroflat_pxxy_t x, retroflat_pxxy_t y,
   retroflat_pxxy_t w, retroflat_pxxy_t h
) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( RETROGUI_NATIVE_WIN )
   /* TODO */
#  else
   assert( NULL != ctl );

   ctl->base.x = x;
   ctl->base.y = y;
   if( 0 < w ) {
      ctl->base.w = w;
   }
   if( 0 < h ) {
      ctl->base.h = h;
   }
#  endif /* RETROGUI_NATIVE_WIN */

   return retval;
}

static void retrogui_destroy_FILLBAR( union RETROGUI_CTL* ctl ) {
}

static MERROR_RETVAL retrogui_init_FILLBAR( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( RETROGUI_TRACE_LVL,
      "initializing fillbar " RETROGUI_IDC_FMT "...", ctl->base.idc );

   if( 2 < retroflat_screen_colors() ) {
      ctl->base.fg_color = RETROGUI_COLOR_BORDER;
      ctl->base.bg_color = RETROFLAT_COLOR_GRAY;
   } else {
      ctl->base.fg_color = RETROFLAT_COLOR_BLACK;
      ctl->base.bg_color = RETROFLAT_COLOR_WHITE;
   }

   return retval;
}

/* === Static Internal Functions === */

static union RETROGUI_CTL* _retrogui_get_ctl_by_idc(
   struct RETROGUI* gui, size_t idc
) {
   size_t i = 0;
   union RETROGUI_CTL* ctl = NULL;

   assert( mdata_vector_is_locked( &((gui)->ctls) ) );

   for( i = 0 ; mdata_vector_ct( &(gui->ctls) ) > i ; i++ ) {
      ctl = mdata_vector_get( &(gui->ctls), i, union RETROGUI_CTL );
      if( idc == ctl->base.idc ) {
         break;
      }
      ctl = NULL;
   }

   if( NULL == ctl ) {
      error_printf( "could not find GUI item IDC " RETROGUI_IDC_FMT, idc );
   }

   return ctl;
}

/* === */

static MERROR_RETVAL _retrogui_sz_ctl(
   struct RETROGUI* gui, retrogui_idc_t idc,
   retroflat_pxxy_t* p_w, retroflat_pxxy_t* p_h,
   retroflat_pxxy_t max_w, retroflat_pxxy_t max_h
) {
   MERROR_RETVAL retval = MERROR_OK;
   union RETROGUI_CTL* ctl = NULL;

   assert( mdata_vector_is_locked( &((gui)->ctls) ) );

   debug_printf( RETROGUI_TRACE_LVL,
      "sizing control " RETROGUI_IDC_FMT " to: " SIZE_T_FMT "x" SIZE_T_FMT,
      idc, max_w, max_h );

   ctl = _retrogui_get_ctl_by_idc( gui, idc );
   if( NULL == ctl ) {
      /* Elaborate on error from _retrogui_get_ctl_by_idc(). */
      error_printf( "could not size item!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   #define RETROGUI_CTL_TABLE_SZ( idx, c_name, c_fields ) \
      } else if( RETROGUI_CTL_TYPE_ ## c_name == ctl->base.type ) { \
         /* Mark dirty first so redraw can unmark it for animation! */ \
         retval = retrogui_sz_ ## c_name( gui, ctl, p_w, p_h, max_w, max_h ); \
         maug_cleanup_if_not_ok();
   
   if( 0 ) {
   RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_SZ )
   }

   debug_printf( RETROGUI_TRACE_LVL,
      "sized control " RETROGUI_IDC_FMT " at " SIZE_T_FMT "x" SIZE_T_FMT "...",
      ctl->base.idc, ctl->base.w, ctl->base.h );

cleanup:

   return retval;
}

/* === Generic Functions === */

retrogui_idc_t retrogui_poll_ctls( 
   struct RETROGUI* gui, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   size_t i = 0;
   retroflat_pxxy_t mouse_x = 0,
      mouse_y = 0;
   retrogui_idc_t idc_out = RETROGUI_IDC_NONE;
   union RETROGUI_CTL* ctl = NULL;
   MERROR_RETVAL retval = MERROR_OK;

   if( 0 == mdata_vector_ct( &(gui->ctls) ) ) {
      return RETROGUI_IDC_NONE;
   }

   assert( !mdata_vector_is_locked( &((gui)->ctls) ) );
   mdata_vector_lock( &(gui->ctls) );

#  if defined( RETROGUI_NATIVE_WIN )

   if( 0 == g_retroflat_state->last_idc ) {
      /* No WM_COMMAND to process. */
      goto cleanup;
   }

   ctl = retrogui_get_ctl_by_idc( gui, g_retroflat_state->last_idc );
   g_retroflat_state->last_idc = 0;
   if( NULL == ctl ) {
      debug_printf( RETROGUI_TRACE_LVL,
         "invalid IDC: " RETROGUI_IDC_FMT, gui->focus );
      goto cleanup;
   }

#  ifndef  RETROGUI_NO_TEXTBOX
   if( RETROGUI_CTL_TYPE_TEXTBOX == ctl->base.type ) {
      if( SendMessage( ctl->base.hwnd, EM_GETMODIFY, 0, 0 ) ) {
         SendMessage( ctl->base.hwnd, EM_SETMODIFY, 0, 0 );
         debug_printf( RETROGUI_TRACE_LVL, "mod: %d",
            SendMessage( ctl->base.hwnd, EM_GETMODIFY, 0, 0 ) );
      }
   }
#  endif /* !RETROGUI_NO_TEXTBOX */

#  else

   /* Use our cross-platform controls. */

#     define RETROGUI_CTL_TABLE_CLICK( idx, c_name, c_fields ) \
         } else if( RETROGUI_CTL_TYPE_ ## c_name == ctl->base.type ) { \
            gui->flags |= RETROGUI_FLAGS_DIRTY; \
            idc_out = retrogui_click_ ## c_name( gui, ctl, p_input, input_evt );

#     define RETROGUI_CTL_TABLE_KEY( idx, c_name, c_fields ) \
         } else if( RETROGUI_CTL_TYPE_ ## c_name == ctl->base.type ) { \
            gui->flags |= RETROGUI_FLAGS_DIRTY; \
            idc_out = retrogui_key_ ## c_name( gui, ctl, p_input, input_evt );

   if( 0 != *p_input ) {
      debug_printf( RETROGUI_TRACE_LVL,
         "focus " RETROGUI_IDC_FMT " input: %d", gui->focus, *p_input );
   }

   if( 0 == *p_input ) {
      goto reset_debounce;

   } else if(
      retroflat_or_key( *p_input, RETROGUI_KEY_ACTIVATE, RETROGUI_PAD_ACTIVATE )
   ) {

      if( 0 <= gui->focus ) {
         idc_out = gui->focus;
         /* gui->focus = -1; */
         gui->flags |= RETROGUI_FLAGS_DIRTY;
      }

   } else if(
      retroflat_or_key( *p_input, RETROGUI_KEY_NEXT, RETROGUI_PAD_NEXT )
   ) {
      retrogui_focus_next( gui );

      debug_printf( RETROGUI_TRACE_LVL, "next: " RETROGUI_IDC_FMT, gui->focus );

      /* Cleanup after the menu. */
      *p_input = 0;

   } else if(
      retroflat_or_key( *p_input, RETROGUI_KEY_PREV, RETROGUI_PAD_PREV )
   ) {
      retrogui_focus_prev( gui );

      debug_printf( RETROGUI_TRACE_LVL, "prev: " RETROGUI_IDC_FMT, gui->focus );

      /* Cleanup after the menu. */
      *p_input = 0;

#     ifndef RETROGUI_NO_MOUSE
   } else if(
      RETROFLAT_MOUSE_B_LEFT == *p_input ||
      RETROFLAT_MOUSE_B_RIGHT == *p_input
   ) {
      /* Handle mouse input. */

      /* Remove all focus before testing if a new control has focus. */
      debug_printf( RETROGUI_TRACE_LVL, "resetting focus for mouse click..." );
      gui->focus = RETROGUI_IDC_NONE;

      mouse_x = input_evt->mouse_x - gui->x;
      mouse_y = input_evt->mouse_y - gui->y;

      for( i = 0 ; mdata_vector_ct( &(gui->ctls) ) > i ; i++ ) {
         ctl = mdata_vector_get( &(gui->ctls), i, union RETROGUI_CTL );
         if(
            mouse_x < ctl->base.x ||
            mouse_y < ctl->base.y ||
            mouse_x > ctl->base.x + ctl->base.w ||
            mouse_y > ctl->base.y + ctl->base.h
         ) {
            continue;
         }

         if( gui->idc_prev == ctl->base.idc ) {
            /* No repeated clicks! */
            /* TODO: Allow exceptions for e.g. scrollbars. */
            idc_out = RETROGUI_IDC_NONE;
            goto cleanup;
         }

         gui->idc_prev = ctl->base.idc;

         debug_printf( RETROGUI_TRACE_LVL,
            "setting focus to clicked control: " RETROGUI_IDC_FMT,
            ctl->base.idc );
         gui->focus = ctl->base.idc;

         if( 0 ) {
         RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_CLICK )
         }
         break;
      }
#     endif /* !RETROGUI_NO_MOUSE */

   } else {

      if( RETROGUI_IDC_NONE == gui->focus ) {
         goto reset_debounce;
      }

      /* Send keystrokes to control that has focus. */

      ctl = _retrogui_get_ctl_by_idc( gui, gui->focus );
      if( NULL == ctl ) {
         /* This is a debug message because an invalid focus isn't necessarily
          * a game-over situation...
          */
         debug_printf( RETROGUI_TRACE_LVL,
            "invalid focus IDC: " RETROGUI_IDC_FMT, gui->focus );
         goto reset_debounce;
      }

      if( 0 ) {
      RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_KEY )
      }
   }

reset_debounce:

   /* Reset repeat detector. */
   gui->idc_prev = RETROGUI_IDC_NONE;

#  endif

cleanup:

   if( MERROR_OK != retval ) {
      idc_out = RETROGUI_IDC_NONE;
   }

   mdata_vector_unlock( &(gui->ctls) );

   return idc_out;
}

/* === */

MERROR_RETVAL retrogui_redraw_ctls( struct RETROGUI* gui ) {
   size_t i = 0;
   union RETROGUI_CTL* ctl = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   int autolock = 0;

   /* OpenGL tends to call glClear on every frame, so always redraw! */
   if( RETROGUI_FLAGS_DIRTY != (RETROGUI_FLAGS_DIRTY & gui->flags) ) {
      /* Shortcut! */
      return MERROR_OK;
   }

   if( 0 == mdata_vector_ct( &(gui->ctls) ) ) {
      return MERROR_OK;
   }

   if( !mdata_vector_is_locked( &((gui)->ctls) ) ) {
      mdata_vector_lock( &(gui->ctls) );
      autolock = 1;
   }

   if(
      RETROFLAT_COLOR_BLACK != gui->bg_color &&
      0 < gui->w && 0 < gui->h
   ) {
      retroflat_2d_rect( gui->draw_bmp,
         gui->bg_color, gui->x, gui->y, gui->w, gui->h, RETROFLAT_FLAGS_FILL );
   }

   #define RETROGUI_CTL_TABLE_REDRAW( idx, c_name, c_fields ) \
      } else if( RETROGUI_CTL_TYPE_ ## c_name == ctl->base.type ) { \
         /* Mark dirty first so redraw can unmark it for animation! */ \
         gui->flags &= ~RETROGUI_FLAGS_DIRTY; \
         retrogui_redraw_ ## c_name( gui, ctl );

   for( i = 0 ; mdata_vector_ct( &(gui->ctls) ) > i ; i++ ) {
      ctl = mdata_vector_get( &(gui->ctls), i, union RETROGUI_CTL );
      if( 0 ) {
      RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_REDRAW )
      }
   }

cleanup:

   if( autolock ) {
      mdata_vector_unlock( &(gui->ctls) );
   }

   return retval;
}

/* === */

MERROR_RETVAL retrogui_pos_ctl(
   struct RETROGUI* gui, retrogui_idc_t idc,
   size_t x, size_t y, size_t w, size_t h
) {
   MERROR_RETVAL retval = MERROR_OK;
   union RETROGUI_CTL* ctl = NULL;
   int autolock = 0;

   if( !mdata_vector_is_locked( &((gui)->ctls) ) ) {
      mdata_vector_lock( &(gui->ctls) );
      autolock = 1;
   }

   debug_printf( RETROGUI_TRACE_LVL,
      "moving control " RETROGUI_IDC_FMT " to: " SIZE_T_FMT ", " SIZE_T_FMT,
      idc, x, y );

   ctl = _retrogui_get_ctl_by_idc( gui, idc );
   if( NULL == ctl ) {
      /* Elaborate on error from _retrogui_get_ctl_by_idc(). */
      error_printf( "could not position control!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   #define RETROGUI_CTL_TABLE_POS( idx, c_name, c_fields ) \
      } else if( RETROGUI_CTL_TYPE_ ## c_name == ctl->base.type ) { \
         /* Mark dirty first so redraw can unmark it for animation! */ \
         retval = retrogui_pos_ ## c_name( gui, ctl, x, y, w, h ); \
         maug_cleanup_if_not_ok();
   
   if( 0 ) {
   RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_POS )
   }

   debug_printf( RETROGUI_TRACE_LVL,
      "moved control " RETROGUI_IDC_FMT " to " SIZE_T_FMT ", " SIZE_T_FMT
         " and sized to " SIZE_T_FMT "x" SIZE_T_FMT "...",
      ctl->base.idc, gui->x + ctl->base.x, gui->y + ctl->base.y,
         ctl->base.w, ctl->base.h );

   /* New position! Redraw! */
   gui->flags |= RETROGUI_FLAGS_DIRTY;

cleanup:

   if( autolock ) {
      mdata_vector_unlock( &(gui->ctls) );
   }

   return retval;

}

/* === */

MERROR_RETVAL retrogui_push_ctl(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl
) {
   MERROR_RETVAL retval = MERROR_OK;
   int autolock = 0;

   assert( 0 < ctl->base.idc );

#ifdef RETROGXC_PRESENT
   if( 0 > gui->font_idx ) {
#else
   if( (MAUG_MHANDLE)NULL == gui->font_h ) {
#endif /* RETROGXC_PRESENT */
      retroflat_message(
         RETROFLAT_MSG_FLAG_ERROR, "Error", "GUI font not loaded!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   /* TODO: Hunt for control IDC and fail if duplicate found! */

   debug_printf( RETROGUI_TRACE_LVL,
      "gui->ctls_ct: " SIZE_T_FMT, mdata_vector_ct( &(gui->ctls) ) );

   if(
      RETROGUI_CTL_TYPE_IMAGE != ctl->base.type &&
      RETROFLAT_COLOR_NULL == ctl->base.bg_color
   ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR, "Error",
         "invalid background color specified for control " RETROGUI_IDC_FMT "!",
         ctl->base.idc );
      retval = MERROR_GUI;
      goto cleanup;

   }

   if(
      RETROGUI_CTL_TYPE_IMAGE != ctl->base.type &&
      RETROFLAT_COLOR_NULL == ctl->base.fg_color
   ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR, "Error",
         "invalid foreground color specified for control " RETROGUI_IDC_FMT "!",
         ctl->base.idc );
      retval = MERROR_GUI;
      goto cleanup;
   }

   /* Perform the actual push. */

#ifdef RETROGUI_TRACE_TOKENS
   debug_printf( RETROGUI_TRACE_LVL,
      "pushing %s " RETROGUI_IDC_FMT " to slot " SIZE_T_FMT "...",
      gc_retrogui_ctl_names[ctl->base.type], ctl->base.idc,
      mdata_vector_ct( &(gui->ctls) ) );
#else
   debug_printf( RETROGUI_TRACE_LVL,
      "pushing control type %d, " RETROGUI_IDC_FMT " to slot " SIZE_T_FMT "...",
      ctl->base.type, ctl->base.idc, mdata_vector_ct( &(gui->ctls) ) );
#endif /* RETROGUI_TRACE_TOKENS */

   mdata_vector_append( &(gui->ctls), ctl, sizeof( union RETROGUI_CTL ) );

   gui->flags |= RETROGUI_FLAGS_DIRTY;

   /* Now that append is done, lock the vector and grab a pointer to our
    * newly-pushed control to run some fixups on.
    */
   if( !mdata_vector_is_locked( &((gui)->ctls) ) ) {
      mdata_vector_lock( &(gui->ctls) );
      autolock = 1;
   }

   /* TODO: More elegant way to grab index. */
   ctl = mdata_vector_get_last( &(gui->ctls),
      union RETROGUI_CTL );
   assert( NULL != ctl );

   #define RETROGUI_CTL_TABLE_PUSH( idx, c_name, c_fields ) \
      } else if( RETROGUI_CTL_TYPE_ ## c_name == ctl->base.type ) { \
         debug_printf( RETROGUI_TRACE_LVL, \
            "running " #c_name " push hook..." ); \
         retval = retrogui_push_ ## c_name( ctl ); \
         maug_cleanup_if_not_ok();

   if( 0 ) {
   RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_PUSH )
   }

   /* Try to auto-size the control now that the push-hook as set its text
    * or whatever else might be needed to determine an automatic size.
    */
   if( 0 == ctl->base.w || 0 == ctl->base.h ) {
#ifdef RETROGUI_TRACE_TOKENS
      debug_printf( RETROGUI_TRACE_LVL,
         "determining size for new %s control " RETROGUI_IDC_FMT "...",
         gc_retrogui_ctl_names[ctl->base.type], ctl->base.idc );
#else
      debug_printf( RETROGUI_TRACE_LVL,
         "determining size for new control type %d, " RETROGUI_IDC_FMT "...",
         ctl->base.type, ctl->base.idc );
#endif /* RETROGUI_TRACE_TOKENS */
      retval = _retrogui_sz_ctl(
         gui, ctl->base.idc, &(ctl->base.w), &(ctl->base.h), 0, 0 );
      maug_cleanup_if_not_ok();
   }

   if( 0 > gui->focus && retrogui_can_focus_ctl( ctl ) ) {
      debug_printf( RETROGUI_TRACE_LVL,
         "setting focus to control: " RETROGUI_IDC_FMT, ctl->base.idc );
      gui->focus = ctl->base.idc;
   }

cleanup:

   if( autolock ) {
      mdata_vector_unlock( &(gui->ctls) );
   }

   return retval;
}

/* === */

MERROR_RETVAL retrogui_remove_ctl( struct RETROGUI* gui, retrogui_idc_t idc ) {
   size_t i = 0;
   union RETROGUI_CTL* ctl = NULL;
   MERROR_RETVAL retval = MERROR_OK;

   if( mdata_vector_is_locked( &((gui)->ctls) ) ) {
      error_printf( "GUI is locked!" );
      goto cleanup;
   }

   assert( !mdata_vector_is_locked( &((gui)->ctls) ) );
   mdata_vector_lock( &(gui->ctls) );

   #define RETROGUI_CTL_TABLE_FREE_CTL( idx, c_name, c_fields ) \
      } else if( RETROGUI_CTL_TYPE_ ## c_name == ctl->base.type ) { \
         retrogui_destroy_ ## c_name( ctl );

   for( i = 0 ; mdata_vector_ct( &(gui->ctls) ) > i ; i++ ) {
      ctl = mdata_vector_get( &(gui->ctls), i, union RETROGUI_CTL );
      if( idc != ctl->base.idc ) {
         continue;
      }

      /* Free the control data. */
      if( 0 ) {
      RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_FREE_CTL )
      }

      /* Remove the control. */
      mdata_vector_unlock( &(gui->ctls) );
      mdata_vector_remove( &(gui->ctls), i );
      mdata_vector_lock( &(gui->ctls) );
      break;
   }

   mdata_vector_unlock( &(gui->ctls) );

cleanup:

   return retval;
}

/* === */

MERROR_RETVAL retrogui_set_font(
   struct RETROGUI* gui, const char* font_path
) {
   MERROR_RETVAL retval = MERROR_OK;

#ifdef RETROGXC_PRESENT
   gui->font_idx = retrogxc_load_font( font_path, 0, 33, 93 );
   maug_cleanup_if_lt(
      gui->font_idx, (ssize_t)0, SSIZE_T_FMT, MERROR_GUI );
#else
   retval = retrofont_load( font_path, &(gui->font_h), 0, 33, 93 );
   maug_cleanup_if_not_ok();
#endif /* RETROGXC_PRESENT */

   gui->flags |= RETROGUI_FLAGS_FONT_OWNED;

cleanup:

   return retval;
}

#ifndef RETROGUI_NO_TEXTBOX

MERROR_RETVAL retrogui_get_ctl_text(
   struct RETROGUI* gui, retrogui_idc_t idc, char* buffer, size_t buffer_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   union RETROGUI_CTL* ctl = NULL;
   int autolock = 0;

   if( !mdata_vector_is_locked( &((gui)->ctls) ) ) {
      mdata_vector_lock( &(gui->ctls) );
      autolock = 1;
   }

   ctl = _retrogui_get_ctl_by_idc( gui, idc );
   if( NULL == ctl ) {
      /* Elaborate on error from _retrogui_get_ctl_by_idc(). */
      error_printf( "could not get control text!" );
      retval = MERROR_GUI;
      goto cleanup;
   }
   
   if( RETROGUI_CTL_TYPE_TEXTBOX == ctl->base.type ) {
#  if defined( RETROGUI_NATIVE_WIN )
      /* TODO */
#else
      maug_mlock( ctl->TEXTBOX.text_h, ctl->TEXTBOX.text );
      maug_cleanup_if_null_lock( char*, ctl->TEXTBOX.text );

      maug_strncpy( buffer, ctl->TEXTBOX.text, buffer_sz );
#  endif
   } else if( RETROGUI_CTL_TYPE_LABEL == ctl->base.type ) {
#  if defined( RETROGUI_NATIVE_WIN )
      /* TODO */
#else
      maug_mlock( ctl->LABEL.label_h, ctl->LABEL.label );
      maug_cleanup_if_null_lock( char*, ctl->LABEL.label );

      maug_strncpy( buffer, ctl->LABEL.label, buffer_sz );
#  endif

   }

cleanup:

   if( RETROGUI_CTL_TYPE_TEXTBOX == ctl->base.type ) {
      if( NULL != ctl->TEXTBOX.text ) {
         maug_munlock( ctl->TEXTBOX.text_h, ctl->TEXTBOX.text );
      }

   } else if( RETROGUI_CTL_TYPE_LABEL == ctl->base.type ) {
      if( NULL != ctl->LABEL.label ) {
         maug_munlock( ctl->LABEL.label_h, ctl->LABEL.label );
      }
   }

   if( autolock ) {
      mdata_vector_unlock( &(gui->ctls) );
   }

   return retval;
}

#endif /* !RETROGUI_NO_TEXTBOX */

/* === */

ssize_t retrogui_get_ctl_sel_idx( struct RETROGUI* gui, retrogui_idc_t idc ) {
   ssize_t idx = -1;
   union RETROGUI_CTL* ctl = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   int autolock = 0;

   if( !mdata_vector_is_locked( &((gui)->ctls) ) ) {
      mdata_vector_lock( &(gui->ctls) );
      autolock = 1;
   }

   ctl = _retrogui_get_ctl_by_idc( gui, idc );
   if( NULL == ctl ) {
      /* Elaborate on error from _retrogui_get_ctl_by_idc(). */
      error_printf( "could not get control selection!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   assert( RETROGUI_CTL_TYPE_LISTBOX == ctl->base.type );

#  if defined( RETROGUI_NATIVE_WIN )
   idx = SendMessage( ctl->base.hwnd, LB_GETCARETINDEX, 0, 0 );
#  else
   idx = ctl->LISTBOX.sel_idx;
#  endif

cleanup:

   if( autolock ) {
      mdata_vector_unlock( &(gui->ctls) );
   }

   if( MERROR_OK != retval ) {
      idx = -1 * retval;
   }

   return idx;
}

/* === */

MERROR_RETVAL retrogui_set_ctl_color(
   struct RETROGUI* gui, retrogui_idc_t idc, uint8_t color_key,
   RETROFLAT_COLOR color_val
) {
   MERROR_RETVAL retval = MERROR_OK;
   union RETROGUI_CTL* ctl = NULL;

   assert( !mdata_vector_is_locked( &((gui)->ctls) ) );
   mdata_vector_lock( &(gui->ctls) );

   debug_printf( RETROGUI_TRACE_LVL,
      "setting control " RETROGUI_IDC_FMT " color %u to: %d",
      idc, color_key, color_val );

   /* Figure out the control to update. */
   ctl = _retrogui_get_ctl_by_idc( gui, idc );
   if( NULL == ctl ) {
      /* Elaborate on error from _retrogui_get_ctl_by_idc(). */
      error_printf( "could not set control color!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   switch( color_key ) {
   case RETROGUI_COLOR_BG: ctl->base.bg_color = color_val; break;
   case RETROGUI_COLOR_FG: ctl->base.fg_color = color_val; break;
   case RETROGUI_COLOR_SEL_BG: ctl->base.sel_bg = color_val; break;
   case RETROGUI_COLOR_SEL_FG: ctl->base.sel_fg = color_val; break;

   default:
      error_printf( "invalid color key specified: %u", color_key );
      break;
   }

cleanup:

   mdata_vector_unlock( &(gui->ctls) );

   return retval;
}

/* === */

MERROR_RETVAL retrogui_set_ctl_text(
   struct RETROGUI* gui, retrogui_idc_t idc, size_t buffer_sz,
   const char* fmt, ...
) {
   MERROR_RETVAL retval = MERROR_OK;
   size_t label_sz = 0;
   char* label_tmp = NULL;
   char* buffer = NULL;
   union RETROGUI_CTL* ctl = NULL;
   MAUG_MHANDLE buffer_h = (MAUG_MHANDLE)NULL;
   va_list args;

   assert( !mdata_vector_is_locked( &((gui)->ctls) ) );
   mdata_vector_lock( &(gui->ctls) );

   debug_printf( RETROGUI_TRACE_LVL,
      "setting control " RETROGUI_IDC_FMT " text to: %s", idc, fmt );

   /* Figure out the control to update. */
   ctl = _retrogui_get_ctl_by_idc( gui, idc );
   if( NULL == ctl ) {
      /* Elaborate on error from _retrogui_get_ctl_by_idc(). */
      error_printf( "could not set control text!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   /* Perform the buffer substitutions. */
   buffer_h = maug_malloc( 1, buffer_sz + 1 );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, buffer_h );

   assert( 0 < buffer_sz );

   maug_mlock( buffer_h, buffer );
   maug_cleanup_if_null_lock( char*, buffer );
   maug_mzero( buffer, buffer_sz + 1 );

   assert( NULL != buffer );

   if( NULL == fmt ) {
      /* Zero the buffer. */
      maug_mzero( buffer, buffer_sz + 1);

   } else {
      /* Format the buffer. */
      va_start( args, fmt );
      maug_vsnprintf( buffer, buffer_sz, fmt, args );
      va_end( args );
   }

   /* Perform the actual update. */
   if( RETROGUI_CTL_TYPE_BUTTON == ctl->base.type ) {
      assert( NULL == ctl->BUTTON.label );
      _retrogui_copy_str( label, buffer, ctl->BUTTON, label_tmp, buffer_sz );
   } else if( RETROGUI_CTL_TYPE_LABEL == ctl->base.type ) {
      assert( NULL == ctl->LABEL.label );
      _retrogui_copy_str( label, buffer, ctl->LABEL, label_tmp, label_sz );
#ifndef RETROGUI_NO_TEXTBOX
   } else if( RETROGUI_CTL_TYPE_TEXTBOX == ctl->base.type ) {
      assert( NULL == ctl->TEXTBOX.text );
      /* This must always be the same and an lvalue! */
      label_sz = RETROGUI_CTL_TEXT_SZ_MAX;
      _retrogui_copy_str(
         text, buffer, ctl->TEXTBOX, label_tmp, label_sz );
      ctl->TEXTBOX.text_cur = 0;
#endif /* !RETROGUI_NO_TEXTBOX */
   } else {
      error_printf( "invalid control type! no label!" );
      goto cleanup;
   }

   /* New text! Redraw! */
   gui->flags |= RETROGUI_FLAGS_DIRTY;

cleanup:

   if( NULL != buffer ) {
      maug_munlock( buffer_h, buffer );
   }

   if( (MAUG_MHANDLE)NULL != buffer_h ) {
      maug_mfree( buffer_h );
   }

   mdata_vector_unlock( &(gui->ctls) );

   return retval;
}

/* === */

MERROR_RETVAL retrogui_set_ctl_image(
   struct RETROGUI* gui, retrogui_idc_t idc, const char* path, uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   union RETROGUI_CTL* ctl = NULL;
   int autolock = 0;

   if( !mdata_vector_is_locked( &((gui)->ctls) ) ) {
      mdata_vector_lock( &(gui->ctls) );
      autolock = 1;
   }

   debug_printf( RETROGUI_TRACE_LVL,
      "setting control " RETROGUI_IDC_FMT " image to: %s", idc, path );

   /* Figure out the control to update. */
   ctl = _retrogui_get_ctl_by_idc( gui, idc );
   if( NULL == ctl ) {
      /* Elaborate on error from _retrogui_get_ctl_by_idc(). */
      error_printf( "could not set control image!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   /* Perform the actual update. */
   if( RETROGUI_CTL_TYPE_IMAGE == ctl->base.type ) {
      if( NULL != path ) {
#  if defined( RETROGXC_PRESENT )
         ctl->IMAGE.image_cache_id = retrogxc_load_bitmap( path, flags );
#  else
         retroflat_2d_load_bitmap( path, &(ctl->IMAGE.image), flags );
#  endif /* RETROGXC_PRESENT */
      } else {
#  ifdef RETROGXC_PRESENT
         ctl->IMAGE.image_cache_id = -1;
#  else
         retroflat_2d_destroy_bitmap( &(ctl->IMAGE.image) );
#  endif /* RETROGXC_PRESENT */
      }
   } else {
      error_printf( "invalid control type! no image!" );
      goto cleanup;
   }

   /* New text! Redraw! */
   gui->flags |= RETROGUI_FLAGS_DIRTY;

cleanup:

   if( autolock ) {
      mdata_vector_unlock( &(gui->ctls) );
   }

   return retval;
}

/* === */

MERROR_RETVAL retrogui_set_ctl_level(
   struct RETROGUI* gui, retrogui_idc_t idc, uint16_t level, uint16_t max,
   uint8_t flags
) {
   MERROR_RETVAL retval = MERROR_OK;
   union RETROGUI_CTL* ctl = NULL;
   int autolock = 0;

   if( !mdata_vector_is_locked( &((gui)->ctls) ) ) {
      mdata_vector_lock( &(gui->ctls) );
      autolock = 1;
   }

   debug_printf( RETROGUI_TRACE_LVL,
      "setting control " RETROGUI_IDC_FMT " level to: %u", idc, level  );

   /* Figure out the control to update. */
   ctl = _retrogui_get_ctl_by_idc( gui, idc );
   if( NULL == ctl ) {
      /* Elaborate on error from _retrogui_get_ctl_by_idc(). */
      error_printf( "could not set control level!" );
      retval = MERROR_GUI;
      goto cleanup;
   }

   /* Perform the actual update. */
   if( RETROGUI_CTL_TYPE_FILLBAR == ctl->base.type ) {
      ctl->FILLBAR.cur = level;
      if( 0 < max ) {
         ctl->FILLBAR.max = max;
      }
   } else {
      error_printf( "invalid control type! no level!" );
      goto cleanup;
   }

   /* New level! Redraw! */
   gui->flags |= RETROGUI_FLAGS_DIRTY;

cleanup:

   if( autolock ) {
      mdata_vector_unlock( &(gui->ctls) );
   }

   return retval;
}

/* === */

MERROR_RETVAL retrogui_init_ctl(
   union RETROGUI_CTL* ctl, uint8_t type, size_t idc
) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( RETROGUI_TRACE_LVL,
      "initializing control base " RETROGUI_IDC_FMT "...", idc );

   maug_mzero( ctl, sizeof( union RETROGUI_CTL ) );

   ctl->base.type = type;
   ctl->base.idc = idc;
   ctl->base.fg_color = RETROFLAT_COLOR_NULL;
   ctl->base.bg_color = RETROFLAT_COLOR_NULL;
   ctl->base.sel_fg = RETROFLAT_COLOR_NULL;
   ctl->base.sel_bg = RETROFLAT_COLOR_NULL;

   #define RETROGUI_CTL_TABLE_INITS( idx, c_name, c_fields ) \
      } else if( RETROGUI_CTL_TYPE_ ## c_name == ctl->base.type ) { \
         retrogui_init_ ## c_name( ctl );

   if( 0 ) {
   RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_INITS )
   }

#  ifdef RETROGXC_PRESENT
   if( RETROGUI_CTL_TYPE_IMAGE == type ) {
      ctl->IMAGE.image_cache_id = -1;
   }
#  endif /* !RETROGXC_PRESENT */

   return retval;
}

/* === */

MERROR_RETVAL retrogui_destroy( struct RETROGUI* gui ) {
   size_t i = 0;
   union RETROGUI_CTL* ctl = NULL;
   MERROR_RETVAL retval = MERROR_OK;

   if( mdata_vector_is_locked( &((gui)->ctls) ) ) {
      error_printf( "GUI is locked!" );
      goto cleanup;
   }

   if( 0 == mdata_vector_ct( &(gui->ctls) ) ) {
      goto cleanup;
   }

   assert( !mdata_vector_is_locked( &((gui)->ctls) ) );
   mdata_vector_lock( &(gui->ctls) );

   #define RETROGUI_CTL_TABLE_FREE( idx, c_name, c_fields ) \
      } else if( RETROGUI_CTL_TYPE_ ## c_name == ctl->base.type ) { \
         retrogui_destroy_ ## c_name( ctl );

   for( i = 0 ; mdata_vector_ct( &(gui->ctls) ) > i ; i++ ) {
      ctl = mdata_vector_get( &(gui->ctls), i, union RETROGUI_CTL );
      if( 0 ) {
      RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_FREE )
      }
   }

   mdata_vector_unlock( &(gui->ctls) );

#  ifndef RETROGXC_PRESENT
   if( RETROGUI_FLAGS_FONT_OWNED == (RETROGUI_FLAGS_FONT_OWNED & gui->flags) ) {
      maug_mfree( gui->font_h );
   }
#  endif /* !RETROGXC_PRESENT */

cleanup:

   mdata_vector_free( &(gui->ctls) );

   return retval;
}

/* === */

retrogui_idc_t retrogui_focus_iter(
   struct RETROGUI* gui, size_t start, ssize_t incr
) {
   retrogui_idc_t idc_out = RETROGUI_IDC_NONE;
   union RETROGUI_CTL* ctl = NULL;
   MERROR_RETVAL retval = MERROR_OK;
   retrogui_idc_t i = 0;
   ssize_t i_before = -1; /* Index of the current selected IDC. */
   int autolock = 0;

   if( 0 == mdata_vector_ct( &(gui->ctls) ) ) {
      goto cleanup;
   }

   if( !mdata_vector_is_locked( &((gui)->ctls) ) ) {
      mdata_vector_lock( &(gui->ctls) );
      autolock = 1;
   }

   /* Find the currently selected IDC. */
   for(
      i = start ; mdata_vector_ct( &(gui->ctls) ) > i && 0 <= i ; i += incr
   ) {
      ctl = mdata_vector_get( &(gui->ctls), i, union RETROGUI_CTL );
      if( !retrogui_can_focus_ctl( ctl ) ) {
         continue;
      } else if( RETROGUI_IDC_NONE == gui->focus || 0 <= i_before ) {
         /* We're primed to set the new focus, so do that and finish. */
         idc_out = ctl->base.idc;
         debug_printf( RETROGUI_TRACE_LVL,
            "moving focus to control: " RETROGUI_IDC_FMT, idc_out );
         gui->focus = idc_out;
         goto cleanup;

      } else if( ctl->base.idc == gui->focus ) {
         /* We've found the current focus, so prime to select the new focus. */
         i_before = i;
      }
   }

   /* We didn't select a focus in the loop above, so we must be wrapping around!
    */

   /* Select the next IDC. */
   if( 0 > i ) {
      /* Wrap around to last SELECTABLE item. */
      for( i = mdata_vector_ct( &(gui->ctls) ) - 1 ; 0 <= i ; i-- ) {
         ctl = mdata_vector_get( &(gui->ctls), i, union RETROGUI_CTL );
         if( !retrogui_can_focus_ctl( ctl ) ) {
            /* Skip NON-SELECTABLE items! */
            debug_printf(
               RETROGUI_TRACE_LVL, "skipping: " RETROGUI_IDC_FMT, i );
            continue;
         } else {
            idc_out = ctl->base.idc;
            debug_printf( RETROGUI_TRACE_LVL,
               "moving focus to control: " RETROGUI_IDC_FMT, idc_out );
            gui->focus = idc_out;
            break;
         }
      }

   } else if( mdata_vector_ct( &(gui->ctls) ) <= i ) {
      /* Wrap around to first SELECTABLE item. */
      for( i = 0 ; mdata_vector_ct( &(gui->ctls) ) > i ; i++ ) {
         ctl = mdata_vector_get( &(gui->ctls), i, union RETROGUI_CTL );
         if( !retrogui_can_focus_ctl( ctl ) ) {
            /* Skip NON-SELECTABLE items! */
            debug_printf(
               RETROGUI_TRACE_LVL, "skipping: " RETROGUI_IDC_FMT, i );
            continue;
         } else {
            idc_out = ctl->base.idc;
            debug_printf( RETROGUI_TRACE_LVL,
               "moving focus to control: " RETROGUI_IDC_FMT, idc_out );
            gui->focus = idc_out;
            break;
         }
      }

   } else {
      error_printf( "invalid focus: " RETROGUI_IDC_FMT, i );

   }

cleanup:

   /* New focus! Dirty! */
   gui->flags |= RETROGUI_FLAGS_DIRTY;

   if( MERROR_OK != retval ) {
      idc_out = merror_retval_to_sz( retval );
   }

   if( autolock ) {
      mdata_vector_unlock( &(gui->ctls) );
   }

   debug_printf(
      RETROGUI_TRACE_LVL, "selected IDC: " RETROGUI_IDC_FMT, idc_out );

   return idc_out;
}

/* === */

MERROR_RETVAL retrogui_init( struct RETROGUI* gui ) {
   MERROR_RETVAL retval = MERROR_OK;

   maug_mzero( gui, sizeof( struct RETROGUI ) );

   gui->bg_color = RETROFLAT_COLOR_BLACK;
   gui->focus = RETROGUI_IDC_NONE;

   debug_printf( RETROGUI_TRACE_LVL, "initialized GUI" );

   return retval;
}

#else

#define RETROGUI_CTL_TABLE_CONSTS( idx, c_name, c_fields ) \
   extern MAUG_CONST uint8_t SEG_MCONST RETROGUI_CTL_TYPE_ ## c_name;

RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_CONSTS )

#ifdef RETROGUI_TRACE_TOKENS
extern MAUG_CONST char* gc_retrogui_ctl_names[];
#endif /* RETROGUI_TRACE_TOKENS */

#endif /* RETROGUI_C */

/*! \} */ /* maug_retrogui */

/*! \} */ /* maug_retroflt */

#endif /* !RETROGUI_H */

