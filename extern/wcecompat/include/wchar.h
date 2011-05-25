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
*wchar.h - declarations for wide character functions
*
*
*Purpose:
*       This file contains the types, macros and function declarations for
*       all wide character-related functions.  They may also be declared in
*       individual header files on a functional basis.
*       [ISO]
*
*       Note: keep in sync with ctype.h, stdio.h, stdlib.h, string.h, time.h.
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_WCHAR
#define _INC_WCHAR

#include <stdlib.h>

#ifdef  _MSC_VER
#pragma pack(push,8)
#endif  /* _MSC_VER */

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

#ifndef WCHAR_MIN
#define WCHAR_MIN       0
#endif /* WCHAR_MIN */
  
#ifndef WCHAR_MAX
#define WCHAR_MAX       ((wchar_t)-1)
#endif /* WCHAR_MAX */

typedef int mbstate_t;
typedef wchar_t _Wint_t;

struct tm;

/* // Not currently supported in WinCE
_CRTIMP wint_t __cdecl btowc(int);
_CRTIMP size_t __cdecl mbrlen(const char *, size_t, mbstate_t *);
_CRTIMP size_t __cdecl mbrtowc(wchar_t *, const char *, size_t, mbstate_t *);
*/

inline _CRTIMP size_t __cdecl mbsrtowcs(wchar_t *wcstr, const char **mbstr, size_t count, mbstate_t *mbstate)
{
   return mbstowcs(wcstr,*mbstr,count);
}

//_CRTIMP size_t __cdecl wcrtomb(char *, wchar_t, mbstate_t *);

inline _CRTIMP size_t __cdecl wcsrtombs(char *mbstr, const wchar_t **wcstr, size_t count, mbstate_t *mbstate)
{
   return wcstombs(mbstr,*wcstr,count);
}

//_CRTIMP int __cdecl wctob(wint_t);


#ifdef  __cplusplus

#ifndef _FILE_DEFINED
typedef unsigned char FILE;
#define _FILE_DEFINED
#endif

inline int fwide(FILE *, int _M)
        {return (_M); }
inline int mbsinit(const mbstate_t *)
        {return (1); }
inline const wchar_t *wmemchr(const wchar_t *_S, wchar_t _C, size_t _N)
        {for (; 0 < _N; ++_S, --_N)
                if (*_S == _C)
                        return (_S);
        return (0); }
inline int wmemcmp(const wchar_t *_S1, const wchar_t *_S2, size_t _N)
        {for (; 0 < _N; ++_S1, ++_S2, --_N)
                if (*_S1 != *_S2)
                        return (*_S1 < *_S2 ? -1 : +1);
        return (0); }
inline wchar_t *wmemcpy(wchar_t *_S1, const wchar_t *_S2, size_t _N)
        {wchar_t *_Su1 = _S1;
        for (; 0 < _N; ++_Su1, ++_S2, --_N)
                *_Su1 = *_S2;
        return (_S1); }
inline wchar_t *wmemmove(wchar_t *_S1, const wchar_t *_S2, size_t _N)
        {wchar_t *_Su1 = _S1;
        if (_S2 < _Su1 && _Su1 < _S2 + _N)
                for (_Su1 += _N, _S2 += _N; 0 < _N; --_N)
                        *--_Su1 = *--_S2;
        else
                for (; 0 < _N; --_N)
                        *_Su1++ = *_S2++;
        return (_S1); }
inline wchar_t *wmemset(wchar_t *_S, wchar_t _C, size_t _N)
        {wchar_t *_Su = _S;
        for (; 0 < _N; ++_Su, --_N)
                *_Su = _C;
        return (_S); }
}       /* end of extern "C" */
inline wchar_t *wmemchr(wchar_t *_S, wchar_t _C, size_t _N)
        {return ((wchar_t *)wmemchr((const wchar_t *)_S, _C, _N)); }
inline wchar_t *wcschr(wchar_t *_S, wchar_t _C)
        {return ((wchar_t *)wcschr((const wchar_t *)_S, _C)); }
inline wchar_t *wcspbrk(wchar_t *_S, const wchar_t *_P)
        {return ((wchar_t *)wcspbrk((const wchar_t *)_S, _P)); }
inline wchar_t *wcsrchr(wchar_t *_S, wchar_t _C)
        {return ((wchar_t *)wcsrchr((const wchar_t *)_S, _C)); }
inline wchar_t *wcsstr(wchar_t *_S, const wchar_t *_P)
        {return ((wchar_t *)wcsstr((const wchar_t *)_S, _P)); }
/* Old name */
#define wcswcs wcsstr

#endif /* __cplusplus */

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_WCHAR */

