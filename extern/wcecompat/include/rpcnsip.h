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

    rpcnsip.h

Abstract:

    This file contains the types and function definitions to use the
    to implement the autohandle features of the runtime.

--*/

#ifndef __RPCNSIP_H__
#define __RPCNSIP_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
   RPC_NS_HANDLE        LookupContext;
   RPC_BINDING_HANDLE   ProposedHandle;
   RPC_BINDING_VECTOR * Bindings;

} RPC_IMPORT_CONTEXT_P, * PRPC_IMPORT_CONTEXT_P;


/* Stub Auto Binding routines. */

RPC_STATUS RPC_ENTRY
I_RpcNsGetBuffer(
    IN PRPC_MESSAGE Message
    );

RPC_STATUS RPC_ENTRY
I_RpcNsSendReceive(
    IN PRPC_MESSAGE Message,
    OUT RPC_BINDING_HANDLE __RPC_FAR * Handle
    );

void RPC_ENTRY
I_RpcNsRaiseException(
    IN PRPC_MESSAGE Message,
    IN RPC_STATUS Status
    );

RPC_STATUS RPC_ENTRY
I_RpcReBindBuffer(
    IN PRPC_MESSAGE Message
    );

RPC_STATUS RPC_ENTRY
I_NsServerBindSearch(
    );

RPC_STATUS RPC_ENTRY
I_NsClientBindSearch(
    );

void RPC_ENTRY
I_NsClientBindDone(
    );

#ifdef __cplusplus
}
#endif

#endif /* __RPCNSIP_H__ */
