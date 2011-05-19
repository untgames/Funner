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
#ifndef __STORAGEMGR__
#define __STORAGEMGR__


#include <windows.h>


// {A4E7EDDA-E575-4252-9D6B-4195D48BB865}
static const GUID BLOCK_DRIVER_GUID = { 0xa4e7edda, 0xe575, 0x4252, { 0x9d, 0x6b, 0x41, 0x95, 0xd4, 0x8b, 0xb8, 0x65 } };
// {C1115848-46FD-4976-BDE9-D79448457004}
static const GUID STORE_MOUNT_GUID  = { 0xc1115848, 0x46fd, 0x4976, { 0xbd, 0xe9, 0xd7, 0x94, 0x48, 0x45, 0x70, 0x4 } };
// {169E1941-04CE-4690-97AC-776187EB67CC}
static const GUID FATFS_MOUNT_GUID = { 0x169e1941, 0x4ce, 0x4690, { 0x97, 0xac, 0x77, 0x61, 0x87, 0xeb, 0x67, 0xcc } };
// {72D75746-D54A-4487-B7A1-940C9A3F259A}
static const GUID CDFS_MOUNT_GUID =  { 0x72d75746, 0xd54a, 0x4487, { 0xb7, 0xa1, 0x94, 0xc, 0x9a, 0x3f, 0x25, 0x9a } };
// {462FEDA9-D478-4b00-86BB-51A8E3D10890}
static const GUID UDFS_MOUNT_GUID = { 0x462feda9, 0xd478, 0x4b00, { 0x86, 0xbb, 0x51, 0xa8, 0xe3, 0xd1, 0x8, 0x90 } };
// {BA6B1343-7980-4d0c-9290-762D527B33AB}
static const GUID CDDA_MOUNT_GUID = { 0xba6b1343, 0x7980, 0x4d0c, { 0x92, 0x90, 0x76, 0x2d, 0x52, 0x7b, 0x33, 0xab } };
// {8C77EDE8-47B9-45ae-8BC9-86E7B8D00EDD}
static const GUID FSD_MOUNT_GUID = { 0x8c77ede8, 0x47b9, 0x45ae, { 0x8b, 0xc9, 0x86, 0xe7, 0xb8, 0xd0, 0xe, 0xdd } };


#define BLOCK_DRIVER_GUID_STRING  L"{A4E7EDDA-E575-4252-9D6B-4195D48BB865}"
#define STORE_MOUNT_GUID_STRING   L"{C1115848-46FD-4976-BDE9-D79448457004}"
#define FATFS_MOUNT_GUID_STRING   L"{169E1941-04CE-4690-97AC-776187EB67CC}"
#define CDFS_MOUNT_GUID_STRING    L"{72D75746-D54A-4487-B7A1-940C9A3F259A}"
#define UDFS_MOUNT_GUID_STRING    L"{462FEDA9-D478-4b00-86BB-51A8E3D10890}"
#define CDDA_MOUNT_GUID_STRING    L"{BA6B1343-7980-4d0c-9290-762D527B33AB}"
#define FSD_MOUNT_GUID_STRING     L"{8C77EDE8-47B9-45ae-8BC9-86E7B8D00EDD}"

// {14ACDEE6-5CF3-496b-B39D-8954C96DF002}
static const GUID STOREMGR_DRIVER_GUID = { 0x14acdee6, 0x5cf3, 0x496b, { 0xb3, 0x9d, 0x89, 0x54, 0xc9, 0x6d, 0xf0, 0x2 } };
#define STOREMGR_DRIVER_GUID_STRING = L"{14ACDEE6-5CF3-496b-B39D-8954C96DF002}"


/*****************************************************************************/
/* attributes of a store */
#define STORE_ATTRIBUTE_READONLY    0x00000001
#define STORE_ATTRIBUTE_REMOVABLE   0x00000002
#define STORE_ATTRIBUTE_UNFORMATTED 0x00000004
#define STORE_ATTRIBUTE_AUTOFORMAT  0x00000008
#define STORE_ATTRIBUTE_AUTOPART    0x00000010
#define STORE_ATTRIBUTE_AUTOMOUNT   0x00000020

/* attributes for a partition */
#define PARTITION_ATTRIBUTE_EXPENDABLE 0x00000001  // partition may be trashed 
#define PARTITION_ATTRIBUTE_READONLY   0x00000002  // partition is read-only   
#define PARTITION_ATTRIBUTE_AUTOFORMAT 0x00000004  
#define PARTITION_ATTRIBUTE_ACTIVE     0x00000008
#define PARTITION_ATTRIBUTE_BOOT       0x00000008  // Active(DOS) == Boot(CE)
#define PARTITION_ATTRIBUTE_MOUNTED    0x00000010


/* some basic constants */
#define DEVICENAMESIZE    8
#define STORENAMESIZE     32
#define FILESYSNAMESIZE   32
#define FORMATNAMESIZE    32
#define PARTITIONNAMESIZE 32
#define PROFILENAMESIZE   32
#define FOLDERNAMESIZE    32
#define VOLUMENAMESIZE    64
#define FSDDESCSIZE       32

/* definition used to represent a sector number or sector range */
typedef ULONGLONG SECTORNUM;

//****************************************************************************
typedef struct tagSTORAGEDEVICEINFO
{
    DWORD cbSize;
    TCHAR szProfile[PROFILENAMESIZE];
    DWORD dwDeviceClass;
    DWORD dwDeviceType;
    DWORD dwDeviceFlags;
} STORAGEDEVICEINFO, *PSTORAGEDEVICEINFO;

#define STORAGE_DEVICE_CLASS_BLOCK         0x1
#define STORAGE_DEVICE_CLASS_MULTIMEDIA    0x2

#define STORAGE_DEVICE_TYPE_PCIIDE               (1 << 0)
#define STORAGE_DEVICE_TYPE_FLASH                (1 << 1)
#define STORAGE_DEVICE_TYPE_ATA                  (1 << 2)
#define STORAGE_DEVICE_TYPE_ATAPI                (1 << 4)
#define STORAGE_DEVICE_TYPE_PCCARD               (1 << 5)
#define STORAGE_DEVICE_TYPE_CFCARD               (1 << 6)
#define STORAGE_DEVICE_TYPE_SRAM                 (1 << 7)
#define STORAGE_DEVICE_TYPE_DVD                  (1 << 8)
#define STORAGE_DEVICE_TYPE_CDROM                (1 << 9)
#define STORAGE_DEVICE_TYPE_USB                  (1 << 10)
#define STORAGE_DEVICE_TYPE_1394                 (1 << 11)
#define STORAGE_DEVICE_TYPE_DOC                  (1 << 12)
#define STORAGE_DEVICE_TYPE_UNKNOWN              (1 << 29)
#define STORAGE_DEVICE_TYPE_REMOVABLE_DRIVE      (1 << 30) // Drive itself is removable
#define STORAGE_DEVICE_TYPE_REMOVABLE_MEDIA      (1 << 31) // Just the media is removable ex. CDROM, FLOPPY

#define STORAGE_DEVICE_FLAG_READWRITE      (1 << 0)
#define STORAGE_DEVICE_FLAG_READONLY       (1 << 1)
#define STORAGE_DEVICE_FLAG_TRANSACTED     (1 << 2)
#define STORAGE_DEVICE_FLAG_MEDIASENSE     (1 << 3)  // Device requires media sense calls
#define STORAGE_DEVICE_FLAG_XIP            (1 << 4)

/*****************************************************************************/
// information about a store */
typedef struct tagSTOREINFO
{
    DWORD      cbSize;                  // sizeof(PD_STOREINFO)                   
    TCHAR      szDeviceName[DEVICENAMESIZE];
    TCHAR      szStoreName[STORENAMESIZE];
    DWORD      dwDeviceClass;
    DWORD      dwDeviceType;
    STORAGEDEVICEINFO sdi;
    DWORD      dwDeviceFlags;
    SECTORNUM  snNumSectors;            // number of sectors on store             
    DWORD      dwBytesPerSector;        // number of bytes per sector             
    SECTORNUM  snFreeSectors;           // number of unallocated sectors          
    SECTORNUM  snBiggestPartCreatable;  // biggest partition currently creatable  
    FILETIME   ftCreated;               // last time store was formatted          
    FILETIME   ftLastModified;          // last time partition table was modified 
    DWORD      dwAttributes;            // store attributes, see below            
    DWORD      dwPartitionCount;        // Number of Partitions 
    DWORD      dwMountCount;            // Number of partitions that have been mounted
} STOREINFO, *PSTOREINFO;


//***************************************************************************
// information about a partition 

typedef struct tagPARTINFO
{
    DWORD     cbSize;                               // sizeof(PD_PARTINFO)             
    TCHAR     szPartitionName[PARTITIONNAMESIZE];   // name of partition                 
    TCHAR     szFileSys[FILESYSNAMESIZE];
    TCHAR     szVolumeName[VOLUMENAMESIZE];
    SECTORNUM snNumSectors;                         // number of sectors in partition    
    FILETIME  ftCreated;                            // creation time of partition        
    FILETIME  ftLastModified;                       // last time partition was modified  
    DWORD     dwAttributes;                         // partition attributes, see below   
    BYTE      bPartType;                  
} PARTINFO, *PPARTINFO;

//****************************************************************************/

typedef struct tagSTORAGECONTEXT
{
    DWORD cbSize;
    STOREINFO StoreInfo;
    PARTINFO PartInfo;
    DWORD dwFlags;
} STORAGECONTEXT, *PSTORAGECONTEXT;


#ifdef __cplusplus
extern "C" {
#endif


BOOL IsStorageManagerRunning();
// Storage Management API's
HANDLE WINAPI OpenStore(LPCTSTR szDeviceName);
BOOL   WINAPI DismountStore(HANDLE hStore);
BOOL   WINAPI FormatStore(HANDLE hStore);
HANDLE WINAPI FindFirstStore(PSTOREINFO pStoreInfo);
BOOL   WINAPI FindNextStore(HANDLE hSearch, PSTOREINFO pStoreInfo);
BOOL   WINAPI FindCloseStore(HANDLE hSearch);
BOOL   WINAPI GetStoreInfo(HANDLE hStore, PSTOREINFO pStoreInfo);
// Partition Management API's
BOOL   WINAPI CreatePartition(HANDLE hStore, LPCTSTR szPartitionName, SECTORNUM snNumSectors);
BOOL   WINAPI CreatePartitionEx(HANDLE hStore, LPCTSTR szPartitionName, BYTE bPartType, SECTORNUM snNumSectors);
BOOL   WINAPI DeletePartition(HANDLE hStore, LPCTSTR szPartitionName);
HANDLE WINAPI OpenPartition( HANDLE hStore, LPCTSTR szPartitionName);
BOOL   WINAPI MountPartition(HANDLE hPartition);
BOOL   WINAPI DismountPartition(HANDLE hPartition);
BOOL   WINAPI RenamePartition(HANDLE hPartition, LPCTSTR szNewName);
BOOL   WINAPI SetPartitionAttributes(HANDLE hPartition, DWORD dwAttrs);
BOOL   WINAPI GetPartitionInfo(HANDLE hPartition, PPARTINFO pPartInfo);
BOOL   WINAPI FormatPartition(HANDLE hPartition);
BOOL   WINAPI FormatPartitionEx(HANDLE hPartition, BYTE bPartType, BOOL bAuto);
HANDLE WINAPI FindFirstPartition(HANDLE hStore, PPARTINFO pPartInfo);
BOOL   WINAPI FindNextPartition(HANDLE hSearch, PPARTINFO pPartInfo);
BOOL   WINAPI FindClosePartition(HANDLE hSearch);

#ifdef UNDER_CE
#define IMPLICIT_CALL(hid, mid) (FIRST_METHOD - ((hid)<<HANDLE_SHIFT | (mid))*APICALL_SCALE)
#define WIN32_FILE_CALL(type, api, args) (*(type (*) args)IMPLICIT_CALL(HT_FILE, api))
#define WIN32_FIND_CALL(type, api, args) (*(type (*) args)IMPLICIT_CALL(HT_FIND, api))
// Storage Management API's
#define PSLGetStoreInfo WIN32_FILE_CALL(BOOL, 1, (HANDLE hStore, PSTOREINFO pInfo))
#define PSLDismountStore WIN32_FILE_CALL(BOOL, 2, (HANDLE hStore))
#define PSLFormatStore WIN32_FILE_CALL(BOOL, 3, (HANDLE hStore))
#define PSLCreatePart WIN32_FILE_CALL(BOOL, 4,(HANDLE hStore, LPCTSTR szPartitionName, DWORD dwPartType, DWORD dwHighSec, DWORD dwLowSec, BOOL bAuto))
#define PSLDeletePartition WIN32_FILE_CALL(BOOL, 5, (HANDLE hStore, LPCTSTR szPartitionName))
#define PSLOpenPartition WIN32_FILE_CALL(HANDLE, 6, (HANDLE hStore, LPCTSTR szPartitionName))
#define PSLMountPartition WIN32_FILE_CALL(BOOL, 7, (HANDLE hPartition))
#define PSLDismountPartition WIN32_FILE_CALL(BOOL, 8, (HANDLE hPartition))
#define PSLRenamePartition WIN32_FILE_CALL(BOOL, 9, (HANDLE hPartition, LPCTSTR szNewName))
#define PSLSetPartitionAttributes WIN32_FILE_CALL(BOOL, 10, (HANDLE hStore, DWORD dwAttrs))
#define PSLGetPartitionInfo WIN32_FILE_CALL(BOOL, 12, (HANDLE hPartition, PPARTINFO pInfo))
#define PSLFormatPart WIN32_FILE_CALL(BOOL, 13, (HANDLE hPartition, DWORD bPartType, BOOL bAuto))
#define PSLFindFirstPartition WIN32_FILE_CALL(HANDLE, 14, (HANDLE hStore, PPARTINFO pInfo))
#define PSLFindNextStore WIN32_FIND_CALL(BOOL, 2, (HANDLE hSearch, PSTOREINFO pInfo))
#define PSLFindCloseStore WIN32_FIND_CALL(BOOL, 0, (HANDLE hSearch))
#define PSLFindNextPartition WIN32_FIND_CALL(BOOL, 2, (HANDLE hSearch, PPARTINFO pInfo))
#define PSLFindClosePartition WIN32_FIND_CALL(BOOL, 0, (HANDLE hSearch))
#endif

#ifdef __cplusplus
}
#endif


#define CE_VOLUME_ATTRIBUTE_READONLY                0x1
#define CE_VOLUME_ATTRIBUTE_HIDDEN                  0x2
#define CE_VOLUME_ATTRIBUTE_REMOVABLE               0x4
#define CE_VOLUME_ATTRIBUTE_SYSTEM                  0x8
#define CE_VOLUME_ATTRIBUTE_BOOT                    0x10

#define CE_VOLUME_TRANSACTION_SAFE                  0x1     // Performs transaction safe operations
#define CE_VOLUME_FLAG_TRANSACT_WRITE               0x2
#define CE_VOLUME_FLAG_WFSC_SUPPORTED               0x4
#define CE_VOLUME_FLAG_LOCKFILE_SUPPORTED           0x8
#define CE_VOLUME_FLAG_NETWORK                      0x10
#define CE_VOLUME_FLAG_STORE                        0x20

typedef struct _CE_VOLUME_INFO {
    DWORD cbSize;
    DWORD dwAttributes;
    DWORD dwFlags;
    DWORD dwBlockSize;
    TCHAR szStoreName[STORENAMESIZE];
    TCHAR szPartitionName[PARTITIONNAMESIZE];
} CE_VOLUME_INFO, *PCE_VOLUME_INFO, *LPCE_VOLUME_INFO;



typedef enum _CE_VOLUME_INFO_LEVEL {
    CeVolumeInfoLevelStandard,
} CE_VOLUME_INFO_LEVEL;

WINBASEAPI
BOOL
CeGetVolumeInfoW(
    IN LPCWSTR pszRootPath,
    IN CE_VOLUME_INFO_LEVEL InfoLevel,
    OUT LPCE_VOLUME_INFO lpVolumeInfo
    );

#define CeGetVolumeInfo CeGetVolumeInfoW    

/*****************************************************************************/
#define IOCTL_STORE_FINDFIRST         CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_STORE_OPEN              CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)


#endif /* __STORAGEMGR__ */
