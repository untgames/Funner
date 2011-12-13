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
/***
*excpt.h - defines exception values, types and routines
*
*
*Purpose:
*       This file contains the definitions and prototypes for the compiler-
*       dependent intrinsics, support functions and keywords which implement
*       the structured exception handling extensions.
*
****/

#ifndef _INC_EXCPT

#ifdef  _MSC_VER
/*
 * Currently, all MS C compilers for Win32 platforms default to 8 byte
 * alignment.
 */
#pragma pack(push,8)
#endif  /* _MSC_VER */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Exception disposition return values.
 */
typedef enum _EXCEPTION_DISPOSITION {
    ExceptionContinueExecution,
    ExceptionContinueSearch,
    ExceptionNestedException,
    ExceptionCollidedUnwind,
    ExceptionExecuteHandler
} EXCEPTION_DISPOSITION;


/*
 * Prototype for SEH support function.
 */

/*
 * Declare some standard structures
 */
typedef struct _EXCEPTION_POINTERS *Exception_info_ptr;
struct _EXCEPTION_RECORD;
struct _RUNTIME_FUNCTION;
struct _EXCEPTION_REGISTRATION_RECORD;

struct _CONTEXT;
struct _EXCEPTION_RECORD;
struct _EXCEPTION_POINTERS;
typedef struct _CONTEXT *PCONTEXT;
typedef struct _EXCEPTION_RECORD *PEXCEPTION_RECORD;
typedef struct _EXCEPTION_POINTERS *PEXCEPTION_POINTERS;
typedef PCONTEXT LPCONTEXT;
typedef PEXCEPTION_RECORD LPEXCEPTION_RECORD;
typedef PEXCEPTION_POINTERS LPEXCEPTION_POINTERS;

typedef struct _DISPATCHER_CONTEXT {
#ifdef _X86_
    struct _EXCEPTION_REGISTRATION_RECORD *RegistrationPointer;
    unsigned long ControlPc;
#else
    unsigned long ControlPc;
    struct _RUNTIME_FUNCTION *FunctionEntry;
    unsigned long EstablisherFrame;
    PCONTEXT ContextRecord;
#endif
} DISPATCHER_CONTEXT, *PDISPATCHER_CONTEXT;

typedef EXCEPTION_DISPOSITION EXCEPTION_ROUTINE (
    struct _EXCEPTION_RECORD *ExceptionRecord,
    void *EstablisherFrame,
    struct _CONTEXT *ContextRecord,
    struct _DISPATCHER_CONTEXT *DispatcherContext
    );
typedef EXCEPTION_ROUTINE *PEXCEPTION_ROUTINE;

#ifdef _X86_
EXCEPTION_ROUTINE _except_handler3;
#else
EXCEPTION_ROUTINE __C_specific_handler;
#endif

/*
 * Keywords and intrinsics for SEH
 */

#ifdef  _MSC_VER

#if     !defined(__cplusplus)
#define try                             __try
#define except                          __except
#define finally                         __finally
#define leave                           __leave
#endif

#define GetExceptionCode                _exception_code
#define exception_code                  _exception_code
#define GetExceptionInformation         (struct _EXCEPTION_POINTERS *)_exception_info
#define exception_info                  (struct _EXCEPTION_POINTERS *)_exception_info
#define AbnormalTermination             _abnormal_termination
#define abnormal_termination            _abnormal_termination

unsigned long __cdecl _exception_code(void);
void *        __cdecl _exception_info(void);
int           __cdecl _abnormal_termination(void);

#endif


/*
 * Legal values for expression in except().
 */

#define EXCEPTION_EXECUTE_HANDLER        1
#define EXCEPTION_CONTINUE_SEARCH        0
#define EXCEPTION_CONTINUE_EXECUTION    -1


/*
 * for convenience, define a type name for a pointer to signal-handler
 */

typedef void (__cdecl * _PHNDLR)(int);

/*
 * Exception-action table used by the C runtime to identify and dispose of
 * exceptions corresponding to C runtime errors or C signals.
 */
struct _XCPT_ACTION {

    /*
     * exception code or number. defined by the host OS.
     */
    unsigned long XcptNum;

    /*
     * signal code or number. defined by the C runtime.
     */
    int SigNum;

    /*
     * exception action code. either a special code or the address of
     * a handler function. always determines how the exception filter
     * should dispose of the exception.
     */
    _PHNDLR XcptAction;
};

extern struct _XCPT_ACTION _XcptActTab[];

/*
 * number of entries in the exception-action table
 */
extern int _XcptActTabCount;

/*
 * size of exception-action table (in bytes)
 */
extern int _XcptActTabSize;

/*
 * index of the first floating point exception entry
 */
extern int _First_FPE_Indx;

/*
 * number of FPE entries
 */
extern int _Num_FPE;

/*
 * return values and prototype for the exception filter function used in the
 * C startup
 */
int __cdecl _XcptFilter(unsigned long, struct _EXCEPTION_POINTERS *);

int __cdecl UnhandledExceptionFilter(struct _EXCEPTION_POINTERS*);

#ifdef __cplusplus
}
#endif

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#define _INC_EXCPT
#endif  /* _INC_EXCPT */
