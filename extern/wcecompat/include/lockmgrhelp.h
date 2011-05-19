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
    lckmgrhelp.h

Abstract:
    Lock Manager Help Functions for FSDs.

Revision History:

--*/

#ifndef __LOCKMGRHELP_H_
#define __LOCKMGRHELP_H_

#include <windows.h>
#include <lockmgrtypes.h>

//
// FSDMGR_OpenFileLockState - Used by FSD to initialize file lock state
//

#define FSDMGR_OpenFileLockState(pFileLockState) \
    pFileLockState->hevUnlock = NULL; \
    pFileLockState->fTerminal = FALSE; \
    pFileLockState->cQueue = 0; \
    pFileLockState->pvLockContainer = NULL; \
    pFileLockState->lpcs = (LPCRITICAL_SECTION)LocalAlloc(LPTR, sizeof(CRITICAL_SECTION)); \
    if (NULL != pFileLockState->lpcs) { \
        InitializeCriticalSection(pFileLockState->lpcs); \
    }

//
// FSDMGR_CloseFileLockState - Used by FSD to deinitialize file lock state
//

#define FSDMGR_CloseFileLockState(pFileLockState, pfnEmptyLockContainer) \
    DEBUGCHK(NULL != pFileLockState); \
    if (NULL != pFileLockState->pvLockContainer) { \
        if (NULL != pfnEmptyLockContainer) { \
            __try { \
                (*pfnEmptyLockContainer)(pFileLockState); \
            } __except(EXCEPTION_EXECUTE_HANDLER) { \
                DEBUGCHK(0); \
            } \
        } \
    } \
    if (NULL != pFileLockState->hevUnlock) { \
        CloseHandle(pFileLockState->hevUnlock); \
    } \
    if (NULL != pFileLockState->lpcs) { \
        DeleteCriticalSection(pFileLockState->lpcs); \
        LocalFree((HLOCAL)pFileLockState->lpcs); \
    } \

#endif // __LOCKMGRHELP_H_

