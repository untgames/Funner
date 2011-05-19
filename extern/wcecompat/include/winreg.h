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
/*++ BUILD Version: 0001    // Increment this if a change has global effects


Module Name:

    Winreg.h

Abstract:

    This module contains the function prototypes and constant, type and
    structure definitions for the Windows 32-Bit Registry API.

--*/

#ifndef _WINREG_
#define _WINREG_


#ifdef _MAC
#include <macwin32.h>
#endif

// @CESYSGEN IF FILESYS_FSREG || FILESYS_FSREGHIVE || FILESYS_FSRGLITE

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WINVER
#define WINVER 0x0500   // version 5.0
#endif /* !WINVER */

//
// Requested Key access mask type.
//

typedef ACCESS_MASK REGSAM;

//
// Reserved Key Handles.
//

#define HKEY_CLASSES_ROOT           (( HKEY ) (ULONG_PTR)0x80000000 )
#define HKEY_CURRENT_USER           (( HKEY ) (ULONG_PTR)0x80000001 )
#define HKEY_LOCAL_MACHINE          (( HKEY ) (ULONG_PTR)0x80000002 )
#define HKEY_USERS                  (( HKEY ) (ULONG_PTR)0x80000003 )

//
// Default values for parameters that do not exist in the Win 3.1
// compatible APIs.
//

#define WIN31_CLASS                 NULL

//
// API Prototypes.
//


WINADVAPI
LONG
APIENTRY
RegCloseKey (
    __in __opt HKEY hKey
    );

WINADVAPI
LONG
APIENTRY
RegCreateKeyExA (
    __in __opt HKEY hKey,
    __in LPCSTR lpSubKey,
    __reserved DWORD Reserved,
    __in __opt LPSTR lpClass,
    __in DWORD dwOptions,
    __in REGSAM samDesired,
    __in __opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    __out PHKEY phkResult,
    __out __opt LPDWORD lpdwDisposition
    );
WINADVAPI
LONG
APIENTRY
RegCreateKeyExW (
    __in __opt HKEY hKey,
    __in LPCWSTR lpSubKey,
    __reserved DWORD Reserved,
    __in __opt LPWSTR lpClass,
    __in DWORD dwOptions,
    __in REGSAM samDesired,
    __in __opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    __out PHKEY phkResult,
    __out __opt LPDWORD lpdwDisposition
    );
#ifdef UNICODE
#define RegCreateKeyEx  RegCreateKeyExW
#else
#define RegCreateKeyEx  RegCreateKeyExA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegDeleteKeyA (
    __in __opt HKEY hKey,
    __in LPCSTR lpSubKey
    );
WINADVAPI
LONG
APIENTRY
RegDeleteKeyW (
    __in __opt HKEY hKey,
    __in LPCWSTR lpSubKey
    );
#ifdef UNICODE
#define RegDeleteKey  RegDeleteKeyW
#else
#define RegDeleteKey  RegDeleteKeyA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegDeleteValueA (
    __in __opt HKEY hKey,
    __in __opt LPCSTR lpValueName
    );
WINADVAPI
LONG
APIENTRY
RegDeleteValueW (
    __in __opt HKEY hKey,
    __in __opt LPCWSTR lpValueName
    );
#ifdef UNICODE
#define RegDeleteValue  RegDeleteValueW
#else
#define RegDeleteValue  RegDeleteValueA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegEnumKeyExA (
    __in __opt HKEY hKey,
    __in DWORD dwIndex,
    __out_ecount(*lpcchName) __opt LPSTR lpName,
    __inout LPDWORD lpcchName,
    __reserved LPDWORD lpReserved,
    __inout_ecount(*lpcchClass) __opt LPSTR lpClass,
    __inout __opt LPDWORD lpcchClass,
    __reserved PFILETIME lpftLastWriteTime
    );
WINADVAPI
LONG
APIENTRY
RegEnumKeyExW (
    __in __opt HKEY hKey,
    __in DWORD dwIndex,
    __out_ecount(*lpcchName) __opt LPWSTR lpName,
    __inout LPDWORD lpcchName,
    __reserved LPDWORD lpReserved,
    __inout_ecount(*lpcchClass) __opt LPWSTR lpClass,
    __inout __opt LPDWORD lpcchClass,
    __reserved PFILETIME lpftLastWriteTime
    );
#ifdef UNICODE
#define RegEnumKeyEx  RegEnumKeyExW
#else
#define RegEnumKeyEx  RegEnumKeyExA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegEnumValueA (
    __in __opt HKEY hKey,
    __in DWORD dwIndex,
    __out_ecount(*lpcchValueName) __opt LPSTR lpValueName,
    __inout LPDWORD lpcchValueName,
    __reserved LPDWORD lpReserved,
    __out __opt LPDWORD lpType,
    __out_bcount(*lpcbData) __opt LPBYTE lpData,
     __opt LPDWORD lpcbData
    );
WINADVAPI
LONG
APIENTRY
RegEnumValueW (
    __in __opt HKEY hKey,
    __in DWORD dwIndex,
    __out_ecount(*lpcchValueName) __opt LPWSTR lpValueName,
    __inout LPDWORD lpcchValueName,
    __reserved LPDWORD lpReserved,
    __out __opt LPDWORD lpType,
    __out_bcount(*lpcbData) __opt LPBYTE lpData,
     __opt LPDWORD lpcbData
    );
#ifdef UNICODE
#define RegEnumValue  RegEnumValueW
#else
#define RegEnumValue  RegEnumValueA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegFlushKey (
    __in HKEY hKey
    );

WINADVAPI
LONG
APIENTRY
RegOpenKeyExA (
    __in __opt HKEY hKey,
    __in __opt LPCSTR lpSubKey,
    __reserved DWORD ulOptions,
    __in REGSAM samDesired,
    __out PHKEY phkResult
    );
WINADVAPI
LONG
APIENTRY
RegOpenKeyExW (
    __in __opt HKEY hKey,
    __in __opt LPCWSTR lpSubKey,
    __reserved DWORD ulOptions,
    __in REGSAM samDesired,
    __out PHKEY phkResult
    );
#ifdef UNICODE
#define RegOpenKeyEx  RegOpenKeyExW
#else
#define RegOpenKeyEx  RegOpenKeyExA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegQueryInfoKeyA (
    __in __opt HKEY hKey,
    __out_ecount(*lpcchClass) __opt LPSTR lpClass,
    __inout __opt LPDWORD lpcchClass,
    __reserved LPDWORD lpReserved,
    __out __opt LPDWORD lpcSubKeys,
    __out __opt LPDWORD lpcbMaxSubKeyLen,
    __out __opt LPDWORD lpcbMaxClassLen,
    __out __opt LPDWORD lpcValues,
    __out __opt LPDWORD lpcbMaxValueNameLen,
    __out __opt LPDWORD lpcbMaxValueLen,
    __reserved LPDWORD lpcbSecurityDescriptor,
    __reserved PFILETIME lpftLastWriteTime
    );
WINADVAPI
LONG
APIENTRY
RegQueryInfoKeyW (
    __in __opt HKEY hKey,
    __out_ecount(*lpcchClass) __opt LPWSTR lpClass,
    __inout __opt LPDWORD lpcchClass,
    __reserved LPDWORD lpReserved,
    __out __opt LPDWORD lpcSubKeys,
    __out __opt LPDWORD lpcbMaxSubKeyLen,
    __out __opt LPDWORD lpcbMaxClassLen,
    __out __opt LPDWORD lpcValues,
    __out __opt LPDWORD lpcbMaxValueNameLen,
    __out __opt LPDWORD lpcbMaxValueLen,
    __reserved LPDWORD lpcbSecurityDescriptor,
    __reserved PFILETIME lpftLastWriteTime
    );
#ifdef UNICODE
#define RegQueryInfoKey  RegQueryInfoKeyW
#else
#define RegQueryInfoKey  RegQueryInfoKeyA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegQueryValueExA (
    __in __opt HKEY hKey,
    __in __opt LPCSTR lpValueName,
    __reserved LPDWORD lpReserved,
    __out __opt LPDWORD lpType,
    __out_bcount(*lpcbData) __opt LPBYTE lpData,
     __opt LPDWORD lpcbData
    );
WINADVAPI
LONG
APIENTRY
RegQueryValueExW (
    __in __opt HKEY hKey,
    __in __opt LPCWSTR lpValueName,
    __reserved LPDWORD lpReserved,
    __out __opt LPDWORD lpType,
    __out_bcount(*lpcbData) __opt LPBYTE lpData,
     __opt LPDWORD lpcbData
    );
#ifdef UNICODE
#define RegQueryValueEx  RegQueryValueExW
#else
#define RegQueryValueEx  RegQueryValueExA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegSetValueExA (
    __in __opt HKEY hKey,
    __in __opt LPCSTR lpValueName,
    __reserved DWORD Reserved,
    __in DWORD dwType,
    __in_bcount(cbData) __opt CONST BYTE* lpData,
    __in DWORD cbData
    );

WINADVAPI
LONG
APIENTRY
RegSetValueExW (
    __in __opt HKEY hKey,
    __in __opt LPCWSTR lpValueName,
    __reserved DWORD Reserved,
    __in DWORD dwType,
    __in_bcount(cbData) __opt CONST BYTE* lpData,
    __in DWORD cbData
    );
#ifdef UNICODE
#define RegSetValueEx  RegSetValueExW
#else
#define RegSetValueEx  RegSetValueExA
#endif // !UNICODE

WINADVAPI
HANDLE  
APIENTRY
CeFindFirstRegChange(
    __in HKEY hKey, 
    __in BOOL bWatchSubTree, 
    __in __opt DWORD dwNotifyFilter
    );

WINADVAPI
BOOL    
APIENTRY
CeFindNextRegChange(
    __in __notnull HANDLE hNotify
    );

WINADVAPI
BOOL    
APIENTRY
CeFindCloseRegChange(
    __in __notnull HANDLE hNotify
    );


#ifdef __cplusplus
}
#endif

#ifdef WINCEOEM
#include <pwinreg.h>
#ifdef WINCEMACRO
#include <mwinreg.h>    // internal defines 
#endif
#endif

// @CESYSGEN ENDIF

#endif // _WINREG_

