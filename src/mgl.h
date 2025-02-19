
#ifndef MGL_H
#define MGL_H

#ifndef MGL_VERTEX_STACK_SZ_MAX
#  define MGL_VERTEX_STACK_SZ_MAX 128
#endif /* !MGL_VERTEX_STACK_SZ_MAX */

#ifndef MGL_TRI_STACK_SZ_MAX
#  define MGL_TRI_STACK_SZ_MAX 128
#endif /* !MGL_TRI_STACK_SZ_MAX */

#ifndef MGL_TRI_ACTIVE_SZ_MAX
#  define MGL_TRI_ACTIVE_SZ_MAX 16
#endif /* !MGL_TRI_ACTIVE_SZ_MAX */

#define MGL

#define MGL_SCALE_MULT 0
#define MGL_SCALE_BIT_RIGHT 1

/**
 * \addtogroup maug_mgl_gl
 * \{
 */

#define GL_TRIANGLES 1

#define glVertex2i( x, y ) glVertex3i( x, y, 0 )

#define glScaler( x, y, z ) \
   g_mgl_matrix.scale_type = MGL_SCALE_BIT_RIGHT; \
   g_mgl_matrix.scale_x = x; \
   g_mgl_matrix.scale_y = y; \
   g_mgl_matrix.scale_z = z; \
   debug_printf( 1, "matrix scale shift right: %d, %d, %d", \
      g_mgl_matrix.scale_x, \
      g_mgl_matrix.scale_y, \
      g_mgl_matrix.scale_z );

/*! \} */ /* maug_mgl_gl */

#define mgl_vertex_set( vn, vo, vi ) \
   switch( g_mgl_matrix.scale_type ) { \
   case MGL_SCALE_MULT: \
      vo = vi * g_mgl_matrix.vn; \
      break; \
   case MGL_SCALE_BIT_RIGHT: \
      vo = vi >> g_mgl_matrix.vn; \
      debug_printf( 1, "v %d shifted right %d: %d", vi, g_mgl_matrix.vn, vo ) \
      break; \
   }

struct MGL_VERTEX {
   int16_t x;
   int16_t y;
   int16_t z;
};

struct MGL_TRIANGLE {
   int16_t v[3];
   RETROFLAT_COLOR color;
   int16_t sz;
};

struct MGL_MATRIX {
   int scale_type;
   int scale_x;
   int scale_y;
   int scale_z;
};

/**
 * \brief Detect if a coordinate is inside of a line defined by two other
 *        coordinates.
 * \return Return is positive if the point is inside the line or 0/negative if
 *         it is not.
 */
int mglt_vertex_inside(
   int16_t x0, int16_t y0, int16_t x1, int16_t y1,
   int16_t tx, int16_t ty );

/**
 * \addtogroup maug_mgl_gl MiniGL Interface
 * \{
 */

void glBegin( int16_t gl_poly_type );

void glEnd();

void glVertex3i( int16_t x, int16_t y, int16_t z );

void glFlush();

/*! \} */ /* maug_mgl_gl */

#ifdef MGL_C

struct MGL_VERTEX g_mgl_vertex_stack[MGL_VERTEX_STACK_SZ_MAX];
uint16_t g_mgl_vertex_stack_sz = 0;
struct MGL_TRIANGLE g_mgl_tri_stack[MGL_TRI_STACK_SZ_MAX];
uint16_t g_mgl_tri_stack_sz = 0;
RETROFLAT_COLOR g_mgl_color = 0;
uint16_t g_mgl_tri_active[MGL_TRI_ACTIVE_SZ_MAX];
uint16_t g_mgl_tri_active_sz = 0;
struct MGL_MATRIX g_mgl_matrix;

int mglt_vertex_inside(
   int16_t x0, int16_t y0, int16_t x1, int16_t y1,
   int16_t tx, int16_t ty
) {
   int16_t a = y0 - y1;
   int16_t b = x1 - x0;
   int16_t c = (x0 * y1) - (x1 * y0);

   return (a * tx) + (b * ty) + c;
}

void glBegin( int16_t gl_poly_type ) {
   assert( GL_TRIANGLES == gl_poly_type );
   g_mgl_tri_stack[g_mgl_tri_stack_sz].sz = 0;
   g_mgl_tri_stack_sz++;
}

void glEnd() {
   assert( 0 < g_mgl_tri_stack_sz );
   assert( 3 == g_mgl_tri_stack[g_mgl_tri_stack_sz - 1].sz );
   g_mgl_tri_stack[g_mgl_tri_stack_sz - 1].color = g_mgl_color;
}

void glColorr( RETROFLAT_COLOR color ) {
   g_mgl_color = color;  
}

void glVertex3i( int16_t x, int16_t y, int16_t z ) {

   assert( MGL_VERTEX_STACK_SZ_MAX > g_mgl_vertex_stack_sz );
   assert( 0 < g_mgl_tri_stack_sz );

   /* Add the vertex to the system vertex stack. */
   mgl_vertex_set( scale_x, g_mgl_vertex_stack[g_mgl_vertex_stack_sz].x, x );
   mgl_vertex_set( scale_y, g_mgl_vertex_stack[g_mgl_vertex_stack_sz].y, y );
   mgl_vertex_set( scale_z, g_mgl_vertex_stack[g_mgl_vertex_stack_sz].z, z );
   debug_printf( 1, "%d %d %d",
      g_mgl_vertex_stack[g_mgl_vertex_stack_sz].x,
      g_mgl_vertex_stack[g_mgl_vertex_stack_sz].y,
      g_mgl_vertex_stack[g_mgl_vertex_stack_sz].z );

   /* Add the vertex to the current triangle. */
   g_mgl_tri_stack[g_mgl_tri_stack_sz - 1].v[
      g_mgl_tri_stack[g_mgl_tri_stack_sz - 1].sz] = g_mgl_vertex_stack_sz;
   g_mgl_tri_stack[g_mgl_tri_stack_sz - 1].sz++;

   g_mgl_vertex_stack_sz++;
}

void glFlush() {
   int16_t i = 0,
      x = 0,
      y = 0;

   for( y = 0 ; retroflat_screen_h() > y ; y++ ) {
      for( x = 0 ; retroflat_screen_w() > x ; x++ ) {
         /* Test if the current pixel is inside any triangles. */
         for( i = 0 ; g_mgl_tri_stack_sz > i ; i++ ) {
            if(
               0 < mglt_vertex_inside(
                  g_mgl_vertex_stack[g_mgl_tri_stack[i].v[0]].x,
                  g_mgl_vertex_stack[g_mgl_tri_stack[i].v[0]].y,
                  g_mgl_vertex_stack[g_mgl_tri_stack[i].v[1]].x,
                  g_mgl_vertex_stack[g_mgl_tri_stack[i].v[1]].y,
                  x, y ) &&
               0 < mglt_vertex_inside(
                  g_mgl_vertex_stack[g_mgl_tri_stack[i].v[1]].x,
                  g_mgl_vertex_stack[g_mgl_tri_stack[i].v[1]].y,
                  g_mgl_vertex_stack[g_mgl_tri_stack[i].v[2]].x,
                  g_mgl_vertex_stack[g_mgl_tri_stack[i].v[2]].y,
                  x, y ) &&
               0 < mglt_vertex_inside(
                  g_mgl_vertex_stack[g_mgl_tri_stack[i].v[2]].x,
                  g_mgl_vertex_stack[g_mgl_tri_stack[i].v[2]].y,
                  g_mgl_vertex_stack[g_mgl_tri_stack[i].v[0]].x,
                  g_mgl_vertex_stack[g_mgl_tri_stack[i].v[0]].y,
                  x, y )
            ) {
               retroflat_px( NULL, g_mgl_tri_stack[i].color, x, y, 0 );
            }
         }
      }
   }

#if 0
      retroflat_rect(
         NULL, RETROFLAT_COLOR_BLUE, g_mgl_vertex_stack[i].x,
         g_mgl_vertex_stack[i].y, 5, 5,
         RETROFLAT_FLAGS_FILL );
#endif

   g_mgl_vertex_stack_sz = 0;
   g_mgl_tri_stack_sz = 0;
   memset( &g_mgl_matrix, '\0', sizeof( struct MGL_MATRIX ) );
}

#else

extern RETROFLAT_COLOR g_mgl_color;
extern struct MGL_MATRIX g_mgl_matrix;

#endif /* MGL_C */

#endif /* !MGL_H */

