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

    fsdmgr.h

Abstract:

    This file contains external definitions for the WinCE FSD Manager (FSDMGR.DLL).

Revision History:

--*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef FSDMGR_H
#define FSDMGR_H

#include <extfile.h>

#ifdef WINCEOEM
#include <diskio.h>
#endif

#ifndef HDSK
#define HDSK    DWORD
#endif

#ifndef HVOL
#define HVOL    DWORD
#endif

#ifndef PVOLUME
#define PVOLUME DWORD
#endif

#ifndef PFILE
#define PFILE   DWORD
#endif

#ifndef PSEARCH
#define PSEARCH DWORD
#endif

/*  FSD_DISK_INFO structure (returned by FSDMGR_GetDiskInfo):
 */
#define FDI_MBR             0x00000001  // device type has/needs an MBR
#define FDI_CHS_UNCERTAIN   0x00000002  // driver generated CHS values are suspect
#define FDI_UNFORMATTED     0x00000004  // low level format (DISK_IOCTL_FORMAT_MEDIA) needed
#define FDI_PAGEABLE        0x00000008  // device can be used for demand paging
#define FDI_READONLY        0x00000010  // device is read-only
#define FDI_RESERVED        0xffffffe0

typedef struct _FSD_DISK_INFO {
    DWORD   cSectors;
    DWORD   cbSector;
    DWORD   cCylinders;
    DWORD   cHeadsPerCylinder;
    DWORD   cSectorsPerTrack;
    DWORD   dwFlags;            // see FDI_* above
} FSD_DISK_INFO, *PFDI;


/*  FSD_BUFFER_INFO (used in FSD_SCATTER_GATHER_INFO):
 */
typedef struct _FSD_BUFFER_INFO {
    PBYTE   pBuffer;            // pointer to buffer
    DWORD   cbBuffer;           // size of buffer, in bytes
} FSD_BUFFER_INFO, *PFBI;


/*  FSD_SCATTER_GATHER_INFO (used by FSDMGR_ReadDiskEx and FSDMGR_WriteDiskEx):
 */
typedef DWORD (*PFNFSG)(struct _FSD_SCATTER_GATHER_INFO *pfsgi, struct _FSD_SCATTER_GATHER_RESULTS *pfsgr);

typedef struct _FSD_SCATTER_GATHER_INFO {
    DWORD   dwFlags;            // reserved (must be zero)
    DWORD   idDsk;              // disk ID (as passed to XXX_MountDisk)
    DWORD   dwSector;           // starting sector #
    DWORD   cSectors;           // number of sectors to transfer
    PFDI    pfdi;               // address of FSD_DISK_INFO
    DWORD   cfbi;               // count of FSD_BUFFER_INFO structures (can be zero)
    PFBI    pfbi;               // pointer to zero or more FSD_BUFFER_INFO structures
    PFNFSG  pfnCallBack;        // callback function
} FSD_SCATTER_GATHER_INFO, *PFSGI;


/*  FSD_SCATTER_GATHER_RESULTS (returned by FSDMGR_ReadWriteDiskEx and FSDMGR_WriteDiskEx):
 */
typedef struct _FSD_SCATTER_GATHER_RESULTS {
    DWORD   dwFlags;            // reserved
    DWORD   cSectorsTransferred;// total sectors transferred
} FSD_SCATTER_GATHER_RESULTS, *PFSGR;


#define FSD_VERSION_0   0
#define MAX_FSD_DESCRIPTOR 32

#define FSD_ATTRIBUTE_READONLY       0x1

#define FSD_FLAG_TRANSACTION_SAFE       0x1     // Performs transaction safe operations
#define FSD_FLAG_TRANSACT_WRITES        0x2    // Transacts write operations
#define FSD_FLAG_WFSC_SUPPORTED         0x4    // WriteFile ScatterGather supported
#define FSD_FLAG_LOCKFILE_SUPPORTED     0x8    // Implements Lockfile
#define FSD_FLAG_NETWORK                0x10   // Network filesystem

typedef struct _FSD_VOLUME_INFO {
    DWORD cbSize;
    DWORD dwFSVersion ; // Version that this FSD conforms to must be one of the defines above
    TCHAR szFSDDesc[MAX_FSD_DESCRIPTOR]; // FSD Name FAT, UDFS
    TCHAR szFSDSubType[MAX_FSD_DESCRIPTOR]; // Subtype FAT12, FAT16, FAT32
    DWORD dwAttributes;
    DWORD dwBlockSize; // This is a hint of the optimal block size for I/O operations.  For FAT this would be cluster size
    DWORD dwFlags;
} FSD_VOLUME_INFO, *PFSD_VOLUME_INFO;


/*  Interfaces exported by FSDMGR.DLL to FSDs
 */
DWORD  FSDMGR_GetDiskInfo(HDSK hDsk, PFDI pfdi);
DWORD  FSDMGR_ReadDisk(HDSK hDsk, DWORD dwSector, DWORD cSectors, PBYTE pBuffer, DWORD cbBuffer);
DWORD  FSDMGR_WriteDisk(HDSK hDsk, DWORD dwSector, DWORD cSectors, PBYTE pBuffer, DWORD cbBuffer);
DWORD  FSDMGR_ReadDiskEx(PFSGI pfsgi, PFSGR pfsgr);
DWORD  FSDMGR_WriteDiskEx(PFSGI pfsgi, PFSGR pfsgr);
HVOL   FSDMGR_RegisterVolume(HDSK hDsk, PWSTR pwsName, PVOLUME pVolume);
int    FSDMGR_GetVolumeName(HVOL hVol, PWSTR pwsName, int cchMax);
void   FSDMGR_DeregisterVolume(HVOL hVol);
HANDLE FSDMGR_CreateFileHandle(HVOL hVol, HANDLE hProc, PFILE pFile);
HANDLE FSDMGR_CreateSearchHandle(HVOL hVol, HANDLE hProc, PSEARCH pSearch);
BOOL   FSDMGR_DiskIoControl(HDSK hDsk, DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);
BOOL   FSDMGR_GetRegistryValue(HDSK hDsk, PCTSTR szValueName, PDWORD pdwValue);
BOOL   FSDMGR_GetRegistryString(HDSK hDsk, PCTSTR szValueName, PTSTR szValue, DWORD dwSize);
BOOL   FSDMGR_GetRegistryFlag(HDSK pDsk,const TCHAR *szValueName, PDWORD pdwFlag, DWORD dwSet);
BOOL   FSDMGR_GetDiskName(HDSK pDsk, TCHAR *szDiskName);
BOOL   FSDMGR_AdvertiseInterface(const GUID *pGuid, LPCWSTR lpszName, BOOL fAdd);
DWORD  FSDMGR_FormatVolume(HDSK pDsk, LPVOID pParams);
DWORD  FSDMGR_ScanVolume(HDSK pDsk, LPVOID pParams);
// LCKMGR interfaces exported by FSDMGR to FSDs
#include <lockmgr.h>

// Caching functions
#ifdef WINCEOEM
DWORD  FSDMGR_CreateCache(HDSK pDsk, DWORD dwStart, DWORD dwEnd, DWORD dwCacheSize, DWORD dwBlockSize, DWORD dwCreateFlags);
DWORD  FSDMGR_DeleteCache(DWORD dwCacheId);
DWORD  FSDMGR_ResizeCache(DWORD dwCacheId, DWORD dwSize, DWORD dwResizeFlags);
DWORD  FSDMGR_CachedRead(DWORD dwCacheId, DWORD dwBlockNum, DWORD dwNumBlocks, PVOID pBuffer, DWORD dwReadFlags);
DWORD  FSDMGR_CachedWrite(DWORD dwCacheId, DWORD dwBlockNum, DWORD dwNumBlocks, PVOID pBuffer, DWORD dwWriteFlags);
DWORD  FSDMGR_FlushCache(DWORD dwCacheId, PSECTOR_LIST_ENTRY pSectorList, DWORD dwNumEntries, DWORD dwFlushFlags);
DWORD  FSDMGR_SyncCache(DWORD dwCacheId, PSECTOR_LIST_ENTRY pSectorList, DWORD dwNumEntries, DWORD dwSyncFlags);
DWORD  FSDMGR_InvalidateCache(DWORD dwCacheId, PSECTOR_LIST_ENTRY pSectorList, DWORD dwNumEntries, DWORD dwFlags);
BOOL   FSDMGR_CacheIoControl(DWORD dwCacheId, DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);
HDSK   FSDMGR_DeviceHandleToHDSK(HANDLE hDisk);
#endif

// Valid Flags for dwCreateFlags in FSDMGR_CreateCache
#define CACHE_FLAG_WARM      0x1
#define CACHE_FLAG_WRITEBACK 0x2

// Valid Flags for dwWriteFlags in FSDMGR_CachedWrite
#define CACHE_FORCE_WRITETHROUGH 0x1

// Return value for error in FSDMGR_CreateCache
#define INVALID_CACHE_ID (DWORD)-1

typedef BOOL(* PCLOSEVOLUME)(DWORD dwVolume);
typedef BOOL(* PCREATEDIRECTORYW)(DWORD dwVolume, PCWSTR pwsPathName, PSECURITY_ATTRIBUTES pSecurityAttributes);
typedef BOOL(* PREMOVEDIRECTORYW)(DWORD dwVolume, PCWSTR pwsPathName);
typedef DWORD(* PGETFILEATTRIBUTESW)(DWORD dwVolume, PCWSTR pwsFileName);
typedef BOOL(* PSETFILEATTRIBUTESW)(DWORD dwVolume, PCWSTR pwsFileName, DWORD dwAttributes);
typedef BOOL(* PDELETEFILEW)(DWORD dwVolume, PCWSTR pwsFileName);
typedef BOOL(* PMOVEFILEW)(DWORD dwVolume, PCWSTR pwsOldFileName, PCWSTR pwsNewFileName);
typedef BOOL(* PDELETEANDRENAMEFILEW)(DWORD dwVolume, PCWSTR pwsOldFileName, PCWSTR pwsNewFileName);
typedef BOOL(* PGETDISKFREESPACEW)(DWORD dwVolume, PCWSTR pwsPathName, PDWORD pSectorsPerCluster, PDWORD pBytesPerSector, PDWORD pFreeClusters, PDWORD pClusters);
typedef void(* PNOTIFY)(DWORD dwVolume, DWORD dwFlags);
typedef BOOL(* PREGISTERFILESYSTEMFUNCTION)(DWORD dwVolume, SHELLFILECHANGEFUNC_t pfn);
typedef HANDLE(* PFINDFIRSTFILEW)(DWORD dwVolume, HANDLE hProc, PCWSTR pwsFileSpec, PWIN32_FIND_DATAW pfd);
typedef BOOL(* PFINDNEXTFILEW)(DWORD dwSearch, PWIN32_FIND_DATAW pfd);
typedef BOOL(* PFINDCLOSE)(DWORD dwSearch);
typedef HANDLE(* PCREATEFILEW)(DWORD dwVolume, HANDLE hProc, PCWSTR pwsFileName, DWORD dwAccess, DWORD dwShareMode, PSECURITY_ATTRIBUTES pSecurityAttributes, DWORD dwCreate, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
typedef BOOL(* PREADFILE)(DWORD dwFile, PVOID pBuffer, DWORD cbRead, PDWORD pcbRead, OVERLAPPED *pOverlapped);
typedef BOOL(* PREADFILEWITHSEEK)(DWORD dwFile, PVOID pBuffer, DWORD cbRead, PDWORD pcbRead, OVERLAPPED *pOverlapped, DWORD dwLowOffset, DWORD dwHighOffset);
typedef BOOL(* PWRITEFILE)(DWORD dwFile, PCVOID pBuffer, DWORD cbWrite, PDWORD pcbWritten, OVERLAPPED *pOverlapped);
typedef BOOL(* PWRITEFILEWITHSEEK)(DWORD dwFile, PCVOID pBuffer, DWORD cbWrite, PDWORD pcbWritten, OVERLAPPED *pOverlapped, DWORD dwLowOffset, DWORD dwHighOffset);
typedef DWORD(* PSETFILEPOINTER)(DWORD dwFile, LONG lDistanceToMove, PLONG pDistanceToMoveHigh, DWORD dwMoveMethod);
typedef DWORD(* PGETFILESIZE)(DWORD dwFile, PDWORD pFileSizeHigh);
typedef BOOL(* PGETFILEINFORMATIONBYHANDLE)(DWORD dwFile, PBY_HANDLE_FILE_INFORMATION pFileInfo);
typedef BOOL(* PFLUSHFILEBUFFERS)(DWORD dwFile);
typedef BOOL(* PGETFILETIME)(DWORD dwFile, FILETIME *pCreation, FILETIME *pLastAccess, FILETIME *pLastWrite);
typedef BOOL(* PSETFILETIME)(DWORD dwFile, CONST FILETIME *pCreation, CONST FILETIME *pLastAccess, CONST FILETIME *pLastWrite);
typedef BOOL(* PSETENDOFFILE)(DWORD dwFile);
typedef BOOL(* PDEVICEIOCONTROL)(DWORD dwFile, DWORD dwIoControlCode, PVOID pInBuf, DWORD nInBufSize, PVOID pOutBuf, DWORD nOutBufSize, PDWORD pBytesReturned, OVERLAPPED *pOverlapped);
typedef BOOL(* PCLOSEFILE)(DWORD dwFile);
typedef BOOL(* PREFRESHVOLUME)(DWORD dwVolume);
typedef BOOL(* PREADFILESCATTER)(DWORD dwFile, FILE_SEGMENT_ELEMENT aSegmentArray[], DWORD nNumberOfBytesToRead, LPDWORD lpReserved, LPOVERLAPPED lpOverlapped);
typedef BOOL(* PWRITEFILEGATHER)(DWORD dwFile, FILE_SEGMENT_ELEMENT aSegmentArray[], DWORD nNumberOfBytesToWrite, LPDWORD lpReserved, LPOVERLAPPED lpOverlapped);
typedef BOOL(* PFSIOCONTROL)(DWORD dwVolume, DWORD dwIoControlCode, PVOID pInBuf, DWORD nInBufSize, PVOID pOutBuf, DWORD nOutBufSize, PDWORD pBytesReturned, OVERLAPPED *pOverlapped);
typedef BOOL(* PLOCKFILEEX)(DWORD dwFile, DWORD dwFlags, DWORD dwReserved, DWORD nNumberOfBytesToLockLow, DWORD nNumberOfBytesToLockHigh, LPOVERLAPPED lpOverlapped);
typedef BOOL(* PUNLOCKFILEEX)(DWORD dwFile, DWORD dwReserved, DWORD nNumberOfBytesToUnlockLow, DWORD nNumberOfBytesToUnlockHigh, LPOVERLAPPED lpOverlapped);
typedef BOOL(* PGETVOLUMEINFO)(DWORD dwVolume, FSD_VOLUME_INFO *pInfo);

typedef struct tagFILTERHOOK {
    DWORD cbSize;
    DWORD hVolume;
    PCLOSEVOLUME pCloseVolume;
    PCREATEDIRECTORYW pCreateDirectoryW;
    PREMOVEDIRECTORYW pRemoveDirectoryW;
    PGETFILEATTRIBUTESW pGetFileAttributesW;
    PSETFILEATTRIBUTESW pSetFileAttributesW;
    PDELETEFILEW pDeleteFileW;
    PMOVEFILEW pMoveFileW;
    PDELETEANDRENAMEFILEW pDeleteAndRenameFileW;
    PGETDISKFREESPACEW pGetDiskFreeSpaceW;
    PNOTIFY pNotify;
    PREGISTERFILESYSTEMFUNCTION pRegisterFileSystemFunction;
    PFINDFIRSTFILEW pFindFirstFileW;
    PFINDNEXTFILEW pFindNextFileW;
    PFINDCLOSE pFindClose;
    PCREATEFILEW pCreateFileW;
    PREADFILE pReadFile;
    PREADFILEWITHSEEK pReadFileWithSeek;
    PWRITEFILE pWriteFile;
    PWRITEFILEWITHSEEK pWriteFileWithSeek;
    PSETFILEPOINTER pSetFilePointer;
    PGETFILESIZE pGetFileSize;
    PGETFILEINFORMATIONBYHANDLE pGetFileInformationByHandle;
    PFLUSHFILEBUFFERS pFlushFileBuffers;
    PGETFILETIME pGetFileTime;
    PSETFILETIME pSetFileTime;
    PSETENDOFFILE pSetEndOfFile;
    PDEVICEIOCONTROL pDeviceIoControl;
    PCLOSEFILE pCloseFile;
    PREFRESHVOLUME pRefreshVolume;
    PFSIOCONTROL pFsIoControl;
    PLOCKFILEEX pLockFileEx;
    PUNLOCKFILEEX pUnlockFileEx;
    PGETVOLUMEINFO pGetVolumeInfo;
    PREADFILESCATTER pReadFileScatter;
    PWRITEFILEGATHER pWriteFileGather;
} FILTERHOOK, *PFILTERHOOK;


#endif /* FSDMGR_H */


#ifdef FSDAPI

#define DECLAPI(ret,prefix,func)    ret prefix##_##func
#define DECLFSDAPI(ret,prefix,func) DECLAPI(ret,prefix,func)


/*  Interfaces imported by FSDMGR.DLL from FSDs for mount support
 *
 *  Note that the XXX_MountDisk interface must also be exported as
 *  "FSD_MountDisk", since DEVICE.EXE specifically looks for that export
 *  when determining how to load the FSD (ie, using FSDMGR.DLL or
 *  stand-alone).
 *
 *  This requirement is easily satisfied in the DEF file for the FSD;
 *  for example, for an FSD named FATFSD.DLL, the DEF file would contain:
 *
 *      EXPORTS
 *          FATFSD_MountDisk
 *          FATFSD_UnmountDisk
 *          FSD_MountDisk=FATFSD_MountDisk
 *          FSD_UnmountDisk=FATFSD_UnmountDisk
 *          ...
 */

DECLFSDAPI(BOOL, FSDAPI, MountDisk(HDSK hdsk));
DECLFSDAPI(BOOL, FSDAPI, UnmountDisk(HDSK hdsk));
DECLFSDAPI(PVOLUME, FSDAPI, HookVolume(HDSK hdsk, PFILTERHOOK pFilterHook));
DECLFSDAPI(BOOL, FSDAPI, UnhookVolume(PVOLUME pVol));


/*  Interfaces imported by FSDMGR.DLL from FSDs for application support
 *
 *  Add a #define for FSDAPI before including this file, so that the following
 *  prototypes match the interface names your FSD must export.  For example,
 *  an FSD named FATFSD.DLL would want to include the following lines in its
 *  source code:
 *
 *      #define FSDAPI FATFSD
 *      #include <fsdmgr.h>
 *
 *  since FSDMGR.DLL will expect to find exports named "FATFSD_CreateDirectoryW",
 *  "FATFSD_RemoveDirectoryW", etc, if it is asked to load an FSD named FATFSD.DLL.
 */

DECLFSDAPI(BOOL,  FSDAPI, CloseVolume(PVOLUME pVolume));
DECLFSDAPI(BOOL,  FSDAPI, CreateDirectoryW(PVOLUME pVolume, PCWSTR pwsPathName, PSECURITY_ATTRIBUTES pSecurityAttributes));
DECLFSDAPI(BOOL,  FSDAPI, RemoveDirectoryW(PVOLUME pVolume, PCWSTR pwsPathName));
DECLFSDAPI(DWORD, FSDAPI, GetFileAttributesW(PVOLUME pVolume, PCWSTR pwsFileName));
DECLFSDAPI(BOOL,  FSDAPI, SetFileAttributesW(PVOLUME pVolume, PCWSTR pwsFileName, DWORD dwAttributes));
DECLFSDAPI(BOOL,  FSDAPI, DeleteFileW(PVOLUME pVolume, PCWSTR pwsFileName));
DECLFSDAPI(BOOL,  FSDAPI, MoveFileW(PVOLUME pVolume, PCWSTR pwsOldFileName, PCWSTR pwsNewFileName));
DECLFSDAPI(BOOL,  FSDAPI, DeleteAndRenameFileW(PVOLUME pVolume, PCWSTR pwsOldFileName, PCWSTR pwsNewFileName));
DECLFSDAPI(BOOL,  FSDAPI, GetDiskFreeSpaceW(PVOLUME pVolume, PCWSTR pwsPathName, PDWORD pSectorsPerCluster, PDWORD pBytesPerSector, PDWORD pFreeClusters, PDWORD pClusters));
DECLFSDAPI(void,  FSDAPI, Notify(PVOLUME pVolume, DWORD dwFlags));
DECLFSDAPI(BOOL,  FSDAPI, RegisterFileSystemFunction(PVOLUME pVolume, SHELLFILECHANGEFUNC_t pfn));
DECLFSDAPI(HANDLE,FSDAPI, FindFirstFileW(PVOLUME pVolume, HANDLE hProc, PCWSTR pwsFileSpec, PWIN32_FIND_DATAW pfd));
DECLFSDAPI(BOOL,  FSDAPI, FindNextFileW(PSEARCH pSearch, PWIN32_FIND_DATAW pfd));
DECLFSDAPI(BOOL,  FSDAPI, FindClose(PSEARCH pSearch));
DECLFSDAPI(HANDLE,FSDAPI, CreateFileW(PVOLUME pVolume, HANDLE hProc, PCWSTR pwsFileName, DWORD dwAccess, DWORD dwShareMode, PSECURITY_ATTRIBUTES pSecurityAttributes, DWORD dwCreate, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile));
DECLFSDAPI(BOOL,  FSDAPI, ReadFile(PFILE pFile, PVOID pBuffer, DWORD cbRead, PDWORD pcbRead, OVERLAPPED *pOverlapped));
DECLFSDAPI(BOOL,  FSDAPI, ReadFileWithSeek(PFILE pFile, PVOID pBuffer, DWORD cbRead, PDWORD pcbRead, OVERLAPPED *pOverlapped, DWORD dwLowOffset, DWORD dwHighOffset));
DECLFSDAPI(BOOL,  FSDAPI, WriteFile(PFILE pFile, PCVOID pBuffer, DWORD cbWrite, PDWORD pcbWritten, OVERLAPPED *pOverlapped));
DECLFSDAPI(BOOL,  FSDAPI, WriteFileWithSeek(PFILE pFile, PCVOID pBuffer, DWORD cbWrite, PDWORD pcbWritten, OVERLAPPED *pOverlapped, DWORD dwLowOffset, DWORD dwHighOffset));
DECLFSDAPI(DWORD, FSDAPI, SetFilePointer(PFILE pFile, LONG lDistanceToMove, PLONG pDistanceToMoveHigh, DWORD dwMoveMethod));
DECLFSDAPI(DWORD, FSDAPI, GetFileSize(PFILE pFile, PDWORD pFileSizeHigh));
DECLFSDAPI(BOOL,  FSDAPI, GetFileInformationByHandle(PFILE pFile, PBY_HANDLE_FILE_INFORMATION pFileInfo));
DECLFSDAPI(BOOL,  FSDAPI, FlushFileBuffers(PFILE pFile));
DECLFSDAPI(BOOL,  FSDAPI, GetFileTime(PFILE pFile, FILETIME *pCreation, FILETIME *pLastAccess, FILETIME *pLastWrite));
DECLFSDAPI(BOOL,  FSDAPI, SetFileTime(PFILE pFile, CONST FILETIME *pCreation, CONST FILETIME *pLastAccess, CONST FILETIME *pLastWrite));
DECLFSDAPI(BOOL,  FSDAPI, SetEndOfFile(PFILE pFile));
DECLFSDAPI(BOOL,  FSDAPI, DeviceIoControl(PFILE pFile, DWORD dwIoControlCode, PVOID pInBuf, DWORD nInBufSize, PVOID pOutBuf, DWORD nOutBufSize, PDWORD pBytesReturned, OVERLAPPED *pOverlapped));
DECLFSDAPI(BOOL,  FSDAPI, CloseFile(PFILE pFile));
DECLFSDAPI(BOOL,  FSDAPI, RefreshVolume(PVOLUME pVolume));
DECLFSDAPI(BOOL,  FSDAPI, FsIoControl(PVOLUME pVolume, DWORD dwIoControlCode, PVOID pInBuf, DWORD nInBufSize, PVOID pOutBuf, DWORD nOutBufSize, PDWORD pBytesReturned, OVERLAPPED *pOverlapped));
DECLFSDAPI(BOOL,  FSDAPI, LockFileEx(PFILE pFile, DWORD dwFlags, DWORD dwReserved, DWORD nNumberOfBytesToLockLow, DWORD nNumberOfBytesToLockHigh, LPOVERLAPPED lpOverlapped));
DECLFSDAPI(BOOL,  FSDAPI, UnlockFileEx(PFILE pFile, DWORD dwReserved, DWORD nNumberOfBytesToUnlockLow, DWORD nNumberOfBytesToUnlockHigh, LPOVERLAPPED lpOverlapped));
DECLFSDAPI(BOOL,  FSDAPI, GetVolumeInfo(PVOLUME pVolume, FSD_VOLUME_INFO *pInfo));

#endif  // FSDAPI


#ifdef __cplusplus
}
#endif

