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

Module Name:
    lckmgr.h

Abstract:
    Lock Manager API for FSDs/File Systems.

Revision History:

--*/

#ifndef __LOCKMGR_H_
#define __LOCKMGR_H_

#include <windows.h>
#include "lockmgrtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// FSDMGR_InstallFileLock - Lock Manager implementation of LockFileEx
//

BOOL
FSDMGR_InstallFileLock(
    PACQUIREFILELOCKSTATE pAcquireFileLockState,
    PRELEASEFILELOCKSTATE pReleaseFileLockState,
    DWORD dwHandle,
    DWORD dwFlags,
    DWORD dwReserved,
    DWORD nNumberOfBytesToLockLow,
    DWORD nNumberOfBytesToLockHigh,
    LPOVERLAPPED lpOverlapped
    );

//
// FSDMGR_RemoveFileLock - Lock Manager implementation of UnlockFileEx
//

BOOL
FSDMGR_RemoveFileLock(
    PACQUIREFILELOCKSTATE pAcquireFileLockState,
    PRELEASEFILELOCKSTATE pReleaseFileLockState,
    DWORD dwHandle,
    DWORD dwReserved,
    DWORD nNumberOfBytesToLockLow,
    DWORD nNumberOfBytesToLockHigh,
    LPOVERLAPPED lpOverlapped
    );

//
// FSDMGR_RemoveFileLockEx - Lock Manager routine to unlock locks owned by handle
//

BOOL
FSDMGR_RemoveFileLockEx(
    PACQUIREFILELOCKSTATE pAcquireFileLockState,
    PRELEASEFILELOCKSTATE pReleaseFileLockState,
    DWORD dwHandle
    );

//
// FSDMGR_TestFileLock - Lock Manager routine to authorize read/write request
//

BOOL
FSDMGR_TestFileLock(
    PACQUIREFILELOCKSTATE pAcquireFileLockState,
    PRELEASEFILELOCKSTATE pReleaseFileLockState,
    DWORD dwHandle,
    BOOL fRead,
    DWORD cbReadWrite
    );

//
// FSDMGR_TestFileLockEx - Lock Manager routine to authorize read/write request
//

BOOL
FSDMGR_TestFileLockEx(
    PACQUIREFILELOCKSTATE pAcquireFileLockState,
    PRELEASEFILELOCKSTATE pReleaseFileLockState,
    DWORD dwHandle,
    BOOL fRead,
    DWORD cbReadWrite,
    DWORD dwOffsetLow,
    DWORD dwOffsetHigh
    );

//
// FSDMGR_EmptyLockContainer - Lock Manager routine to unblock threads waiting to install lock
//

VOID
FSDMGR_EmptyLockContainer(
    PFILELOCKSTATE pFileLockState
    );

#ifdef __cplusplus
}
#endif

#endif // __LOCKMGR_H_

