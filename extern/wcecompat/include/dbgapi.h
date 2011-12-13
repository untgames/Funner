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


Module Name: dbgapi.h

Purpose: Debug Message and Zone APIs.

--*/
#ifndef __DBGAPI_H__
#define __DBGAPI_H__

#ifdef __cplusplus
extern "C" {
#endif
    
void WINAPIV NKDbgPrintfW(LPCWSTR lpszFmt, ...);
void WINAPI WriteDebugLED(WORD wIndex, DWORD dwPattern);


/*
@doc    EXTERNAL    KERNEL
@struct DBGPARAM | Debug zone information structure
@comm   The name of the module is used to look for zone initialization
        information in the host PC registry. Zone names are displayed by
        the control app (eg shell) which allows the user to dynamically
        set zones.
@xref   <f DEBUGREGISTER>
*/
typedef struct _DBGPARAM {
	WCHAR	lpszName[32];           // @field Name of module
	WCHAR   rglpszZones[16][32];    // @field names of zones for first 16 bits
	ULONG   ulZoneMask;             // @field Current zone Mask
} DBGPARAM, *LPDBGPARAM;

BOOL RegisterDbgZones(HMODULE hMod, LPDBGPARAM lpdbgparam);


/*
@func   BOOL | DEBUGZONE | Associates a mask bit with a zone
@parm   int | bitnum | Bitnumber being defined
@rdesc  A boolean which is TRUE if bitnum in '1' else is FALSE
@ex     Example of use is |
        // associate bit 0 with an info zone
        #define ZONE_INFO   DEBUGZONE(0)
        // we can now use ZONE_INFO as a boolean for anything.
        // We'd typically use it in a DEBUGMSG ...
*/
#define DEBUGZONE(n)  (dpCurSettings.ulZoneMask&(0x00000001<<(n)))

#ifdef DEBUG
#ifdef SHIP_BUILD
#undef SHIP_BUILD
#pragma message (__FILE__ ":WARNING: SHIP_BUILD turned off since DEBUG defined")
#endif
#endif

#ifdef SHIP_BUILD

#define ERRORMSG(cond,printf_exp) ((void)0)
#define RETAILMSG(cond,printf_exp) ((void)0)
#define DEBUGMSG(cond,printf_exp) ((void)0)
#define DEBUGLED(cond,parms) ((void)0)
#define DBGCHK(module,exp) ((void)0)
#define DEBUGCHK(exp) ((void)0)
#define DEBUGREGISTER(hMod) ((void)0)
#define RETAILREGISTERZONES(hMod) ((void)0)

#else // SHIP_BUILD

#ifdef DEBUG

/*
@func BOOL | DEBUGMSG | Output a debug message conditionally
@parm BOOL | cond | The condition under which the message is printed
@parm <null> | (printf_exp) | A printf style expression to be output. Must be enclosed
      in parentheses. Gets passed on to the <f NKDbgPrintf> function.
@ex   Example of use |
      DEBUGMSG(ZONE_INFO, (L"DLLNAME: Entered func1. Param 1 = %d\r\n", par1));
@xref <f RETAILMSG> <tab> <f ERRORMSG> <tab> <f NKDbgPrintf>
*/
#define DEBUGMSG(cond,printf_exp)   \
   ((void)((cond)?(NKDbgPrintfW printf_exp),1:0))
#define DBGCHK(module,exp) \
   ((void)((exp)?1:(          \
       NKDbgPrintfW ( TEXT("%s: DEBUGCHK failed in file %s at line %d \r\n"), \
                 (LPWSTR)module, TEXT(__FILE__) ,__LINE__ ),    \
       DebugBreak(), \
	   0  \
   )))

/*
@func BOOL | DEBUGLED | Output a debug LED pattern conditionally
@parm BOOL | cond | The condition under which the message is printed
@parm <null> | (parms) | The parameters to be passed to the WriteDebugLED
      function.  Must be in parentheses.  First parameter is wIndex and second
      parameter is dwPattern.
@ex   Example of use |
      DEBUGLED(ZONE_INFO, (3, 0x2345);
@xref <f RETAILMSG> <tab> <f ERRORMSG> <tab> <f WriteDebugLED>
*/
#define DEBUGLED(cond,parms)   \
   ((void)((cond)?(WriteDebugLED parms),1:0))    


/*
@func BOOL | DEBUGCHK | Asserts an expression
@parm BOOL | exp | Expression to be asserted
@comm If the expression is false, this will cause a DebugBreak to be hit which
      will cause you to enter the debugger if you are running with one. It will
      also give you the line number and file name where the assert failed.
*/
#define DEBUGCHK(exp) DBGCHK(dpCurSettings.lpszName, exp)
extern  DBGPARAM    dpCurSettings;
/*
@func  BOOL | DEBUGREGISTER | Registers debug settings for a process / module
@parm  HINSTANCE | hInstance | If target is a module this is it's hInstance. If
       target is a process this should be NULL.
@comm  This simply calls through to <f RegisterDebugZones>. It assumes that
       there is a variable of name <b dpCurSettings> visible in the code.
*/
#define DEBUGREGISTER(hMod)  RegisterDbgZones(hMod, &dpCurSettings)
#else // DEBUG

#define DEBUGMSG(cond,printf_exp) ((void)0)
#define DEBUGLED(cond,parms) ((void)0)
#define DBGCHK(module,exp) ((void)0)
#define DEBUGCHK(exp) ((void)0)
#define DEBUGREGISTER(hMod) ((void)0)

#endif // DEBUG

/*
@func BOOL | RETAILMSG | Output a message in retail builds
@parm BOOL | cond | The condition under which the message is printed
@parm <null> | (printf_exp) | A printf style expression to be output. Must be enclosed
      in parentheses. This is simply passed in to the <f NKDbgPrintf> function.
@comm This should be used in a very limited fashion since it can increase
      the size of your retail build.
@ex   Example of use |
      RETAILMSG(x==y, (L"DLLNAME: Expect. x==y = %d\r\n", x));
@xref <f DEBUGMSG> <tab> <f ERRORMSG> <tab> <f NKDbgPrintf>
*/
#define RETAILMSG(cond,printf_exp)   \
   ((cond)?(NKDbgPrintfW printf_exp),1:0)

/*
@func BOOL | ERRORMSG | Output an error msg
@parm BOOL | cond | The condition under which the message is printed
@parm <null> | (printf_exp) | A printf style expression to be output. Must be enclosed
      in parentheses. This is passed in to the <f NKDbgPrintf> function.
@comm Very similar to <f RETAILMSG> except that this will prefix the message
      with "ERROR" and give the file name & line number of the error.
@ex   Example of use |
      ERRORMSG(x==y, (L"DLLNAME: x===y = %d\r\n", x));
@xref <f DEBUGMSG> <tab> <f RETAILMSG> <tab> <f NKDbgPrintf>
*/
#define ERRORMSG(cond,printf_exp)	 \
   ((cond)?(NKDbgPrintfW(TEXT("ERROR: %s line %d: "),TEXT(__FILE__),__LINE__), NKDbgPrintfW printf_exp),1:0)

/*
@func  BOOL | RETAILREGISTERZONES | Registers zone settings for a process / module
       (Same as DEBUGREGISTER except available in retail and debug builds)
@parm  HINSTANCE | hInstance | If target is a module this is it's hInstance. If
       target is a process this should be NULL.
@comm  This simply calls through to <f RegisterDebugZones>. It assumes that
       there is a variable of name <b dpCurSettings> visible in the code.
*/
#define RETAILREGISTERZONES(hMod)  RegisterDbgZones(hMod, &dpCurSettings)

#endif // SHIP_BUILD

/*
@func BOOL | RETAILLED | Output a LED code in retail builds
@parm BOOL | cond | The condition under which the message is printed
@parm <null> | (parms) | The parameters to be passed to the WriteDebugLED
      function.  Must be in parentheses.  First parameter is wIndex and second
      parameter is dwPattern.
@comm This should be used in a very limited fashion since it can increase
      the size of your retail build.
@ex   Example of use |
      RETAILLED(ZONE_INFO, (3, 0x2345);
@xref <f DEBUGMSG> <tab> <f ERRORMSG> <tab> <f WriteDebugLED>
*/
#define RETAILLED(cond,parms) \
   ((void)((cond)?(WriteDebugLED parms),1:0))    

// some alternate ways to get to these
#define ASSERTMSG(msg, exp) (DEBUGMSG(!exp,(msg)),DBGCHK(TEXT("Unknown"),exp))
#define ASSERT( exp )   DBGCHK(TEXT("Unknown"), exp)
#define ASSERT_IMPLIES( cond, exp ) ASSERT( !(cond) || (exp) )
#ifdef DEBUG
#define VERIFY(exp)  ASSERT(exp)
#else
#define VERIFY(exp)  ((void)(exp))
#endif

// Enable providing a hint to prefast via __assume
// e.g.
//   PREFAST_ASSUME( pPointer); // we know pPointer can never be NULL
#ifdef _PREFAST_
#define PREFAST_ASSUME(exp) __assume(exp)
#else
#define PREFAST_ASSUME(exp)
#endif

// Simplify the cases where a PREFAST_ASSUME would be followed by an assertion
#define PREFAST_DEBUGCHK(exp) \
    { \
        PREFAST_ASSUME(exp); \
        DEBUGCHK(exp); \
    }

#define PREFAST_ASSERT(exp) \
    { \
        PREFAST_ASSUME(exp); \
        ASSERT(exp); \
    }

// macro to tell Prefast to not issue a specific warning for the following line of code
// use to suppress false positives from Prefast
// e.g.
// if( fPointerNotNull )
//    PREFAST_SUPPRESS( 11, "pointer access is guarded by 'fPointerNotNull'" )
//    p->Foo();
#ifdef _PREFAST_
#define PREFAST_SUPPRESS( cWarning, comment) __pragma ( prefast(suppress: cWarning, comment) )
#else
#define PREFAST_SUPPRESS( cWarning, comment)
#endif

// Helpful DBG macros that tighten up code.
// They prepend __FUNCTION__ to your debug msg and add a \n to the end.

#define DEB_0(ZONE,STR) DEBUGMSG(ZONE,(L"%S:"STR L"\n",__FUNCTION__))
#define DEB_1(ZONE,STR,ARG1) DEBUGMSG(ZONE,(L"%S:"STR L"\n",__FUNCTION__,ARG1))
#define DEB_2(ZONE,STR,ARG1,ARG2) DEBUGMSG(ZONE,(L"%S:"STR L"\n",__FUNCTION__,ARG1,ARG2))
#define DEB_3(ZONE,STR,ARG1,ARG2,ARG3) DEBUGMSG(ZONE,(L"%S:"STR L"\n",__FUNCTION__,ARG1,ARG2,ARG3))
#define DEB_4(ZONE,STR,ARG1,ARG2,ARG3,ARG4) DEBUGMSG(ZONE,(L"%S:"STR L"\n",__FUNCTION__,ARG1,ARG2,ARG3,ARG4))
#define DEB_5(ZONE,STR,ARG1,ARG2,ARG3,ARG4,ARG5) DEBUGMSG(ZONE,(L"%S:"STR L"\n",__FUNCTION__,ARG1,ARG2,ARG3,ARG4,ARG5))

// 
//  The _A version of the above are identical, except they call assert  after the  DEB_*.  
//  It's highly recommend that you use DEB_*_A with ZONE_ERROR so the user can always see the error.
//  printed with the assert.
//  NOTE: The assert is not conditioned on the ZONE. 

#define DEB_0_A(ZONE,STR) { DEB_0(ZONE,STR);ASSERT(FALSE); }
#define DEB_1_A(ZONE,STR,ARG1) { DEB_1(ZONE,STR,ARG1); ASSERT(FALSE);}
#define DEB_2_A(ZONE,STR,ARG1,ARG2) { DEB_2(ZONE,STR,ARG1,ARG2); ASSERT(FALSE);}
#define DEB_3_A(ZONE,STR,ARG1,ARG2,ARG3) { DEB_3(ZONE,STR,ARG1,ARG2,ARG3); ASSERT(FALSE);}
#define DEB_4_A(ZONE,STR,ARG1,ARG2,ARG3,ARG4) {  DEB_4(ZONE,STR,ARG1,ARG2,ARG3,ARG4); ASSERT(FALSE);}
#define DEB_5_A(ZONE,STR,ARG1,ARG2,ARG3,ARG4,ARG5) {DEB_5(ZONE,STR,ARG1,ARG2,ARG3,ARG4,ARG5); ASSERT(FALSE);}


#ifdef __cplusplus
}
#endif
    
#endif

