//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
/***
*wctype.h - declarations for wide character functions
*
*       Created from wchar.h January 1996 by P.J. Plauger
*
*Purpose:
*       This file contains the types, macros and function declarations for
*       all ctype-style wide-character functions.  They may also be declared in
*       wchar.h.
*       [ISO]
*
*       Note: keep in sync with ctype.h and wchar.h.
*
*       [Public]
*
****/

#ifndef _INC_WCTYPE
#define _INC_WCTYPE

#ifdef  _MSC_VER
#pragma pack(push,8)
#endif  /* _MSC_VER */

#include <stdlib.h>

#ifdef  __cplusplus
extern "C" {
#endif


/* Define _CRTIMP */

#ifndef _CRTIMP
#ifdef  _DLL
#define _CRTIMP __declspec(dllimport)
#else   /* ndef _DLL */
#define _CRTIMP
#endif  /* _DLL */
#endif  /* _CRTIMP */

/* Define _CRTIMP */
#ifndef _CRTIMP
#ifdef  _DLL
#define _CRTIMP __declspec(dllimport)
#else   /* ndef _DLL */
#define _CRTIMP
#endif  /* _DLL */
#endif  /* _CRTIMP */

/* Define __cdecl for non-Microsoft compilers */

#if     ( !defined(_MSC_VER) && !defined(__cdecl) )
#define __cdecl
#endif

/* Define _CRTAPI1 (for compatibility with the NT SDK) */

#ifndef _CRTAPI1
#if	_MSC_VER >= 800 && _M_IX86 >= 300
#define _CRTAPI1 __cdecl
#else
#define _CRTAPI1
#endif
#endif

/* set bit masks for the possible character types */

#define _LEADBYTE       0x8000                  /* multibyte leadbyte */

typedef wchar_t wctrans_t;

/* // Not supported in Windows CE
_CRTIMP wint_t __cdecl towctrans(wint_t, wctrans_t);
_CRTIMP wctrans_t __cdecl wctrans(const char *);
_CRTIMP wctype_t __cdecl wctype(const char *);
*/

#ifdef  __cplusplus
}
#endif

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_WCTYPE */

