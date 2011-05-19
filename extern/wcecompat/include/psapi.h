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
/*++


Module Name: psapi.h

Purpose: Declaration of Process Status Helper APIs.

--*/

#ifndef _PSAPI_H_
#define _PSAPI_H_

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _MODULEINFO {
    LPVOID lpBaseOfDll;
    DWORD SizeOfImage;
    LPVOID EntryPoint;
} MODULEINFO, *LPMODULEINFO;


#if !defined (IN_KERNEL) && !defined (COREDLL)

BOOL
WINAPI
GetModuleInformation(
    HANDLE hProcess,
    HMODULE hModule,
    __out_bcount(cb) LPMODULEINFO lpmodinfo,
    __in DWORD cb
    );

BOOL 
WINAPI 
CeSetProcessVersion (
    __in HANDLE hProcess,
    __in DWORD dwVersion
    );

#endif

#ifdef __cplusplus
}
#endif

#endif
