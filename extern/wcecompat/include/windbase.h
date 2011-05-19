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
/**


 Module: windbase.h

 Purpose: Master include file for WINCE Database APIs

**/

// @doc OBJSTORE
/*
@topic Windows CE Object Store |
    The Windows CE object store has 3 parts - a registry API, a file system API and a database API. 
    <nl>The standard Win32 API's supported by the registry are:
    <nl>RegCloseKey 
    <nl>RegCreateKeyEx
    <nl>RegDeleteKey
    <nl>RegDeleteValue
    <nl>RegEnumValue
    <nl>RegEnumKeyEx
    <nl>RegOpenKeyEx
    <nl>RegQueryInfoKey
    <nl>RegQueryValueEx
    <nl>RegSetValueEx

    The standard Win32 API's supported by the filesytem are:
    <nl>CreateDirectory
    <nl>RemoveDirectory
    <nl>MoveFile
    <nl>CopyFile
    <nl>DeleteFile
    <nl>GetFileAttributes
    <nl>FindFirstFile
    <nl>CreateFileW
    <nl>ReadFile
    <nl>WriteFile
    <nl>GetFileSize
    <nl>SetFilePointer
    <nl>GetFileInformationByHandle
    <nl>FlushFileBuffers
    <nl>GetFileTime
    <nl>SetFileTime
    <nl>SetEndOfFile
    <nl>FindClose
    <nl>FindNextFile

    In addition, the following additional filesystem call is available:
    <nl>CreateContainer

    The various functions and data structures are described in this
    document.
*/

#ifndef __WINDBASE__
#define __WINDBASE__

// @CESYSGEN IF CE_MODULES_FILESYS

/*
@type CEOID | Unique identifier for all WINCE objects
@comm Every WINCE object can be efficiently referred to by its OID. OID's are unique
      in the system and are not reused 
*/
typedef DWORD CEOID;
typedef CEOID *PCEOID;

typedef struct  _CEGUID {
    DWORD Data1;
    DWORD Data2;
    DWORD Data3;
    DWORD Data4;
} CEGUID, *PCEGUID;


#ifndef WM_DBNOTIFICATION
#define WM_DBNOTIFICATION 0x03FD
#else
ERRFALSE(WM_DBNOTIFICATION == 0x03FD);
#endif

#define CEDB_EXNOTIFICATION 0x00000001
typedef struct _CENOTIFYREQUEST {
    DWORD dwSize;   // must be set to the structure size
    HWND  hwnd;     // window handle for notifications to be posted
    DWORD dwFlags;
    HANDLE hHeap;   // heap from which to allocate EX-NOTIFICATIONS
    DWORD  dwParam;
} CENOTIFYREQUEST, *PCENOTIFYREQUEST;


typedef struct _CENOTIFICATION {
    DWORD dwSize;
    DWORD dwParam;
    UINT  uType;
    CEGUID guid;
    CEOID  oid;
    CEOID  oidParent;
} CENOTIFICATION, *PCENOTIFICATION;

// @CESYSGEN IF FILESYS_FSMAIN

// @struct CEFILEINFO | Contains information about a file object    
typedef struct _CEFILEINFO {
    DWORD    dwAttributes;         //@field File attributes
    CEOID    oidParent;            //@field CEOID of parent directory
    WCHAR    szFileName[MAX_PATH]; //@field Full path name of the file
    FILETIME ftLastChanged;        //@field Time stamp of last change
    DWORD    dwLength;             //@field Length of file
} CEFILEINFO, *PCEFILEINFO;

//@struct CEDIRINFO | Contains information about a directory object    
typedef struct _CEDIRINFO {
    DWORD dwAttributes;        //@field Directory attributes
    CEOID oidParent;           //@field CEOID of parent directory
    WCHAR szDirName[MAX_PATH]; //@field Full path name of the directory
} CEDIRINFO, *PCEDIRINFO;

/*
@msg DB_CEOID_CREATED | Msg sent on creation of new oid
@comm WParam == CEOID modified
      LParam == CEOID's parent CEOID
@xref <f CeRegisterReplNotification>      
*/
#define DB_CEOID_CREATED            (WM_USER + 0x1)
/*
@msg DB_CEOID_DATABASE_DELETED | Msg sent on deletion of database
@comm WParam == CEOID modified
      LParam == CEOID's parent CEOID
@xref <f CeRegisterReplNotification>      
*/
#define DB_CEOID_DATABASE_DELETED   (WM_USER + 0x2)
/*
@msg DB_CEOID_RECORD_DELETED | Msg sent on deletion of record
@comm WParam == CEOID modified
      LParam == CEOID's parent CEOID
@xref <f CeRegisterReplNotification>      
*/
#define DB_CEOID_RECORD_DELETED     (WM_USER + 0x3)
/*
@msg DB_CEOID_FILE_DELETED | Msg sent on deletion of file
@comm WParam == CEOID modified
      LParam == CEOID's parent CEOID
@xref <f CeRegisterReplNotification>      
*/
#define DB_CEOID_FILE_DELETED       (WM_USER + 0x4)
/*
@msg DB_CEOID_DIRECTORY_DELETED | Msg sent on deletion of directory
@comm WParam == CEOID modified
      LParam == CEOID's parent CEOID
@xref <f CeRegisterReplNotification>      
*/
#define DB_CEOID_DIRECTORY_DELETED  (WM_USER + 0x5)
/*
@msg DB_CEOID_CHANGED | Msg sent on item modification
@comm WParam == CEOID modified
      LParam == CEOID's parent CEOID
@xref <f CeRegisterReplNotification>      
*/
#define DB_CEOID_CHANGED            (WM_USER + 0x6)

//--------------------------------------------------------------------------
// New for EDB
#ifdef EDB
#define DB_NOTIFY_VOLUME_CHANGED               (WM_USER+0x8)
#define DB_NOTIFY_RECORD_CREATED               (DB_CEOID_CREATED)
#define DB_NOTIFY_RECORD_CHANGED               (DB_CEOID_CHANGED)
#define DB_NOTIFY_RECORD_DELETED               (DB_CEOID_RECORD_DELETED)
#endif

// flags for CeGetReplChangeMask
#define REPL_CHANGE_WILLCLEAR   0x00000001

typedef struct STORE_INFORMATION {
    DWORD dwStoreSize;
    DWORD dwFreeSize;
} STORE_INFORMATION, *LPSTORE_INFORMATION;

BOOL GetStoreInformation (LPSTORE_INFORMATION lpsi);

// @CESYSGEN ENDIF

// @CESYSGEN IF FILESYS_FSDBASE
// @CESYSGEN ENDIF


// @CESYSGEN IF FILESYS_FSMAIN

/*
@struct CEOIDINFO | Contains information about a WINCE object
@field WORD | wObjType | Type of object
   @flag   OBJTYPE_INVALID   | There was no valid object with this CEOID
   @flag   OBJTYPE_FILE      | The object is a file
   @flag   OBJTYPE_DIRECTORY | The object is a directory
   @flag   OBJTYPE_DATABASE  | The object is a database
   @flag   OBJTYPE_RECORD    | The object is a record inside a database
@field <lt>SeeBelow<gt> | <lt>CEOIDINFOUNIONref<gt> | Note: The remaining members form a union
@field CEFILEINFO   | infFile      | Valid for file objects
@field CEDIRINFO    | infDirectory | Valid for directory objects
@field CEDBASEINFO  | infDatabase  | Valid for database objects
@field CERECORDINFO | infRecord    | Valid for record objects
@xref   <t CEFILEINFO>  <t CEDIRINFO> <t CEDBASEINFO>  <t CERECORDINFO>
*/
#define OBJTYPE_INVALID       0
#define OBJTYPE_FILE             1
#define OBJTYPE_DIRECTORY   2
#define OBJTYPE_DATABASE    3
#define OBJTYPE_RECORD        4
#define OBJTYPE_EDB         5

typedef struct _CEOIDINFO {
    WORD  wObjType;     //Type of object
            //        OBJTYPE_INVALID   | There was no valid object with this CEOID
            //        OBJTYPE_FILE      | The object is a file
            //        OBJTYPE_DIRECTORY | The object is a directory
            //        OBJTYPE_DATABASE  | The object is a database
            //        OBJTYPE_RECORD    | The object is a record inside a database
    WORD   wPad;        // dword alignment            
    union {             //This is a union 
        CEFILEINFO  infFile;           //Valid for file objects
        CEDIRINFO   infDirectory;      //Valid for directory objects
// @CESYSGEN IF FILESYS_FSDBASE
// @CESYSGEN ENDIF
    };
} CEOIDINFO, *PCEOIDINFO;

#ifndef EDB
#define CEOIDINFOEX_VERSION 1
#else
#define CEOIDINFOEX_VERSION 2
#endif
/*
@struct CEOIDINFOEX | Contains extended information about a WINCE object
@field WORD | wObjType | Type of object
   @flag   OBJTYPE_INVALID   | There was no valid object with this CEOID
   @flag   OBJTYPE_FILE      | The object is a file
   @flag   OBJTYPE_DIRECTORY | The object is a directory
   @flag   OBJTYPE_DATABASE  | The object is a database
   @flag   OBJTYPE_RECORD    | The object is a record inside a database
@field <lt>SeeBelow<gt> | <lt>CEOIDINFOUNIONref<gt> | Note: The remaining members form a union
@field CEFILEINFO   | infFile      | Valid for file objects
@field CEDIRINFO    | infDirectory | Valid for directory objects
@field CEDBASEINFO  | infDatabase  | Valid for database objects
@field CERECORDINFO | infRecord    | Valid for record objects
@xref   <t CEFILEINFO>  <t CEDIRINFO> <t CEDBASEINFO>  <t CERECORDINFO>
*/
typedef struct _CEOIDINFOEX {
    WORD  wVersion;    //@field Version of this structure
    WORD  wObjType;    //@field Type of object
                //@flag OBJTYPE_INVALID   | There was no valid object with this CEOID
                //@flag OBJTYPE_FILE      | The object is a file
                //@flag OBJTYPE_DIRECTORY | The object is a directory
                //@flag OBJTYPE_DATABASE  | The object is a database
                //@flag OBJTYPE_RECORD    | The object is a record inside a database
    union {
        CEFILEINFO    infFile;       // Valid for file objects
        CEDIRINFO     infDirectory;  // Valid for directory objects
// @CESYSGEN IF FILESYS_FSDBASE
// @CESYSGEN ENDIF
    };
} CEOIDINFOEX,  *PCEOIDINFOEX;

// Functions
BOOL CeOidGetInfoEx2 (PCEGUID pguid, CEOID oid, CEOIDINFOEX *oidInfo);
BOOL CeOidGetInfoEx (PCEGUID pguid, CEOID oid, CEOIDINFO *oidInfo);
BOOL CeOidGetInfo (CEOID oid, CEOIDINFO *oidInfo);

// @CESYSGEN IF SQLCE_MODULES_EDB
// @CESYSGEN ENDIF

// @CESYSGEN ENDIF

#ifdef WINCEOEM
#include <pwindbas.h>   // internal defines 
#ifdef WINCEMACRO
#include <mwindbas.h>
#endif
#endif

// @CESYSGEN ENDIF

#endif

