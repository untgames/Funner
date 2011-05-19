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
* SpecStrings.h - additional markers for documenting the semantics of APIs    *
*                                                                             *
* Version 1.0                                                                 *
*                                                                             *
* Copyright (c) Microsoft Corporation. All rights reserved.                   *
*                                                                             *
\*****************************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif  // #if _MSC_VER > 1000

#define __null
#define __notnull
#define __maybenull
#define __reserved
#define __readonly
#define __notreadonly
#define __maybereadonly
#define __valid
#define __novalid
#define __maybevalid
#define __checkReturn
#define __readableTo(size)
#define __writableTo(size)
#define __typefix(ctype)        
#define __deref
#define __pre
#define __post
#define __exceptthat
#define __override
#ifndef __fallthrough
    #define __fallthrough
#endif
#define __callback
#define __in
#define __out
#define __inout
#define __opt
#define __out_ecount(size)
#define __in_ecount(size)
#define __inout_ecount(size)
#define __out_bcount(size)
#define __in_bcount(size)
#define __inout_bcount(size)
#define __format_string
#define __blocksOn(resource)


