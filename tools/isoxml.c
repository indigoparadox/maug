
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define NAME_SZ_MAX 255

struct ISO_NODE {
   int active;
   char name[NAME_SZ_MAX];
   char path[NAME_SZ_MAX];
   struct ISO_NODE* child;
   struct ISO_NODE* sibling;
};

void isoxml_print( struct ISO_NODE* node, int is_root ) {
   size_t i = 0;

   if( NULL == node ) {
      return;
   }

   /* Format node names for display. */
   for( i = 0 ; strlen( node->name ) > i ; i++ ) {
      if( 96 < node->name[i] && 123 > node->name[i] ) {
         node->name[i] -= 32;
      }
   }

   if( NULL != node->child ) {
      if( 0 < strlen( node->name ) ) {
         printf( "<dir name=\"%s\">\n", node->name );
      }
#ifdef DEBUG
      fprintf( stderr, "printing children for: %s\n", node->name );
#endif /* DEBUG */
      isoxml_print( node->child, 0 );
#ifdef DEBUG
      fprintf( stderr, "done printing children for: %s\n", node->name );
#endif /* DEBUG */
   } else {
      printf( "<file name=\"%s\" type=\"data\" source=\"%s\" />\n", node->name, node->path );
   }

   if( NULL != node->sibling ) {
#ifdef DEBUG
      fprintf( stderr, "printing sibling for: %s (%p)\n", node->name, node->sibling );
#endif /* DEBUG */
      isoxml_print( node->sibling, 0 );
#ifdef DEBUG
      fprintf( stderr, "done printing sibling for: %s\n", node->name );
#endif /* DEBUG */
   } else {
      if( !is_root ) {
         printf( "</dir>\n" );
      }
   }
}

struct ISO_NODE* isoxml_insert(
   struct ISO_NODE** p_all, struct ISO_NODE* root,
   size_t* p_ct, size_t* p_ct_max, char* path_rem, char* path_full
) {
   char* slash_pos = NULL;
   char name[NAME_SZ_MAX + 1];
   struct ISO_NODE* iter_node = root->child;
   struct ISO_NODE* path_node = NULL;
   struct ISO_NODE* new_all = NULL;

#if 0
   /* Ensure enough nodes. */
   if( *p_ct + 1 > *p_ct_max ) {
      *p_ct_max *= 2;
      new_all = realloc( *p_all, *p_ct_max );
      assert( NULL != new_all );
      *p_all = new_all;
   }
#endif
   /* Realloc causes the pointers to move around! What a pain! */
   assert( *p_ct < *p_ct_max );

   /* Prepare name buffer. */
   memset( name, '\0', NAME_SZ_MAX + 1 );
   strncpy( name, path_rem, NAME_SZ_MAX );

   /* See if this is a dir. */
   slash_pos = strchr( name, '/' );
   if( NULL != slash_pos ) {
      /* Terminate at the slash for this node name. */
      *slash_pos = '\0';
      assert( strlen( name ) < NAME_SZ_MAX );
   }

   /* Find the path node. */
   while( NULL != iter_node && 0 != strcmp( iter_node->name, name ) ) {
      iter_node = iter_node->sibling;
   }

   if( NULL == iter_node ) {
      /* Path node not found! */
      if( NULL != root->child ) {
         /* Add the path node as a sibling to existing children. */
         iter_node = root->child;
         while( NULL != iter_node->sibling ) {
            iter_node = iter_node->sibling;
         }
#ifdef DEBUG
         fprintf( stderr, "adding sibling: %s\n", name );
#endif /* DEBUG */
         iter_node->sibling = &((*p_all)[*p_ct]);
#ifdef DEBUG
         fprintf(
            stderr, "added sibling for %s: %p\n", iter_node->name, iter_node->sibling );
#endif /* DEBUG */
         (*p_ct)++;
         iter_node = iter_node->sibling;
      } else {
         /* Add the path node as the only child. */
#ifdef DEBUG
         fprintf( stderr, "adding child: %s\n", name );
#endif /* DEBUG */
         root->child = &((*p_all)[*p_ct]);
         (*p_ct)++;
         iter_node = root->child;
      }

      memset( iter_node, '\0', sizeof( struct ISO_NODE ) );
      strncpy( iter_node->name, name, NAME_SZ_MAX );
      strncpy( iter_node->path, path_full, NAME_SZ_MAX );
   }
#ifdef DEBUG
   else {
      fprintf( stderr, "found node: %s\n", name );
   }
#endif /* DEBUG */
   

   if( NULL != slash_pos ) {
      /* There are children, so descend! */
      return isoxml_insert(
         p_all, iter_node, p_ct, p_ct_max, slash_pos + 1, path_full );
   }
}

int main( int argc, char* argv[] ) {
   int retval = 0;
   int i = 0, j = 0;
   struct ISO_NODE* iso_all = NULL;
   struct ISO_NODE* iso_next = NULL;
   size_t iso_ct_max = 1000;
   size_t iso_ct = 1;

   iso_all = calloc( sizeof( struct ISO_NODE ), iso_ct_max );
   assert( NULL != iso_all );

   iso_all[0].active = 1;

   for( i = 1 ; argc > i ; i++ ) {
      isoxml_insert(
         &iso_all, &(iso_all[0]), &iso_ct, &iso_ct_max, argv[i], argv[i] );

      /*
      printf( "siblings:\n" );
      for( j = 0 ; iso_ct > j ; j++ ) {
         printf( "p %d: %p\n", j, iso_all[j].sibling );
      }
      */

   }

   isoxml_print( &(iso_all[0]), 1 );

   free( iso_all );

   return retval;
}

