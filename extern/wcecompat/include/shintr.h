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
*shintr.h - definitions and declarations for SH intrinisc library
*
*Purpose:
*       This file contains constant definitions and external subroutine
*       declarations for the SH intrinisc subroutine library.
*
****/

#ifndef _INC_SHINTR
#define _INC_SHINTR

#ifdef _M_SH

#ifdef __cplusplus
extern "C" {
#endif

float * _Xform4dV(float *retval, float *vector, float *matrix);
float * _Xform3dV(float *retval, float *vector, float *matrix);
float * _XDXform4dV(float *src, float *dst);
float * _XDXform3dV(float *src, float *dst);
float  _Dot4dV(float *vector1, float *vector2);
float  _Dot3dVW0(float *vector1, float *vector2);
float  _Dot3dVW1(float *vector1, float *vector2);
float * _Multiply4dM(float *retval, float *m4x4m1, float *m4x4m2);
void _XDMultMatrix(float *matrix);
float * _LoadMatrix(float *matrix);
float * _SaveMatrix(float *matrix);
float __fmac(float i, float m, float n);
void __movca(unsigned long value, unsigned long *addr);
float _Convolve(int nelement, float *pstart, float *pend, 
                float *pdata, float *pfilter);

#pragma intrinsic(_Xform4dV,_Xform3dV,_XDXform4dV,_XDXform3dV)
#pragma intrinsic(_Dot4dV,_Dot3dVW0,_Dot3dVW1,_Multiply4dM)
#pragma intrinsic(_XDMultMatrix,_LoadMatrix,_SaveMatrix)
#pragma intrinsic(__fmac,__movca,_Convolve)

#ifdef __cplusplus
}
#endif

#endif // _M_SH

#endif //_INC_SHINTR
