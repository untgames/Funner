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

#ifndef __LMEMDEBUG_H__
#define __LMEMDEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define LMEMDEBUG_DLL               L"LMemDebug.DLL"
#define LMEM_REG_KEY                L"Software\\Microsoft\\LMemDebug"
#define LMEM_REG_FLAGS              L"Flags"
#define LMEM_REG_BREAK_SIZE         L"BreakSize"
#define LMEM_REG_BREAK_COUNT        L"BreakCount"
#define LMEM_REG_CALLSTACK_DEPTH    L"CallStackDepth"
#define LMEM_REG_CALLSTACK_SKIP     L"CallStackSkip"
#define LMEM_REG_CALLSTACK_MOD      L"CallStackMod"


    
struct _LMEM_ALLOC_INFO;
typedef struct _LMEM_HEAP_INFO {
    DWORD               dwSIG1;
    HANDLE              hHeap;
    struct _LMEM_HEAP_INFO *pNext;
    struct _LMEM_HEAP_INFO *pPrev;
    DWORD               dwTotalAlloc;       // Total bytes allocated
    DWORD               dwCurrentAlloc;     // Bytes currently allocated
    DWORD               dwCountAlloc;       // Count of total allocations
    DWORD               dwCurrentCount;     // Count current allocations.
    struct _LMEM_ALLOC_INFO *pAlloc;
#define HEAP_HIST_BUCKETS   32
#define HEAP_HIST_SIZE      32
    DWORD               dwHist[HEAP_HIST_BUCKETS];
    DWORD               dwMaxAlloc;         // Largest number of bytes allocated.
    DWORD               dwMaxCount;         // Largest count of objects allocated
    DWORD               dwSIG2;
} LMEM_HEAP_INFO, *PLMEM_HEAP_INFO;

typedef struct _LMEM_ALLOC_INFO {
    WORD                wStructSize;
    WORD                wFlags;
#define LMEM_ALLOC_BIT_0        0x0001  // User bit 0
#define LMEM_ALLOC_BIT_1        0x0002  // User bit 1
#define LMEM_ALLOC_BIT_2        0x0004  // User bit 2
#define LMEM_ALLOC_BIT_3        0x0008  // User bit 3
#define LMEM_ALLOC_IGNORE_LEAK  0x0010  // Ignore this item on leak check
#define LMEM_ALLOC_CHKPOINT     0x0020  // Checkpoint bit (used by shell.exe)
#define LMEM_ALLOC_ADDITIONAL   0x0040  // Additional data at end
    PLMEM_HEAP_INFO     pHeap;
    DWORD               dwSize;
#define LMEM_ALLOC_SIZE_EVENT    0xFFFFFFFF  // Special size for event objects
#define LMEM_ALLOC_SIZE_CS       0xFFFFFFFE  // Special size for CritSect objects
#define LMEM_ALLOC_SIZE_FILE     0xFFFFFFFD  // Special size for File objects
    struct _LMEM_ALLOC_INFO  *pNext;
    LPVOID              lpMem;
    DWORD               dwLineNum;
    PCHAR               szFilename;
    DWORD               dwAllocCount;
    DWORD               hThread;            // Thread that created the object
} LMEM_ALLOC_INFO, *PLMEM_ALLOC_INFO;

typedef struct _LMEM_MODINFO {
    PCHAR   szModName;                      // Module Name
    DWORD   dwOffset;                       // Offset in module
} LMEM_MODINFO, *PLMEM_MODINFO;



// Type codes for additional data
// Additional data is preceded by word type, word length
// chain is terminated with TYPE_END and length 0.
// length is length of data (so move pointer forward by length + 2*sizeof(word))
#define LMEM_TYPE_END       0
#define LMEM_TYPE_STACK     1
#define LMEM_TYPE_UNICODE   2
#define LMEM_TYPE_ASCII     3
#define LMEM_TYPE_HEX       4
#define LMEM_TYPE_STACK_MOD 5

#define LMEM_SIG        0x6c6d656d
#define LMEM_ALLOC_SIG  0x6c6d

#define LMEM_FNAME_LEN  MAX_PATH
typedef struct _LMEM_RECENT {
    HANDLE              hHeap;
    DWORD               dwTickCount;
    LPVOID              pData;
    DWORD               dwSize;
    DWORD               dwLineNum;
    CHAR                szFilename[LMEM_FNAME_LEN];
} LMEM_RECENT, *PLMEM_RECENT;

// Flags for SetAllocOptions
#define LMEMDBG_SHOW_LEAKS          0x00000001  // On HeapDestroy() show all remaining 
#define LMEMDBG_SHOW_OOM            0x00000002
#define LMEMDBG_BREAK_ON_OOM        0x00000010
#define LMEMDBG_BREAK_ON_FREE_NULL  0x00000020
#define LMEMDBG_BREAK_ON_LEAK       0x00000040
#define LMEMDBG_SHOW_CS_CONTENTIONS 0x00000080


typedef HANDLE (WINAPI *PFN_HeapCreate)(DWORD flOptions, DWORD dwInitialSize, DWORD dwMaximumSize);
HANDLE WINAPI Int_HeapCreate(DWORD flOptions, DWORD dwInitialSize, DWORD dwMaximumSize);

typedef BOOL (WINAPI *PFN_HeapDestroy)(HANDLE hHeap);
BOOL WINAPI Int_HeapDestroy(HANDLE hHeap);

typedef LPVOID (WINAPI *PFN_HeapAlloc) (HANDLE hHeap, DWORD dwFlags, DWORD dwBytes);
typedef LPVOID (WINAPI *PFN_HeapAllocTrace) (HANDLE hHeap, DWORD dwFlags, DWORD dwBytes, DWORD dwLineNum, PCHAR szFileName);
LPVOID WINAPI Int_HeapAlloc(HANDLE hHeap, DWORD dwFlags, DWORD dwBytes);

typedef LPVOID (WINAPI *PFN_HeapReAlloc)(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, DWORD dwBytes);
LPVOID WINAPI Int_HeapReAlloc(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, DWORD dwBytes);

typedef DWORD (WINAPI *PFN_HeapSize)(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem);
DWORD WINAPI Int_HeapSize(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem);

typedef BOOL (WINAPI *PFN_HeapFree)(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);
BOOL WINAPI Int_HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);

typedef BOOL (WINAPI *PFN_CloseHandle)(HANDLE hObject);
BOOL WINAPI Int_CloseHandle(HANDLE hObject);

typedef HANDLE (WINAPI *PFN_CreateEventW)(LPSECURITY_ATTRIBUTES lpsa, BOOL bManualReset, BOOL bInitialState, LPCWSTR lpName);
HANDLE WINAPI Int_CreateEventW(LPSECURITY_ATTRIBUTES lpsa, BOOL bManualReset, BOOL bInitialState, LPCWSTR lpName);

BOOL WINAPI LMEMAddTrackedItem (HANDLE hHeap, LPVOID pAllocMem, DWORD dwBytes, DWORD dwLineNum, PCHAR szFilename);
typedef BOOL (WINAPI *PFN_LMEMAddTrackedItem) (HANDLE hHeap, LPVOID pAllocMem, DWORD dwBytes, DWORD dwLineNum, PCHAR szFilename);
VOID WINAPI LMEMRemoveTrackedItem (HANDLE hHeap, LPVOID pAllocMem);
typedef VOID (WINAPI *PFN_LMEMRemoveTrackedItem) (HANDLE hHeap, LPVOID pAllocMem);

PLMEM_HEAP_INFO GetProcessHeapInfo(HANDLE hProc);
typedef PLMEM_HEAP_INFO (WINAPI *PFN_GetProcessHeapInfo)(HANDLE hProc);
PLMEM_ALLOC_INFO GetCurrentAllocs(HANDLE hProc, HANDLE hHeap);
typedef PLMEM_ALLOC_INFO (WINAPI *PFN_GetCurrentAllocs)(HANDLE hProc, HANDLE hHeap);
typedef PLMEM_RECENT (WINAPI *PFN_GetRecentAllocs)(HANDLE hProc, LPDWORD pdwNumAllocs);
VOID FreeHeapInfo(LPVOID pHeapInfo);
typedef VOID (WINAPI *PFN_FreeHeapInfo)(LPVOID pHeapInfo);
typedef BOOL (WINAPI *PFN_GetLMEMDbgOptions)(HANDLE hProc, LPDWORD pdwFlags, LPDWORD pdwBreakOnCount, LPDWORD pdwBreakOnSize);
typedef BOOL (WINAPI *PFN_SetLMEMDbgOptions)(HANDLE hProc, DWORD dwOp, DWORD dwFlags, DWORD dwBreakOnCount, DWORD dwBreakOnSize);
typedef BOOL (WINAPI *PFN_SetAllocFlags)(HANDLE hProc, HANDLE hHeap, LPVOID lpMem, DWORD dwOp, WORD wFlag);
typedef DWORD (WINAPI *PFN_SetAllAllocFlags)(HANDLE hProc, HANDLE hHeap, DWORD dwOp, WORD wFlag);

typedef BOOL (WINAPI *PFN_SetAllocAdditionalData)(HANDLE hHeap, LPVOID lpMem, WORD wType, WORD wLen, LPVOID pData);
BOOL SetAllocAdditionalData (HANDLE hHeap, LPVOID lpMem, WORD wType, WORD wLen, LPVOID pData);

#define LMEM_SET_ALLOC_FLAG_ASSIGN  0
#define LMEM_SET_ALLOC_FLAG_SET     1
#define LMEM_SET_ALLOC_FLAG_CLEAR   2

#if !defined(WIN32_CALL) && !defined(IN_KERNEL)
_inline BOOL
CallGetLMEMDbgOptions (HANDLE hProc, LPDWORD pdwFlags, LPDWORD pdwBreakOnCount, LPDWORD pdwBreakOnSize)
{
    HINSTANCE hLibInst;
    PFN_GetLMEMDbgOptions pfnGetLMEMDbgOptions;
    BOOL    fRetVal = FALSE;

    if (NULL != (hLibInst = LoadLibraryW(LMEMDEBUG_DLL))) {
        pfnGetLMEMDbgOptions = (PFN_GetLMEMDbgOptions)GetProcAddressW(hLibInst, L"GetLMEMDbgOptions");
        if (pfnGetLMEMDbgOptions) {
            fRetVal = pfnGetLMEMDbgOptions (hProc, pdwFlags, pdwBreakOnCount, pdwBreakOnSize);
        }
        FreeLibrary (hLibInst);
    }
    return fRetVal;
}
_inline BOOL
CallSetLMEMDbgOptions (HANDLE hProc, DWORD dwOp, DWORD dwFlags, DWORD dwBreakOnCount, DWORD dwBreakOnSize)
{
    HINSTANCE hLibInst;
    PFN_SetLMEMDbgOptions pfnSetLMEMDbgOptions;
    BOOL    fRetVal = FALSE;

    if (NULL != (hLibInst = LoadLibraryW(LMEMDEBUG_DLL))) {
        pfnSetLMEMDbgOptions = (PFN_SetLMEMDbgOptions)GetProcAddressW(hLibInst, L"SetLMEMDbgOptions");
        if (pfnSetLMEMDbgOptions) {
            fRetVal = pfnSetLMEMDbgOptions (hProc, dwOp, dwFlags, dwBreakOnCount, dwBreakOnSize);
        }
        FreeLibrary (hLibInst);
    }
    return fRetVal;
}
_inline BOOL
CallSetAllocFlags (HANDLE hProc, HANDLE hHeap, LPVOID lpMem, DWORD dwOp, WORD wFlag)
{
    HINSTANCE hLibInst;
    PFN_SetAllocFlags pfnSetAllocFlags;
    BOOL    fRetVal = FALSE;

    if (NULL != (hLibInst = LoadLibraryW(LMEMDEBUG_DLL))) {
        pfnSetAllocFlags = (PFN_SetAllocFlags)GetProcAddressW(hLibInst, L"SetAllocFlags");
        if (pfnSetAllocFlags) {
            fRetVal = pfnSetAllocFlags (hProc, hHeap, lpMem, dwOp, wFlag);
        }
        FreeLibrary (hLibInst);
    }
    return fRetVal;
}
// Shortcut for common case
#define LocalSetAllocFlags(lpMem,dwOp,wFlag) CallSetAllocFlags(GetCurrentProcessId(), GetProcessHeap(), lpMem, dwOp, wFlag)

_inline BOOL
CallSetAllAllocFlags (HANDLE hProc, HANDLE hHeap, DWORD dwOp, WORD wFlag)
{
    HINSTANCE hLibInst;
    PFN_SetAllAllocFlags pfnSetAllAllocFlags;
    DWORD    dwRetVal = 0;

    if (NULL != (hLibInst = LoadLibraryW(LMEMDEBUG_DLL))) {
        pfnSetAllAllocFlags = (PFN_SetAllAllocFlags)GetProcAddressW(hLibInst, L"SetAllAllocFlags");
        if (pfnSetAllAllocFlags) {
            dwRetVal = pfnSetAllAllocFlags (hProc, hHeap, dwOp, wFlag);
        }
        FreeLibrary (hLibInst);
    }
    return dwRetVal;
}


_inline BOOL
CallSetAllocAdditionalData(HANDLE hHeap, LPVOID lpMem, WORD wType, WORD wLen, LPVOID pData)
{
    HINSTANCE hLibInst;
    PFN_SetAllocAdditionalData pfnSetAllocAdditionalData;
    BOOL    fRetVal = FALSE;

    if (NULL != (hLibInst = LoadLibraryW(LMEMDEBUG_DLL))) {
        pfnSetAllocAdditionalData = (PFN_SetAllocAdditionalData)GetProcAddressW(hLibInst, L"SetAllocAdditionalData");
        if (pfnSetAllocAdditionalData) {
            fRetVal = pfnSetAllocAdditionalData(hHeap, lpMem, wType, wLen, pData);
        }
        FreeLibrary (hLibInst);
    }
    return fRetVal;
}
#endif
#ifdef __cplusplus
}
#endif

#endif // __LMEMDEBUG_H__
