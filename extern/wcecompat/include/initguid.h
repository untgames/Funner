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
/*****************************************************************************\
*                                                                             *
* initguid.h -  Definitions for controlling GUID initialization               *
*                                                                             *
*               OLE Version 2.0                                               *
*                                                                             *
*                                                                             *
\*****************************************************************************/

// Include after compobj.h to enable GUID initialization.  This
//              must be done once per exe/dll.
//
// After this file, include one or more of the GUID definition files.
//
// NOTE: ole2.lib contains references to all GUIDs defined by OLE.

#ifndef DEFINE_GUID
#error initguid: must include objbase.h first.
#endif

#undef DEFINE_GUID

#ifdef _MAC
#define __based(a)
#endif

#ifdef _WIN32
#define __based(a)
#endif

#ifdef __TURBOC__
#define __based(a)
#endif

#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    EXTERN_C const GUID __based(__segname("_CODE")) name \
                    = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
