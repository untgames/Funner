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
// --------------------------------------------------------------------------

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    GUID guidDevClass;          // the device interface id for this notification
    DWORD dwReserved;           // do not use this
    BOOL fAttached;             // true if the di is present, false otherwise
    int cbName;                 // byte count of the interface's name
    TCHAR szName[1];            // beginning of the interface's name
} DEVDETAIL, *PDEVDETAIL;

// Maximum size of a device interface name. The choice of value is arbitrary
// but necessary for componenents that want to browse available interfaces
// so that they can set up their message queues. This value does not include
// the (required) terminating zero - that's already counted in DEVDETAIL.
#define MAX_DEVCLASS_NAMELEN 64

// Indicates an ordinary "stream" interface: open/read/write/iocontrol/close.
// Devices that do not specify anything else and which expose a "ABCN:"
// type of name automatically generate a notification with this GUID and
// and a name equal to the device name (e.g., "FOO2:").
#define DEVCLASS_STREAM_STRING TEXT("{f8a6ba98-087a-43ac-a9d8-b7f13c5bae31}")
#define DEVCLASS_STREAM_GUID { 0xf8a6ba98, 0x087a, 0x43ac, { 0xa9, 0xd8, 0xb7, 0xf1, 0x3c, 0x5b, 0xae, 0x31} }

#ifndef GUID_ISEQUAL
#define GUID_ISEQUAL(a,b) (memcmp(&(a),&(b),sizeof(a)) == 0)
#endif

#ifdef __cplusplus
}
#endif

