
#ifndef RETROGUI_H
#define RETROGUI_H

/**
 * \addtogroup maug_retrogui RetroGUI API
 * \{
 * \file retrogui.h
 */

#ifndef RETROFONT_PRESENT
#  error "retrofont not present!"
#endif /* !RETROFONT_PRESENT */

/*! \brief RETROGUI::flags indicating controls should be redrawn. */
#define RETROGUI_FLAGS_DIRTY 0x01

#ifndef RETROGUI_TRACE_LVL
#  define RETROGUI_TRACE_LVL 0
#endif /* !RETROGUI_TRACE_LVL */

#ifndef RETROGUI_CTL_TEXT_SZ_MAX
#  define RETROGUI_CTL_TEXT_SZ_MAX 128
#endif /* !RETROGUI_CTL_TEXT_SZ_MAX */

#ifndef RETROGUI_CTL_SZ_MAX_INIT
#  define RETROGUI_CTL_SZ_MAX_INIT 10
#endif /* !RETROGUI_CTL_SZ_MAX_INIT */

#ifndef RETROGUI_PADDING
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

#define retrogui_lock( gui ) \
   if( NULL == (gui)->ctls ) { \
      maug_mlock( (gui)->ctls_h, (gui)->ctls ); \
      maug_cleanup_if_null_alloc( union RETROGUI_CTL*, (gui)->ctls ); \
   }

#define retrogui_unlock( gui ) \
   if( NULL != (gui)->ctls ) { \
      maug_munlock( (gui)->ctls_h, (gui)->ctls ); \
   }

#define retrogui_is_locked( gui ) (NULL != (gui)->ctls)

#define retrogui_copy_label( ctl, ctl_type ) \
   size_t label_sz = 0; \
   char* label_tmp = NULL; \
   \
   /* Sanity checking. */ \
   assert( NULL != ctl->ctl_type.label ); \
   label_sz = strlen( ctl->ctl_type.label ); \
   assert( 0 < label_sz ); \
   assert( (MAUG_MHANDLE)NULL == ctl->ctl_type.label_h ); \
   \
   /* Allocate new label space. */ \
   ctl->ctl_type.label_h = maug_malloc( label_sz + 1, 1 ); \
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, ctl->ctl_type.label_h ); \
   maug_mlock( ctl->ctl_type.label_h, label_tmp ); \
   maug_cleanup_if_null_alloc( char*, label_tmp ); \
   \
   /* Copy the label text over. */ \
   maug_mzero( label_tmp, label_sz + 1 ); \
   strncpy( label_tmp, ctl->ctl_type.label, label_sz ); \
   maug_munlock( ctl->ctl_type.label_h, label_tmp ); \
   ctl->ctl_type.label = NULL;

/*! \brief Unique identifying constant number for controls. */
typedef size_t RETROGUI_IDC;

#define RETROGUI_IDC_NONE 0

/**
 * \addtogroup maug_retrogui_ctl RetroGUI Controls
 * \{
 */

/**
 * \brief Table defining all control types and their specific fields in
 *        ::RETROGUI_CTL.
 *
 * All entries in this table have corresponding retrogui_redraw_*,
 * retrogui_poll_*, retrogui_init_*, and retrogui_push_* functions which are
 * called internally from retrogui_redraw_ctls(), retrogui_poll_ctls(),
 * retrogui_init_ctl(), and retrogui_push_ctl(), respectively.
 */
#define RETROGUI_CTL_TABLE( f ) \
   f( 0, NONE, void* none; ) \
   f( 1, LISTBOX, MAUG_MHANDLE list_h; char* list; size_t list_sz; size_t list_sz_max; size_t sel_idx; ) \
   f( 2, BUTTON, MAUG_MHANDLE label_h; char* label; size_t label_sz; int16_t push_frames; ) \
   f( 3, TEXTBOX, MAUG_MHANDLE text_h; char* text; size_t text_sz; size_t text_sz_max; size_t text_cur; int16_t blink_frames; ) \
   f( 4, LABEL, MAUG_MHANDLE label_h; char* label; size_t label_sz; )

#if 0
   f( 5, SCROLLBAR, size_t min; size_t max; size_t value; )
#endif

/*! \brief Fields common to ALL ::RETROGUI_CTL types. */
struct RETROGUI_CTL_BASE {
   uint8_t type;
   RETROGUI_IDC idc;
   size_t x;
   size_t y;
   size_t w;
   size_t h;
   RETROFLAT_COLOR bg_color;
   RETROFLAT_COLOR fg_color;
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

typedef void (*retrogui_xy_cb)( size_t* x, size_t* y, void* data );

struct RETROGUI {
   uint8_t flags;
   size_t x;
   size_t y;
   RETROGUI_IDC idc_prev;
   MAUG_MHANDLE ctls_h;
   union RETROGUI_CTL* ctls;
   size_t ctls_sz;
   size_t ctls_sz_max;
   RETROGUI_IDC focus;
   struct RETROFLAT_BITMAP* draw_bmp;
#ifdef RETROGXC_PRESENT
   ssize_t font_idx;
#else
   MAUG_MHANDLE font_h;
#endif /* RETROGXC_PRESENT */
};

MERROR_RETVAL retrogui_push_listbox_item(
   struct RETROGUI* gui, RETROGUI_IDC idc, const char* item, size_t item_sz );

union RETROGUI_CTL* retrogui_get_ctl_by_idc( struct RETROGUI* gui, size_t idc );

/**
 * \brief Poll for the last clicked control and maintain listboxes
 *        and menus.
 * \param input Input integer returned from retroflat_poll_input().
 * \param input_evt ::RETROFLAT_INPUT initialized by retroflat_poll_input().
 * \return IDC of clicked control or ::RETROGUI_IDC_NONE if none clicked
 *         since last poll.
 */
RETROGUI_IDC retrogui_poll_ctls(
   struct RETROGUI* gui, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt );

void retrogui_redraw_ctls( struct RETROGUI* gui );

MERROR_RETVAL retrogui_sz_ctl(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   size_t* p_w, size_t* p_h, size_t max_w, size_t max_h );

MERROR_RETVAL retrogui_pos_ctl(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   size_t x, size_t y, size_t w, size_t h );

MERROR_RETVAL retrogui_push_ctl(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl );

MERROR_RETVAL retrogui_get_ctl_text(
   struct RETROGUI* gui, RETROGUI_IDC idc, char* buffer, size_t buffer_sz );

size_t retrogui_get_ctl_sel_idx( struct RETROGUI* gui, size_t idc );

MERROR_RETVAL retrogui_init_ctl(
   union RETROGUI_CTL* ctl, uint8_t type, size_t idc );

MERROR_RETVAL retrogui_init( struct RETROGUI* gui );

void retrogui_free( struct RETROGUI* gui );

#ifdef RETROGUI_C

#define RETROGUI_CTL_TABLE_CONSTS( idx, c_name, c_fields ) \
   MAUG_CONST uint8_t RETROGUI_CTL_TYPE_ ## c_name = idx;

RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_CONSTS )

#define RETROGUI_CTL_TABLE_NAMES( idx, c_name, f_fields ) \
   #c_name,

MAUG_CONST char* gc_retrogui_ctl_names[] = {
   RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_NAMES )
   ""
};

/* === Control: NONE === */

static RETROGUI_IDC retrogui_click_NONE( 
   struct RETROGUI* gui,
   union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   RETROGUI_IDC idc_out = RETROGUI_IDC_NONE;

   return idc_out;
}

static RETROGUI_IDC retrogui_key_NONE( 
   union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   RETROGUI_IDC idc_out = RETROGUI_IDC_NONE;

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
   size_t* p_w, size_t* p_h, size_t max_w, size_t max_h
) {
   return MERROR_OK;
}

static MERROR_RETVAL retrogui_pos_NONE(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   size_t x, size_t y, size_t w, size_t h
) {
   return MERROR_OK;
}

static void retrogui_free_NONE( union RETROGUI_CTL* ctl ) {
}

static MERROR_RETVAL retrogui_init_NONE( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_GUI;

   return retval;
}

/* === Control: LISTBOX === */

static RETROGUI_IDC retrogui_click_LISTBOX( 
   struct RETROGUI* gui,
   union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   RETROGUI_IDC idc_out = RETROGUI_IDC_NONE;
   size_t i = 0,
      j = 0,
      w = 0,
      h = 0;

#  if defined( RETROGUI_NATIVE_WIN )
   /* Do nothing. */
#  else

   assert( NULL == ctl->LISTBOX.list );
   maug_mlock( ctl->LISTBOX.list_h, ctl->LISTBOX.list );

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
         (size_t)(input_evt->mouse_y) < 
         ctl->base.y + ((j + 1) * (h + RETROGUI_PADDING))
      ) {
         ctl->LISTBOX.sel_idx = j;
         break;
      }

      /* Try next variable-length string. */
      i += strlen( &(ctl->LISTBOX.list[i]) ) + 1;
      assert( i <= ctl->LISTBOX.list_sz );
      j++;
   }

   if( NULL != ctl->LISTBOX.list ) {
      maug_munlock( ctl->LISTBOX.list_h, ctl->LISTBOX.list );
   }

#endif

   return idc_out;
}

static RETROGUI_IDC retrogui_key_LISTBOX( 
   union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   RETROGUI_IDC idc_out = RETROGUI_IDC_NONE;

   /* TODO: Move up or down to next/prev item. */

   return idc_out;
}

static void retrogui_redraw_LISTBOX(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl
) {
   size_t i = 0,
      j = 0,
      w = 0,
      h = 0;
   
   assert( NULL == ctl->LISTBOX.list );

#  if defined( RETROGUI_NATIVE_WIN )
   /* TODO: InvalidateRect()? */
#  else

   maug_mlock( ctl->LISTBOX.list_h, ctl->LISTBOX.list );
   if( NULL == ctl->LISTBOX.list ) {
      goto cleanup;
   }
   
   retroflat_rect( NULL, ctl->base.bg_color, ctl->base.x, ctl->base.y,
      ctl->base.w, ctl->base.h, RETROFLAT_FLAGS_FILL );

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
      if( j == ctl->LISTBOX.sel_idx ) {
         /* TODO: Configurable selection colors. */
         retroflat_rect( gui->draw_bmp, RETROFLAT_COLOR_BLUE,
            ctl->base.x, ctl->base.y + (j * (h + RETROGUI_PADDING)),
            ctl->base.w, h, RETROFLAT_FLAGS_FILL );
         
      }
#ifdef RETROGXC_PRESENT
      retrogxc_string(
         gui->draw_bmp, ctl->base.fg_color, &(ctl->LISTBOX.list[i]), 0,
         gui->font_idx, ctl->base.x, ctl->base.y + (j * (h + RETROGUI_PADDING)),
         0, 0, 0 );
#else
      retrofont_string(
         gui->draw_bmp, ctl->base.fg_color, &(ctl->LISTBOX.list[i]), 0,
         gui->font_h, ctl->base.x, ctl->base.y + (j * (h + RETROGUI_PADDING)),
         0, 0, 0 );
#endif /* RETROGXC_PRESENT */

      /* Move to next variable-length string. */
      i += strlen( &(ctl->LISTBOX.list[i]) ) + 1;
      assert( i <= ctl->LISTBOX.list_sz );
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
   struct RETROGUI* gui, RETROGUI_IDC idc, const char* item, size_t item_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   union RETROGUI_CTL* ctl = NULL;
   MAUG_MHANDLE listbox_h_new = (MAUG_MHANDLE)NULL;

   retrogui_lock( gui );

   debug_printf( RETROGUI_TRACE_LVL,
      "pushing item \"%s\" to listbox " SIZE_T_FMT "...", item, idc );

   ctl = retrogui_get_ctl_by_idc( gui, idc );
   if( NULL == ctl ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR, "Error",
         "Adding item \"%s\" failed: Control missing!", item );
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

   strncpy( &(ctl->LISTBOX.list[ctl->LISTBOX.list_sz]), item, item_sz );
   ctl->LISTBOX.list[ctl->LISTBOX.list_sz + item_sz] = '\0';
   ctl->LISTBOX.list_sz += item_sz + 1;

#endif

cleanup:

   if( NULL != ctl->LISTBOX.list ) {
      maug_munlock( ctl->LISTBOX.list_h, ctl->LISTBOX.list );
   }

   return retval;
}

static MERROR_RETVAL retrogui_push_LISTBOX( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( RETROGUI_NATIVE_WIN )

   ctl->base.hwnd = CreateWindow(
      "LISTBOX", NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD,
      ctl->base.x, ctl->base.y, ctl->base.w, ctl->base.h,
      g_retroflat_state->window, (HMENU)(ctl->base.idc),
      g_retroflat_instance, NULL );
   debug_printf( RETROGUI_TRACE_LVL,
      "listbox hwnd: %p", ctl->LISTBOX.base.hwnd );
   if( (HWND)NULL == ctl->base.hwnd ) {
      error_printf( "could not create listbox" );
      retval = MERROR_GUI;
      goto cleanup;
   }

cleanup:

#  else

   /* TODO? */

#  endif

   return retval;
}

static MERROR_RETVAL retrogui_sz_LISTBOX(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   size_t* p_w, size_t* p_h, size_t max_w, size_t max_h
) {
   MERROR_RETVAL retval = MERROR_GUI;
   /* TODO */
   return retval;
}

static MERROR_RETVAL retrogui_pos_LISTBOX(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   size_t x, size_t y, size_t w, size_t h
) {
   MERROR_RETVAL retval = MERROR_GUI;
   /* TODO */
   return retval;
}

static void retrogui_free_LISTBOX( union RETROGUI_CTL* ctl ) {
   assert( NULL == ctl->LISTBOX.list );
   maug_mfree( ctl->LISTBOX.list_h );
}

static MERROR_RETVAL retrogui_init_LISTBOX( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( RETROGUI_TRACE_LVL,
      "initializing listbox " SIZE_T_FMT "...", ctl->base.idc );

   ctl->base.fg_color = RETROFLAT_COLOR_BLACK;
   ctl->base.bg_color = RETROFLAT_COLOR_WHITE;

   return retval;
}

/* === Control: BUTTON === */

static RETROGUI_IDC retrogui_click_BUTTON( 
   struct RETROGUI* gui,
   union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   RETROGUI_IDC idc_out = RETROGUI_IDC_NONE;

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

static RETROGUI_IDC retrogui_key_BUTTON( 
   union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   RETROGUI_IDC idc_out = RETROGUI_IDC_NONE;

   /* Set the last button clicked. */
   /* TODO: Only set out on ENTER/SPACE. */
   /* idc_out = ctl->base.idc; */

   return idc_out;
}

static void retrogui_redraw_BUTTON(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl
) {
   size_t w = 0,
      h = 0,
      text_offset = 0;

   retroflat_rect( gui->draw_bmp, ctl->base.bg_color, ctl->base.x, ctl->base.y,
      ctl->base.w, ctl->base.h, RETROFLAT_FLAGS_FILL );

   retroflat_rect( gui->draw_bmp, RETROFLAT_COLOR_BLACK,
      ctl->base.x, ctl->base.y,
      ctl->base.w, ctl->base.h, 0 );

   if( 0 < ctl->BUTTON.push_frames ) {
      retroflat_line(
         gui->draw_bmp, RETROFLAT_COLOR_DARKGRAY,
         ctl->base.x + 1, ctl->base.y + 1,
         ctl->base.x + ctl->base.w - 2, ctl->base.y + 1, 0 );
      retroflat_line(
         gui->draw_bmp, RETROFLAT_COLOR_DARKGRAY,
         ctl->base.x + 1, ctl->base.y + 2,
         ctl->base.x + 1, ctl->base.y + ctl->base.h - 3, 0 );

      gui->flags |= RETROGUI_FLAGS_DIRTY; /* Mark dirty for push animation. */
      ctl->BUTTON.push_frames--;
      text_offset = 1;
   } else {
      /* Button is not pushed. */
      retroflat_line(
         gui->draw_bmp, RETROFLAT_COLOR_WHITE,
         ctl->base.x + 1, ctl->base.y + 1,
         ctl->base.x + ctl->base.w - 2, ctl->base.y + 1, 0 );
      retroflat_line(
         gui->draw_bmp, RETROFLAT_COLOR_WHITE,
         ctl->base.x + 1, ctl->base.y + 2,
         ctl->base.x + 1, ctl->base.y + ctl->base.h - 3, 0 );
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
      ctl->base.w, ctl->base.h, &w, &h, 0 );

#ifdef RETROGXC_PRESENT
   retrogxc_string(
#else
   retrofont_string(
#endif /* RETROGXC_PRESENT */
      gui->draw_bmp, ctl->base.fg_color, ctl->BUTTON.label, 0,
#ifdef RETROGXC_PRESENT
      gui->font_idx,
#else
      gui->font_h,
#endif /* RETROGXC_PRESENT */
      ctl->base.x + ((ctl->base.w >> 1) - (w >> 1)) + text_offset,
      ctl->base.y + ((ctl->base.h >> 1) - (h >> 1)) + text_offset,
      /* TODO: Pad max client area. */
      ctl->base.w, ctl->base.h, 0 );

   maug_munlock( ctl->BUTTON.label_h, ctl->BUTTON.label );

cleanup:

   return;
}

static MERROR_RETVAL retrogui_push_BUTTON( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( RETROGUI_NATIVE_WIN )

   ctl->base.hwnd = CreateWindow(
      "BUTTON", ctl->BUTTON.label, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
      ctl->base.x, ctl->base.y, ctl->base.w, ctl->base.h,
      g_retroflat_state->window, (HMENU)(ctl->base.idc),
      g_retroflat_instance, NULL );
   if( (HWND)NULL == ctl->base.hwnd ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR, "Error",
         "Could not create button " SIZE_T_FMT ": %s",
         ctl->base.idc, ctl->BUTTON.label );
      retval = MERROR_GUI;
      goto cleanup;
   }

#  else
   retrogui_copy_label( ctl, BUTTON );
#  endif

cleanup:

   return retval;
}

static MERROR_RETVAL retrogui_sz_BUTTON(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   size_t* p_w, size_t* p_h, size_t max_w, size_t max_h
) {
   MERROR_RETVAL retval = MERROR_OK;

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
      p_h, 0 );

   /* Add space for borders and stuff. */
   *p_w += RETROGUI_BTN_LBL_PADDED_X;
   *p_h += RETROGUI_BTN_LBL_PADDED_Y;

   return retval;
}

static MERROR_RETVAL retrogui_pos_BUTTON(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   size_t x, size_t y, size_t w, size_t h
) {
   MERROR_RETVAL retval = MERROR_OK;

#  if defined( RETROGUI_NATIVE_WIN )
   /* TODO */
#  else
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

static void retrogui_free_BUTTON( union RETROGUI_CTL* ctl ) {
   if( NULL != ctl->BUTTON.label_h ) {
      maug_mfree( ctl->BUTTON.label_h );
   }
}

static MERROR_RETVAL retrogui_init_BUTTON( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( RETROGUI_TRACE_LVL,
      "initializing button " SIZE_T_FMT "...", ctl->base.idc );

   ctl->base.fg_color = RETROFLAT_COLOR_BLACK;
   ctl->base.bg_color = RETROFLAT_COLOR_GRAY;

   return retval;
}

/* === Control: TEXTBOX === */

static RETROGUI_IDC retrogui_click_TEXTBOX(
   struct RETROGUI* gui,
   union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   RETROGUI_IDC idc_out = RETROGUI_IDC_NONE;

   return idc_out;
}

static RETROGUI_IDC retrogui_key_TEXTBOX(
   union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   RETROGUI_IDC idc_out = RETROGUI_IDC_NONE;
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

#  if defined( RETROGUI_NATIVE_WIN )
   /* Do nothing. */
#  else

   retroflat_rect( gui->draw_bmp, ctl->base.bg_color, ctl->base.x, ctl->base.y,
      ctl->base.w, ctl->base.h, RETROFLAT_FLAGS_FILL );

   /* Draw chiselled inset border. */

   retroflat_rect( gui->draw_bmp, RETROFLAT_COLOR_BLACK,
      ctl->base.x, ctl->base.y, ctl->base.w, 2,
      RETROFLAT_FLAGS_FILL );

   retroflat_rect( gui->draw_bmp, RETROFLAT_COLOR_BLACK,
      ctl->base.x, ctl->base.y, 2, ctl->base.h,
      RETROFLAT_FLAGS_FILL );

   retroflat_rect( gui->draw_bmp, RETROFLAT_COLOR_DARKGRAY,
      ctl->base.x, ctl->base.y + ctl->base.h - 1,
      ctl->base.w, 2,
      RETROFLAT_FLAGS_FILL );

   retroflat_rect( gui->draw_bmp, RETROFLAT_COLOR_DARKGRAY,
      ctl->base.x + ctl->base.w - 1, ctl->base.y, 2, ctl->base.h,
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
      ctl->base.x + RETROGUI_PADDING,
      ctl->base.y + RETROGUI_PADDING, ctl->base.w, ctl->base.h, 0 );
#else
   retrofont_string(
      gui->draw_bmp, ctl->base.fg_color, ctl->TEXTBOX.text, 0, gui->font_h,
      ctl->base.x + RETROGUI_PADDING,
      ctl->base.y + RETROGUI_PADDING, ctl->base.w, ctl->base.h, 0 );
#endif /* RETROGXC_PRESENT */

cleanup:

   if( NULL != ctl->TEXTBOX.text ) {
      maug_munlock( ctl->TEXTBOX.text_h, ctl->TEXTBOX.text );
   }

   /* TODO: Get cursor color from GUI. */
   retroflat_rect( gui->draw_bmp, RETROFLAT_COLOR_BLUE,
      ctl->base.x + RETROGUI_PADDING + (8 * ctl->TEXTBOX.text_cur),
      ctl->base.y + RETROGUI_PADDING,
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
      ctl->base.x, ctl->base.y, ctl->base.w, ctl->base.h,
      g_retroflat_state->window, (HMENU)(ctl->base.idc),
      g_retroflat_instance, NULL );
   if( (HWND)NULL == ctl->base.hwnd ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR, "Error",
         "Could not create textbox: " SIZE_T_FMT, ctl->base.idc );
      retval = MERROR_GUI;
      goto cleanup;
   }

#  else

   debug_printf( RETROGUI_TRACE_LVL,
      "clearing textbox " SIZE_T_FMT " buffer...", ctl->base.idc );
   assert( NULL == ctl->TEXTBOX.text_h );
   ctl->TEXTBOX.text_h = maug_malloc( RETROGUI_CTL_TEXT_SZ_MAX, 1 );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, ctl->TEXTBOX.text_h );
   ctl->TEXTBOX.text_sz_max = RETROGUI_CTL_TEXT_SZ_MAX;

   maug_mlock( ctl->TEXTBOX.text_h, ctl->TEXTBOX.text );
   maug_cleanup_if_null_alloc( char*, ctl->TEXTBOX.text );
   debug_printf( RETROGUI_TRACE_LVL,
      "clearing textbox " SIZE_T_FMT " buffer...", ctl->base.idc );
   maug_mzero( ctl->TEXTBOX.text, RETROGUI_CTL_TEXT_SZ_MAX );
   maug_munlock( ctl->TEXTBOX.text_h, ctl->TEXTBOX.text );

#  endif

cleanup:

   return retval;
}

static MERROR_RETVAL retrogui_sz_TEXTBOX(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   size_t* p_w, size_t* p_h, size_t max_w, size_t max_h
) {
   MERROR_RETVAL retval = MERROR_GUI;
   /* TODO */
   return retval;
}

static MERROR_RETVAL retrogui_pos_TEXTBOX(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   size_t x, size_t y, size_t w, size_t h
) {
   MERROR_RETVAL retval = MERROR_GUI;
   /* TODO */
   return retval;
}

static void retrogui_free_TEXTBOX( union RETROGUI_CTL* ctl ) {
   if( NULL != ctl->TEXTBOX.text_h ) {
      maug_mfree( ctl->TEXTBOX.text_h );
   }
}

static MERROR_RETVAL retrogui_init_TEXTBOX( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( RETROGUI_TRACE_LVL,
      "initializing textbox " SIZE_T_FMT "...", ctl->base.idc );

   ctl->base.fg_color = RETROFLAT_COLOR_BLACK;
   ctl->base.bg_color = RETROFLAT_COLOR_WHITE;

   return retval;
}

/* === Control: LABEL === */

static RETROGUI_IDC retrogui_click_LABEL(
   struct RETROGUI* gui,
   union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   return RETROGUI_IDC_NONE;
}

static RETROGUI_IDC retrogui_key_LABEL(
   union RETROGUI_CTL* ctl, RETROFLAT_IN_KEY* p_input,
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
      ctl->base.x + RETROGUI_PADDING,
      ctl->base.y + RETROGUI_PADDING, ctl->base.w, ctl->base.h, 0 );

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
   retrogui_copy_label( ctl, LABEL );
#  endif

cleanup:

   return retval;
}

static MERROR_RETVAL retrogui_sz_LABEL(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   size_t* p_w, size_t* p_h, size_t max_w, size_t max_h
) {
   MERROR_RETVAL retval = MERROR_GUI;
   /* TODO */
   return retval;
}

static MERROR_RETVAL retrogui_pos_LABEL(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   size_t x, size_t y, size_t w, size_t h
) {
   MERROR_RETVAL retval = MERROR_GUI;
   /* TODO */
   return retval;
}

static void retrogui_free_LABEL( union RETROGUI_CTL* ctl ) {
   if( NULL != ctl->LABEL.label_h ) {
      maug_mfree( ctl->LABEL.label_h );
   }
}

static MERROR_RETVAL retrogui_init_LABEL( union RETROGUI_CTL* ctl ) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( RETROGUI_TRACE_LVL,
      "initializing textbox " SIZE_T_FMT "...", ctl->base.idc );

   ctl->base.fg_color = RETROFLAT_COLOR_BLACK;
   ctl->base.bg_color = RETROFLAT_COLOR_WHITE;

   return retval;
}

/* === Generic Functions === */

union RETROGUI_CTL* retrogui_get_ctl_by_idc(
   struct RETROGUI* gui, size_t idc
) {
   size_t i = 0;
   union RETROGUI_CTL* ctl = NULL;

   if( !retrogui_is_locked( gui ) ) {
      error_printf( "GUI is not locked!" );
      goto cleanup;
   }

   for( i = 0 ; gui->ctls_sz > i ; i++ ) {
      if( idc == gui->ctls[i].base.idc ) {
         ctl = &(gui->ctls[i]);
         break;
      }
   }

   if( NULL == ctl ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR, "Error",
         "Could not find GUI item: " SIZE_T_FMT, idc );
   }

cleanup:

   return ctl;
}

RETROGUI_IDC retrogui_poll_ctls( 
   struct RETROGUI* gui, RETROFLAT_IN_KEY* p_input,
   struct RETROFLAT_INPUT* input_evt
) {
   size_t i = 0,
      mouse_x = 0,
      mouse_y = 0;
   RETROGUI_IDC idc_out = RETROGUI_IDC_NONE;
   union RETROGUI_CTL* ctl = NULL;

   assert( NULL != gui->ctls );

#  if defined( RETROGUI_NATIVE_WIN )

   if( 0 == g_retroflat_state->last_idc ) {
      /* No WM_COMMAND to process. */
      goto cleanup;
   }

   ctl = retrogui_get_ctl_by_idc( gui, g_retroflat_state->last_idc );
   g_retroflat_state->last_idc = 0;
   if( NULL == ctl ) {
      debug_printf( RETROGUI_TRACE_LVL,
         "invalid IDC: " SIZE_T_FMT, gui->focus );
   }

   if( RETROGUI_CTL_TYPE_TEXTBOX == ctl->base.type ) {
      if( SendMessage( ctl->base.hwnd, EM_GETMODIFY, 0, 0 ) ) {
         SendMessage( ctl->base.hwnd, EM_SETMODIFY, 0, 0 );
         debug_printf( RETROGUI_TRACE_LVL, "mod: %d",
            SendMessage( ctl->base.hwnd, EM_GETMODIFY, 0, 0 ) );
      }
   }

#  else

   /* Use our cross-platform controls. */

   #define RETROGUI_CTL_TABLE_CLICK( idx, c_name, c_fields ) \
      } else if( RETROGUI_CTL_TYPE_ ## c_name == gui->ctls[i].base.type ) { \
         gui->flags |= RETROGUI_FLAGS_DIRTY; \
         idc_out = \
            retrogui_click_ ## c_name( \
               gui, &(gui->ctls[i]), p_input, input_evt );

   #define RETROGUI_CTL_TABLE_KEY( idx, c_name, c_fields ) \
      } else if( RETROGUI_CTL_TYPE_ ## c_name == ctl->base.type ) { \
         gui->flags |= RETROGUI_FLAGS_DIRTY; \
         idc_out = retrogui_key_ ## c_name( ctl, p_input, input_evt );

   if( 0 == *p_input ) {
      goto reset_debounce;

#     ifndef RETROGUI_NO_MOUSE
   } else if(
      RETROFLAT_MOUSE_B_LEFT == *p_input ||
      RETROFLAT_MOUSE_B_RIGHT == *p_input
   ) {
      /* Remove all focus before testing if a new control has focus. */
      gui->focus = RETROGUI_IDC_NONE;

      mouse_x = input_evt->mouse_x - gui->x;
      mouse_y = input_evt->mouse_y - gui->y;

      for( i = 0 ; gui->ctls_sz > i ; i++ ) {
         if(
            mouse_x < gui->ctls[i].base.x ||
            mouse_y < gui->ctls[i].base.y ||
            mouse_x > gui->ctls[i].base.x + gui->ctls[i].base.w ||
            mouse_y > gui->ctls[i].base.y + gui->ctls[i].base.h
         ) {
            continue;
         }

         if( gui->idc_prev == gui->ctls[i].base.idc ) {
            /* No repeated clicks! */
            /* TODO: Allow exceptions for e.g. scrollbars. */
            idc_out = RETROGUI_IDC_NONE;
            goto cleanup;
         }

         gui->idc_prev = gui->ctls[i].base.idc;

         gui->focus = gui->ctls[i].base.idc;

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

      ctl = retrogui_get_ctl_by_idc( gui, gui->focus );
      if( NULL == ctl ) {
         debug_printf( RETROGUI_TRACE_LVL,
            "invalid IDC: " SIZE_T_FMT, gui->focus );
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

   return idc_out;
}

void retrogui_redraw_ctls( struct RETROGUI* gui ) {
   size_t i = 0;

   assert( NULL != gui->ctls );

   if(
      RETROGUI_FLAGS_DIRTY !=
      (RETROGUI_FLAGS_DIRTY & gui->flags)
   ) {
      return;
   }

   #define RETROGUI_CTL_TABLE_REDRAW( idx, c_name, c_fields ) \
      } else if( RETROGUI_CTL_TYPE_ ## c_name == gui->ctls[i].base.type ) { \
         /* Mark dirty first so redraw can unmark it for animation! */ \
         gui->flags &= ~RETROGUI_FLAGS_DIRTY; \
         retrogui_redraw_ ## c_name( gui, &(gui->ctls[i]) );

   for( i = 0 ; gui->ctls_sz > i ; i++ ) {
      if( 0 ) {
      RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_REDRAW )
      }
   }
}

MERROR_RETVAL retrogui_sz_ctl(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   size_t* p_w, size_t* p_h, size_t max_w, size_t max_h
) {
   MERROR_RETVAL retval = MERROR_OK;

   #define RETROGUI_CTL_TABLE_SZ( idx, c_name, c_fields ) \
      } else if( RETROGUI_CTL_TYPE_ ## c_name == ctl->base.type ) { \
         /* Mark dirty first so redraw can unmark it for animation! */ \
         retval = retrogui_sz_ ## c_name( gui, ctl, p_w, p_h, max_w, max_h ); \
         maug_cleanup_if_not_ok();
   
   if( 0 ) {
   RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_SZ )
   }

   debug_printf( 1,
      "sized control " SIZE_T_FMT " at " SIZE_T_FMT "x" SIZE_T_FMT "...",
      ctl->base.idc, ctl->base.w, ctl->base.h );

cleanup:

   return retval;
}

MERROR_RETVAL retrogui_pos_ctl(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl,
   size_t x, size_t y, size_t w, size_t h
) {
   MERROR_RETVAL retval = MERROR_OK;

   #define RETROGUI_CTL_TABLE_POS( idx, c_name, c_fields ) \
      } else if( RETROGUI_CTL_TYPE_ ## c_name == ctl->base.type ) { \
         /* Mark dirty first so redraw can unmark it for animation! */ \
         retval = retrogui_pos_ ## c_name( gui, ctl, x, y, w, h ); \
         maug_cleanup_if_not_ok();
   
   if( 0 ) {
   RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_POS )
   }

   debug_printf( 1,
      "moved control " SIZE_T_FMT " to " SIZE_T_FMT "x" SIZE_T_FMT "...",
      ctl->base.idc, ctl->base.x, ctl->base.y );

cleanup:

   return retval;

}

MERROR_RETVAL retrogui_push_ctl(
   struct RETROGUI* gui, union RETROGUI_CTL* ctl
) {
   MERROR_RETVAL retval = MERROR_OK;

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

   assert( NULL != gui->ctls );

   /* TODO: Hunt for control IDC and fail if duplicate found! */

   /* TODO: Grow controls if needed. */
   assert( gui->ctls_sz + 1 < gui->ctls_sz_max );

   if( RETROFLAT_COLOR_NULL == ctl->base.bg_color ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR, "Error",
         "invalid background color specified for control " SIZE_T_FMT "!",
         ctl->base.idc );
      retval = MERROR_GUI;
      goto cleanup;

   }

   if( RETROFLAT_COLOR_NULL == ctl->base.fg_color ) {
      retroflat_message( RETROFLAT_MSG_FLAG_ERROR, "Error",
         "invalid foreground color specified for control " SIZE_T_FMT "!",
         ctl->base.idc );
      retval = MERROR_GUI;
      goto cleanup;
   }

   /* Ensure physical size of control. */

   if( 0 == ctl->base.w || 0 == ctl->base.h ) {
      debug_printf( RETROGUI_TRACE_LVL,
         "determining size for new %s control " SIZE_T_FMT "...",
         gc_retrogui_ctl_names[ctl->base.type], ctl->base.idc );
      retval = retrogui_sz_ctl(
         gui, ctl, &(ctl->base.w), &(ctl->base.h), 0, 0 );
      maug_cleanup_if_not_ok();
   }

   /* Perform the actual push. */

   debug_printf( RETROGUI_TRACE_LVL,
      "pushing %s " SIZE_T_FMT " to slot " SIZE_T_FMT "...",
      gc_retrogui_ctl_names[ctl->base.type], ctl->base.idc, gui->ctls_sz );

   memcpy(
      &(gui->ctls[gui->ctls_sz]),
      ctl,
      sizeof( union RETROGUI_CTL ) );
   gui->ctls_sz++;

   gui->flags |= RETROGUI_FLAGS_DIRTY;

   #define RETROGUI_CTL_TABLE_PUSH( idx, c_name, c_fields ) \
      } else if( RETROGUI_CTL_TYPE_ ## c_name == ctl->base.type ) { \
         retval = retrogui_push_ ## c_name( &(gui->ctls[gui->ctls_sz - 1]) ); \
         maug_cleanup_if_not_ok();

   if( 0 ) {
   RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_PUSH )
   }

cleanup:

   return retval;
}

MERROR_RETVAL retrogui_get_ctl_text(
   struct RETROGUI* gui, RETROGUI_IDC idc, char* buffer, size_t buffer_sz
) {
   MERROR_RETVAL retval = MERROR_OK;
   union RETROGUI_CTL* ctl = NULL;

   ctl = retrogui_get_ctl_by_idc( gui, idc );
   if( NULL == ctl ) {
      goto cleanup;
   }
   
   if( RETROGUI_CTL_TYPE_TEXTBOX == ctl->base.type ) {
#  if defined( RETROGUI_NATIVE_WIN )
      /* TODO */
#else
      maug_mlock( ctl->TEXTBOX.text_h, ctl->TEXTBOX.text );
      maug_cleanup_if_null_alloc( char*, ctl->TEXTBOX.text );

      strncpy( buffer, ctl->TEXTBOX.text, buffer_sz );
#  endif
   }

cleanup:

   if( RETROGUI_CTL_TYPE_TEXTBOX == ctl->base.type ) {
      if( NULL != ctl->TEXTBOX.text ) {
         maug_munlock( ctl->TEXTBOX.text_h, ctl->TEXTBOX.text );
      }
   }

   return retval;
}

size_t retrogui_get_ctl_sel_idx( struct RETROGUI* gui, RETROGUI_IDC idc ) {
   size_t idx = -1;
   union RETROGUI_CTL* ctl = NULL;

   ctl = retrogui_get_ctl_by_idc( gui, idc );
   if( NULL == ctl ) {
      goto cleanup;
   }

   assert( RETROGUI_CTL_TYPE_LISTBOX == ctl->base.type );

#  if defined( RETROGUI_NATIVE_WIN )
   idx = SendMessage( ctl->base.hwnd, LB_GETCARETINDEX, 0, 0 );
#  else
   idx = ctl->LISTBOX.sel_idx;
#  endif

cleanup:

   return idx;
}

MERROR_RETVAL retrogui_init_ctl(
   union RETROGUI_CTL* ctl, uint8_t type, size_t idc
) {
   MERROR_RETVAL retval = MERROR_OK;

   debug_printf( RETROGUI_TRACE_LVL,
      "initializing control base " SIZE_T_FMT "...", idc );

   maug_mzero( ctl, sizeof( union RETROGUI_CTL ) );

   ctl->base.type = type;
   ctl->base.idc = idc;
   ctl->base.fg_color = RETROFLAT_COLOR_NULL;
   ctl->base.bg_color = RETROFLAT_COLOR_NULL;

   #define RETROGUI_CTL_TABLE_INITS( idx, c_name, c_fields ) \
      } else if( RETROGUI_CTL_TYPE_ ## c_name == ctl->base.type ) { \
         retrogui_init_ ## c_name( ctl );

   if( 0 ) {
   RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_INITS )
   }

   return retval;
}

void retrogui_free( struct RETROGUI* gui ) {
   size_t i = 0;

   if( NULL == gui->ctls && (MAUG_MHANDLE)NULL != gui->ctls_h ) {
      maug_mlock( gui->ctls_h, gui->ctls );
      if( NULL == gui->ctls ) {
         goto cleanup;
      }
   }

   #define RETROGUI_CTL_TABLE_FREE( idx, c_name, c_fields ) \
      } else if( RETROGUI_CTL_TYPE_ ## c_name == gui->ctls[i].base.type ) { \
         retrogui_free_ ## c_name( &(gui->ctls[i]) );

   for( i = 0 ; gui->ctls_sz > i ; i++ ) {
      if( 0 ) {
      RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_FREE )
      }
   }

   maug_munlock( gui->ctls_h, gui->ctls );

cleanup:

   if( NULL != gui->ctls ) {
      maug_munlock( gui->ctls_h, gui->ctls );
   }

   maug_mfree( gui->ctls_h );

}

MERROR_RETVAL retrogui_init( struct RETROGUI* gui ) {
   MERROR_RETVAL retval = MERROR_OK;

   maug_mzero( gui, sizeof( struct RETROGUI ) );

   gui->ctls_h = maug_malloc(
      RETROGUI_CTL_SZ_MAX_INIT, sizeof( struct RETROGUI ) );
   maug_cleanup_if_null_alloc( MAUG_MHANDLE, gui );
   gui->ctls_sz_max = RETROGUI_CTL_SZ_MAX_INIT;

cleanup:

   return retval;
}

#else

#define RETROGUI_CTL_TABLE_CONSTS( idx, c_name, c_fields ) \
   extern MAUG_CONST uint8_t RETROGUI_CTL_TYPE_ ## c_name;

RETROGUI_CTL_TABLE( RETROGUI_CTL_TABLE_CONSTS )

extern MAUG_CONST char* gc_retrogui_ctl_names[];

#endif /* RETROGUI_C */

/*! \} */ /* maug_retrogui */

#endif /* !RETROGUI_H */

