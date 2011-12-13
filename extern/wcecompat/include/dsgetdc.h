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


Module Name: DsGetDc.h

Purpose: Declares structures and routines related to DsGetDcNameW.
--*/


#ifndef _DSGETDC_
#define _DSGETDC_

#ifdef __cplusplus
extern "C" {
#endif

#include <lmcons.h>

// WinCE does not support any DsGetDcName() flags for input.
// It always contacts an LDAP server to perform its query.

//
// Structure returned from DsGetDcName
//

typedef struct _DOMAIN_CONTROLLER_INFOW {
#ifdef MIDL_PASS
    [string,unique] wchar_t *DomainControllerName;
#else // MIDL_PASS
    LPWSTR DomainControllerName;
#endif // MIDL_PASS
#ifdef MIDL_PASS
    [string,unique] wchar_t *DomainControllerAddress;
#else // MIDL_PASS
    LPWSTR DomainControllerAddress;
#endif // MIDL_PASS
    ULONG DomainControllerAddressType;
    GUID DomainGuid;
#ifdef MIDL_PASS
    [string,unique] wchar_t *DomainName;
#else // MIDL_PASS
    LPWSTR DomainName;
#endif // MIDL_PASS
#ifdef MIDL_PASS
    [string,unique] wchar_t *DnsForestName;
#else // MIDL_PASS
    LPWSTR DnsForestName;
#endif // MIDL_PASS
    ULONG Flags;
#ifdef MIDL_PASS
    [string,unique] wchar_t *DcSiteName;
#else // MIDL_PASS
    LPWSTR DcSiteName;
#endif // MIDL_PASS
#ifdef MIDL_PASS
    [string,unique] wchar_t *ClientSiteName;
#else // MIDL_PASS
    LPWSTR ClientSiteName;
#endif // MIDL_PASS
} DOMAIN_CONTROLLER_INFOW, *PDOMAIN_CONTROLLER_INFOW;

#define DOMAIN_CONTROLLER_INFO DOMAIN_CONTROLLER_INFOW
#define PDOMAIN_CONTROLLER_INFO PDOMAIN_CONTROLLER_INFOW


//
// Values for returned Flags
//

#define DS_PDC_FLAG            0x00000001    // DC is PDC of Domain
#define DS_GC_FLAG             0x00000004    // DC is a GC of forest
#define DS_LDAP_FLAG           0x00000008    // Server supports an LDAP server
#define DS_DS_FLAG             0x00000010    // DC supports a DS and is a Domain Controller
#define DS_KDC_FLAG            0x00000020    // DC is running KDC service
#define DS_TIMESERV_FLAG       0x00000040    // DC is running time service
#define DS_CLOSEST_FLAG        0x00000080    // DC is in closest site to client
#define DS_WRITABLE_FLAG       0x00000100    // DC has a writable DS
#define DS_GOOD_TIMESERV_FLAG  0x00000200    // DC is running time service (and has clock hardware)
#define DS_NDNC_FLAG           0x00000400    // DomainName is non-domain NC serviced by the LDAP server
#define DS_PING_FLAGS          0x0000FFFF    // Flags returned on ping

#define DS_DNS_CONTROLLER_FLAG 0x20000000    // DomainControllerName is a DNS name
#define DS_DNS_DOMAIN_FLAG     0x40000000    // DomainName is a DNS name
#define DS_DNS_FOREST_FLAG     0x80000000    // DnsForestName is a DNS name

//
// Function Prototypes
//

DWORD
WINAPI
DsGetDcNameW(
    IN LPCWSTR ComputerName OPTIONAL,
    IN LPCWSTR DomainName OPTIONAL,
    IN GUID *DomainGuid OPTIONAL,
    IN LPCWSTR SiteName OPTIONAL,
    IN ULONG Flags,
    OUT PDOMAIN_CONTROLLER_INFOW *DomainControllerInfo
);

#define DsGetDcName DsGetDcNameW

// Free buffers allocated with DsGetDcNameW()
NET_API_STATUS NetApiBufferFree(LPVOID Buffer);


#ifdef __cplusplus
}
#endif


#endif // _DSGETDC_


