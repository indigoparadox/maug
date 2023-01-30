
#ifndef RETROGUI_H
#define RETROGUI_H

#ifndef RETROGUI_CTL_TEXT_SZ_MAX
#  define RETROGUI_CTL_TEXT_SZ_MAX 128
#endif /* !RETROGUI_CTL_TEXT_SZ_MAX */

#ifndef RETROGUI_PADDING
#  define RETROGUI_PADDING 5
#endif /* !RETROGUI_PADDING */

#define RETROGUI_CTL_NONE 0
#define RETROGUI_CTL_BUTTON 1
#define RETROGUI_CTL_LISTBOX 2

struct RETROGUI_CTL;

/*! \brief Unique identifying constant number for controls. */
typedef int16_t RETROGUI_IDC;

#define RETROGUI_IDC_NONE -1

#if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

struct RETROGUI_CTL {
   HWND hwnd;
   RETROGUI_IDC idc;
   int ctl_type;
};

#else

struct RETROGUI_CTL {
   int x;
   int y;
   int w;
   int h;
   RETROGUI_IDC idc;
   RETROFLAT_COLOR bg_color;
   RETROFLAT_COLOR fg_color;
   char** text_list;
   int text_list_sz;
   int sel_idx;
   int ctl_type;
};

#endif

MERROR_RETVAL retrogui_add_button(
   const char* text, int x, int y, int w, int h,
   RETROFLAT_COLOR bg_color, RETROFLAT_COLOR fg_color,
   RETROGUI_IDC idc, uint8_t flags, struct RETROGUI_CTL* button );

MERROR_RETVAL retrogui_add_listbox(
   char* const* text_list, int text_list_sz, int sel_idx,
   int x, int y, int w, int h,
   RETROFLAT_COLOR bg_color, RETROFLAT_COLOR fg_color,
   RETROGUI_IDC idc, uint8_t flags, struct RETROGUI_CTL* listbox );

MERROR_RETVAL retrogui_get_ctl_text(
   char* buffer, int buffer_sz, struct RETROGUI_CTL* ctl );

int16_t retrogui_get_ctl_sel_idx( struct RETROGUI_CTL* ctl );

/**
 * \brief Poll for the last clicked control and maintain listboxes
 *        and menus.
 * \param input Input integer returned from retroflat_poll_input().
 * \param input_evt ::RETROFLAT_INPUT initialized by retroflat_poll_input().
 * \return IDC of clicked control or ::RETROGUI_IDC_NONE if none clicked
 *         since last poll.
 */
RETROGUI_IDC retrogui_poll_ctls(
   int input, struct RETROFLAT_INPUT* input_evt,
   struct RETROGUI_CTL* ctls, int ctls_sz );

void retrogui_redraw_ctls( struct RETROGUI_CTL* ctls, int ctls_sz );

#ifdef RETROGUI_C

RETROGUI_IDC g_retrogui_last_idc = RETROGUI_IDC_NONE;

MERROR_RETVAL retrogui_add_button(
   const char* text, int x, int y, int w, int h,
   RETROFLAT_COLOR bg_color, RETROFLAT_COLOR fg_color,
   RETROGUI_IDC idc, uint8_t flags, struct RETROGUI_CTL* button
) {
   MERROR_RETVAL retval = MERROR_OK;

   assert( 0 < idc );

#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   button->hwnd = CreateWindow(
      "BUTTON", text, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
      x, y, w, h, g_window, (HMENU)idc, g_instance, NULL );
   if( NULL == button->hwnd ) {
      error_printf( "could not create button: %s", text );
      goto cleanup;
   }
   button->idc = idc;
   button->ctl_type = RETROGUI_CTL_BUTTON;

cleanup:

#  else

   button->x = x;
   button->y = y;
   button->w = w;
   button->h = h;
   button->bg_color = bg_color;
   button->fg_color = fg_color;
   button->idc = idc;
   button->text_list = calloc( 1, sizeof( char* ) );
   button->text_list[0] = calloc( 1, RETROGUI_CTL_TEXT_SZ_MAX + 1 );
   strncpy( button->text_list[0], text, RETROGUI_CTL_TEXT_SZ_MAX );
   button->text_list_sz = 1;
   button->sel_idx = -1;
   button->ctl_type = RETROGUI_CTL_BUTTON;

#  endif

   return retval;
}

MERROR_RETVAL retrogui_add_listbox(
   char* const* text_list, int text_list_sz, int sel_idx,
   int x, int y, int w, int h,
   RETROFLAT_COLOR bg_color, RETROFLAT_COLOR fg_color,
   RETROGUI_IDC idc, uint8_t flags, struct RETROGUI_CTL* listbox
) {
   MERROR_RETVAL retval = MERROR_OK;
   int i = 0;

   assert( 0 < idc );

#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   listbox->hwnd = CreateWindow(
      "LISTBOX", NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD,
      x, y, w, h, g_window, (HMENU)idc, g_instance, NULL );
   if( NULL == listbox->hwnd ) {
      error_printf( "could not create listbox" );
      goto cleanup;
   }
   listbox->idc = idc;
   listbox->ctl_type = RETROGUI_CTL_LISTBOX;

   for( i = 0 ; text_list_sz > i ; i++ ) {
      SendMessage( listbox->hwnd, LB_ADDSTRING, 0, (LPARAM)text_list[i] );
   }

   /* TODO: Select sel_idx. */

cleanup:

#  else

   listbox->x = x;
   listbox->y = y;
   listbox->w = w;
   listbox->h = h;
   listbox->bg_color = bg_color;
   listbox->fg_color = fg_color;
   listbox->idc = idc;
   listbox->text_list = calloc( text_list_sz, sizeof( char* ) );
   listbox->text_list_sz = text_list_sz;
   for( i = 0 ; text_list_sz > i ; i++ ) {
      listbox->text_list[i] =
         calloc( strlen( text_list[i] ) + 1, sizeof( char ) );
      assert( NULL != listbox->text_list[i] );
      strcpy( listbox->text_list[i], text_list[i] );
   }
   listbox->sel_idx = sel_idx;
   listbox->ctl_type = RETROGUI_CTL_LISTBOX;

#  endif

   return retval;
}

RETROGUI_IDC retrogui_poll_ctls( 
   int input, struct RETROFLAT_INPUT* input_evt,
   struct RETROGUI_CTL* ctls, int ctls_sz
) {
   int i = 0;

#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

   for( i = 0 ; ctls_sz > i ; i++ ) {
      if( ctls[i].idc == g_last_idc ) {
         g_last_idc = 0;
         return ctls[i].idc;
      }
   }
#  else
   /* Use our cross-platform controls. */
   int j = 0,
      w = 0,
      h = 0;

   for( i = 0 ; ctls_sz > i ; i++ ) {
      if(
         RETROFLAT_MOUSE_B_LEFT == input &&
         input_evt->mouse_x > ctls[i].x &&
         input_evt->mouse_y > ctls[i].y &&
         input_evt->mouse_x < ctls[i].x + ctls[i].w &&
         input_evt->mouse_y < ctls[i].y + ctls[i].h
      ) {
         if( RETROGUI_CTL_BUTTON == ctls[i].ctl_type ) {
            /* Set the last button clicked. */
            if( g_retrogui_last_idc == ctls[i].idc ) {
               /* No repeated clicks! */
               return RETROGUI_IDC_NONE;
            } else {
               g_retrogui_last_idc = ctls[i].idc;
               return ctls[i].idc;
            }
         } else if( RETROGUI_CTL_LISTBOX == ctls[i].ctl_type ) {
            /* Figure out the item clicked. */
            /* TODO */
            for( j = 0 ; ctls[i].text_list_sz > j ; j++ ) {
               retroflat_string_sz(
                  NULL, ctls[i].text_list[j], -1, NULL, &w, &h );
               if(
                  input_evt->mouse_y < 
                  ctls[i].y + ((j + 1) * (h + RETROGUI_PADDING))
               ) {
                  ctls[i].sel_idx = j;
                  return RETROGUI_IDC_NONE;
               }
            }
         }
      }
   }
   
   /* Reset repeat detector. */
   g_retrogui_last_idc = RETROGUI_IDC_NONE;

#  endif

   return RETROGUI_IDC_NONE;
}

void retrogui_redraw_ctls( struct RETROGUI_CTL* ctls, int ctls_sz ) {

#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   /* TODO: InvalidateRect()? */
#  else
   int i = 0,
      j = 0,
      w = 0,
      h = 0;

   for( i = 0 ; ctls_sz > i ; i++ ) {
      retroflat_rect( NULL, ctls[i].bg_color,
         ctls[i].x, ctls[i].y, ctls[i].w, ctls[i].h, RETROFLAT_FLAGS_FILL );

      for( j = 0 ; ctls[i].text_list_sz > j ; j++ ) {
         retroflat_string_sz( NULL, ctls[i].text_list[j], -1, NULL, &w, &h );
         if( j == ctls[i].sel_idx ) {
            /* TODO: Configurable selection colors. */
            retroflat_rect( NULL, RETROFLAT_COLOR_BLUE,
               ctls[i].x, ctls[i].y + (j * (h + RETROGUI_PADDING)),
               ctls[i].w, h, RETROFLAT_FLAGS_FILL );
            
         }
         retroflat_string(
            NULL, ctls[i].fg_color, ctls[i].text_list[j], -1, NULL,
            ctls[i].x, ctls[i].y + (j * (h + RETROGUI_PADDING)), 0 );
      }
   }

#  endif

}

MERROR_RETVAL retrogui_get_ctl_text(
   char* buffer, int buffer_sz, struct RETROGUI_CTL* ctl
) {
   MERROR_RETVAL retval = MERROR_OK;
   
   /* TODO */

   return retval;
}

int16_t retrogui_get_ctl_sel_idx( struct RETROGUI_CTL* ctl ) {
   int16_t idx = -1;

#  if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )
   idx = SendMessage( ctl->hwnd, LB_GETCARETINDEX, 0, 0 );
#  else
   idx = ctl->sel_idx;
#  endif

   return idx;
}

#endif /* RETROGUI_C */

#endif /* !RETROGUI_H */

