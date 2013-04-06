#include "libtask_private.h"

static int Channel_IsClosed( Channel * chan ) 
{
	return 0 == chan->NumberOfItems;
}

static void Channel_MarkClosed( Channel * chan ) 
{
	chan->NumberOfItems= 0;
}


Channel * Channel_New( size_t numberOfSlots, size_t sizeOfType )
{
	size_t toAlloc = sizeof( Channel ) + numberOfSlots * sizeOfType;

	return LT_TAG( Channel_NewFromSpace( numberOfSlots, sizeOfType, toAlloc, lt_malloc( toAlloc ) ), Channel * );
}

Channel * Channel_NewFromSpace( size_t numberOfSlots, size_t sizeOfType, size_t sizeOfSpace, void * space )
{
	Channel * out = (Channel *)space;

	if ( NULL == space ) {
		return NULL;
	}

	out->NumberOfItems = numberOfSlots;
	out->SizeOfItem = sizeOfType;
	
	out->SendI = 
	out->RecvI = 0;

	out->SenderList =
	out->RecverList = NULL;

	return out;
}

void Channel_Send( Channel * chan , void * data, size_t sizeOfData )
{
	LT_UNTAG(chan, Channel*);

	for (;;) {
		// if channel closed, return
		if ( Channel_IsClosed( chan ) ) {
			return;
		}

		// if enough space, copy and return.
		// if recvers waiting, wake one and continue
		// add self to senders list and yeild
	}
}

void Channel_Recv( Channel * chan , void * data, size_t sizeOfData )
{
	LT_UNTAG(chan, Channel*);

	for (;;) {
		// if channel closed, return
		if ( Channel_IsClosed( chan ) ) {
			return;
		}
		// if enough data, copy and return.
		// if senders waiting, wake one and continue
		// add self to recvers list and yeild
	}
}

void Channel_CloseAndDelete( Channel * chan )
{
	const int doFree = LT_ISTAGED( chan );

	LT_UNTAG(chan, Channel*);

	// mark channel closed.
	Channel_MarkClosed( chan );
	// wake all senders.
	// wake all recvers.
	// free if nessary
	if (doFree) {
		lt_free( chan );
	}
}
