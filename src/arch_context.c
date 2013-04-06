
#ifndef _MSC_VER
#error Attempting to copile Microsoft Visual C version of asm.
#endif

#include "arch_context.h"

#ifdef _M_IX86

__declspec( naked ) int __cdecl ArchContext_Get(ArchContext * pContext)
{
	__asm {
		// use pContext from eax
		mov eax, [esp+4]

		// copy the 16bit registers
		mov [eax].fs,	fs
		mov [eax].es,	es
		mov [eax].ds,	ds
		mov [eax].ss,	ss

		// copy most of the 32 bit regsters
		mov [eax].edi, edi
		mov [eax].esi, esi
		mov [eax].ebp, ebp
		mov [eax].ebx, ebx
		mov [eax].edx, edx
		mov [eax].ecx, ecx

		// eax is our 'return' value for next time
		mov [eax].eax, 1 

		// get the return address of where we were called from
		mov ecx, [esp]
		mov [eax].eip, ecx

		// get the address of the stack
		lea ecx, [esp+4]
		mov [eax].esp, ecx

		// restore ecx. cound be important
		mov ecx, [eax].ecx

		// return 0
		mov eax, 0
		ret
	}
}

#pragma warning( disable : 4731 ) // modifying the frame pointer

__declspec( naked ) void __cdecl ArchContext_Set(ArchContext * pContext)
{
	__asm {
		// use pContext from eax
		mov eax, [esp+4]

		// stomp the 16 bits
		mov fs,  [eax].fs
		mov es,  [eax].es
		mov ds,  [eax].ds
		mov ss,  [eax].ss

		// stomp the 32 bits
		mov edi, [eax].edi
		mov esi, [eax].esi
		mov ebp, [eax].ebp
		mov ebx, [eax].ebx
		mov edx, [eax].edx
		mov ecx, [eax].ecx
		mov esp, [eax].esp

		// push our return instruction address
		// when we do the 'ret' eip will be set to this
		push [eax].eip
		
		// return the stored eax (probably 1)
		// to where we want to go.
		mov eax, [eax].eax
		ret
	}
	
}

void ArchContext_Twiddle( ArchContext * pContext, void * opt_InstructionPointer, void * opt_StackPointer )
{
	if ( opt_InstructionPointer ) {
		pContext->eip = (_32_Bits) opt_InstructionPointer;
	}

	if ( opt_StackPointer ) {
		pContext->esp = (_32_Bits) opt_StackPointer;
	}
}


#endif

#ifndef _M_AMD64
#endif



