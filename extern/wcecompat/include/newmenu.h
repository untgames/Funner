//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of your Microsoft Windows CE
// Source Alliance Program license form.  If you did not accept the terms of
// such a license, you are not authorized to use this source code.
//
// stolen from \public\shellw\sdk\inc

/*++

Module Name:

    newmenu.h

Abstract:

    New menu.

--*/

#pragma once

#include <windows.h>
#include <olectl.h>

//++++++
//
// New menu notifications
//

// get the application specific reg key for "new" menu items
#define  NMN_GETAPPREGKEY       1101
// Sent to app before shared new menu is destroyed.
#define  NMN_NEWMENUDESTROY     1102
// Sent to app before COM object is instantiated.
#define  NMN_INVOKECOMMAND      1103
// Sent to app when new button style changes
#define  NMN_NEWBUTTONUPDATED   1104
// Get the host application's property bag, if there is one, which will be 
// used to initialize the new object.
#define  NMN_GETPROPERTYBAG     1105

typedef struct tagNMNEWMENU 
{
    NMHDR hdr;
    TCHAR szReg[80];
    HMENU hMenu;
    CLSID clsid;
    IPropertyBag **pppropbag;
} NMNEWMENU, *PNMNEWMENU;

// For application added menu items.
#define IDM_NEWMENUMAX      3000

//
// End New menu notifications
//
//------
