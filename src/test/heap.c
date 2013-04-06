#include <assert.h>
#include <string.h> // for memmove

#include "heap.h"

typedef unsigned int	Block;
typedef unsigned char	Byte;

#define IsPowerOf2( X )			( (X) && !((X) & ((X)-1)) ) 
#define RoundUp( X, R )			( ( (X) + ((R)-1) ) & ~((R)-1) )

#define NoOfSentinals		1
#define Size_ToBlockCount( S, AS )	( ((S)+((1<<(AS))-1)) >> AS )

#define NumberOfInfoBits	2
#define InfoAllocated		0x1
#define InfoExtraAlign		0x2

#define Block_GetSize(b)		( (b) >> NumberOfInfoBits )

#define Block_IsAllocated(b)	( (b) & InfoAllocated )
#define Block_IsFree(b)			( 0 == ( (b) & InfoAllocated ) )

#define Block_NewAlloced(sz)	( ( (sz) << NumberOfInfoBits ) | InfoAllocated )
#define Block_NewFree(sz)		(   (sz) << NumberOfInfoBits )

struct Heap {
	size_t			NumberOfBlocks;

	Heap_Aligment	ChunkSizeAsShift;
};

//
// Util
//
static size_t calcNumOfManagedBlocks(size_t totalBytes, size_t alignmentShift)
{
	const size_t sizeof_Block = sizeof( Block );

	//////////////////////////////////////////////////////////////////////////
	// This equation is to work out how many blocks I can manage
	// give a total memory budget.
	//
	//  Given I want to use as much of the memory as possible (totalBytes),
	//  Now this system works in blocks.
	//
	const size_t totalChunks = totalBytes >> alignmentShift; //totalChunks {Total chunk} = totalBytes {Total memory} / alignment
	//
	// For each chunk we can use we have a tBlock.
	// we can get bpc {blocks per chunk} by the following
	//
	const size_t bpc = (1 << alignmentShift) / sizeof_Block; 
	//
	// The total chunks we can use (x) can be expressed as
	//
	//			 totalChunks	= x + Mc
	// where	 Mc	= (x + 2) / bpc // {Mc = Management chunks} {2 is for sentinel blocks at either end)
	//
	// so
	//			 totalChunks	= x + (x + 2) / bpc				// substitute Mc
	//	   totalChunks * bpc	= x * bpc + x + 2				// multiply by bpc
	// totalChunks * bpc - 2	= x * bpc + x					// minus 2
	// totalChunks * bpc - 2	= x * ( bpc + 1 )				// gather like terms
	//			  x = (totalChunks * bpc - 2) / ( bpc + 1 )	// divide by ( bpc + 1 )
	//
	return (totalChunks * bpc - NoOfSentinals) / ( bpc + 1 );
}

static Block * Heap_GetBlocks( Heap * heap )
{
	return ((Block*) (heap)) - ( heap->NumberOfBlocks + NoOfSentinals);
}

static Byte * Heap_GetChunk( Heap * heap, size_t i )
{
	return ( ( Byte * ) ( heap ) ) + (i << heap->ChunkSizeAsShift);
}

static size_t Heap_GetChunkIndex( Heap * heap, void * ptr )
{
	return  ( ( (size_t) ptr ) - ( (size_t) heap ) ) >> heap->ChunkSizeAsShift;
}

static void Heap_MarkAlloc( Heap * heap, size_t i, size_t s )
{
	Block * mb = Heap_GetBlocks( heap );
	size_t  bs = Block_GetSize( mb[i] );

	mb[i] = 
		mb[i+s-1] = Block_NewAlloced( s );

	if ( bs > s ) {
		mb[i+s] = 
			mb[i+bs-1] = Block_NewFree( bs-s );
	}
}

static size_t Heap_MarkFree( Heap * heap, size_t i )
{
	Block * mb = Heap_GetBlocks( heap );
	size_t  bs = Block_GetSize( mb[i] );

	// collate next
	if ( Block_IsFree( mb[i+bs] ) ) {
		bs += Block_GetSize( mb[i+bs] );
	}
	
	// collate previous
	if ( Block_IsFree( mb[i-1] ) ) {
		size_t ps =Block_GetSize( mb[i-1] );
		i -= ps;
		bs += ps;
	}

	mb[i] =
		mb[i+bs-1] = Block_NewFree( bs );

	return i;
}

static size_t Heap_FindFirstFreeForSize( Heap * heap, size_t minimum )
{
	size_t i;
	
	const size_t c =heap->NumberOfBlocks;
	Block * mb = Heap_GetBlocks( heap );
	
	for (i = 0; i < c; ) {

		const size_t bs = Block_GetSize( mb[i] );

		if ( Block_IsFree( mb[i] ) && (bs >= minimum) ) {
			return i;
		}

		i += bs;
	}

	return 0; //didn't find. not enough space
}

//
// API
//
Heap * Heap_New( size_t size, Heap_Aligment align )
{
	return Heap_NewToManage( size, align, malloc( size ) );
}

Heap * Heap_NewToManage( size_t size, Heap_Aligment align, void * ptr )
{
	Block * blocks;

	size_t heapSizeInBlocks = Size_ToBlockCount( sizeof(Heap), align );
	size_t blockCount = calcNumOfManagedBlocks( size, align );
	Heap * heap = (Heap*) (((Block*) ptr) + blockCount + NoOfSentinals);

	heap->ChunkSizeAsShift = align;
	heap->NumberOfBlocks = blockCount;

	blocks = Heap_GetBlocks( heap );
	
	blocks[0] = 
		blocks[blockCount-1] =	 Block_NewFree( blockCount );

	// sentinals
	blocks[blockCount] =	 Block_NewAlloced( 0 ); // sentinal
	Heap_MarkAlloc( heap, 0, heapSizeInBlocks );    // actual management for the heap alloc.
													// never deleted, so woks as a sentinal

	return heap;
}

void * Heap_Alloc( Heap * heap, size_t size )
{
	size_t bs = Size_ToBlockCount( size, heap->ChunkSizeAsShift );
	size_t i = Heap_FindFirstFreeForSize( heap, bs );

	if ( 0 == i ) {
		return NULL;
	}

	Heap_MarkAlloc( heap, i, bs );
	return Heap_GetChunk( heap, i );
}

void * Heap_AllocAligned( Heap * heap, size_t size, Heap_Aligment align )
{
	if ( align <= heap->ChunkSizeAsShift ) {
		return Heap_Alloc( heap, size );
	}

	return 0;
}

void * Heap_Realloc( Heap * heap, void * ptr, size_t size )
{
	Block * mb = Heap_GetBlocks( heap );
	size_t ni, i = Heap_GetChunkIndex( heap, ptr );
	size_t minimum = Size_ToBlockCount( size, heap->ChunkSizeAsShift );
	
	size_t currentSize = Block_GetSize(mb[i]);

	// same size
	if ( currentSize == minimum ) {
		return ptr;
	}

	// shrink
	if ( currentSize > minimum ) {
		Heap_MarkAlloc( heap, i, minimum );
		return ptr;
	}

	// grow
	ni = Heap_MarkFree( heap, i );
	currentSize = Block_GetSize(mb[i]);

	if ( currentSize >= minimum ) {
		Heap_MarkAlloc( heap, ni, minimum );

		// if moved, then move content
		if (ni != i) {
			void * nptr = Heap_GetChunk( heap, ni );

			memmove( nptr, ptr, currentSize << heap->ChunkSizeAsShift ); 
			ptr = nptr;
		}

		return ptr;
	}

	return 0;
}

void   Heap_Free( Heap * heap, void * ptr )
{
	Heap_MarkFree( heap, Heap_GetChunkIndex( heap, ptr ) );
}

size_t Heap_SizeOf( Heap * heap, void * ptr )
{
	Block * mb = Heap_GetBlocks( heap );
	size_t i = Heap_GetChunkIndex( heap, ptr );

	return Block_GetSize( mb[i] ) << heap->ChunkSizeAsShift;
}

size_t Heap_AlignOf( Heap * heap, void * ptr )
{
	(void)ptr;//not used

	return 1 << heap->ChunkSizeAsShift;
}

void   Heap_Stats( Heap * heap, size_t * opt_TotalFree, size_t * opt_TotalUsed, size_t * opt_LargestFree )
{
	size_t i;
	
	const size_t c =heap->NumberOfBlocks;
	Block * mb = Heap_GetBlocks( heap );

	if (opt_TotalFree) { *opt_TotalFree = 0; }
	if (opt_TotalUsed) { *opt_TotalUsed = 0; }
	if (opt_LargestFree) { *opt_LargestFree = 0; }

	for (i = 0; i < c; ) {

		const size_t bs = Block_GetSize( mb[i] );
		const size_t s = bs << heap->ChunkSizeAsShift;

		if ( Block_IsFree( mb[i] ) ) {

			if (opt_TotalFree) { *opt_TotalFree += s; }
			if (opt_LargestFree) { 
				if (*opt_LargestFree < s) {
					*opt_LargestFree= s;
				}
			}

		} else { 

			if (opt_TotalUsed) { *opt_TotalUsed += s; }
		}

		i += bs;
	}
}

//void   Heap_Defrag( Heap * heap, int ( * IsMovable )( void * ptr ), void ( * Moving )( void * ptr ), void ( * Moved )( void * from, void * to ), unsigned int CycleCount )
//{
//	size_t i;
//	
//	const size_t c =heap->NumberOfBlocks;
//	Block * mb = Heap_GetBlocks( heap );
//
//	while ( CycleCount-- ) {
//
//	}
//}

