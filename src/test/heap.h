#ifndef _HEAP_
#define _HEAP_

#include <stdlib.h> // for size_t

#ifdef __cplusplus
	#define HEAP_API extern "C"
#else
	#define HEAP_API
#endif

typedef struct Heap Heap;
typedef enum Heap_Aligment Heap_Aligment;

enum Heap_Aligment 
{
	HeapAligment_1Byte = 0,
	HeapAligment_2Byte,
	HeapAligment_4Byte,
	HeapAligment_8Byte,
	HeapAligment_16Byte = 4, //starts being useful
	HeapAligment_32Byte,
	HeapAligment_64Byte,
	HeapAligment_128Byte,
	HeapAligment_256Byte,
	HeapAligment_512Byte,
	HeapAligment_1024Byte,
	HeapAligment_2048Byte,

	HeapAligment_Default = HeapAligment_16Byte
};	

HEAP_API Heap * Heap_New( size_t size, Heap_Aligment align );
HEAP_API Heap * Heap_NewToManage( size_t size, Heap_Aligment align, void * ptr );

HEAP_API void * Heap_Alloc( Heap * heap, size_t size );
HEAP_API void * Heap_AllocAligned( Heap * heap, size_t size, Heap_Aligment align );

HEAP_API void * Heap_Realloc( Heap * heap, void * ptr, size_t size );

HEAP_API void   Heap_Free( Heap * heap, void * ptr );

HEAP_API size_t Heap_SizeOf( Heap * heap, void * ptr );
HEAP_API size_t Heap_AlignOf( Heap * heap, void * ptr );
HEAP_API void   Heap_Stats( Heap * heap, size_t * opt_TotalFree, size_t * opt_TotalUsed, size_t * opt_LargestFree );


//void   Heap_Defrag( Heap * heap, int ( * IsMovable )( void * ptr ), void ( * Moving )( void * ptr ), void ( * Moved )( void * from, void * to ), unsigned int CycleCount );

#endif//_HEAP_
