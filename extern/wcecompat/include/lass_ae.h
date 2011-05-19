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

    lass_ae.h

Abstract:

    This file defines the GUIDs  for CE OS Authentication Events (AEs).
    Policy for these GUIDs  will be found in the registry under:

    [HKEY_LOCAL_MACHINE\Comm\Security\LASSD\AE\GUID_NAME_HERE]
    ;  Authentication info here.
    
Environment:

    User Mode - Win32

--*/

#ifndef __LASS_AE_H
#define  __LASS_AE_H


//
// Pre-defined AE's
//

// AE_EVERY_TIME - will authenticate on every call.
// {8E6B0AF7-9295-45b4-8B73-55D95304144E}
static const GUID AE_EVERY_TIME = 
{ 0x8e6b0af7, 0x9295, 0x45b4, { 0x8b, 0x73, 0x55, 0xd9, 0x53, 0x4, 0x14, 0x4e } };

// AE_DEFAULT_POLICY - useful when your app wants to 
// piggyback on the default policy.
// {3C2424CC-95D3-4bea-BEB1-ABBBCF15DA96}
static const GUID AE_DEFAULT_POLICY = 
{ 0x3c2424cc, 0x95d3, 0x4bea, { 0xbe, 0xb1, 0xab, 0xbb, 0xcf, 0x15, 0xda, 0x96 } };


// Useful for testing - 1.
// {7B6A373C-F16E-4051-8F47-25EFF6F0D532}
static const GUID AE_TEST1 = 
{ 0x7b6a373c, 0xf16e, 0x4051, { 0x8f, 0x47, 0x25, 0xef, 0xf6, 0xf0, 0xd5, 0x32 } };

// Useful for testing - 2 .
// {578DCD29-9596-42e6-98C3-188C75BB4C04}
static const GUID AE_TEST2 = 
{ 0x578dcd29, 0x9596, 0x42e6, { 0x98, 0xc3, 0x18, 0x8c, 0x75, 0xbb, 0x4c, 0x4 } };


//
// We might want to move the AEs for OS components to another file.
//

// {1B38A686-7CDC-4017-A5E9-4C6A9DD7E5BA}
static const GUID AE_STARTUI = 
{ 0x1b38a686, 0x7cdc, 0x4017, { 0xa5, 0xe9, 0x4c, 0x6a, 0x9d, 0xd7, 0xe5, 0xba } };

// {8FD146DC-623E-499d-AA82-50F31454A161}
static const GUID AE_CPLMAIN = 
{ 0x8fd146dc, 0x623e, 0x499d, { 0xaa, 0x82, 0x50, 0xf3, 0x14, 0x54, 0xa1, 0x61 } };

// You must have this set to authenticate on every call.
// {71F82FA4-8589-4516-A0D7-9B7FFCB0C313}
static const GUID AE_RELOAD_SETTINGS_ENCRYPT_MASTERKEY = 
{ 0x71f82fa4, 0x8589, 0x4516, { 0xa0, 0xd7, 0x9b, 0x7f, 0xfc, 0xb0, 0xc3, 0x13 } };
#endif // __LASS_AE_H


