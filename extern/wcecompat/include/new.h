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
*new.h - declarations and definitions for C++ memory allocation functions
*
*
*Purpose:
*       Contains the declarations for C++ memory allocation functions.
*
*       [Public]
*
****/

#if     _MSC_VER > 1000 /*IFSTRIP=IGN*/
#pragma once
#endif

#ifndef _INC_NEW
#define _INC_NEW

#ifdef  __cplusplus

#ifndef _MSC_EXTENSIONS
#include <new>
#endif

#if     !defined(_WIN32)
#error ERROR: Only Win32 target supported!
#endif


/* Define _CRTIMP */

#ifndef _CRTIMP
#ifdef  CRTDLL
#define _CRTIMP __declspec(dllexport)
#else   /* ndef CRTDLL */
#ifdef  _DLL
#define _CRTIMP __declspec(dllimport)
#else   /* ndef _DLL */
#define _CRTIMP
#endif  /* _DLL */
#endif  /* CRTDLL */
#endif  /* _CRTIMP */


/* Define __cdecl for non-Microsoft compilers */

#if     ( !defined(_MSC_VER) && !defined(__cdecl) )
#define __cdecl
#endif


/* types and structures */

#ifndef _SIZE_T_DEFINED
#ifdef  _WIN64
typedef unsigned __int64    size_t;
#else
typedef unsigned int   size_t;
#endif
#define _SIZE_T_DEFINED
#endif

#ifdef  _MSC_EXTENSIONS
typedef void (__cdecl * new_handler) ();
_CRTIMP new_handler __cdecl set_new_handler(new_handler);
#endif

#ifndef __PLACEMENT_NEW_INLINE
#define __PLACEMENT_NEW_INLINE
inline void *__cdecl operator new(size_t, void *_P)
        {return (_P); }
#if     _MSC_VER >= 1200 /*IFSTRIP=IGN*/
inline void __cdecl operator delete(void *, void *)
        {return; }
#endif
#endif


/* 
 * new mode flag -- when set, makes malloc() behave like new()
 */

_CRTIMP int __cdecl _query_new_mode( void );
_CRTIMP int __cdecl _set_new_mode( int );

#ifndef _PNH_DEFINED
typedef int (__cdecl * _PNH)( size_t );
#define _PNH_DEFINED
#endif

_CRTIMP _PNH __cdecl _query_new_handler( void );
_CRTIMP _PNH __cdecl _set_new_handler( _PNH );

#endif  /* __cplusplus */

#endif  /* _INC_NEW */
