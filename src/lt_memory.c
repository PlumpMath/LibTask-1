#include "libtask_private.h"

//
// globals
//
void * ( * lt_malloc )( size_t s ) = malloc;
void   ( * lt_free )( void * ptr ) = free;

//
// locals
//
static void * lt_opaque;
static void * ( * lt_mallocEx )( void * opaque, size_t s );
static void   ( * lt_freeEx )( void * opaque, void * ptr );

//
// util
//
static void * ex_malloc( size_t s )
{
	return lt_mallocEx( lt_opaque, s );
}

static void ex_free( void * ptr )
{
	lt_freeEx( lt_opaque, ptr );
}

//
// API
//
void LibTask_SetMemory( void * ( * lib_malloc )( size_t s ), void ( * lib_free )( void * p ) )
{
	if ( NULL != lib_malloc ) {
		lt_malloc = lib_malloc;
		lt_free = lib_free;
	} else {
		lt_malloc = malloc;
		lt_free = free;
	}
	lt_opaque = NULL;
}

void LibTask_SetMemoryWithOpaque( void * ( * lib_malloc )( void * opaque, size_t s ), void ( * lib_free )(  void * opaque, void * p ),  void * opaque )
{
	if ( NULL != lib_malloc ) {
		lt_malloc = ex_malloc;
		lt_free = ex_free;

		lt_opaque = opaque;
		lt_mallocEx = lib_malloc;
		lt_freeEx = lib_free;

	} else {
		LibTask_SetMemory( NULL, NULL );
	}
}


