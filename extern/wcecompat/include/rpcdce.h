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

    rpcdce.h

Abstract:

    This module contains the DCE RPC runtime APIs.

--*/

#ifndef __RPCDCE_H__
#define __RPCDCE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define IN
#define OUT
#define OPTIONAL

/*typedef char small;*/
/*typedef unsigned char byte;*/
/*typedef unsigned char boolean;*/

typedef I_RPC_HANDLE RPC_BINDING_HANDLE;
typedef RPC_BINDING_HANDLE handle_t;
#define rpc_binding_handle_t RPC_BINDING_HANDLE

#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct _GUID
{
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} GUID;
#endif /* GUID_DEFINED */

#ifndef UUID_DEFINED
#define UUID_DEFINED
typedef GUID UUID;
#ifndef uuid_t
#define uuid_t UUID
#endif
#endif

typedef struct _RPC_BINDING_VECTOR
{
    unsigned long Count;
    RPC_BINDING_HANDLE BindingH[1];
} RPC_BINDING_VECTOR;
#ifndef rpc_binding_vector_t
#define rpc_binding_vector_t RPC_BINDING_VECTOR
#endif

typedef struct _UUID_VECTOR
{
  unsigned long Count;
  UUID *Uuid[1];
} UUID_VECTOR;
#ifndef uuid_vector_t
#define uuid_vector_t UUID_VECTOR
#endif

typedef void __RPC_FAR * RPC_IF_HANDLE;

#ifndef IFID_DEFINED
#define IFID_DEFINED
typedef struct _RPC_IF_ID
{
    UUID Uuid;
    unsigned short VersMajor;
    unsigned short VersMinor;
} RPC_IF_ID;
#endif

#define RPC_C_BINDING_INFINITE_TIMEOUT 10
#define RPC_C_BINDING_MIN_TIMEOUT 0
#define RPC_C_BINDING_DEFAULT_TIMEOUT 5
#define RPC_C_BINDING_MAX_TIMEOUT 9

#define RPC_C_CANCEL_INFINITE_TIMEOUT -1

#define RPC_C_LISTEN_MAX_CALLS_DEFAULT 1234
#define RPC_C_PROTSEQ_MAX_REQS_DEFAULT 10

// RPC_POLICY EndpointFlags.
#define RPC_C_BIND_TO_ALL_NICS          1
#define RPC_C_USE_INTERNET_PORT         1  
#define RPC_C_USE_INTRANET_PORT         2

// RPC_POLICY EndpointFlags specific to the Falcon/RPC transport:
#define RPC_C_MQ_TEMPORARY                  0x0000
#define RPC_C_MQ_PERMANENT                  0x0001
#define RPC_C_MQ_CLEAR_ON_OPEN              0x0002
#define RPC_C_MQ_USE_EXISTING_SECURITY      0x0004
#define RPC_C_MQ_AUTHN_LEVEL_NONE           0x0000
#define RPC_C_MQ_AUTHN_LEVEL_PKT_INTEGRITY  0x0008
#define RPC_C_MQ_AUTHN_LEVEL_PKT_PRIVACY    0x0010

// Client: RpcBindingSetOption() values for the Falcon/RPC transport:
#define RPC_C_OPT_MQ_DELIVERY            1
#define RPC_C_OPT_MQ_PRIORITY            2
#define RPC_C_OPT_MQ_JOURNAL             3
#define RPC_C_OPT_MQ_ACKNOWLEDGE         4
#define RPC_C_OPT_MQ_AUTHN_SERVICE       5
#define RPC_C_OPT_MQ_AUTHN_LEVEL         6
#define RPC_C_OPT_MQ_TIME_TO_REACH_QUEUE 7
#define RPC_C_OPT_MQ_TIME_TO_BE_RECEIVED 8
#define RPC_C_OPT_BINDING_NONCAUSAL          9
#define RPC_C_OPT_MAX_OPTIONS                   10

#define RPC_C_MQ_EXPRESS                0  // Client: RPC_C_MQ_DELIVERY.
#define RPC_C_MQ_RECOVERABLE            1

#define RPC_C_MQ_JOURNAL_NONE           0  // Client: RPC_C_MQ_JOURNAL.
#define RPC_C_MQ_JOURNAL_DEADLETTER     1
#define RPC_C_MQ_JOURNAL_ALWAYS         2


#ifdef RPC_UNICODE_SUPPORTED
typedef struct _RPC_PROTSEQ_VECTORA
{
    unsigned int Count;
    unsigned char __RPC_FAR * Protseq[1];
} RPC_PROTSEQ_VECTORA;

typedef struct _RPC_PROTSEQ_VECTORW
{
    unsigned int Count;
    unsigned short __RPC_FAR * Protseq[1];
} RPC_PROTSEQ_VECTORW;

#ifdef UNICODE
#define RPC_PROTSEQ_VECTOR RPC_PROTSEQ_VECTORW
#else /* UNICODE */
#define RPC_PROTSEQ_VECTOR RPC_PROTSEQ_VECTORA
#endif /* UNICODE */

#else /* RPC_UNICODE_SUPPORTED */

typedef struct _RPC_PROTSEQ_VECTOR
{
    unsigned int Count;
    unsigned char __RPC_FAR * Protseq[1];
} RPC_PROTSEQ_VECTOR;

#endif /* RPC_UNICODE_SUPPORTED */
typedef struct _RPC_POLICY {
    unsigned int Length ;
    unsigned long EndpointFlags ;
    unsigned long NICFlags ;
    } RPC_POLICY,  __RPC_FAR *PRPC_POLICY ;

typedef void __RPC_USER
RPC_OBJECT_INQ_FN (
    IN UUID __RPC_FAR * ObjectUuid,
    OUT UUID __RPC_FAR * TypeUuid,
    OUT RPC_STATUS __RPC_FAR * Status
    );

typedef RPC_STATUS RPC_ENTRY
RPC_IF_CALLBACK_FN (
    IN RPC_IF_HANDLE  InterfaceUuid,
    IN void *Context
    ) ;

#define RPC_MGR_EPV void

typedef struct
{
    unsigned int Count;
    unsigned long Stats[1];
} RPC_STATS_VECTOR;

#define RPC_C_STATS_CALLS_IN 0
#define RPC_C_STATS_CALLS_OUT 1
#define RPC_C_STATS_PKTS_IN 2
#define RPC_C_STATS_PKTS_OUT 3

typedef struct
{
  unsigned long Count;
  RPC_IF_ID __RPC_FAR * IfId[1];
} RPC_IF_ID_VECTOR;

// @CESYSGEN IF DCOM_MODULES_RPCRT4
// @CESYSGEN ENDIF

typedef void __RPC_FAR * RPC_AUTH_IDENTITY_HANDLE;
typedef void __RPC_FAR * RPC_AUTHZ_HANDLE;

#define RPC_C_AUTHN_LEVEL_DEFAULT       0
#define RPC_C_AUTHN_LEVEL_NONE          1
#define RPC_C_AUTHN_LEVEL_CONNECT       2
#define RPC_C_AUTHN_LEVEL_CALL          3
#define RPC_C_AUTHN_LEVEL_PKT           4
#define RPC_C_AUTHN_LEVEL_PKT_INTEGRITY 5
#define RPC_C_AUTHN_LEVEL_PKT_PRIVACY   6

#define RPC_C_IMP_LEVEL_ANONYMOUS    1
#define RPC_C_IMP_LEVEL_IDENTIFY     2
#define RPC_C_IMP_LEVEL_IMPERSONATE  3
#define RPC_C_IMP_LEVEL_DELEGATE     4

#define RPC_C_QOS_IDENTITY_STATIC    0
#define RPC_C_QOS_IDENTITY_DYNAMIC   1

#define RPC_C_QOS_CAPABILITIES_DEFAULT     0
#define RPC_C_QOS_CAPABILITIES_MUTUAL_AUTH 1

#define RPC_C_PROTECT_LEVEL_DEFAULT       (RPC_C_AUTHN_LEVEL_DEFAULT)
#define RPC_C_PROTECT_LEVEL_NONE          (RPC_C_AUTHN_LEVEL_NONE)
#define RPC_C_PROTECT_LEVEL_CONNECT       (RPC_C_AUTHN_LEVEL_CONNECT)
#define RPC_C_PROTECT_LEVEL_CALL          (RPC_C_AUTHN_LEVEL_CALL)
#define RPC_C_PROTECT_LEVEL_PKT           (RPC_C_AUTHN_LEVEL_PKT)
#define RPC_C_PROTECT_LEVEL_PKT_INTEGRITY (RPC_C_AUTHN_LEVEL_PKT_INTEGRITY)
#define RPC_C_PROTECT_LEVEL_PKT_PRIVACY   (RPC_C_AUTHN_LEVEL_PKT_PRIVACY)

#define RPC_C_AUTHN_NONE          0
#define RPC_C_AUTHN_DCE_PRIVATE   1
#define RPC_C_AUTHN_DCE_PUBLIC    2
#define RPC_C_AUTHN_DEC_PUBLIC    4
#define RPC_C_AUTHN_WINNT        10
#define RPC_C_AUTHN_DPA          16
#define RPC_C_AUTHN_MSN          17
#define RPC_C_AUTHN_GSS_KERBEROS 18
#define RPC_C_AUTHN_MQ          100
#define RPC_C_AUTHN_DEFAULT     0xFFFFFFFFL

#define RPC_C_SECURITY_QOS_VERSION  1L

typedef struct _RPC_SECURITY_QOS {
  unsigned long Version;
  unsigned long Capabilities;
  unsigned long IdentityTracking;
  unsigned long ImpersonationType;
} RPC_SECURITY_QOS, *PRPC_SECURITY_QOS;

#if defined(__RPC_DOS__) || defined(__RPC_WIN16__)
typedef struct _SEC_WINNT_AUTH_IDENTITY
{
  char __RPC_FAR *User;
  char __RPC_FAR *Domain;
  char __RPC_FAR *Password;
} SEC_WINNT_AUTH_IDENTITY;
#else


#define SEC_WINNT_AUTH_IDENTITY_ANSI    0x1
#define SEC_WINNT_AUTH_IDENTITY_UNICODE 0x2

typedef struct _SEC_WINNT_AUTH_IDENTITY_W {
  unsigned short __RPC_FAR *User;
  unsigned long UserLength;
  unsigned short __RPC_FAR *Domain;
  unsigned long DomainLength;
  unsigned short __RPC_FAR *Password;
  unsigned long PasswordLength;
  unsigned long Flags;
} SEC_WINNT_AUTH_IDENTITY_W, *PSEC_WINNT_AUTH_IDENTITY_W;

typedef struct _SEC_WINNT_AUTH_IDENTITY_A {
  unsigned char __RPC_FAR *User;
  unsigned long UserLength;
  unsigned char __RPC_FAR *Domain;
  unsigned long DomainLength;
  unsigned char __RPC_FAR *Password;
  unsigned long PasswordLength;
  unsigned long Flags;
} SEC_WINNT_AUTH_IDENTITY_A, *PSEC_WINNT_AUTH_IDENTITY_A;


#ifdef UNICODE
#define SEC_WINNT_AUTH_IDENTITY SEC_WINNT_AUTH_IDENTITY_W
#define PSEC_WINNT_AUTH_IDENTITY PSEC_WINNT_AUTH_IDENTITY_W
#define _SEC_WINNT_AUTH_IDENTITY _SEC_WINNT_AUTH_IDENTITY_W
#else // UNICODE
#define SEC_WINNT_AUTH_IDENTITY SEC_WINNT_AUTH_IDENTITY_A
#define PSEC_WINNT_AUTH_IDENTITY PSEC_WINNT_AUTH_IDENTITY_A
#define _SEC_WINNT_AUTH_IDENTITY _SEC_WINNT_AUTH_IDENTITY_A
#endif // UNICODE
#endif // defined(__RPC_DOS__) || defined(__RPC_WIN16__)

#define RPC_C_AUTHZ_NONE 0
#define RPC_C_AUTHZ_NAME 1
#define RPC_C_AUTHZ_DCE 2

// @CESYSGEN IF DCOM_MODULES_RPCRT4
// @CESYSGEN ENDIF

typedef struct {
    unsigned char __RPC_FAR * UserName;
    unsigned char __RPC_FAR * ComputerName;
    unsigned short Privilege;
    unsigned long AuthFlags;
} RPC_CLIENT_INFORMATION1, __RPC_FAR * PRPC_CLIENT_INFORMATION1;

// @CESYSGEN IF DCOM_MODULES_RPCRT4
// @CESYSGEN ENDIF

#define DCE_C_ERROR_STRING_LEN 256

typedef I_RPC_HANDLE * RPC_EP_INQ_HANDLE;

#define  RPC_C_EP_ALL_ELTS        0
#define  RPC_C_EP_MATCH_BY_IF     1
#define  RPC_C_EP_MATCH_BY_OBJ    2
#define  RPC_C_EP_MATCH_BY_BOTH   3

#define  RPC_C_VERS_ALL           1
#define  RPC_C_VERS_COMPATIBLE    2
#define  RPC_C_VERS_EXACT         3
#define  RPC_C_VERS_MAJOR_ONLY    4
#define  RPC_C_VERS_UPTO          5

// @CESYSGEN IF DCOM_MODULES_RPCRT4
// @CESYSGEN ENDIF

#define RPC_IF_AUTOLISTEN 0x0001
#define RPC_IF_OLE        0x0002

#include <rpcdcep.h>

#ifdef __cplusplus
}
#endif

#endif /* __RPCDCE_H__ */
