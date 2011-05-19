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

//
// This routine contains definitions and APIs for communicating with a platform-specific
// power policy manager.
//

#pragma once

#include <msgqueue.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// notification messages
#define PPN_REEVALUATESTATE             0x0001  // dwData is reserved, use 0
#define PPN_POWERCHANGE                 0x0002  // dwData is reserved, use 0
#define PPN_UNATTENDEDMODE              0x0003  // dwData is TRUE or FALSE
#define PPN_SUSPENDKEYPRESSED           0x0004  // dwData is reserved. use 0
#define PPN_SUSPENDKEYRELEASED          0x0005  // dwData is reserved. use 0

// OEMs can define values higher than this (as PPN_OEMBASE+0, +1, etc.)
#define PPN_OEMBASE                     0x10000

// This routine notifies the power policy manager of events it needs in order to
// implement the OEM's power policy.  The dwMessage parameter is one of the PPN_
// values (or an OEM-defined one).  The dwData parameter is a 32-bit value whose
// interpretation varies with the notification message.  A return value of TRUE
// indicates success.
BOOL PowerPolicyNotify(DWORD dwMessage, DWORD dwData);

#ifdef __cplusplus
}
#endif  // __cplusplus

