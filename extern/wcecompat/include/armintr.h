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
*armintr.h - definitions and declarations for ARM specific intrinsics
*
*Purpose:
*       This file contains constant definitions and external subroutine
*       declarations for the ARM specific intriniscs
*
****/

#ifndef _INC_ARMINTR
#define _INC_ARMINTR

#if defined(_M_ARM)

#ifdef __cplusplus
extern "C" {
#endif

int _AddSatInt(int, int);                           //qadd
int _DAddSatInt(int, int);                          //qdadd
int _DSubSatInt(int, int);                          //qsub
int _SubSatInt(int, int);                           //qdsub

int _SmulAddLo_SW_SL(int, int,int);                 //smlabb
int _SmulAddHi_SW_SL(int, int, int);                //smlatt
int _SmulAddHiLo_SW_SL(int, int, int);              //smlatb
int _SmulAddLoHi_SW_SL(int, int, int);              //smlabt

int _SmulLo_SW_SL(int, int);                        //smulbb
int _SmulHi_SW_SL(int, int);                        //smultt
int _SmulHiLo_SW_SL(int, int);                      //smultb
int _SmulLoHi_SW_SL(int, int);                      //sumlbt

__int64 _SmulAddLo_SW_SQ(__int64, int, int);        //smlalbb
__int64 _SmulAddHi_SW_SQ(__int64, int, int);        //smlaltt
__int64 _SmulAddHiLo_SW_SQ(__int64,int,int);        //smlaltb
__int64 _SmulAddLoHi_SW_SQ(__int64,int,int);        //smlalbt

int _SmulAddWLo_SW_SL(int, int, int);               //smlawb
int _SmulAddWHi_SW_SL(int, int, int);               //smlawt

int _SmulWLo_SW_SL(int, int);                       //smulwb
int _SmulWHi_SW_SL(int, int);                       //smulwt

void _PreLoad(unsigned long *addr);                 //pld

void _WriteCoProcessor(__int64,int);                //mrrc/mar
__int64 _ReadCoProcessor(int);                      //mcrr/mra
int  _MoveFromCoprocessor(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);              //mrc
int  _MoveFromCoprocessor2(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);             //mrc2
void _MoveToCoprocessor(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);  //mcr
void _MoveToCoprocessor2(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int); //mcr2

void _SmulAdd_SL_ACC(int,int);                       //mia
void _SmulAddPack_2SW_ACC(int,int);                  //miaph
void _SmulAddLo_SW_ACC(int, int);                    //miabb
void _SmulAddHi_SW_ACC(int, int);                    //miatt
void _SmulAddHiLo_SW_ACC(int, int);                  //miatb
void _SmulAddLoHi_SW_ACC(int, int);                  //miabt

void __emit(const unsigned __int32);
unsigned int __gregister_get(const unsigned int);
void __gregister_set(const unsigned int, unsigned int);

int _ReadStatusReg(int);
unsigned int __swi(unsigned, ...);
void _WriteStatusReg(int,int,int);

#ifdef __cplusplus
}
#endif

#endif //defined(_M_ARM)

#endif //_INC_ARMINTR




