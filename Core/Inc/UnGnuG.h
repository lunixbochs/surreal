/*=============================================================================
	UnGnuG.h: Unreal definitions for Gnu G++. Unfinished. Unsupported.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

/*----------------------------------------------------------------------------
	Platform compiler definitions.
----------------------------------------------------------------------------*/

#define __UNIX__  1
#define __INTEL__ 1
#undef ASM
#undef ASM3DNOW
#undef ASMKNI

#if defined(__LINUX__)

	#undef ASMLINUX
	#if !defined(__LP64__)
		#define ASMLINUX 1
	#endif

	#include <endian.h>
	#if __BYTE_ORDER == __LITTLE_ENDIAN
		#define __INTEL_BYTE_ORDER__ 1
	#endif

#elif defined(__APPLE__)

	#undef ASMLINUX

	#if defined(__LITTLE_ENDIAN__)
		#define __INTEL_BYTE_ORDER__ 1
	#endif

#else
	#error Unsupported platform.
#endif

#if defined(__clang__)
	#define COMPILER "Compiled with Clang ("__VERSION__")"
#else
	#define COMPILER "Compiled with GNU g++ ("__VERSION__")"
#endif

// Stack control.
#include <sys/wait.h>
#include <signal.h>
#include <setjmp.h>
class __Context
{
public:
	__Context() { memcpy(Last, Env, sizeof(jmp_buf)); }
	~__Context() { memcpy(Env, Last, sizeof(jmp_buf)); }
	static void StaticInit();
	static jmp_buf Env;

protected:
	static void HandleSignal( int Sig );
	static struct sigaction Act_SIGHUP;
	static struct sigaction Act_SIGQUIT;
	static struct sigaction Act_SIGILL;
	static struct sigaction Act_SIGTRAP;
	static struct sigaction Act_SIGIOT;
	static struct sigaction Act_SIGBUS;
	static struct sigaction Act_SIGFPE;
	static struct sigaction Act_SIGSEGV;
	static struct sigaction Act_SIGTERM;
	jmp_buf Last;
};

/*----------------------------------------------------------------------------
	Platform specifics types and defines.
----------------------------------------------------------------------------*/

// Undo any Windows defines.
#undef BYTE
#undef WORD
#undef DWORD
#undef INT
#undef FLOAT
#undef MAXBYTE
#undef MAXWORD
#undef MAXDWORD
#undef MAXINT
#undef VOID
#undef CDECL

// Make sure HANDLE is defined.
#define HANDLE DWORD
#define HINSTANCE DWORD

// Sizes.
enum {DEFAULT_ALIGNMENT = 16}; // Default boundary to align memory allocations on.
enum {CACHE_LINE_SIZE   = 32}; // Cache line size.
#define GCC_PACK(n) __attribute__((packed,aligned(n)))
//#define GCC_MOVE_ALIGN(n) __attribute__((aligned(n))) __attribute__((section (".bss")))
#define GCC_ALIGN(n) __attribute__((aligned(n)))
#define GCC_MOVE_ALIGN(n)
//#define GCC_ALIGN(n)

// Optimization macros
#define DISABLE_OPTIMIZATION
#define ENABLE_OPTIMIZATION

// Function type macros.
#define DLL_IMPORT
#define DLL_EXPORT			extern "C"
#define DLL_EXPORT_CLASS
#define VARARGS
#define CDECL
#define STDCALL
#define FORCEINLINE /* Force code to be inline */
#define ZEROARRAY 0 /* Zero-length arrays in structs */
#define __cdecl

// Variable arguments.
#define GET_VARARGS(msg,len,fmt)	\
{	\
	va_list ArgPtr;	\
	va_start( ArgPtr, fmt );	\
	vsprintf( msg, fmt, ArgPtr );	\
	va_end( ArgPtr );	\
}

#define GET_VARARGS_RESULT(msg,len,fmt,result)	\
{	\
	va_list ArgPtr;	\
	va_start( ArgPtr, fmt );	\
	result = vsprintf( msg, fmt, ArgPtr );	\
	va_end( ArgPtr );	\
}

#include <stdint.h>

// Unsigned base types.
typedef uint8_t		BYTE;		// 8-bit  unsigned.
typedef uint16_t	_WORD;		// 16-bit unsigned.
typedef uint32_t	DWORD;		// 32-bit unsigned.
typedef uint64_t	QWORD;		// 64-bit unsigned.

// Signed base types.
typedef	int8_t		SBYTE;		// 8-bit  signed.
typedef int16_t		SWORD;		// 16-bit signed.
typedef int32_t 	INT;		// 32-bit signed.
typedef int64_t		SQWORD;		// 64-bit signed.

// Character types.
typedef char			    ANSICHAR;	// An ANSI character.
typedef unsigned short      UNICHAR;	// A unicode character.
typedef unsigned char		ANSICHARU;	// An ANSI character.
typedef unsigned short      UNICHARU;	// A unicode character.

// Other base types.
typedef signed int			UBOOL;		// Boolean 0 (false) or 1 (true).
typedef float				FLOAT;		// 32-bit IEEE floating point.
typedef double				DOUBLE;		// 64-bit IEEE floating point.
typedef size_t				SIZE_T;		// Corresponds to C SIZE_T.

// Bitfield type.
typedef unsigned int		BITFIELD;	// For bitfields.

// Make sure characters are unsigned.
#ifdef __CHAR_UNSIGNED__
	#error "Bad compiler option: Characters must be signed"
#endif

// Strings.
#if __UNIX__
#define LINE_TERMINATOR TEXT("\n")
#define PATH_SEPARATOR TEXT("/")
#define DLLEXT TEXT(".so")
#else
#define LINE_TERMINATOR TEXT("\r\n")
#define PATH_SEPARATOR TEXT("\\")
#define DLLEXT TEXT(".dll")
#endif

// NULL.
#undef NULL
#define NULL 0

// Package implementation.
#define IMPLEMENT_PACKAGE_PLATFORM(pkgname) \
	BYTE GLoaded##pkgname;

// Platform support options.
#define PLATFORM_NEEDS_ARRAY_NEW 1
#define FORCE_ANSI_LOG           0

// OS unicode function calling.
#define TCHAR_CALL_OS(funcW,funcA) (funcA)
#define TCHAR_TO_ANSI(str) str
#define ANSI_TO_TCHAR(str) str

// !! Fixme: This is a workaround.
#define GCC_OPT_INLINE

// Memory
#define appAlloca(size) alloca((size+7)&~7)

// Forward declaration.
extern "C"
{
	CORE_API FTime appSecondsSlow();
}

//
// Round a floating point number to an integer.
// Note that (int+.5) is rounded to (int+1).
//
#define DEFINED_appRound 1
inline INT appRound( FLOAT f )
{
	return (INT)(f);
}

//
// Converts to integer equal to or less than.
//
#define DEFINED_appFloor 1
inline INT appFloor( FLOAT f )
{
	return (INT)(f);
}

#if defined(__MACH__)
	#include <mach/mach_time.h>
#else
	#include <time.h>
#endif

//
// CPU cycles, related to GSecondsPerCycle.
//
#if ASMLINUX
#define DEFINED_appCycles 1
inline DWORD appCycles()
{
	if( GTimestamp )
	{
		DWORD r;
		asm("rdtsc" : "=a" (r) : "d" (r));
		return r;
	}
}
#endif

//
// Seconds, arbitrarily based.
//
#define DEFINED_appSeconds 1
inline FTime appSeconds()
{
#if defined(CLOCK_MONOTONIC)
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return 1e-9 * (FLOAT)t.tv_sec + (FLOAT)t.tv_nsec;
#elif defined(__MACH__)
	static FLOAT factor = 0;
	if( factor == 0 ){
		mach_timebase_info_data_t info;
		if( mach_timebase_info( &info ) == 0 )
			factor = 1e-9 * (FLOAT)info.numer / (FLOAT)info.denom;
		else
			factor = -1;
	}
	if( factor != -1 )
		return factor * (FLOAT)mach_absolute_time();
	else
		return appSecondsSlow();
#else
	return appSecondsSlow();
#endif
}

//
// Memory copy.
//
#if ASMLINUX
#define DEFINED_appMemcpy 1
inline void appMemcpy( void* Dest, const void* Src, INT Count )
{
	asm volatile(
		"pushl %%ebx;"
		"pushl %%ecx;"
		"pushl %%esi;"
		"pushl %%edi;"
		"mov %%ecx, %%ebx;"
		"shr $2, %%ecx;"
		"and $3, %%ebx;"
		"rep;"
		"movsl;"
		"mov %%ebx, %%ecx;"
		"rep;"
		"movsb;"
		"popl %%edi;"
		"popl %%esi;"
		"popl %%ecx;"
		"popl %%ebx;"
	:
	: "S" (Src),
	  "D" (Dest),
	  "c" (Count)
	);
}
#endif

//
// Memory zero.
//
#define DEFINED_appMemzero 1
inline void appMemzero( void* Dest, INT Count )
{
	memset( Dest, 0, Count );
}

/*----------------------------------------------------------------------------
	Globals.
----------------------------------------------------------------------------*/

// System identification.
extern "C"
{
	extern HINSTANCE      hInstance;
	extern CORE_API UBOOL GIsMMX;
	extern CORE_API UBOOL GIsPentiumPro;
	extern CORE_API UBOOL GIsKatmai;
	extern CORE_API UBOOL GIsK6;
	extern CORE_API UBOOL GIs3DNow;
	extern CORE_API UBOOL GTimestamp;
}

// Module name
extern ANSICHAR GModule[32];

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
