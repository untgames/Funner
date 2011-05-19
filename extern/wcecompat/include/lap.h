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
#ifndef LAP_H
#define LAP_H

#ifdef __cplusplus 
extern "C" { 
#endif 

#include <windows.h>
#include <lass.h>

//
// All Local Authentication Plugin (LAP) must  implement the following interface:
//


// Support masterkey protection sementics
#define LAP_CAPABILITIES_MASTER_KEY 0x1 

typedef struct _InitLap
{
    DWORD size; // Used for version info contains size of this struct.
    DWORD capabilities; // Expect LAP will fill in with the LAP_CAPABILITIES_* it supports.
} InitLap;


// Sent to a LAP when it is activated.
//  Allows LAP to indicate its capabilities.
BOOL InitLAP(
        InitLap *il
        );


// Sent to a LAP when it is being unloaded.
void DeinitLAP();


//Tells the lap to verify the user.
#if 0
// From lass.h
BOOL VerifyUser(const GUID *AEKey, /* Authenication Event Identifier */
                LPCWSTR pwszAEDisplayText, /*Text Plugin will display, if null use from registry.*/
                HWND   hWndParent, /*Parent Window if Available-else use desktop window*/
                DWORD  dwOptions, /*Bitmask of possible options.*/
                PVOID pExtended /*Reserved, must be 0*/
                );

#endif 

// Causes the LAP to enroll.
// App calls CreateEnrollmentConfigDialog and then LASS will call LAPCreateEnrollmentConfigDialog
BOOL LAPCreateEnrollmentConfigDialog(HWND hParentWindow,DWORD dwOptions);

#ifdef __cplusplus 
}
#endif 

#endif  // LAP_H
