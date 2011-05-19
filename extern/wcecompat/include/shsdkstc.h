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
#ifndef _SHSDKSTC_H_
#define _SHSDKSTC_H_


#ifdef __cplusplus
extern "C" {
#endif

typedef struct _FILECHANGEINFO {
	DWORD cbSize;              //  Size of this structure
	LONG wEventId;             //  File System Notification flags
	ULONG uFlags;              //  Flags
	DWORD dwItem1;            
	DWORD dwItem2;
	DWORD dwAttributes;        //  File attributes
	FILETIME ftModified;       //  Modified time
	ULONG nFileSize;           
} FILECHANGEINFO, * LPFILECHANGEINFO;
typedef const FILECHANGEINFO * LPCFILECHANGEINFO;


typedef struct tagFILECHANGENOTIFY {
    DWORD dwRefCount;
    FILECHANGEINFO fci;
} FILECHANGENOTIFY;

typedef struct tagSHCHANGENOTIFYENTRY {
    DWORD   dwEventMask;    // Events to watch
    LPTSTR  pszWatchDir;    // Directory or root for the events we want. NULL means all.
    BOOL    fRecursive;     // Indicates whether look just for pszWatchDir or recursively.
} SHCHANGENOTIFYENTRY;

#include <wtypes.h>

// notification priority
typedef enum _SHNP
{
    SHNP_INFORM = 0x1B1,      // bubble shown for duration, then goes away
    SHNP_ICONIC,              // no bubble, icon shown for duration then goes away
} SHNP;

// notification data

typedef struct _SHNOTIFICATIONDATA
{
    DWORD        cbStruct;     // for verification and versioning
    DWORD        dwID;         // identifier for this particular notification
    SHNP         npPriority;   // priority
    DWORD        csDuration;   // duration of the notification (usage depends on prio)
    HICON        hicon;        // the icon for the notification
    DWORD        grfFlags;     // flags
    CLSID        clsid;        // unique identifier for the notification class
    HWND         hwndSink;     // window to receive command choices, dismiss, etc.
    LPCTSTR      pszHTML;      // HTML content for the bubble
    LPCTSTR      pszTitle;     // Optional title for bubble
    LPARAM       lParam;       // User-defined parameter
} SHNOTIFICATIONDATA, *PSHNOTIFICATIONDATA;


#ifdef __cplusplus
}
#endif

#endif // _SHSDKSTC_H_
