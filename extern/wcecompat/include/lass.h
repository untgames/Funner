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

#ifndef LASS_H
#define LASS_H

#ifdef __cplusplus 
extern "C" { 
#endif 

//Acronyms:
// (LASS)- Local Authentication Sub System
// (LAP) Local Authentication Plugin



//
//  Config Info
//  Per Authentication Event.
// AEFrequencyType
//

#define FT_Every_FV 0  // Every  FrequencyValue Time. set to 0 to validate every time.
#define FT_Minutes_Idle  1// Minites idle -- Not Yet Supported
#define FT_Minutes_Since_Any_AE_Verified  2 // Minites since Any AE returned from VerifyUser Succesfully
#define FT_Minutes_Since_This_AE_Verified 3 // Minites since This AE returned from VerifyUser Succesfully.


typedef struct  _AuthenticationEvent 
{
    GUID AEKey; /*Key to make all entries unique.*/
    LPWSTR Name; /*Friendly Name*/
    int  FrequencyType; /*Tells us how to interpret FrequencyValue*/
    int  FrequencyValue;   /* Used with FrequencyType */
} AuthenticationEvent;


//
//
// Some authentication Events are pre-defined - and should have special handlers in 
// The LAPs 
// Examples: 
//  



//
//  LASS Exports: 
//




// VerifyUser
// 
//  returns
//     TRUE ->  User Verified.
//     FALSE -> User not verified. GetLastError contains error.
// GetLastError () returns 
//  ERROR_CANCELLED <= The user cancelled the verification Request.
//  ERROR_WRONG_PASSWORD <= Generic code for user failed authentication. If more applicable 
                        //  code the LAP  will set it.

// Flags for dwOptions feild in Verify User.
#define VU_NO_UI  0x1      /*Return true only if a call to the LAP isn't necassary.*/
#define VU_UNTIL_SUCCESS  0x2 /* Verify User will return only if user clikced cancel, or got password correct.*/
#define VU_HANDLE_MASTER_KEY  0x4 /* Handle MasterKey Encryption/Decryption if possible. */

BOOL VerifyUser(const GUID *AEKey, /* Authenication Event Identifier */
                LPCWSTR wszAEDisplayText, /*Text Plugin will display, if null use from registry.*/
                HWND   hWndParent, /*Parent Window if Available-else use desktop window*/
                DWORD  dwOptions, /*Bitmask of possible options.*/
                PVOID pExtended /*Reserved, must be 0*/
                );

//
// Example 1: 
//     An app that wished to authenticate the user before calling a Secure Function.
//
// APP A:
// HWND hMyWindow;
// VerifyAndCallSecureFunction () 
// {
//    DWORD LastError;
//    GUID AEKeyForFoo = ...;
//
//    if (VerifyUser(&AEKeyForFoo,"App A",hMyWindow,VU_UNTIL_SUCCESS,0))  //  Call into LASS; This will  
//          CallSecureFunction()                //  call into the active LAP 
//                                              //  and show LAP specific UI.
//     else 
//          TellUserVerificationFailed(GetLastError()));  // <- display your own UI
// }
//
//


// Must be trusted to call this.
// Causes the LASS to re-read its configuration settings.
BOOL  LASSReloadConfig();

// Causes the LAP to enroll.
// Must be trusted to call this.
BOOL CreateEnrollmentConfigDialog(HWND hParentWindow);





// Paths used during installation - may move to another location.
//////////////////////////////////////////////////////////////////////////////
//  LAP Installation Information. - move to lap.h
//////////////////////////////////////////////////////////////////////////////
// Paths used during installation - may move to another location.
#define REG_LAP_ROOT L"Comm\\Security\\LASSD\\LAP"
#define REG_LOCKOUT_ROOT L"Comm\\Security\\LASSD\\LockOut"
#define REG_AE_ROOT L"Comm\\Security\\LASSD\\AE"
#define REG_LAP_ACTIVE_VALUE L"ActiveLap"

#define REG_AE_FRIENDLY_NAME_VALUE L"FriendlyName"
#define REG_AE_DISPLAY_TEXT_VALUE  L"DisplayText"
#define REG_AE_AUTH_FREQ_VALUE L"AEFrequencyValue"
#define REG_AE_AUTH_FREQ_TYPE L"AEFrequencyType"


#define REG_LOCKOUT_THRESHOLD L"Theshold"
#define REG_LOCKOUT_INITIAL_PENALTY L"InitialPenalty"
#define REG_LOCKOUT_INCRIMENTAL_PENALTY  L"IncrementalPenalty"

#ifdef __cplusplus 
}
#endif 

#endif //#ifndef LASS_H

