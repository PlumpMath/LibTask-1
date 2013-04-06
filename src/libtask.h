#ifndef _LIB_TASK_
#define _LIB_TASK_

#include <stdlib.h> // for size_t

#ifdef __cplusplus
	#define LT_API extern "C"
#else
	#define LT_API
#endif

//
// Libaries memory management
//
LT_API void LibTask_SetMemory( void * ( * lib_malloc )( size_t s ), void ( * lib_free )( void * p ) );
LT_API void LibTask_SetMemoryWithOpaque( void * ( * lib_malloc )( void * opaque, size_t s ), void ( * lib_free )(  void * opaque, void * p ),  void * opaque );

//
// Coroutine
//  I'd have the def here, but it varies in size depending on arch
//
typedef struct Coroutine Coroutine;

LT_API Coroutine * Coroutine_New( void ( * proc )( void * arg ), void * arg, size_t stackSize );
LT_API Coroutine * Coroutine_NewWithStack( void ( * proc )( void * arg ), void * arg, size_t stackSize, void * stack );

LT_API void Coroutine_Run( Coroutine * toRun );
LT_API void * Coroutine_Genorate( Coroutine * toRun );

LT_API void Coroutine_Delete( Coroutine * toDel );

LT_API void InCoroutine_Yeild();
LT_API void InCoroutine_Genorate( void * value );
LT_API Coroutine * InCoroutine_Current();
LT_API void InCoroutine_Exit();
LT_API void InCoroutine_ExitTo( Coroutine * next );

//
// Channel
//  NOT DONE YET. DO NOT USE
//
typedef struct Channel Channel;

LT_API Channel * Channel_New( size_t numberOfSlots, size_t sizeOfType );
LT_API Channel * Channel_NewFromSpace( size_t numberOfSlots, size_t sizeOfType, size_t sizeOfSpace, void * space );

LT_API void Channel_Send( Channel * chan , void * data, size_t sizeOfData );
LT_API void Channel_Recv( Channel * chan , void * data, size_t sizeOfData );

LT_API void Channel_CloseAndDelete( Channel * chan );


#endif//_LIB_TASK_
