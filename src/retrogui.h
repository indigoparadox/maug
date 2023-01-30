
#ifndef RETROGUI_H
#define RETROGUI_H

#ifndef RETROGUI_CTL_TEXT_SZ_MAX
#  define RETROGUI_CTL_TEXT_SZ_MAX 128
#endif /* !RETROGUI_CTL_TEXT_SZ_MAX */

struct RETROGUI_CTL;

/*! \brief Unique identifying constant number for controls. */
typedef int16_t RETROGUI_IDC;

#define RETROGUI_IDC_NONE -1

#if defined( RETROFLAT_API_WIN16 ) || defined( RETROFLAT_API_WIN32 )

struct RETROGUI_CTL {
   HWND hwnd;
   RETROGUI_IDC idc;
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
   char text[RETROGUI_CTL_TEXT_SZ_MAX];
};

#endif

MERROR_RETVAL retrogui_add_button(
   const char* text, int x, int y, int w, int h,
   RETROFLAT_COLOR bg_color, RETROFLAT_COLOR fg_color,
   RETROGUI_IDC idc, uint8_t flags, struct RETROGUI_CTL* button );

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
   button->idc = idc;

#  else

   button->x = x;
   button->y = y;
   button->w = w;
   button->h = h;
   button->bg_color = bg_color;
   button->fg_color = fg_color;
   button->idc = idc;
   strncpy( button->text, text, RETROGUI_CTL_TEXT_SZ_MAX );

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

   for( i = 0 ; ctls_sz > i ; i++ ) {
      if(
         RETROFLAT_MOUSE_B_LEFT == input &&
         input_evt->mouse_x > ctls[i].x &&
         input_evt->mouse_y > ctls[i].y &&
         input_evt->mouse_x < ctls[i].x + ctls[i].w &&
         input_evt->mouse_y < ctls[i].y + ctls[i].h
      ) {
         if( g_retrogui_last_idc == ctls[i].idc ) {
            /* No repeated clicks! */
            return RETROGUI_IDC_NONE;
         } else {
            g_retrogui_last_idc = ctls[i].idc;
            return ctls[i].idc;
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
   int i = 0;

   for( i = 0 ; ctls_sz > i ; i++ ) {
      retroflat_rect( NULL, ctls[i].bg_color,
         ctls[i].x, ctls[i].y, ctls[i].w, ctls[i].h, RETROFLAT_FLAGS_FILL );
      retroflat_string(
         NULL, ctls[i].fg_color, ctls[i].text, -1, NULL,
         ctls[i].x, ctls[i].y, 0 );
   }

#  endif

}

#endif /* RETROGUI_C */

#endif /* !RETROGUI_H */

