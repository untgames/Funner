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
#ifndef __MSGQUEUE_H__
#define __MSGQUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//
// prototypes
//
HANDLE WINAPI CreateMsgQueue(LPCWSTR lpName, LPMSGQUEUEOPTIONS lpOptions);
HANDLE WINAPI OpenMsgQueue(HANDLE hSrcProc, HANDLE hMsgQ, LPMSGQUEUEOPTIONS lpOptions);
BOOL WINAPI ReadMsgQueue(HANDLE hMsgQ, LPVOID lpBuffer, DWORD cbBufferSize,
                LPDWORD lpNumberOfBytesRead, DWORD dwTimeout, DWORD *pdwFlags);
BOOL WINAPI WriteMsgQueue(HANDLE hMsgQ, LPVOID lpBuffer, DWORD cbDataSize,
                DWORD dwTimeout, DWORD dwFlags);
BOOL WINAPI GetMsgQueueInfo(HANDLE hMsgQ, LPMSGQUEUEINFO lpInfo);

BOOL WINAPI CloseMsgQueue(HANDLE hMsgQ);

#ifdef __cplusplus
}
#endif

#endif // __MSGQUEUE_H__

