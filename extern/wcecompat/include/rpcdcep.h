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

    rpcdcep.h

Abstract:

    This module contains the private RPC runtime APIs for use by the
    stubs and by support libraries.  Applications must not call these
    routines.

--*/

#ifndef __RPCDCEP_H__
#define __RPCDCEP_H__

// Set the packing level for RPC structures for Dos and Windows.

#if defined(__RPC_DOS__) || defined(__RPC_WIN16__)
#pragma pack(2)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _RPC_VERSION {
    unsigned short MajorVersion;
    unsigned short MinorVersion;
} RPC_VERSION;

typedef struct _RPC_SYNTAX_IDENTIFIER {
    GUID SyntaxGUID;
    RPC_VERSION SyntaxVersion;
} RPC_SYNTAX_IDENTIFIER, __RPC_FAR * PRPC_SYNTAX_IDENTIFIER;

typedef struct _RPC_MESSAGE
{
    RPC_BINDING_HANDLE Handle;
    unsigned long DataRepresentation;
    void __RPC_FAR * Buffer;
    unsigned int BufferLength;
    unsigned int ProcNum;
    PRPC_SYNTAX_IDENTIFIER TransferSyntax;
    void __RPC_FAR * RpcInterfaceInformation;
    void __RPC_FAR * ReservedForRuntime;
    RPC_MGR_EPV __RPC_FAR * ManagerEpv;
    void __RPC_FAR * ImportContext;
    unsigned long RpcFlags;
} RPC_MESSAGE, __RPC_FAR * PRPC_MESSAGE;


typedef RPC_STATUS
RPC_ENTRY RPC_FORWARD_FUNCTION(
                       IN UUID             __RPC_FAR * InterfaceId,
                       IN RPC_VERSION      __RPC_FAR * InterfaceVersion,
                       IN UUID             __RPC_FAR * ObjectId,
                       IN unsigned char         __RPC_FAR * Rpcpro,
                       IN void __RPC_FAR * __RPC_FAR * ppDestEndpoint);

/*
 * Types of function calls for datagram rpc
 */

#define RPC_NCA_FLAGS_DEFAULT       0x00000000  /* 0b000...000 */
#define RPC_NCA_FLAGS_IDEMPOTENT    0x00000001  /* 0b000...001 */
#define RPC_NCA_FLAGS_BROADCAST     0x00000002  /* 0b000...010 */
#define RPC_NCA_FLAGS_MAYBE         0x00000004  /* 0b000...100 */

#define RPC_BUFFER_COMPLETE         0x00001000 /* used by pipes */
#define RPC_BUFFER_PARTIAL          0x00002000 /* used by pipes */
#define RPC_BUFFER_EXTRA            0x00004000 /* used by pipes */
#define RPC_BUFFER_ASYNC            0x00008000 /* used by async rpc */
#define RPC_BUFFER_NONOTIFY         0x00010000 /* used by async pipes */

#define RPCFLG_MESSAGE              0x01000000UL
#define RPCFLG_AUTO_COMPLETE        0x08000000UL
#define RPCFLG_LOCAL_CALL           0x10000000UL
#define RPCFLG_INPUT_SYNCHRONOUS    0x20000000UL
#define RPCFLG_ASYNCHRONOUS         0x40000000UL
#define RPCFLG_NON_NDR              0x80000000UL


#if defined(__RPC_DOS__) || defined(__RPC_WIN16__)
#define RPC_FLAGS_VALID_BIT 0x8000
#endif

#if defined(__RPC_WIN32__) || defined(__RPC_MAC__)
#define RPC_FLAGS_VALID_BIT 0x00008000
#endif

typedef
void
(__RPC_STUB __RPC_FAR * RPC_DISPATCH_FUNCTION) (
    IN OUT PRPC_MESSAGE Message
    );

typedef struct {
    unsigned int DispatchTableCount;
    RPC_DISPATCH_FUNCTION __RPC_FAR * DispatchTable;
    int Reserved;
} RPC_DISPATCH_TABLE, __RPC_FAR * PRPC_DISPATCH_TABLE;

typedef struct _RPC_PROTSEQ_ENDPOINT
{
    unsigned char __RPC_FAR * RpcProtocolSequence;
    unsigned char __RPC_FAR * Endpoint;
} RPC_PROTSEQ_ENDPOINT, __RPC_FAR * PRPC_PROTSEQ_ENDPOINT;

/*
Both of these types MUST start with the InterfaceId and TransferSyntax.
Look at RpcIfInqId and I_RpcIfInqTransferSyntaxes to see why.
*/
#define NT351_INTERFACE_SIZE 0x40
#define RPC_INTERFACE_HAS_PIPES           0x0001

typedef struct _RPC_SERVER_INTERFACE
{
    unsigned int Length;
    RPC_SYNTAX_IDENTIFIER InterfaceId;
    RPC_SYNTAX_IDENTIFIER TransferSyntax;
    PRPC_DISPATCH_TABLE DispatchTable;
    unsigned int RpcProtseqEndpointCount;
    PRPC_PROTSEQ_ENDPOINT RpcProtseqEndpoint;
    RPC_MGR_EPV __RPC_FAR *DefaultManagerEpv;
    void const __RPC_FAR *InterpreterInfo;
    unsigned int Flags ;
} RPC_SERVER_INTERFACE, __RPC_FAR * PRPC_SERVER_INTERFACE;

typedef struct _RPC_CLIENT_INTERFACE
{
    unsigned int Length;
    RPC_SYNTAX_IDENTIFIER InterfaceId;
    RPC_SYNTAX_IDENTIFIER TransferSyntax;
    PRPC_DISPATCH_TABLE DispatchTable;
    unsigned int RpcProtseqEndpointCount;
    PRPC_PROTSEQ_ENDPOINT RpcProtseqEndpoint;
    unsigned long Reserved;
    void const __RPC_FAR * InterpreterInfo;
    unsigned int Flags ;
} RPC_CLIENT_INTERFACE, __RPC_FAR * PRPC_CLIENT_INTERFACE;

#define TRANSPORT_TYPE_CN        0x01
#define TRANSPORT_TYPE_DG        0x02
#define TRANSPORT_TYPE_LPC       0x04
#define TRANSPORT_TYPE_WMSG      0x08

typedef void * I_RPC_MUTEX;

typedef
void
(__RPC_USER __RPC_FAR * PRPC_RUNDOWN) (
    void __RPC_FAR * AssociationContext
    );

typedef struct _RPC_TRANSFER_SYNTAX
{
    UUID Uuid;
    unsigned short VersMajor;
    unsigned short VersMinor;
} RPC_TRANSFER_SYNTAX;

#if defined(WINNT) || defined (UNDER_CE)
typedef RPC_STATUS (*RPC_BLOCKING_FN) (
    IN void *wnd,
    IN void *Context,
    IN HANDLE hSyncEvent
    ) ;

#else

typedef RPC_STATUS (*RPC_BLOCKING_FN) (
    IN void * hWnd,
    IN void * Context,
    IN OPTIONAL void * hSyncEvent
    ) ;
#endif

// @CESYSGEN IF DCOM_MODULES_RPCRT4
// @CESYSGEN ENDIF

#ifdef __cplusplus
}
#endif

// Reset the packing level for Dos and Windows.

#if defined(__RPC_DOS__) || defined(__RPC_WIN16__)
#pragma pack()
#endif

#endif /* __RPCDCEP_H__ */
