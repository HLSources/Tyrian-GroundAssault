//====================================================================
//
// Purpose: absolutely global things
//
//====================================================================
#ifndef PLATFORM_H
#define PLATFORM_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* !__MINGW32__ */
#endif /* _WIN32 */

// Allow "DEBUG" in addition to default "_DEBUG"
#ifdef _DEBUG
#define DEBUG 1
#endif

#include <stdio.h>// NULL, sprintf, sscanf, etc.

#ifdef _WIN32

#ifdef _MSC_VER
//#pragma warning(disable: 4244)// possible loss of data converting float to int
#pragma warning(disable: 4305)// truncation from 'const double' to 'float'
#pragma warning(disable: 4100)// unreferenced formal parameter
#pragma warning(disable: 4201)// nonstandard extension used : nameless struct/union
#pragma warning(disable: 4244)// conversion from 'int' to 'unsigned char', possible loss of data
#pragma warning(disable: 4310)// cast truncates constant value
#pragma warning(disable: 4514)// unreferenced inline function has been removed

#endif // _MSC_VER

#include <direct.h>// _mkdir

#else// _WIN32
/*
#pragma warning(disable : ??? )

warning: deprecated conversion from string constant to ‘char*’
error: cast from ‘char*’ to ‘int’ loses precision
*/
//#include <sys/dir.h>// _mkdir
//#include <unistd.h>
#include <sys/stat.h>
#include <math.h>

#ifndef _snprintf
#define _snprintf snprintf
#endif // !_snprintf

#ifndef _mkdir
#define _mkdir(path) mkdir(path, 0777)
#endif // !_mkdir

#endif// _WIN32


// Makes these more explicit, and easier to find
#define FILE_GLOBAL static
#define DLL_GLOBAL
// Until we figure out why "const" gives the compiler problems, we'll just have to use
// this bogus "empty" define to mark things as constant.
//#define CONSTANT
// More explicit than "int"
typedef int EOFFSET;

#ifndef TRUE
#define FALSE 0
#define TRUE 1
//#define TRUE (!FALSE)

#endif // !TRUE

// hack dragged since quake
typedef int BOOL;

#ifndef WORD
typedef unsigned short WORD;
#endif
#ifndef DWORD
typedef unsigned long DWORD;
#endif

typedef unsigned long ULONG;

// too bad it doesn't fit binary bool
enum// POLICY_VALUE
{
	POLICY_UNDEFINED = 0,// must be the initial value of a policy
	POLICY_ALLOW,// explicit value
	POLICY_DENY,// explicit value
};


// Prevent tons of unused windows definitions
#ifdef _WIN32
/*
#define WIN32_LEAN_AND_MEAN
#define NOWINRES
#define NOSERVICE
#define NOMCX
#define NOIME
#include "windows.h"
*/
#ifndef MAX_PATH
#define MAX_PATH _MAX_PATH
#endif
#else // _WIN32

#define MAX_PATH PATH_MAX
#include <limits.h>
#define _vsnprintf(a,b,c,d) vsnprintf(a,b,c,d)
#endif //_WIN32


#ifndef PATHSEPARATOR
#ifdef _WINDOWS
#define PATHSEPARATOR "\\"
#else
#define PATHSEPARATOR "/"
#endif // _WINDOWS
#endif // PATHSEPARATOR


/* Used for standard calling conventions */
#if (_MSC_VER > 0)/* VC compiler used */
	#define  STDCALL				__stdcall
	#define  FASTCALL			   __fastcall
#ifndef FORCEINLINE// GNU C may define it
	#define  FORCEINLINE		   __forceinline
#endif
#else
	#define  STDCALL
	#define  FASTCALL
#ifndef FORCEINLINE
	#define  FORCEINLINE		   inline
#endif
#endif

#ifdef _WIN32
#define DLLEXPORT	__declspec(dllexport)// XDM: right name, but conflicts with stupidity in eiface.h
#define EXPORT		__declspec(dllexport)// XDM3034: ANSI C++
#else
#define DLLEXPORT	/* */
#define EXPORT		/* */
#endif

// Keeps clutter down a bit, when declaring external entity/global method prototypes
//#define DECLARE_GLOBAL_METHOD(MethodName) extern void DLLEXPORT MethodName(void)
//#define GLOBAL_METHOD(funcname) void DLLEXPORT funcname(void)


#ifdef _DEBUG
bool DBG_AssertFunction(bool fExpr, const char *szExpr, const char *szFile, int szLine, const char *szMessage);
#define ASSERT(f)		DBG_AssertFunction(f, #f, __FILE__, __LINE__, NULL)
#define ASSERTSZ(f, sz)	DBG_AssertFunction(f, #f, __FILE__, __LINE__, sz)
#ifdef _MSC_VER
#define DBG_FORCEBREAK	_asm {int 3};// XDM3035
#else
#define DBG_FORCEBREAK	ASSERT(0);
#endif
#else	// !DEBUG

bool NDB_AssertFunction(bool fExpr, const char *szExpr, const char *szMessage);
#define ASSERT(f)		NDB_AssertFunction(f, #f, NULL)
#define ASSERTSZ(f, sz) NDB_AssertFunction(f, #f, sz)
#define DBG_FORCEBREAK	ASSERT(0);
#endif	// !DEBUG

#ifndef Assert
#define Assert			ASSERT
#define Assertsz		ASSERTSZ
#endif/* Assert */



#ifndef min
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif

#ifndef clamp// XDM
#define clamp(a,min,max)	((a < min)?(min):((a > max)?(max):(a)))
#endif


#endif // PLATFORM_H
