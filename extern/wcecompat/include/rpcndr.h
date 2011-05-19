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

    rpcndr.h

Abstract:

    Definitions for stub data structures and prototypes of helper functions.

Environment:

    DOS, Win 3.X, and Win/NT.

--*/

#ifndef __RPCNDR_H_VERSION__
#define __RPCNDR_H_VERSION__        ( 450 )
#endif // __RPCNDR_H_VERSION__


#ifndef __RPCNDR_H__
#define __RPCNDR_H__

//
// Set the packing level for RPC structures for Dos, Windows and Mac.
//

#if defined(__RPC_DOS__) || defined(__RPC_WIN16__) || defined(__RPC_MAC__)
#pragma pack(2)
#endif

#if defined(__RPC_MAC__)
#define _MAC_
#endif

#include <rpcnsip.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************

     Network Computing Architecture (NCA) definition:

     Network Data Representation: (NDR) Label format:
     An unsigned long (32 bits) with the following layout:

     3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
     1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
    +---------------+---------------+---------------+-------+-------+
    |   Reserved    |   Reserved    |Floating point | Int   | Char  |
    |               |               |Representation | Rep.  | Rep.  |
    +---------------+---------------+---------------+-------+-------+

     Where

         Reserved:

             Must be zero (0) for NCA 1.5 and NCA 2.0.

         Floating point Representation is:

             0 - IEEE
             1 - VAX
             2 - Cray
             3 - IBM

         Int Rep. is Integer Representation:

             0 - Big Endian
             1 - Little Endian

         Char Rep. is Character Representation:

             0 - ASCII
             1 - EBCDIC

     The Microsoft Local Data Representation (for all platforms which are
     of interest currently is edefined below:

 ****************************************************************************/

#define NDR_CHAR_REP_MASK               (unsigned long)0X0000000FL
#define NDR_INT_REP_MASK                (unsigned long)0X000000F0L
#define NDR_FLOAT_REP_MASK              (unsigned long)0X0000FF00L

#define NDR_LITTLE_ENDIAN               (unsigned long)0X00000010L
#define NDR_BIG_ENDIAN                  (unsigned long)0X00000000L

#define NDR_IEEE_FLOAT                  (unsigned long)0X00000000L
#define NDR_VAX_FLOAT                   (unsigned long)0X00000100L
#define NDR_IBM_FLOAT                   (unsigned long)0X00000300L

#define NDR_ASCII_CHAR                  (unsigned long)0X00000000L
#define NDR_EBCDIC_CHAR                 (unsigned long)0X00000001L

#if defined(__RPC_MAC__)
#define NDR_LOCAL_DATA_REPRESENTATION   (unsigned long)0X00000000L
#define NDR_LOCAL_ENDIAN                NDR_BIG_ENDIAN
#else
#define NDR_LOCAL_DATA_REPRESENTATION   (unsigned long)0X00000010L
#define NDR_LOCAL_ENDIAN                NDR_LITTLE_ENDIAN
#endif


/****************************************************************************
 *  Macros for targeted platforms
 ****************************************************************************/
#if defined (UNDER_CE)
#define _WIN32_DCOM	1
#endif

#if (0x500 <= _WIN32_WINNT)
#define TARGET_IS_NT50_OR_LATER                   1
#else
#define TARGET_IS_NT50_OR_LATER                   0
#endif

#if (defined(_WIN32_DCOM) || 0x400 <= _WIN32_WINNT)
#define TARGET_IS_NT40_OR_LATER                   1
#else
#define TARGET_IS_NT40_OR_LATER                   0
#endif

#if (0x400 <= WINVER)
#define TARGET_IS_NT351_OR_WIN95_OR_LATER         1
#else
#define TARGET_IS_NT351_OR_WIN95_OR_LATER         0
#endif

/****************************************************************************
 *  Other MIDL base types / predefined types:
 ****************************************************************************/

#define small char
typedef unsigned char byte;
typedef unsigned char boolean;

#ifndef _HYPER_DEFINED
#define _HYPER_DEFINED

#if !defined(__RPC_DOS__) && !defined(__RPC_WIN16__) && !defined(__RPC_MAC__) && (!defined(_M_IX86) || (defined(_INTEGRAL_MAX_BITS) && _INTEGRAL_MAX_BITS >= 64))
#define  hyper           __int64
#define MIDL_uhyper  unsigned __int64
#else
typedef double  hyper;
typedef double MIDL_uhyper;
#endif

#endif // _HYPER_DEFINED

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

#ifndef _SIZE_T_DEFINED
typedef unsigned int size_t;
#define _SIZE_T_DEFINED
#endif

#ifdef __RPC_DOS__
#define __RPC_CALLEE       __far __pascal
#endif

#ifdef __RPC_WIN16__
#define __RPC_CALLEE       __far __pascal __export
#endif

#ifdef __RPC_WIN32__
#if   (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
#define __RPC_CALLEE       __stdcall
#else
#define __RPC_CALLEE
#endif
#endif

#ifdef __RPC_MAC__
#define __RPC_CALLEE __far
#endif

#ifndef __MIDL_USER_DEFINED
#define midl_user_allocate MIDL_user_allocate
#define midl_user_free     MIDL_user_free
#define __MIDL_USER_DEFINED
#endif

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void             __RPC_USER MIDL_user_free( void __RPC_FAR * );

#ifdef __RPC_WIN16__
#define RPC_VAR_ENTRY __export __cdecl
#else
#define RPC_VAR_ENTRY __cdecl
#endif


/* winnt only */
#if defined(_M_MRX000) || defined(_M_IX86) || defined(_M_ALPHA)
#define __MIDL_DECLSPEC_DLLIMPORT   __declspec(dllimport)
#define __MIDL_DECLSPEC_DLLEXPORT   __declspec(dllexport)
#else
#define __MIDL_DECLSPEC_DLLIMPORT
#define __MIDL_DECLSPEC_DLLEXPORT
#endif




/****************************************************************************
 * Context handle management related definitions:
 *
 * Client and Server Contexts.
 *
 ****************************************************************************/

typedef void __RPC_FAR * NDR_CCONTEXT;

typedef struct
    {
    void __RPC_FAR * pad[2];
    void __RPC_FAR * userContext;
    } __RPC_FAR * NDR_SCONTEXT;

#define NDRSContextValue(hContext) (&(hContext)->userContext)

#define cbNDRContext 20         /* size of context on WIRE */

typedef void (__RPC_USER __RPC_FAR * NDR_RUNDOWN)(void __RPC_FAR * context);

typedef struct _SCONTEXT_QUEUE {
    unsigned long   NumberOfObjects;
    NDR_SCONTEXT  * ArrayOfObjects;
    } SCONTEXT_QUEUE, __RPC_FAR * PSCONTEXT_QUEUE;

// @CESYSGEN IF DCOM_MODULES_RPCRT4
// @CESYSGEN ENDIF


/****************************************************************************
    NDR conversion related definitions.
 ****************************************************************************/

#define byte_from_ndr(source, target) \
    { \
    *(target) = *(*(char __RPC_FAR * __RPC_FAR *)&(source)->Buffer)++; \
    }

#define byte_array_from_ndr(Source, LowerIndex, UpperIndex, Target) \
    { \
    NDRcopy ( \
        (((char __RPC_FAR *)(Target))+(LowerIndex)), \
        (Source)->Buffer, \
        (unsigned int)((UpperIndex)-(LowerIndex))); \
    *(unsigned long __RPC_FAR *)&(Source)->Buffer += ((UpperIndex)-(LowerIndex)); \
    }

#define boolean_from_ndr(source, target) \
    { \
    *(target) = *(*(char __RPC_FAR * __RPC_FAR *)&(source)->Buffer)++; \
    }

#define boolean_array_from_ndr(Source, LowerIndex, UpperIndex, Target) \
    { \
    NDRcopy ( \
        (((char __RPC_FAR *)(Target))+(LowerIndex)), \
        (Source)->Buffer, \
        (unsigned int)((UpperIndex)-(LowerIndex))); \
    *(unsigned long __RPC_FAR *)&(Source)->Buffer += ((UpperIndex)-(LowerIndex)); \
    }

#define small_from_ndr(source, target) \
    { \
    *(target) = *(*(char __RPC_FAR * __RPC_FAR *)&(source)->Buffer)++; \
    }

#define small_from_ndr_temp(source, target, format) \
    { \
    *(target) = *(*(char __RPC_FAR * __RPC_FAR *)(source))++; \
    }

#define small_array_from_ndr(Source, LowerIndex, UpperIndex, Target) \
    { \
    NDRcopy ( \
        (((char __RPC_FAR *)(Target))+(LowerIndex)), \
        (Source)->Buffer, \
        (unsigned int)((UpperIndex)-(LowerIndex))); \
    *(unsigned long __RPC_FAR *)&(Source)->Buffer += ((UpperIndex)-(LowerIndex)); \
    }

/****************************************************************************
    Platform specific mapping of c-runtime functions.
 ****************************************************************************/

#ifdef __RPC_DOS__
#define MIDL_ascii_strlen(string) \
    _fstrlen(string)
#define MIDL_ascii_strcpy(target,source) \
    _fstrcpy(target,source)
#define MIDL_memset(s,c,n) \
    _fmemset(s,c,n)
#endif

#ifdef __RPC_WIN16__
#define MIDL_ascii_strlen(string) \
    _fstrlen(string)
#define MIDL_ascii_strcpy(target,source) \
    _fstrcpy(target,source)
#define MIDL_memset(s,c,n) \
    _fmemset(s,c,n)
#endif

#if defined(__RPC_WIN32__) || defined(__RPC_MAC__)
#define MIDL_ascii_strlen(string) \
    strlen(string)
#define MIDL_ascii_strcpy(target,source) \
    strcpy(target,source)
#define MIDL_memset(s,c,n) \
    memset(s,c,n)
#endif

/****************************************************************************
    Ndr Library helper function prototypes for MIDL 1.0 ndr functions.
 ****************************************************************************/

// @CESYSGEN IF DCOM_MODULES_RPCRT4
// @CESYSGEN ENDIF

/****************************************************************************
    MIDL 2.0 ndr definitions.
 ****************************************************************************/

typedef unsigned long error_status_t;

#define _midl_ma1( p, cast )    *(*( cast **)&p)++
#define _midl_ma2( p, cast )    *(*( cast **)&p)++
#define _midl_ma4( p, cast )    *(*( cast **)&p)++
#define _midl_ma8( p, cast )    *(*( cast **)&p)++

#define _midl_unma1( p, cast )  *(( cast *)p)++
#define _midl_unma2( p, cast )  *(( cast *)p)++
#define _midl_unma3( p, cast )  *(( cast *)p)++
#define _midl_unma4( p, cast )  *(( cast *)p)++

// Some alignment specific macros.


#define _midl_fa2( p )          (p = (RPC_BUFPTR )((unsigned long)(p+1) & 0xfffffffe))
#define _midl_fa4( p )          (p = (RPC_BUFPTR )((unsigned long)(p+3) & 0xfffffffc))
#define _midl_fa8( p )          (p = (RPC_BUFPTR )((unsigned long)(p+7) & 0xfffffff8))

#define _midl_addp( p, n )      (p += n)

// Marshalling macros

#define _midl_marsh_lhs( p, cast )  *(*( cast **)&p)++
#define _midl_marsh_up( mp, p )     *(*(unsigned long **)&mp)++ = (unsigned long)p
#define _midl_advmp( mp )           *(*(unsigned long **)&mp)++
#define _midl_unmarsh_up( p )       (*(*(unsigned long **)&p)++)


////////////////////////////////////////////////////////////////////////////
// Ndr macros.
////////////////////////////////////////////////////////////////////////////

#define NdrMarshConfStringHdr( p, s, l )    (_midl_ma4( p, unsigned long) = s, \
                                            _midl_ma4( p, unsigned long) = 0, \
                                            _midl_ma4( p, unsigned long) = l)

#define NdrUnMarshConfStringHdr(p, s, l)    ((s=_midl_unma4(p,unsigned long),\
                                            (_midl_addp(p,4)),               \
                                            (l=_midl_unma4(p,unsigned long))

#define NdrMarshCCtxtHdl(pc,p)  (NDRCContextMarshall( (NDR_CCONTEXT)pc, p ),p+20)

#define NdrUnMarshCCtxtHdl(pc,p,h,drep) \
        (NDRCContextUnmarshall((NDR_CONTEXT)pc,h,p,drep), p+20)

#define NdrUnMarshSCtxtHdl(pc, p,drep)  (pc = NdrSContextUnMarshall(p,drep ))


#define NdrMarshSCtxtHdl(pc,p,rd)   (NdrSContextMarshall((NDR_SCONTEXT)pc,p, (NDR_RUNDOWN)rd)

#define NdrFieldOffset(s,f)     (long)(& (((s __RPC_FAR *)0)->f))
#define NdrFieldPad(s,f,p,t)    (NdrFieldOffset(s,f) - NdrFieldOffset(s,p) - sizeof(t))

#if defined(__RPC_MAC__)
#define NdrFcShort(s)   (unsigned char)(s >> 8), (unsigned char)(s & 0xff)
#define NdrFcLong(s)    (unsigned char)(s >> 24), (unsigned char)((s & 0x00ff0000) >> 16), \
                        (unsigned char)((s & 0x0000ff00) >> 8), (unsigned char)(s & 0xff)
#else
#define NdrFcShort(s)   (unsigned char)(s & 0xff), (unsigned char)(s >> 8)
#define NdrFcLong(s)    (unsigned char)(s & 0xff), (unsigned char)((s & 0x0000ff00) >> 8), \
                        (unsigned char)((s & 0x00ff0000) >> 16), (unsigned char)(s >> 24)
#endif //  Mac

//
// On the server side, the following exceptions are mapped to
// the bad stub data exception if -error stub_data is used.
//

#define RPC_BAD_STUB_DATA_EXCEPTION_FILTER  \
                 ( (RpcExceptionCode() == STATUS_ACCESS_VIOLATION)  || \
                   (RpcExceptionCode() == STATUS_DATATYPE_MISALIGNMENT) || \
                   (RpcExceptionCode() == RPC_X_BAD_STUB_DATA) )

/////////////////////////////////////////////////////////////////////////////
// Some stub helper functions.
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// Stub helper structures.
////////////////////////////////////////////////////////////////////////////

struct _MIDL_STUB_MESSAGE;
struct _MIDL_STUB_DESC;
struct _FULL_PTR_XLAT_TABLES;

typedef unsigned char __RPC_FAR * RPC_BUFPTR;
typedef unsigned long             RPC_LENGTH;

// Expression evaluation callback routine prototype.
typedef void (__RPC_USER __RPC_FAR * EXPR_EVAL)( struct _MIDL_STUB_MESSAGE __RPC_FAR * );

typedef const unsigned char __RPC_FAR * PFORMAT_STRING;

/*
 * Multidimensional conformant/varying array struct.
 */
typedef struct
    {
    long                            Dimension;

    /* These fields MUST be (unsigned long *) */
    unsigned long __RPC_FAR *       BufferConformanceMark;
    unsigned long __RPC_FAR *       BufferVarianceMark;

    /* Count arrays, used for top level arrays in -Os stubs */
    unsigned long __RPC_FAR *       MaxCountArray;
    unsigned long __RPC_FAR *       OffsetArray;
    unsigned long __RPC_FAR *       ActualCountArray;
    } ARRAY_INFO, __RPC_FAR *PARRAY_INFO;

/*
 *  Pipe related definitions.
 */

typedef void
(__RPC_FAR __RPC_API * NDR_PIPE_PULL_RTN)(
        char          __RPC_FAR *  state,
        void          __RPC_FAR *  buf,
        unsigned long              esize,
        unsigned long __RPC_FAR *  ecount );

typedef void
(__RPC_FAR __RPC_API * NDR_PIPE_PUSH_RTN)(
        char          __RPC_FAR *  state,
        void          __RPC_FAR *  buf,
        unsigned long              ecount );

typedef void
(__RPC_FAR __RPC_API * NDR_PIPE_ALLOC_RTN)(
        char             __RPC_FAR *  state,
        unsigned long                 bsize,
        void __RPC_FAR * __RPC_FAR *  buf,
        unsigned long    __RPC_FAR *  bcount );


typedef struct  _GENERIC_PIPE_TYPE
    {
    NDR_PIPE_PULL_RTN       pfnPull;
    NDR_PIPE_PUSH_RTN       pfnPush;
    NDR_PIPE_ALLOC_RTN      pfnAlloc;
    char  __RPC_FAR *       pState;
    } GENERIC_PIPE_TYPE;


typedef struct {
    int                         CurrentState;
    int                         ElemsInChunk;
    int                         ElemAlign;
    int                         ElemWireSize;
    int                         ElemMemSize;
    int                         PartialBufferSize;
    unsigned char __RPC_FAR *   PartialElem;
    int                         PartialElemSize;
    int                         PartialOffset;
    int                         EndOfPipe;
    } NDR_PIPE_STATE;

typedef struct  _PIPE_MESSAGE
    {
    unsigned short                  Signature;
    unsigned short                  PipeId;
    GENERIC_PIPE_TYPE  __RPC_FAR *  pPipeType;
    PFORMAT_STRING                  pTypeFormat;
    unsigned short                  PipeStatus;
    unsigned short                  PipeFlags;
    struct _MIDL_STUB_MESSAGE  __RPC_FAR *  pStubMsg;
    } NDR_PIPE_MESSAGE, __RPC_FAR * PNDR_PIPE_MESSAGE;

typedef struct  _NDR_PIPE_DESC
    {
    NDR_PIPE_MESSAGE  __RPC_FAR *   pPipeMsg;
    short                           CurrentPipe;
    short                           InPipes;
    short                           OutPipes;
    short                           TotalPipes;
    short                           PipeVersion;
    short                           Flags;
    unsigned char  __RPC_FAR *      DispatchBuffer;
    unsigned char  __RPC_FAR *      LastPartialBuffer;
    unsigned long                   LastPartialSize;
    unsigned char  __RPC_FAR *      BufferSave;
    unsigned long                   LengthSave;
    NDR_PIPE_STATE                  RuntimeState;
    } NDR_PIPE_DESC, __RPC_FAR * PNDR_PIPE_DESC;


/*
 * MIDL Stub Message
 */
#if !defined(__RPC_DOS__) && !defined(__RPC_WIN16__) && !defined(__RPC_MAC__)
#include <pshpack4.h>
#endif

typedef struct _MIDL_STUB_MESSAGE
    {
    /* RPC message structure. */
    PRPC_MESSAGE                RpcMsg;

    /* Pointer into RPC message buffer. */
    unsigned char __RPC_FAR *   Buffer;

    /*
     * These are used internally by the Ndr routines to mark the beginning
     * and end of an incoming RPC buffer.
     */
    unsigned char __RPC_FAR *   BufferStart;
    unsigned char __RPC_FAR *   BufferEnd;

    /*
     * Used internally by the Ndr routines as a place holder in the buffer.
     * On the marshalling side it's used to mark the location where conformance
     * size should be marshalled.
     * On the unmarshalling side it's used to mark the location in the buffer
     * used during pointer unmarshalling to base pointer offsets off of.
     */
    unsigned char __RPC_FAR *   BufferMark;

    /* Set by the buffer sizing routines. */
    unsigned long               BufferLength;

    /* Set by the memory sizing routines. */
    unsigned long               MemorySize;

    /* Pointer to user memory. */
    unsigned char __RPC_FAR *   Memory;

    /* Is the Ndr routine begin called from a client side stub. */
    int                         IsClient;

    /* Can the buffer be re-used for memory on unmarshalling. */
    int                         ReuseBuffer;

    /* Holds the current pointer to an allocate all nodes memory block. */
    unsigned char __RPC_FAR *   AllocAllNodesMemory;

    /* Used for debugging asserts only, remove later. */
    unsigned char __RPC_FAR *   AllocAllNodesMemoryEnd;

    /*
     * Stuff needed while handling complex structures
     */

    /* Ignore imbeded pointers while computing buffer or memory sizes. */
    int                         IgnoreEmbeddedPointers;

    /*
     * This marks the location in the buffer where pointees of a complex
     * struct reside.
     */
    unsigned char __RPC_FAR *   PointerBufferMark;

    /*
     * Used to catch errors in SendReceive.
     */
    unsigned char               fBufferValid;

    /*
     * Obsolete unused field (formerly MaxContextHandleNumber).
     */
    unsigned char               uFlags;

    /*
     * Used internally by the Ndr routines.  Holds the max counts for
     * a conformant array.
     */
    unsigned long               MaxCount;

    /*
     * Used internally by the Ndr routines.  Holds the offsets for a varying
     * array.
     */
    unsigned long               Offset;

    /*
     * Used internally by the Ndr routines.  Holds the actual counts for
     * a varying array.
     */
    unsigned long               ActualCount;

    /* Allocation and Free routine to be used by the Ndr routines. */
    void __RPC_FAR *    (__RPC_FAR __RPC_API * pfnAllocate)(size_t);
    void                (__RPC_FAR __RPC_API * pfnFree)(void __RPC_FAR *);

    /*
     * Top of parameter stack.  Used for "single call" stubs during marshalling
     * to hold the beginning of the parameter list on the stack.  Needed to
     * extract parameters which hold attribute values for top level arrays and
     * pointers.
     */
    unsigned char __RPC_FAR *       StackTop;

    /*
     *  Fields used for the transmit_as and represent_as objects.
     *  For represent_as the mapping is: presented=local, transmit=named.
     */
    unsigned char __RPC_FAR *       pPresentedType;
    unsigned char __RPC_FAR *       pTransmitType;

    /*
     * When we first construct a binding on the client side, stick it
     * in the rpcmessage and later call RpcGetBuffer, the handle field
     * in the rpcmessage is changed. That's fine except that we need to
     * have that original handle for use in unmarshalling context handles
     * (the second argument in NDRCContextUnmarshall to be exact). So
     * stash the contructed handle here and extract it when needed.
     */
    handle_t                        SavedHandle;

    /*
     * Pointer back to the stub descriptor.  Use this to get all handle info.
     */
    const struct _MIDL_STUB_DESC __RPC_FAR *    StubDesc;

    /*
     * Full pointer stuff.
     */
    struct _FULL_PTR_XLAT_TABLES __RPC_FAR *    FullPtrXlatTables;

    unsigned long                   FullPtrRefId;

    /*
     * flags
     */

    int                             fCheckBounds;

    int                             fInDontFree       :1;
    int                             fDontCallFreeInst :1;
    int                             fInOnlyParam      :1;
    int                             fHasReturn        :1;

    unsigned long                   dwDestContext;
    void __RPC_FAR *                pvDestContext;

    NDR_SCONTEXT *                  SavedContextHandles;

    long                            ParamNumber;

    struct IRpcChannelBuffer __RPC_FAR *    pRpcChannelBuffer;

    PARRAY_INFO                     pArrayInfo;

    /*
     * This is where the Beta2 stub message ends.
     */

    unsigned long __RPC_FAR *       SizePtrCountArray;
    unsigned long __RPC_FAR *       SizePtrOffsetArray;
    unsigned long __RPC_FAR *       SizePtrLengthArray;

    /*
     * Interpreter argument queue.  Used on server side only.
     */
    void __RPC_FAR *                pArgQueue;

    unsigned long                   dwStubPhase;

    /*
     * Pipe descriptor, defined for the 4.0 release.
     */

    NDR_PIPE_DESC   __RPC_FAR *     pPipeDesc;

    unsigned long                   Reserved[4];

    /*
     *  Fields up to this point present since the 3.50 release.
     */

    } MIDL_STUB_MESSAGE, __RPC_FAR *PMIDL_STUB_MESSAGE;

#if !defined(__RPC_DOS__) && !defined(__RPC_WIN16__) && !defined(__RPC_MAC__)
#include <poppack.h>
#endif

/*
 * Generic handle bind/unbind routine pair.
 */
typedef void __RPC_FAR *
        (__RPC_FAR __RPC_API * GENERIC_BINDING_ROUTINE)
        (void __RPC_FAR *);
typedef void
        (__RPC_FAR __RPC_API * GENERIC_UNBIND_ROUTINE)
        (void __RPC_FAR *, unsigned char __RPC_FAR *);

typedef struct _GENERIC_BINDING_ROUTINE_PAIR
    {
    GENERIC_BINDING_ROUTINE     pfnBind;
    GENERIC_UNBIND_ROUTINE      pfnUnbind;
    } GENERIC_BINDING_ROUTINE_PAIR, __RPC_FAR *PGENERIC_BINDING_ROUTINE_PAIR;

typedef struct __GENERIC_BINDING_INFO
    {
    void __RPC_FAR *            pObj;
    unsigned int                Size;
    GENERIC_BINDING_ROUTINE     pfnBind;
    GENERIC_UNBIND_ROUTINE      pfnUnbind;
    } GENERIC_BINDING_INFO, __RPC_FAR *PGENERIC_BINDING_INFO;

// typedef EXPR_EVAL - see above
// typedefs for xmit_as

#if (defined(_MSC_VER)) && !defined(MIDL_PASS)
// a Microsoft C++ compiler
#define NDR_SHAREABLE __inline
#else
#define NDR_SHAREABLE static
#endif


typedef void (__RPC_FAR __RPC_USER * XMIT_HELPER_ROUTINE)
    ( PMIDL_STUB_MESSAGE );

typedef struct _XMIT_ROUTINE_QUINTUPLE
    {
    XMIT_HELPER_ROUTINE     pfnTranslateToXmit;
    XMIT_HELPER_ROUTINE     pfnTranslateFromXmit;
    XMIT_HELPER_ROUTINE     pfnFreeXmit;
    XMIT_HELPER_ROUTINE     pfnFreeInst;
    } XMIT_ROUTINE_QUINTUPLE, __RPC_FAR *PXMIT_ROUTINE_QUINTUPLE;

typedef unsigned long
(__RPC_FAR __RPC_USER * USER_MARSHAL_SIZING_ROUTINE)
    (unsigned long __RPC_FAR *,
     unsigned long,
     void __RPC_FAR * );

typedef unsigned char __RPC_FAR *
(__RPC_FAR __RPC_USER * USER_MARSHAL_MARSHALLING_ROUTINE)
    (unsigned long __RPC_FAR *,
     unsigned char  __RPC_FAR * ,
     void __RPC_FAR * );

typedef unsigned char __RPC_FAR *
(__RPC_FAR __RPC_USER * USER_MARSHAL_UNMARSHALLING_ROUTINE)
    (unsigned long __RPC_FAR *,
     unsigned char  __RPC_FAR * ,
     void __RPC_FAR * );

typedef void (__RPC_FAR __RPC_USER * USER_MARSHAL_FREEING_ROUTINE)
    (unsigned long __RPC_FAR *,
     void __RPC_FAR * );

typedef struct _USER_MARSHAL_ROUTINE_QUADRUPLE
    {
    USER_MARSHAL_SIZING_ROUTINE          pfnBufferSize;
    USER_MARSHAL_MARSHALLING_ROUTINE     pfnMarshall;
    USER_MARSHAL_UNMARSHALLING_ROUTINE   pfnUnmarshall;
    USER_MARSHAL_FREEING_ROUTINE         pfnFree;
    } USER_MARSHAL_ROUTINE_QUADRUPLE;

typedef struct _USER_MARSHAL_CB
{
    unsigned long       Flags;
    PMIDL_STUB_MESSAGE  pStubMsg;
    PFORMAT_STRING      pReserve;
} USER_MARSHAL_CB;


#define USER_CALL_CTXT_MASK(f)  ((f) & 0xff)
#define GET_USER_DATA_REP(f)    ((f) >> 16)

typedef struct _MALLOC_FREE_STRUCT
    {
    void __RPC_FAR *    (__RPC_FAR __RPC_USER * pfnAllocate)(size_t);
    void                (__RPC_FAR __RPC_USER * pfnFree)(void __RPC_FAR *);
    } MALLOC_FREE_STRUCT;

typedef struct _COMM_FAULT_OFFSETS
    {
    short       CommOffset;
    short       FaultOffset;
    } COMM_FAULT_OFFSETS;

/*
 * MIDL Stub Descriptor
 */

typedef struct _MIDL_STUB_DESC
    {

    void __RPC_FAR *    RpcInterfaceInformation;

    void __RPC_FAR *    (__RPC_FAR __RPC_API * pfnAllocate)(size_t);
    void                (__RPC_FAR __RPC_API * pfnFree)(void __RPC_FAR *);

    union
        {
        handle_t __RPC_FAR *            pAutoHandle;
        handle_t __RPC_FAR *            pPrimitiveHandle;
        PGENERIC_BINDING_INFO           pGenericBindingInfo;
        } IMPLICIT_HANDLE_INFO;

    const NDR_RUNDOWN __RPC_FAR *                   apfnNdrRundownRoutines;
    const GENERIC_BINDING_ROUTINE_PAIR __RPC_FAR *  aGenericBindingRoutinePairs;

    const EXPR_EVAL __RPC_FAR *                     apfnExprEval;

    const XMIT_ROUTINE_QUINTUPLE __RPC_FAR *        aXmitQuintuple;

    const unsigned char __RPC_FAR *                 pFormatTypes;

    int                                             fCheckBounds;

    /* Ndr library version. */
    unsigned long                                   Version;

    /*
     * Reserved for future use. (no reserves )
     */

    MALLOC_FREE_STRUCT __RPC_FAR *                  pMallocFreeStruct;

    long                                MIDLVersion;

    const COMM_FAULT_OFFSETS __RPC_FAR *    CommFaultOffsets;

    // New fields for version 3.0+

    const USER_MARSHAL_ROUTINE_QUADRUPLE __RPC_FAR * aUserMarshalQuadruple;

    long                                    Reserved1;
    long                                    Reserved2;
    long                                    Reserved3;
    long                                    Reserved4;
    long                                    Reserved5;

    } MIDL_STUB_DESC;

typedef const MIDL_STUB_DESC __RPC_FAR * PMIDL_STUB_DESC;

typedef void __RPC_FAR * PMIDL_XMIT_TYPE;

/*
 * MIDL Stub Format String.  This is a const in the stub.
 */
#if !defined( RC_INVOKED )
#pragma warning( disable:4200 )
#endif
typedef struct _MIDL_FORMAT_STRING
    {
    short               Pad;
    unsigned char       Format[];
    } MIDL_FORMAT_STRING;
#if !defined( RC_INVOKED )
#pragma warning( default:4200 )
#endif

/*
 * Stub thunk used for some interpreted server stubs.
 */
typedef void (__RPC_FAR __RPC_API * STUB_THUNK)( PMIDL_STUB_MESSAGE );

typedef long (__RPC_FAR __RPC_API * SERVER_ROUTINE)();

/*
 * Server Interpreter's information strucuture.
 */
typedef struct  _MIDL_SERVER_INFO_
    {
    PMIDL_STUB_DESC             pStubDesc;
    const SERVER_ROUTINE *      DispatchTable;
    PFORMAT_STRING              ProcString;
    const unsigned short *      FmtStringOffset;
    const STUB_THUNK *          ThunkTable;
    PFORMAT_STRING              LocalFormatTypes;
    PFORMAT_STRING              LocalProcString;
    const unsigned short *      LocalFmtStringOffset;
    } MIDL_SERVER_INFO, *PMIDL_SERVER_INFO;

/*
 * Stubless object proxy information structure.
 */
typedef struct _MIDL_STUBLESS_PROXY_INFO
    {
    PMIDL_STUB_DESC                     pStubDesc;
    PFORMAT_STRING                      ProcFormatString;
    const unsigned short __RPC_FAR *    FormatStringOffset;
    PFORMAT_STRING                      LocalFormatTypes;
    PFORMAT_STRING                      LocalProcString;
    const unsigned short __RPC_FAR *    LocalFmtStringOffset;
    } MIDL_STUBLESS_PROXY_INFO;

typedef MIDL_STUBLESS_PROXY_INFO __RPC_FAR * PMIDL_STUBLESS_PROXY_INFO;

/*
 * This is the return value from NdrClientCall.
 */
typedef union _CLIENT_CALL_RETURN
    {
    void __RPC_FAR *        Pointer;
    long                    Simple;
    } CLIENT_CALL_RETURN;

/*
 * Full pointer data structures.
 */

typedef enum
        {
        XLAT_SERVER = 1,
        XLAT_CLIENT
        } XLAT_SIDE;

/*
 * Stores the translation for the conversion from a full pointer into it's
 * corresponding ref id.
 */
typedef struct _FULL_PTR_TO_REFID_ELEMENT
    {
    struct _FULL_PTR_TO_REFID_ELEMENT __RPC_FAR *  Next;

    void __RPC_FAR *            Pointer;
    unsigned long       RefId;
    unsigned char       State;
    } FULL_PTR_TO_REFID_ELEMENT, __RPC_FAR *PFULL_PTR_TO_REFID_ELEMENT;

/*
 * Full pointer translation tables.
 */
typedef struct _FULL_PTR_XLAT_TABLES
    {
    /*
     * Ref id to pointer translation information.
     */
    struct
        {
        void __RPC_FAR *__RPC_FAR *             XlatTable;
        unsigned char __RPC_FAR *     StateTable;
        unsigned long       NumberOfEntries;
        } RefIdToPointer;

    /*
     * Pointer to ref id translation information.
     */
    struct
        {
        PFULL_PTR_TO_REFID_ELEMENT __RPC_FAR *  XlatTable;
        unsigned long                   NumberOfBuckets;
        unsigned long                   HashMask;
        } PointerToRefId;

    /*
     * Next ref id to use.
     */
    unsigned long           NextRefId;

    /*
     * Keep track of the translation size we're handling : server or client.
     * This tells us when we have to do reverse translations when we insert
     * new translations.  On the server we must insert a pointer-to-refid
     * translation whenever we insert a refid-to-pointer translation, and
     * vica versa for the client.
     */
    XLAT_SIDE               XlatSide;
    } FULL_PTR_XLAT_TABLES, __RPC_FAR *PFULL_PTR_XLAT_TABLES;

/***************************************************************************
 ** New MIDL 2.0 Ndr routine templates
 ***************************************************************************/

/*
 * Marshall routines
 */

// @CESYSGEN IF DCOM_MODULES_RPCRT4
// @CESYSGEN ENDIF

#define USER_MARSHAL_FC_BYTE         1
#define USER_MARSHAL_FC_CHAR         2
#define USER_MARSHAL_FC_SMALL        3
#define USER_MARSHAL_FC_USMALL       4
#define USER_MARSHAL_FC_WCHAR        5
#define USER_MARSHAL_FC_SHORT        6
#define USER_MARSHAL_FC_USHORT       7
#define USER_MARSHAL_FC_LONG         8
#define USER_MARSHAL_FC_ULONG        9
#define USER_MARSHAL_FC_FLOAT       10
#define USER_MARSHAL_FC_HYPER       11
#define USER_MARSHAL_FC_DOUBLE      12

// @CESYSGEN IF DCOM_MODULES_RPCRT4
// @CESYSGEN ENDIF

/* server */
typedef enum {
    STUB_UNMARSHAL,
    STUB_CALL_SERVER,
    STUB_MARSHAL,
    STUB_CALL_SERVER_NO_HRESULT
}STUB_PHASE;

typedef enum {
    PROXY_CALCSIZE,
    PROXY_GETBUFFER,
    PROXY_MARSHAL,
    PROXY_SENDRECEIVE,
    PROXY_UNMARSHAL
}PROXY_PHASE;

// @CESYSGEN IF DCOM_MODULES_RPCRT4
// @CESYSGEN ENDIF

/****************************************************************************
    MIDL 2.0 memory package: rpc_ss_* rpc_sm_*
 ****************************************************************************/

typedef void __RPC_FAR * RPC_SS_THREAD_HANDLE;

typedef void __RPC_FAR * __RPC_API
RPC_CLIENT_ALLOC (
    IN size_t Size
    );

typedef void __RPC_API
RPC_CLIENT_FREE (
    IN void __RPC_FAR * Ptr
    );

// @CESYSGEN IF DCOM_MODULES_RPCRT4
// @CESYSGEN ENDIF

#ifdef CONST_VTABLE
#define CONST_VTBL const
#else
#define CONST_VTBL
#endif

/****************************************************************************
 * Special things for VC5 Com support
 ****************************************************************************/

#if (_MSC_VER >= 1100)
#define DECLSPEC_UUID(x)    __declspec(uuid(x))
#define MIDL_INTERFACE(x)   struct __declspec(uuid(x)) __declspec(novtable)
#else
#define DECLSPEC_UUID(x)
#define MIDL_INTERFACE(x)   struct
#endif

#if (_MSC_VER >= 1100)
#define EXTERN_GUID(itf,l1,s1,s2,c1,c2,c3,c4,c5,c6,c7,c8)  \
  EXTERN_C const IID __declspec(selectany) itf = {l1,s1,s2,{c1,c2,c3,c4,c5,c6,c7,c8}}
#else
#define EXTERN_GUID(itf,l1,s1,s2,c1,c2,c3,c4,c5,c6,c7,c8) EXTERN_C const IID itf
#endif


#ifdef __cplusplus
}
#endif

// Reset the packing level for DOS, Windows and Mac.

#if defined(__RPC_DOS__) || defined(__RPC_WIN16__) || defined(__RPC_MAC__)
#pragma pack()
#endif

#endif /* __RPCNDR_H__ */





