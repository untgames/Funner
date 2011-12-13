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


Module Name: dsrole.h

Purpose: Declares structures and routines related to DsRoleGetPrimaryDomainInformation.
--*/

#ifndef __DSROLE_H__
#define __DSROLE_H__

#ifdef __cplusplus
extern "C" {
#endif


//
// Domain information
//
typedef enum _DSROLE_MACHINE_ROLE {

    DsRole_RoleStandaloneWorkstation,
    DsRole_RoleMemberWorkstation,
    DsRole_RoleStandaloneServer,
    DsRole_RoleMemberServer,
    DsRole_RoleBackupDomainController,
    DsRole_RolePrimaryDomainController

} DSROLE_MACHINE_ROLE;

//
// Previous server state
//
typedef enum _DSROLE_SERVER_STATE {

    DsRoleServerUnknown = 0,
    DsRoleServerPrimary,
    DsRoleServerBackup

} DSROLE_SERVER_STATE, *PDSROLE_SERVER_STATE;

typedef enum _DSROLE_PRIMARY_DOMAIN_INFO_LEVEL {

    DsRolePrimaryDomainInfoBasic = 1,
} DSROLE_PRIMARY_DOMAIN_INFO_LEVEL;

//
// Flags to be used with the PRIMARY_DOMAIN_INFO_LEVEL structures below
//
#define DSROLE_PRIMARY_DS_RUNNING           0x00000001
#define DSROLE_PRIMARY_DS_MIXED_MODE        0x00000002
#define DSROLE_UPGRADE_IN_PROGRESS          0x00000004
#define DSROLE_PRIMARY_DOMAIN_GUID_PRESENT  0x01000000

//
// Structure that correspond to the DSROLE_PRIMARY_DOMAIN_INFO_LEVEL
//
typedef struct _DSROLE_PRIMARY_DOMAIN_INFO_BASIC {

    DSROLE_MACHINE_ROLE MachineRole;
    ULONG Flags;
    LPWSTR DomainNameFlat;
    LPWSTR DomainNameDns;
    LPWSTR DomainForestName;
    GUID DomainGuid;

} DSROLE_PRIMARY_DOMAIN_INFO_BASIC, *PDSROLE_PRIMARY_DOMAIN_INFO_BASIC;


DWORD
WINAPI
DsRoleGetPrimaryDomainInformation(
    IN  LPCWSTR lpServer OPTIONAL,
    IN  DSROLE_PRIMARY_DOMAIN_INFO_LEVEL InfoLevel,
    OUT PBYTE *Buffer 
    );

VOID
WINAPI
DsRoleFreeMemory(
    IN PVOID    Buffer
    );




#ifdef __cplusplus
}
#endif

#endif // __DSROLE_H__

