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
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Module Name:  

    fsioctl.h

Abstract:

    This module contains the constant, type and structure definitions for
    WINCE file system control codes.

Notes: 


--*/
#include <winioctl.h>

#ifndef _FSIOCTL_H_
#define _FSIOCTL_H_

#ifdef __cplusplus
extern "C" {
#endif

//
// The following fsctl codes are for object store file system
//
#define FSCTL_NOTIFY_AFS_MOUNT_COMPLETE \
    CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 9, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// The following fsctl code is for copy external
//
#define FSCTL_COPY_EXTERNAL_START \
    CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 0x13, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_COPY_EXTERNAL_COMPLETE \
    CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 0x14, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// The following fsctl codes are for file compression
//
#define FSCTL_GET_COMPRESSION \
    CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 15, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_SET_COMPRESSION \
    CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 16, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)

//
// The following fsctl codes are for sparse files
//

#define FSCTL_SET_SPARSE \
    CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 49, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define FSCTL_SET_ZERO_DATA \
    CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 50, METHOD_BUFFERED, FILE_WRITE_DATA) // FILE_ZERO_DATA_INFORMATION,
#define FSCTL_QUERY_ALLOCATED_RANGES \
    CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 51,  METHOD_NEITHER, FILE_READ_DATA)  // FILE_ALLOCATED_RANGE_BUFFER, FILE_ALLOCATED_RANGE_BUFFER

//
// The following fsctl codes are for mounted file system volumes
// 
#define FSCTL_REFRESH_VOLUME \
    CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 31, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define FSCTL_GET_VOLUME_INFO \
    CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 32, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define FSSCTL_FLUSH_BUFFERS \
    CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 33, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define FSCTL_SET_SIMULATION_MODE \
    CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 34, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define FSCTL_GET_REGISTRY_INFO \
    CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 35, METHOD_BUFFERED, FILE_ANY_ACCESS)


// 
// Specified so that file system drivers can implement their own ioctl's without
// fear of overlapping with OS defined codes.
//
#define FSCTL_USER_START 0x7E0
#define FSCTL_USER_END   0x7FF
#define FSCTL_USER(Function) \
    CTL_CODE( FILE_DEVICE_FILE_SYSTEM, FSCTL_USER_START+Function, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Structures for copy external
//
#define COPY_EXTERNAL_READ	1
#define COPY_EXTERNAL_WRITE	2 

typedef struct _FILE_COPY_EXTERNAL {
    DWORD cbSize; 
    DWORD dwDirection;
    PVOID pUserData;
    DWORD cbUserDataSize;
    TCHAR szCancelEventName[MAX_PATH];
} FILE_COPY_EXTERNAL, *PFILE_COPY_EXTERNAL;    

//
// Structure for FSCTL_SET_ZERO_DATA
//

typedef struct _FILE_ZERO_DATA_INFORMATION {

    LARGE_INTEGER FileOffset;
    LARGE_INTEGER BeyondFinalZero;

} FILE_ZERO_DATA_INFORMATION, *PFILE_ZERO_DATA_INFORMATION;

//
// Structure for FSCTL_QUERY_ALLOCATED_RANGES
//

//
// Querying the allocated ranges requires an output buffer to store the
// allocated ranges and an input buffer to specify the range to query.
// The input buffer contains a single entry, the output buffer is an
// array of the following structure.
//

typedef struct _FILE_ALLOCATED_RANGE_BUFFER {

    LARGE_INTEGER FileOffset;
    LARGE_INTEGER Length;

} FILE_ALLOCATED_RANGE_BUFFER, *PFILE_ALLOCATED_RANGE_BUFFER;

//
// Structure for FSCTL_NOTIFY_AFS_MOUNT_COMPLETE
//
typedef struct _AFS_MOUNT_INFO {

    DWORD cbSize;
    int iAFSIndex;
    DWORD dwVolume;
    DWORD dwMountFlags;
    
} AFS_MOUNT_INFO, *PAFS_MOUNT_INFO;


//
// Structure for FSCTL_SET_SIMULATION_MODE
//
typedef struct _FS_SIMULATION_MODE_INFO {
    
    DWORD cbSize; // set to sizeof(FS_SIMULATION_MODE_INFO)
    DWORD dwSimulationMode; // set to 0 to disable, 1 to enable

} FS_SIMULATION_MODE_INFO, *PFS_SIMULATION_MODE_INFO;

//
// Structure and defines for FSCTL_GET_REGISTRY_INFO
//

#define CE_REG_INFO_FLAG_TRUST_PROTECTED        0x1

typedef struct _CE_REGISTRY_INFO {
    DWORD cbSize; // set to sizeof(FS_REGISTRY_INFO)
    HKEY hRootKey; // Root key of the registry HKLM, HKCU
    DWORD dwFlags;
    TCHAR *pszFullKeyName;
    LPDWORD pdwKeyNameLen;
} CE_REGISTRY_INFO, *PCE_REGISTRY_INFO;
        
#ifdef __cplusplus
}
#endif

#endif // _FSIOCTL_H_
