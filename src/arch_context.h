typedef struct ArchContext ArchContext;

int __cdecl ArchContext_Get(ArchContext * pContext);
__declspec(noreturn) void __cdecl ArchContext_Set(ArchContext * pContext);

void ArchContext_Twiddle( ArchContext * pContext, void * opt_InstructionPointer, void * opt_StackPointer );

typedef unsigned short _16_Bits;
typedef unsigned int   _32_Bits;

//
// These are now 'mini' versions of jmpbuf 's or ucontext_t 's
//  I dont need a union with signaling, nor most other stuff
//  I just want enough information to reconstruct register state.
//
#ifdef _M_IX86

struct ArchContext {

	_32_Bits	edi,			
				esi,			
				ebp,			
				ebx,			
				edx,			
				ecx,			
				eax,			
				eip,			
				esp;

	_16_Bits	fs,	
				es,		
				ds,		
				ss;
};

#endif

