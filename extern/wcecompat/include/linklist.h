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
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Module Name:  

linklist.h

Abstract:  

Macros for linked-list manipulation.

Notes: 


--*/


#ifndef _LINKLIST_H_
#define _LINKLIST_H_

#ifdef __cplusplus
extern "C" {
#endif


/*NOINC*/

#if !defined(WIN32)

typedef struct _LIST_ENTRY {
    struct _LIST_ENTRY FAR * Flink;
    struct _LIST_ENTRY FAR * Blink;
} LIST_ENTRY;
typedef LIST_ENTRY FAR * PLIST_ENTRY;

#endif  // !WIN32

//
// Linked List Manipulation Functions - from NDIS.H
//

//
// Calculate the address of the base of the structure given its type, and an
// address of a field within the structure. - from NDIS.H
//
#ifndef CONTAINING_RECORD
#define CONTAINING_RECORD(address, type, field) ((type *)( \
                          (LPBYTE)(address) - \
                          (LPBYTE)(&((type *)0)->field)))
#endif  // CONTAINING_RECORD

//
//  Doubly-linked list manipulation routines.  Implemented as macros
//
#define InitializeListHead(ListHead) \
    ((ListHead)->Flink = (ListHead)->Blink = (ListHead) )

#define IsListEmpty(ListHead) \
    (( ((ListHead)->Flink == (ListHead)) ? TRUE : FALSE ) )

#define RemoveHeadList(ListHead) \
    (ListHead)->Flink;\
    {\
    PLIST_ENTRY FirstEntry;\
    FirstEntry = (ListHead)->Flink;\
    FirstEntry->Flink->Blink = (ListHead);\
    (ListHead)->Flink = FirstEntry->Flink;\
    }

#define RemoveEntryList(Entry) do {\
    PLIST_ENTRY _EX_Entry;\
    _EX_Entry = (Entry);\
    _EX_Entry->Blink->Flink = _EX_Entry->Flink;\
    _EX_Entry->Flink->Blink = _EX_Entry->Blink;\
    } while(0);

_inline
PLIST_ENTRY
RemoveTailList(PLIST_ENTRY ListHead)
{
    PLIST_ENTRY _Tail_Entry;
    _Tail_Entry = ListHead->Blink;
    RemoveEntryList(_Tail_Entry);
    return _Tail_Entry;
}

#define InsertTailList(_ListHead,_Entry) do {\
      PLIST_ENTRY _EX_ListHead = _ListHead; \
      PLIST_ENTRY _EX_Blink = _EX_ListHead->Blink; \
      (_Entry)->Flink = _EX_ListHead; \
      (_Entry)->Blink = _EX_Blink; \
      _EX_Blink->Flink = _Entry; \
      _EX_ListHead->Blink = _Entry; \
    } while(0);

#define InsertHeadList(_ListHead,_Entry) do {\
      PLIST_ENTRY _EX_ListHead = _ListHead; \
      PLIST_ENTRY _EX_Flink = _EX_ListHead->Flink; \
      (_Entry)->Flink = _EX_Flink; \
      (_Entry)->Blink = _EX_ListHead; \
      _EX_Flink->Blink = _Entry; \
      _EX_ListHead->Flink = _Entry; \
    } while (0);

/*INC*/

#ifdef __cplusplus
}
#endif


#endif  // _LINKLIST_H_
