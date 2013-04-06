#include <stdio.h>

#include "..\libtask.h"
#include "heap.h"

void c1(void * nu)
{
	int i;

	for( i = 0; i < 10; i++ ) {
		printf("c1 %d\n", i );
		InCoroutine_Yeild();
	}
}

void c2(void * nu)
{
	int i;

	for( i = 0; i < 10; i++ ) {
		printf("c2 %d\n", i );
		InCoroutine_Yeild();
	}
}

int main(int argc, char ** argv)
{
	Coroutine * _1;
	Coroutine * _2;

	Heap * heap = Heap_New( 10 << 20, HeapAligment_Default );

	LibTask_SetMemoryWithOpaque( Heap_Alloc, Heap_Free, heap );

	_1 = Coroutine_New( c1, NULL, 6 << 10 );
	_2 = Coroutine_New( c2, NULL, 6 << 10 );

	Coroutine_Run( _1 );
	Coroutine_Run( _1 );
	Coroutine_Run( _2 );
	Coroutine_Run( _1 );
	Coroutine_Run( _1 );
	Coroutine_Run( _2 );

	Coroutine_Delete( _2 );
	Coroutine_Delete( _1 );

	//exit(0);
	return 0;
}

