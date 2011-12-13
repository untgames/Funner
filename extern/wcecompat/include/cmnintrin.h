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
//  This file includes the function prototypes and support definitions
//  for the new common intrinsic functions.  
//  This file also describes the level of support for each of the 
//  common intrinsics.  All machines supporting the common intrinsics
//  will have a machine dependent description of support in this file.

#if defined(_CMMN_INTRIN_FUNC)

#ifndef _SIZE_T_DEFINED
typedef unsigned int size_t;
#define _SIZE_T_DEFINED
#endif

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

#if defined(__cplusplus)
extern "C"
{
#endif

// general-purpose integer functions
// (from stdlib.h)
int  __cdecl abs(int);
long __cdecl labs(long);

unsigned int _rotl( unsigned int, int);
unsigned int _rotr( unsigned int, int);
unsigned long _lrotl( unsigned long, int);
unsigned long _lrotr( unsigned long, int);

// (new)
__int64 __cdecl _abs64(__int64);
unsigned __int64 __cdecl _rotl64(unsigned __int64,int);
unsigned __int64 __cdecl _rotr64(unsigned __int64,int);
__int32 _CopyInt32FromFloat(float);
float _CopyFloatFromInt32(__int32);
__int64 _CopyInt64FromDouble(double);
double _CopyDoubleFromInt64(__int64);

long __cdecl _MulHigh(long,long);
unsigned long __cdecl _MulUnsignedHigh (unsigned long,unsigned long);

unsigned short __cdecl _byteswap_ushort(unsigned short);
unsigned long __cdecl _byteswap_ulong(unsigned long);
unsigned __int64 __cdecl _byteswap_uint64(unsigned __int64);

unsigned __cdecl _CountOneBits(long);
unsigned __cdecl _CountOneBits64(__int64);
unsigned __cdecl _CountLeadingZeros(long);
unsigned __cdecl _CountLeadingZeros64(__int64);
unsigned __cdecl _CountLeadingOnes(long);
unsigned __cdecl _CountLeadingOnes64(__int64);
unsigned __cdecl _CountLeadingSigns(long);
unsigned __cdecl _CountLeadingSigns64(__int64);

#if !defined(_CMNINTRIN_DECLARE_ONLY)
#pragma intrinsic (abs, labs)
#pragma intrinsic (_rotl, _rotr, _lrotl, _lrotr)
#pragma intrinsic(_abs64, _rotl64, _rotr64)
#pragma intrinsic(_MulHigh, _MulUnsignedHigh)
#pragma intrinsic(_byteswap_ushort, _byteswap_ulong, _byteswap_uint64)
#pragma intrinsic(_CountOneBits, _CountOneBits64)
#pragma intrinsic(_CountLeadingZeros, _CountLeadingZeros64)
#pragma intrinsic(_CountLeadingOnes, _CountLeadingOnes64)
#pragma intrinsic(_CountLeadingSigns, _CountLeadingSigns64)
#pragma intrinsic(_CopyInt32FromFloat)
#pragma intrinsic(_CopyFloatFromInt32)
#pragma intrinsic(_CopyInt64FromDouble)
#pragma intrinsic(_CopyDoubleFromInt64)
#endif

// floating-point intrinsic functions
// (from math.h)
float     __cdecl ceilf(float);
float     __cdecl fabsf(float);
float     __cdecl floorf(float);
float     __cdecl fmodf(float,float);
float     __cdecl sqrtf(float);
double    __cdecl acos(double);
double    __cdecl asin(double);
double    __cdecl atan(double);
double    __cdecl atan2(double, double);
double    __cdecl cos(double);
double    __cdecl cosh(double);
double    __cdecl exp(double);
double    __cdecl fabs(double);
double    __cdecl fmod(double, double);
double    __cdecl log(double);
double    __cdecl log10(double);
double    __cdecl pow(double, double);
double    __cdecl sin(double);
double    __cdecl sinh(double);
double    __cdecl sqrt(double);
double    __cdecl tan(double);
double    __cdecl tanh(double);
double    __cdecl ceil(double);
double    __cdecl floor(double);

// (new)
int __cdecl _isnan(double);
int __cdecl _isnanf(float);
int __cdecl _isunordered(double,double);
int __cdecl _isunorderedf(float,float);

#if !defined(_CMNINTRIN_DECLARE_ONLY)
#pragma intrinsic (ceilf, fabsf, floorf, fmodf, sqrtf)
#pragma intrinsic (acos, asin, atan, atan2)
#pragma intrinsic (cos, cosh, exp, fabs)
#pragma intrinsic (fmod, log, log10, pow)
#pragma intrinsic (sin, sinh, sqrt, tan)
#pragma intrinsic (tanh, ceil, floor)
#pragma intrinsic(_isnan, _isnanf, _isunordered, _isunorderedf)
#endif

#if defined(_M_MRX000) // MIPS
#pragma function(floor, floorf, ceil, ceilf)
#endif

// new system intrinsic functions
void __cdecl __prefetch(void *);
void __cdecl __CacheRelease(void *);
void __cdecl __CacheWriteback(void *);
void __cdecl __ICacheRefresh(void *);

void __cdecl _ReadWriteBarrier(void);
void __cdecl _WriteBarrier(void);

void __cdecl __debugbreak(void);
int __cdecl __trap(int,...);

#if !defined(_CMNINTRIN_DECLARE_ONLY)
#pragma intrinsic(__prefetch, __CacheRelease, __CacheWriteback, __ICacheRefresh)
#pragma intrinsic(_ReadWriteBarrier, _WriteBarrier)
#pragma intrinsic(__debugbreak, __trap)
#endif

// old intrinsic functions from string.h

int    __cdecl memcmp(const void *, const void *, size_t);
void * __cdecl memcpy(void *, const void *, size_t);
void * __cdecl memset(void *, int, size_t);
void * __cdecl memchr(const void *, int, size_t);

size_t __cdecl strlen(const char *);
int    __cdecl strcmp(const char *, const char *);
char * __cdecl strcat(char *, const char *);
char * __cdecl strcpy(char *, const char *);
int __cdecl strncmp( const char *, const char *, size_t );
char * __cdecl strncpy( char *, const char *, size_t );
char *  __cdecl _strset(char *, int);
size_t    __cdecl wcslen(const wchar_t *); 
int       __cdecl wcscmp(const wchar_t *, const wchar_t *);
wchar_t * __cdecl wcscat(wchar_t *, const wchar_t *);
wchar_t * __cdecl wcscpy(wchar_t *, const wchar_t *);
int __cdecl wcsncmp( const wchar_t *, const wchar_t *, size_t );
wchar_t * __cdecl wcsncpy( wchar_t *, const wchar_t *, size_t );
wchar_t * __cdecl _wcsset(wchar_t *, wchar_t);


#if !defined(_CMNINTRIN_DECLARE_ONLY)
#pragma intrinsic (memcmp, memcpy, memset, memchr)
#pragma intrinsic (strlen, strcmp, strcat, strcpy, strncmp, strncpy, _strset)
#pragma intrinsic (wcslen, wcscmp, wcscat, wcscpy, wcsncmp, wcsncpy, _wcsset)
#endif


// start level of support description

// the intrinsic is defined and recognized but does nothing
#define _INTRINSIC_UNSUPPORTED      0
// instantiated with a call to the CRT
#define _INTRINSIC_HELPER           1
// may be one or more lines
#define _INTRINSIC_INLINE           2

//OS was not used to instantiate the intrinsic.
#define _INTRINSIC_SYSTEM_SAFE      4
//OS was used to instantiate the intrinsic.
#define _INTRINSIC_SYSTEM_UNSAFE    8


#define _INTRINSIC_IS_SUPPORTED(arg) ((_INTRINSIC_ ## arg) != 0)
#define _INTRINSIC_IS_SAFE(arg)      (((_INTRINSIC_ ## arg) & _INTRINSIC_SYSTEM_SAFE) != 0)
#define _INTRINSIC_IS_UNSAFE(arg)    (((_INTRINSIC_ ## arg) & _INTRINSIC_SYSTEM_UNSAFE) != 0)
#define _INTRINSIC_IS_HELPER(arg)    (((_INTRINSIC_ ## arg) & _INTRINSIC_HELPER) != 0)
#define _INTRINSIC_IS_INLINE(arg)    (((_INTRINSIC_ ## arg) & _INTRINSIC_INLINE) != 0)

//each chip should have it's own defines

// common intrinsic function support for SH-4
#if defined(_M_SH)

//general floating point
#define _INTRINSIC_ceil                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_ceilf                _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_floor                _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_floorf               _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_fmod                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_fmodf                _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_fabs                 _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_fabsf                _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_sqrt                 _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_sqrtf                _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE

//transcendental floating point
#define _INTRINSIC_acos                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_asin                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_atan                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_atan2                _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_cos                  _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_cosh                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_exp                  _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_log                  _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_log10                _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_pow                  _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_sin                  _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_sinh                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_tan                  _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_tanh                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE

//general integer
#define _INTRINSIC_abs                  _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__abs64               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__byteswap_ushort     _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__byteswap_ulong      _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__byteswap_uint64     _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountLeadingOnes    _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountLeadingSigns   _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountLeadingZeros   _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountLeadingOnes64  _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountLeadingSigns64 _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountLeadingZeros64 _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountOneBits        _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountOneBits64      _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_labs                 _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__lrotl               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__lrotr               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__MulHigh             _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__MulUnsignedHigh     _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__rotl                _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__rotl64              _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__rotr                _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__rotr64              _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE

//floating point
#define _INTRINSIC__isnan               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__isnanf              _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__isunordered         _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__isunorderedf        _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE

//string and block
#define _INTRINSIC_memchr               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_memcmp               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_memcpy               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_memset               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_strcat               _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_strcmp               _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_strcpy               _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_strlen               _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_strncmp              _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_strncpy              _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__strset              _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_wcscat               _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_wcscmp               _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_wcscpy               _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_wcslen               _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_wcsncmp              _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_wcsncpy              _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__wcsset              _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE

//system
#define _INTRINSIC___CacheRelease        _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC___CacheWriteback      _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC___ICacheRefresh       _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__debugbreak           _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC___ICacheRefresh       _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC___prefetch            _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__ReadWriteBarrier     _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__ReturnAddress        _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC___trap                _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_UNSAFE
#define _INTRINSIC__WriteBarrier         _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE

#endif // SH

#if defined (_M_ARM)

//general floating point
#define _INTRINSIC_ceil                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_fabs                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_floor                _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_fmod                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_sqrt                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE

//transcendental floating point
#define _INTRINSIC_acos                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_asin                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_atan                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_atan2                _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_cos                  _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_cosh                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_exp                  _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_log                  _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_log10                _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_pow                  _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_sin                  _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_sinh                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_tan                  _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_tanh                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE

//general integer
#define _INTRINSIC_abs                  _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__abs64               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__byteswap_ushort     _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__byteswap_ulong      _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__byteswap_uint64     _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountLeadingOnes    _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountLeadingSigns   _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountLeadingZeros   _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountLeadingOnes64  _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountLeadingSigns64 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountLeadingZeros64 _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountOneBits        _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountOneBits64      _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_labs                 _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__lrotl               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__lrotr               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__MulHigh             _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__MulUnsignedHigh     _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__rotl                _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__rotl64              _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__rotr                _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__rotr64              _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE

//floating point
#define _INTRINSIC__isnan               _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__isnanf              _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__isunordered         _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__isunorderedf        _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE

//string and block
#define _INTRINSIC_memchr               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_memcmp               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_memcpy               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_memset               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_strcat               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_strcmp               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_strcpy               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_strlen               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_strncmp              _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_strncpy              _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__strset              _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_wcscat               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_wcscmp               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_wcscpy               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_wcslen               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_wcsncmp              _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_wcsncpy              _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__wcsset              _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE

//system
#define _INTRINSIC___CacheRelease       _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC___CacheWriteback     _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC___ICacheRefresh      _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC___ICacheRefresh      _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__ReadWriteBarrier    _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__WriteBarrier        _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE

#define _INTRINSIC__debugbreak          _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__ReturnAddress       _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC___trap               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_UNSAFE
#endif  // ARM

#if defined (_M_MRX000)  // MIPS

//general floating point
#define _INTRINSIC_ceil                 _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_fabs                 _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_floor                _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_fmod                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_sqrt                 _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE

//transcendental floating point
#define _INTRINSIC_acos                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_asin                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_atan                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_atan2                _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_cos                  _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_cosh                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_exp                  _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_log                  _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_log10                _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_pow                  _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_sin                  _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_sinh                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_tan                  _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_tanh                 _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE

//general integer
#define _INTRINSIC_abs                  _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__abs64               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__byteswap_ushort     _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__byteswap_ulong      _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__byteswap_uint64     _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountLeadingOnes    _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountLeadingSigns   _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountLeadingZeros   _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountLeadingOnes64  _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountLeadingSigns64 _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountLeadingZeros64 _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountOneBits        _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__CountOneBits64      _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_labs                 _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__lrotl               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__lrotr               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__MulHigh             _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__MulUnsignedHigh     _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__rotl                _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__rotl64              _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__rotr                _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__rotr64              _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE

//floating point
#define _INTRINSIC__isnan               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__isnanf              _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__isunordered         _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__isunorderedf        _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE

//string and block
#define _INTRINSIC_memchr               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_memcmp               _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_memcpy               _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_memset               _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_strcat               _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_strcmp               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_strcpy               _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_strlen               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_strncmp              _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_strncpy              _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__strset              _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_wcscat               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_wcscmp               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_wcscpy               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_wcslen               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_wcsncmp              _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC_wcsncpy              _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__wcsset              _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE

//system
#define _INTRINSIC__alloca              _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__abnormal_termination _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC___CacheRelease       _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC___CacheWriteback     _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC___ICacheRefresh      _INTRINSIC_UNSUPPORTED
#define _INTRINSIC__debugbreak          _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__exception_code      _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__exception_info      _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__InterlockedAdd      _INTRINSIC_UNSUPPORTED
#define _INTRINSIC__InterlockedExchange _INTRINSIC_HELPER|_INTRINSIC_SYSTEM_UNSAFE        
#define _INTRINSIC___prefetch           _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__ReadWriteBarrier    _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC__ReturnAddress       _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE
#define _INTRINSIC___trap               _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_UNSAFE
#define _INTRINSIC__WriteBarrier        _INTRINSIC_INLINE|_INTRINSIC_SYSTEM_SAFE

#endif //MIPS

#if defined(__cplusplus)
}
#endif

#else  // _CMMN_INTRIN_FUNC

// compiler does not use common intrinsics

#define _INTRINSIC_IS_SUPPORTED(arg) (0)
#define _INTRINSIC_IS_SAFE(arg)      (0)
#define _INTRINSIC_IS_UNSAFE(arg)    (0)
#define _INTRINSIC_IS_HELPER(arg)    (0)
#define _INTRINSIC_IS_INLINE(arg)    (0)

#endif  // _CMMN_INTRIN_FUNC
