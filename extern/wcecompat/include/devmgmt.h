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


Module Name:devmgmt.h

Purpose:Public Header for Device Management API

--*/

#ifndef _DEVMGMT_H_
#define _DEVMGMT_H_

// task type bits
//---------------------------------------------------------------------------
// four task types
#define DMSTASK_POLL             0x00000001
#define DMSTASK_DOWNLOAD         0x00000002
#define DMSTASK_REPORT           0x00000004
#define DMSTASK_TYPE_MASK        (DMSTASK_POLL | DMSTASK_DOWNLOAD | DMSTASK_REPORT)                              

// source of the data
#define DMSTASK_SRC_FILE         0x00010000
#define DMSTASK_SRC_BUFFER       0x00020000 // point a null terminated LPTSTR
#define DMSTASK_SRC_MASK         (DMSTASK_SRC_FILE | DMSTASK_SRC_BUFFER)         

// the task is scheduled to launch immediately
#define DMSTASK_IMMEDIATE        0x10000000
//---------------------------------------------------------------------------

// task flag bits
//---------------------------------------------------------------------------
// For the nonce, a "fast network" runs at >= 1Mb/sec
#define ADL_FAST_NETWORK_SPEED 1000000

// This operation can be performed over a slow connection
#define ADL_SLOW_NETWORK 0x0001

// This operation can be performed over a fast network
#define ADL_FAST_NETWORK 0x0002

// There's some means other than desktop passthrough through
// which this operation could be performed
#define ADL_NONPASSTHROUGH_ALLOWED (ADL_FAST_NETWORK | ADL_SLOW_NETWORK)

// This operation can be performed over desktop passthrough
#define ADL_PASSTHROUGH 0x0004

// This operation can ONLY be performed over desktop passthrough
#define ADL_PASSTHROUGH_ONLY 0x0008

// This operation is performed repeatedly
#define ADL_RECURRENT           0x80000000

// This bit set means the package is downloaded once, but executed recurrently
#define ADL_RECUR_EXEC_ONLY     0x20000000

// Inventory Report task type
#define ADL_REPORT_SOFTWARE     0x00010000
#define ADL_REPORT_FILECOLLECT  0x00020000
#define ADL_REPORT_HARDWARE     0x00040000
#define ADL_REPORT_INVENTORY    0x00070000  // any of the above three

// This bit set means no source files at this package, so execute the post command line only
#define ADL_NOSOURCEFILES       0x00100000
//---------------------------------------------------------------------------


// package record state
//---------------------------------------------------------------------------
// < package type >
#define PACKAGE_OPTIONAL        0x00010000
#define PACKAGE_MANDATORY       0x00020000

// < download state >
// download task launched
#define PACKAGE_DL_PENDING      0x00000010

// download failed
#define PACKAGE_DL_FAIL         0x00000020 

// after a package is stored
#define PACKAGE_DL_SUCCESS      0x00000040 

#define PACKAGE_DL_STATES       (PACKAGE_DL_PENDING | PACKAGE_DL_FAIL | PACKAGE_DL_SUCCESS)


#define PACKAGE_ALL             0x00030070 // bit or of all the above

// the entry is empty
#define PACKAGE_NULL            0x00000000 // must be zero


// used in Poll response processing (internal, not be used in db)
#define PACKAGE_TO_REQUEST      0x10000000



// CeGetDevMgmtPackageList filter flags
// --------------------------------------------------------------------------
// Have CeGetDevMgmtPackageList retrieve a record with a package id
#define PACKAGE_SRCH_BY_ID       0x10000000

// Get package id and state only
#define PACKAGE_GET_ID_ONLY      0x20000000



//---------------------------------------------------------------------------


// flags value used by CeSetDevMgmtPackageList
//---------------------------------------------------------------------------
#define PACKAGE_WRITE		    0x01	// overwrite or append a record (default)
#define	PACKAGE_DELETE		    0x02	// delete the record found

//---------------------------------------------------------------------------

// if a caller wants to overwrite the text field it must specify explicitly
#define PACKAGE_WRITE_STORTEXT  0x1000
#define PACKAGE_WRITE_LONGTEXT  0x2000
#define PACKAGE_WRITE_APPNAME   0x4000
#define PACKAGE_WRITE_SIZE      0x8000
#define PACKAGE_WRITE_DLINFO    0x0100
#define PACKAGE_WR_STATE_OVR    0x0200
#define PACKAGE_WR_STATE_ADD    0x0400    // set a state bit 
#define PACKAGE_WR_STATE_DEL    0x0800    // clear a state big

#define PACKAGE_WRITE_ALL   (PACKAGE_WRITE | PACKAGE_WRITE_DLINFO | \
                             PACKAGE_WRITE_STORTEXT | PACKAGE_WRITE_LONGTEXT | \
                             PACKAGE_WRITE_APPNAME | PACKAGE_WRITE_SIZE | PACKAGE_WR_STATE_OVR)

// Action types of CeStartDevMgmtService
#define DEVMGMT_SVC_STOP        0
#define DEVMGMT_SVC_START       1
#define DEVMGMT_SVC_GETSTATUS   2       // detect whether the service is running
#define DEVMGMT_SVC_REGWND      3
#define DEVMGMT_SVC_UNREGWND    4

#define PACKAGE_APPNAME_TEXT_LEN    128
#define PACKAGE_SHORT_TEXT_LEN      140
#define PACKAGE_LONG_TEXT_LEN       512

#define DEVICE_ID_LEN               300

#define DMSINVHEADER_MAGIC          0xA54088CE

#define DMS_CTL_SETTASK    CTL_CODE(FILE_DEVICE_NULL, 1000, METHOD_NEITHER, FILE_ANY_ACCESS)
#define DMS_CTL_REGWND     CTL_CODE(FILE_DEVICE_NULL, 1001, METHOD_NEITHER, FILE_ANY_ACCESS)
#define DMS_CTL_UNREGWND   CTL_CODE(FILE_DEVICE_NULL, 1002, METHOD_NEITHER, FILE_ANY_ACCESS)

#ifdef __cplusplus
extern "C" {
#endif

// task information, passed from application to dmsdklib
typedef struct {
    // task info
    DWORD      dwSize;
    SYSTEMTIME stStartTime;
    SYSTEMTIME stDeltaTime;
    SYSTEMTIME stRetryTime;
    DWORD      dwType;
    DWORD      dwMaxRetryTime;
    DWORD      dwFlags;
    LPTSTR     pwzRDN;          // reserved
    LPTSTR     pwzURL;
    LPTSTR     pwzURLSig;       // reserved
    LPTSTR     pwzMisc;         // mostly used for Action name (Report task may use "Status" or "Report")
    LPTSTR     pwzData;         // either a file name or a blob (contains data in TCHAR)
    LPTSTR     pwzXml;          // reserved
} DMTASKINFO, *PDMTASKINFO;


// db header
typedef struct {
    DWORD   dwMagic;
    DWORD   dwRecSize;  
    DWORD   dwRecNumber;
    DWORD   dwReserved;
} DMSINVHEADER, *PDMSINVHEADER;


typedef struct {
    SYSTEMTIME stStartTime;
    SYSTEMTIME stDeltaTime;
    DWORD    dwFlags;
    TCHAR    wzDest[MAX_PATH];
    TCHAR    wzCmdLine[MAX_PATH];    
} DMSDLINFO, *PDMSDLINFO;

// Package db record
typedef struct {
    TCHAR    wzPackageID[DEVICE_ID_LEN];
    TCHAR    wzShortText[PACKAGE_SHORT_TEXT_LEN];
    TCHAR    wzLongText [PACKAGE_LONG_TEXT_LEN];
    TCHAR    wzAppName  [PACKAGE_APPNAME_TEXT_LEN];
    DWORD    dwPackageSize;         // in bytes
    DWORD    dwState;
    DMSDLINFO dlInfo;
    DWORD    dwAux;                 // reserved
} DMSINVRECORD, *PDMSINVRECORD;


// Package db record -- id and state only
typedef struct {
    TCHAR wzPackageID[DEVICE_ID_LEN];
    DWORD dwState;
} DMSINVPKGID, *PDMSINVPKGID;


// Communication between engine and apps
#define WM_REFRESH_DMSDATA          0xA001

// wParam (magic number)
#define DMS_CALLBACK_MAGIC          0x22480EE4

// lParam (update info)

// task list changed
#define DMS_REFRESH_TASKS           0x00000001

// database changed
#define DMS_REFRESH_DB              0x00000002
#define DMS_REFRESH_DB_ADD          0x00000012
#define DMS_REFRESH_DB_REMOVE       0x00000022
#define DMS_REFRESH_DB_DL_FAIL      0x00000042

//
// API prototypes
//
//---------------------------------------------------------------------------
HRESULT WINAPI CeSetDevMgmtTask
(
    PDMTASKINFO pdi
); 


//---------------------------------------------------------------------------
HRESULT WINAPI CeGetDevMgmtPackageList    
(
    PDMSINVRECORD   pRecord,
    LPDWORD         pdwEntryNumber,
    DWORD           dwFilter
);

//---------------------------------------------------------------------------
HRESULT WINAPI CeSetDevMgmtPackageList    
(
    PDMSINVRECORD   pRecord,
    DWORD           dwFlags
);

//---------------------------------------------------------------------------
HANDLE WINAPI CeStartDevMgmtService
(
    int     nAction,
#ifdef __cplusplus
    HWND    hCallbackWnd = NULL
#else
    HWND    hCallbackWnd
#endif
);

typedef HANDLE  (*PFNCESTARTDEVMGMTSERVICE) (int nAction, HWND hWnd);
typedef HRESULT (*PFNCESETDEVMGMTTASK) (PDMTASKINFO pti);
typedef HRESULT (*PFNCEGETDEVMGMTPACKAGELIST) (PDMSINVRECORD, LPDWORD, DWORD);
typedef HRESULT (*PFNCESETDEVMGMTPACKAGELIST) (PDMSINVRECORD, DWORD);


//
// Prototypes for functions supported by Bin handler DLL
//
//---------------------------------------------------------------------------
typedef BOOL (*PFNPROCESSDOWNLOADBEGIN)(void);
typedef BOOL (*PFNPROCESSDOWNLOADEND)(void);

#ifdef __cplusplus
}
#endif

#endif // _DEVMGMT_H_
