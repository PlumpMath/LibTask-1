#include <assert.h>
#include <string.h> // for memset

#include "libtask.h"
#include "arch_context.h"

//
// in lt_memory.c
//  Lib wide memory functions
//
extern void * ( * lt_malloc )( size_t s );
extern void   ( * lt_free )( void * ptr );

//
// Util
//
#define LT_TAG( P, T)		(T) ( (unsigned long)(P) | 0x1UL)
#define LT_UNTAG( P, T )	P = (T)	( (unsigned long)(P) & ~0x1UL)
#define LT_ISTAGED( P )		( (unsigned long)(P) & 0x1UL)

//
// Coroutine
//
struct Coroutine {
	
	ArchContext Context;

	Coroutine * Caller,
			  * NextInList;

	void *		GenorateSpace;

};


//
// Channel
//
struct Channel {
	Coroutine	* SenderList,
				* RecverList;

	size_t		NumberOfItems,
				SizeOfItem,
				SendI,
				RecvI;
};
