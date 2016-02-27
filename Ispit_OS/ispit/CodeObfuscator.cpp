#include<BaseTsd.h>
#include"CodeObfuscator.h"

#if defined _WIN64
DWORD64 _malware_hide_function_mask=0x3f6202ab;  // this should be randomized
#else
DWORD32 _malware_hide_function_mask=0x3f6202ab;  // this should be randomized
#endif

int __X0 = 0;
int __X2 = 2;


int __fastcall  _function_1_returns_0(void)
{
#if defined _WIN64
	return 0;
#else
	//////////////////////////////////////////////////////////////////////////
	// formula: (3*x*x - 1)/2 + (2*x + 1)/3 - (x*x - 2)/4 + 1/3
	//	for x=0 returns 0
	//////////////////////////////////////////////////////////////////////////
	_asm{
		xor					ebx,ebx
		mov         eax,ebx
		imul        eax,eax,3  
		imul        eax,ebx
		sub         eax,1  
		cdq  
		sub         eax,edx  
		mov         ecx,eax  
		sar         ecx,1  
		mov         edx,ebx
		lea         eax,[edx+edx+1]  
		cdq  
		mov         esi,3  
		idiv        esi  
		add         ecx,eax  
		mov         eax,ebx
		imul        eax,ebx
		sub         eax,2  
		cdq  
		and         edx,3  
		add         eax,edx  
		sar         eax,2  
		sub         ecx,eax  
		mov         eax,ecx  /* rezultat */
	}
#endif
}

int __fastcall  _function_2_returns_0(void)
{
#if defined _WIN64
	return 0;
#else
	//////////////////////////////////////////////////////////////////////////
	// formula: (x/2)*(x*x+1)-(2*((x+1)*(x+1))-5*x-3)
	//	for x=2 returns 0
	//////////////////////////////////////////////////////////////////////////
	_asm{
		mov					ebx,2
		mov         eax,ebx  
		cdq  
		sub         eax,edx  
		sar         eax,1  
		mov         ecx,ebx  
		imul        ecx,ebx  
		add         ecx,1  
		imul        eax,ecx  
		mov         edx,ebx  
		add         edx,1  
		mov         ecx,ebx  
		add         ecx,1  
		imul        edx,ecx  
		shl         edx,1  
		mov         ecx,ebx  
		imul        ecx,ecx,5  
		sub         edx,ecx  
		sub         edx,3  
		sub         eax,edx  
	}
#endif
}

void __fastcall ObfuscateBranchAsFunction()
{
#if defined _WIN64
	//FIXME: make this call in .ASM
#else
	_asm
	{
		xchg [esp+4], ecx
		xchg [esp+0], ecx
		pop ecx
		ret
	}
#endif
}