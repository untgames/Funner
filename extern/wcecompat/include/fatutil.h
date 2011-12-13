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
#ifndef INCLUDE_FATUTIL
#define INCLUDE_FATUTIL 1

#include <windows.h>
#include <diskio.h>

#define DEFAULT_ROOT_ENTRIES    512
#define DEFAULT_FAT_VERSION     32
#define DEFAULT_NUM_FATS        1

// Definition of callback indicating progress of scan disk
typedef VOID (*PFN_PROGRESS)(DWORD dwPercent);

// Definition of callback to display a message or to prompt user
typedef BOOL (*PFN_MESSAGE)(LPTSTR szMessage, LPTSTR szCaption, BOOL fYesNo);

// Flags for dwFlags in FORMAT_OPTIONS, SCAN_OPTIONS, and DEFRAG_OPTIONS
#define FATUTIL_FULL_FORMAT                 0x1     // perform a full format
#define FATUTIL_FORMAT_TFAT                 0x2    // format volume as TFAT
#define FATUTIL_DISABLE_MOUNT_CHK     0x4    // disable checking to see if volume is mounted before doing operation
#define FATUTIL_SCAN_VERIFY_FIX           0x1   // prompt user to perform a fix

//
// Options specified by client of format disk
//
typedef struct _FORMAT_OPTIONS {
    DWORD   dwClusSize;        // desired cluster size in bytes (must be power of 2)
    DWORD   dwRootEntries;     // desired number of root directory entries
    DWORD   dwFatVersion;      // FAT Version.  Either 12, 16, or 32    
    DWORD   dwNumFats;         // Number of FATs to create
    DWORD   dwFlags;
} FORMAT_OPTIONS;
typedef FORMAT_OPTIONS  *PFORMAT_OPTIONS;


typedef struct _FORMAT_RESULTS{
    DWORD dwSectorsPerFat;
    DWORD dwReservedSectors;
    DWORD dwSectorsPerCluster;
    DWORD dwRootSectors;
    DWORD dwTotalSectors;
    DWORD dwNumFats;
    DWORD dwFatVersion;
} FORMAT_RESULTS, *PFORMAT_RESULTS;

typedef struct _FORMAT_PARAMS {
    DWORD           cbSize;
    FORMAT_OPTIONS  fo;
    FORMAT_RESULTS  fr;    
    PFN_PROGRESS    pfnProgress;
    PFN_MESSAGE     pfnMessage;
    
} FORMAT_PARAMS, *PFORMAT_PARAMS;

//
// Options specified by client of scan disk
//
typedef struct _SCAN_OPTIONS {
    DWORD   dwFlags;
    DWORD   dwFatToUse;         // Indicates which FAT to use for scanning (one-based indexing)
} SCAN_OPTIONS, *PSCAN_OPTIONS;

typedef struct _SCAN_RESULTS {
    DWORD   dwLostClusters;       // Number of lost cluster found
    DWORD   dwInvalidClusters;    // Number of invalid cluster values in FAT
    DWORD   dwLostChains;         // Number of lost chains found
    DWORD   dwInvalidDirs;        // Number of directories with invalid cluster references
    DWORD   dwInvalidFiles;       // Number of files with invalid cluster references
    DWORD   dwTotalErrors;      // Total number of errors.  Sum of 5 fields above
    DWORD   dwPercentFrag;        // Percent fragmentation on disk
    BOOL    fConsistentFats;      // TRUE if FATs are consistent
    BOOL    fErrorNotFixed;       // TRUE if there are still errors on drive after scan
    
} SCAN_RESULTS, *PSCAN_RESULTS;

typedef struct _SCAN_PARAMS {
    DWORD         cbSize;
    SCAN_OPTIONS  so;
    SCAN_RESULTS  sr;
    PFN_PROGRESS  pfnProgress;
    PFN_MESSAGE   pfnMessage;
    HANDLE        hevCancel;
    
} SCAN_PARAMS, *PSCAN_PARAMS;

//
// Options specified by client of defrag disk
//
typedef struct _DEFRAG_OPTIONS {
    DWORD   dwFatToUse;             // Which FAT to use for scanning before beginning defrag
    DWORD   dwFlags;
} DEFRAG_OPTIONS, *PDEFRAG_OPTIONS;

typedef struct _DEFRAG_RESULTS {
    SCAN_RESULTS   sr;       // Number of lost cluster found    
} DEFRAG_RESULTS, *PDEFRAG_RESULTS;

typedef struct _DEFRAG_PARAMS {
    DWORD           cbSize;
    DEFRAG_OPTIONS  dopt;
    DEFRAG_RESULTS  dr;
    PFN_PROGRESS    pfnProgress;
    PFN_MESSAGE     pfnMessage;
    HANDLE          hevCancel;
    
} DEFRAG_PARAMS, *PDEFRAG_PARAMS;



typedef DWORD (*PFN_FORMATVOLUME)(HANDLE hFile, PDISK_INFO pdi, 
    PFORMAT_OPTIONS pfo, PFN_PROGRESS pfnProgress, PFN_MESSAGE pfnMessage);

typedef DWORD (*PFN_FORMATVOLUMEEX)(HANDLE hVolume, PFORMAT_PARAMS pfp);

typedef VOID (*PFN_FORMATVOLUMEUI)(HANDLE hVolume, HWND hDlg);


typedef BOOL (*PFN_SCANVOLUME)(HANDLE hFile, PDISK_INFO pdi, PSCAN_OPTIONS pso, 
    PFN_PROGRESS pfnProgress, PFN_MESSAGE pfnMessage);

typedef DWORD (*PFN_SCANVOLUMEEX)(HANDLE hVolume, PSCAN_PARAMS psp);

typedef VOID (*PFN_SCANVOLUMEUI)(HANDLE hVolume, HWND hDlg);

typedef BOOL (*PFN_DEFRAGVOLUME)(HANDLE hVolume, PDISK_INFO pdi, PDEFRAG_OPTIONS pdo, 
    PFN_PROGRESS pfnProgress, PFN_MESSAGE pfnMessage);

typedef DWORD (*PFN_DEFRAGVOLUMEEX)(HANDLE hVolume, PDEFRAG_PARAMS pdp);

typedef VOID (*PFN_DEFRAGVOLUMEUI)(HANDLE hVolume, HWND hDlg);


#endif // INCLUDE_FATUTIL
