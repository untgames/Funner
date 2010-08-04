

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Wed Aug 04 13:26:17 2010
 */
/* Compiler settings for gen/FireBreathWin.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, LIBID_FunnerEngineLib,0x46e34257,0x129e,0x5f51,0xa1,0x44,0x7d,0x32,0x89,0x55,0x53,0x53);


MIDL_DEFINE_GUID(IID, IID_IFBControl,0xb9a315e9,0x2144,0x5100,0xa5,0xf0,0xf7,0x58,0x99,0x09,0x01,0x02);


MIDL_DEFINE_GUID(IID, IID_IFBComJavascriptObject,0x626dc8c1,0x3889,0x546d,0x89,0x48,0xce,0x2a,0xcb,0x96,0x35,0xa9);


MIDL_DEFINE_GUID(IID, DIID_IFBComEventSource,0xa74a0e7c,0x31e3,0x59a6,0xb7,0xcd,0x09,0x2c,0x5b,0x3f,0xe9,0xe1);


MIDL_DEFINE_GUID(CLSID, CLSID_FBControl,0x6fb8bcf0,0x8a37,0x5048,0x8f,0x08,0x25,0x3c,0xc5,0x7d,0x90,0x49);


MIDL_DEFINE_GUID(CLSID, CLSID_FBComJavascriptObject,0x202f9d37,0x5ec0,0x5bec,0x8f,0xee,0xb6,0x84,0x71,0xa2,0xeb,0x47);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



