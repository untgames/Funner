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


Module Name: windows.h

Purpose: Master include file for Windows applications.

--*/

#ifndef __WINDOWS__
#define __WINDOWS__

#include <windef.h>
#include <types.h>
#include <winbase.h>
#include <wingdi.h>
#include <winuser.h>
#include <winreg.h>
#include <shellapi.h>
#if !defined(WINCEMACRO) && !defined(WIN32_NO_OLE)
#include <ole2.h>
#endif

#include <imm.h>

#include <tchar.h>
#include <excpt.h>

// On CE force the functions to be linked instead of inline and
// turn off auto deprecate
#ifdef UNDER_CE
#define STRSAFE_LIB
#define STRSAFE_NO_DEPRECATE
#endif
#include <strsafe.h>

#endif /* __WINDOWS__ */

