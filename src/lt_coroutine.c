#include "libtask_private.h"

__declspec( thread ) Coroutine   threadLocal_Thread;
__declspec( thread ) Coroutine * threadLocal_Current = NULL;

//
// Util
//
static void ThreadCoroutine_Init()
{
	assert( NULL == threadLocal_Current );

	memset( &threadLocal_Thread, 0, sizeof( threadLocal_Thread ) );
	threadLocal_Current = &threadLocal_Thread;
}

typedef struct Coroutine_EntryInfo {
	void ( * call ) ( void * arg );
	void * arg;
} Coroutine_EntryInfo;

static void Coroutine_Entry()
{
	Coroutine_EntryInfo * info = (Coroutine_EntryInfo*)( InCoroutine_Current() + 1 );
	info->call( info->arg );
}

static void Coroutine_SwitchFromTo( Coroutine * from, Coroutine * to )
{
	if ( 0 == ArchContext_Get( &from->Context ) ) {
		threadLocal_Current = to;
		ArchContext_Set( &to->Context );
	}
}

//
// API
//
Coroutine * Coroutine_New( void ( * proc )( void * arg ), void * arg, size_t stackSize )
{
	stackSize += sizeof( Coroutine );

	return LT_TAG( Coroutine_NewWithStack( proc, arg, stackSize, lt_malloc( stackSize ) ), Coroutine * );
}

Coroutine * Coroutine_NewWithStack( void ( * proc )( void * arg ), void * arg, size_t stackSize, void * stack )
{
	Coroutine_EntryInfo * info;
	Coroutine * out = (Coroutine *) stack;

	if ( NULL == stack ) {
		return NULL;
	}

	ArchContext_Get( &out->Context );
	ArchContext_Twiddle( &out->Context, Coroutine_Entry, ((char *)stack) + stackSize );

	out->Caller	=
	out->NextInList = NULL;
	out->GenorateSpace = NULL;

	info = (Coroutine_EntryInfo*)(out + 1);
	info->call = proc;
	info->arg = arg;

	if ( NULL == InCoroutine_Current() ) {
		ThreadCoroutine_Init();
	}

	return out;
}

void Coroutine_Run( Coroutine * toRun )
{
	LT_UNTAG( toRun, Coroutine * );

	toRun->Caller = InCoroutine_Current();
	Coroutine_SwitchFromTo( InCoroutine_Current(), toRun );
}

void * Coroutine_Genorate( Coroutine * toRun )
{
	LT_UNTAG( toRun, Coroutine * );

	Coroutine_Run( toRun );
	return toRun->GenorateSpace;
}

void Coroutine_Delete( Coroutine * toDel )
{
	const int doFree = LT_ISTAGED(toDel);
	LT_UNTAG( toDel, Coroutine * );

	if (doFree) {
		lt_free( toDel );
	}
}

void InCoroutine_Yeild()
{
	Coroutine_SwitchFromTo( InCoroutine_Current(), InCoroutine_Current()->Caller );
}

void InCoroutine_Genorate( void * value )
{
	InCoroutine_Current()->GenorateSpace = value;
	InCoroutine_Yeild();
}

Coroutine * InCoroutine_Current()
{
	return threadLocal_Current;
}

void InCoroutine_Exit()
{
	InCoroutine_ExitTo( InCoroutine_Current()->Caller );
}

void InCoroutine_ExitTo( Coroutine * next )
{
	LT_UNTAG( next, Coroutine * );

}
