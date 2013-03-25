/*****************************************************************************/
/* load_dll.h                                              (c) XeNotRoN 2002 */
/*---------------------------------------------------------------------------*/
/* Use these functions to load simple DLL files that have only data and code */
/* in their sections and do not take any advantage of other DLL              */   
/* functionality (TLS, resources) that require a windows HMODULE. You can    */
/* import and export functions. Use the MyGetProcAddress() function to       */
/* get the function addresses of a DLL that was loaded by these functions.   */
/*****************************************************************************/

#ifndef load_dll_h
#define load_dll_h

#include <windows.h>
#include <stdio.h>

    /*
    // Conventions:
    // RVA: relative virtual address
    // VA: virtual address that is RVA+ImageBase
    //
    // LOAD_DLL_INFO
    // A structure that holds information about a "manually loaded" DLL.
    //
    // * dwSize: size of the structure.
    // * dwFlags: The dwFlags parameter that was passed to the LoadDLL()
    //   function.
    // * dwImageBase: VA that was used to do the base relocation.
    //   It is always the same as lpvMemBlock when you loaded the DLL
    //   including the headers.
    // * lpvMemBlock: VA of the big memory block that was allocated to
    //   hold the data of sections and additionally the headers if we
    //   want.
    // * lpfDllMain: VA of DllMain(). Be careful cause it can be NULL.
    // * dwExportDirRVA: RVA of the export directory of the DLL.
    //   It can be null if there is no export info. It is needed
    //   when you want to load a DLL without header but the address
    //   of exported functions must be retrieved.
    */


    typedef BOOL (WINAPI *LPDLLMAIN) (DWORD dwImageBase, DWORD fdwReason, LPVOID lpvReserved);

    typedef struct _LOAD_DLL_INFO {
        DWORD   dwSize;
        DWORD   dwFlags;
        DWORD   dwImageBase;
        void    *lpvMemBlock;
        LPDLLMAIN lpfDllMain;
        DWORD   dwExportDirRVA;
    } LOAD_DLL_INFO;


    /*
    // LOAD_DLL_READPROC:
    // The type of callback procedure we must support for LoadDLL().
    // It reads dwSize number of bytes to lpBuff from the DLL data after
    // seeking to dwPosition. It must return TRUE on success and FALSE
    // if the specified number of bytes could not be read.
    // LoadDLL() will assume that the DLL file's DOS header is at zero
    // dwPosition. We can use this callback easily to load module from
    // both file or memory.
    // The lpvParam receives the lpvParam value we pass to LoadDLL. This
    // is useful when you are multithreading.
    */

    typedef BOOL (*LOAD_DLL_READPROC) (void *lpBuff, DWORD dwPosition, DWORD dwSize, void *lpvParam);


    /*
    // LoadDLL returns an error code that is zero on success. It loads a DLL file
    // that must be readed by lpfRead() at zero position from its on stream.
    // If LoadDLL returns without error, then the structure pointed to by lpInfo
    // will be filled out with information unless this pointer is NULL.
    //
    // UnloadDLL unloads the module and returns TRUE if everything is OK.
    */

    DWORD LoadDLL (
        LOAD_DLL_READPROC   lpfRead,    // Address of our read procedure.
        void                *lpvParam,  // A parameter that will be passed to lpfRead()
        DWORD               dwFlags,    // Zero or combination of DLL_XXX flags.
        LOAD_DLL_INFO       *lpInfo     // Pointer to structure that recives DLL info
        );                              // on success. It can be NULL.

    /*
    // Some LoadDLL() functions if you don't want to mess with callback functions.
    // They provide you the ability to load a DLL from file or from memory.
    // * LoadDLLFromFile:
    //   Opens a file and loads a DLL whose data is placed in the file at
    //   offset dwDLLOffset. The size of the DLL is dwDLLSize bytes.
    // * LoadDLLFromMemory:
    //   We assume that you read the data of a DLL file to memory.
    //   lpvDLLData points to it and dwDLLSize it its size in bytes.
    //
    // You can use the DLL_SIZE_UNK constant as dwDLLSize parameter if you
    // do not know the size of the DLL and you let to read any number of
    // bytes starting from dwDLLOffset. Only the required amount of data
    // will be read from the file or memory buffer not more. (Headers, sections)
    //
    // All these funcs are thread safe.
    */

#define DLL_SIZE_UNK            0xFFFFFFFF

    DWORD LoadDLLFromFile (char *lpszFileName, DWORD dwDLLOffset, DWORD dwDLLSize,
        DWORD dwFlags, LOAD_DLL_INFO *lpInfo);

    DWORD LoadDLLFromFile (FILE *f, DWORD dwDLLOffset, DWORD dwDLLSize,
        DWORD dwFlags, LOAD_DLL_INFO *lpInfo);

    DWORD LoadDLLFromMemory (void *lpvDLLData, DWORD dwDLLSize,
        DWORD dwFlags, LOAD_DLL_INFO *lpInfo);

    /*
    // Unload a succesfully loaded module:
    */

    BOOL UnloadDLL (
        LOAD_DLL_INFO       *lpInfo,    // Idetify the module to unload.
        DWORD dwFlags                   // Zero or combination of DLL_XXX flags.
        );

    /*
    // LoadDLL and UnloadDLL dwFlags:
    // * DLL_NO_ENTRY_CALL
    //   Do not call the entry of the DLL with DLL_PROCESS_ATTACH
    //   or DLL_PROCESS_DETACH (flag for both LoadDLL and UnloadDLL);
    // * DLL_NO_HEADERS
    //   Do not load the DOS/NT/SECTION headers, only sections.
    //   (only for LoadDLL)
    */

#define DLL_NO_ENTRY_CALL               0x00000001
#define DLL_NO_HEADERS                  0x00000002


    /*
    // LoadDLL results:
    // * LOAD_DLL_OK:
    //   Success.
    // * LOAD_DLL_READ_ERR:
    //   The lpfRead() procedure we provided returned FALSE for a
    //   read request.
    // * LOAD_DLL_INVALID_IMAGE:
    //   Bad DLL file. Wrong header, or something similar error.
    // * LOAD_DLL_MEMORY_ERR:
    //   Memory allocation error.
    // * LOAD_DLL_RELOC_ERR:
    //   The DLL could not be loaded to the preferred imagebase and
    //   there is no base relocation info in the module to load
    //   to another address.
    // * LOAD_DLL_BAD_RELOC:
    //   The DLL relocation data is bad.
    // * LOAD_DLL_IMPORT_ERR:
    //   An imported DLL or function was not found.
    // * LOAD_DLL_BAD_IMPORT:
    //   Bad import data.
    // * LOAD_DLL_BOUND_IMPORT_ERR:
    //   We do not support import directories that contain
    //   only bound import info.
    // * LOAD_DLL_PROTECT_ERR:
    //   Error setting the memory page protection of loaded and
    //   relocated sections.
    // * LOAD_DLL_DLLENTRY_ERR:
    //   The DllMain() returned FALSE or caused an exception.
    // * LOAD_DLL_FILE_NOT_FOUND:
    //   It is returned by LoadDLLFromFile if the specified
    //   file can not be opened.
    // * LOAD_DLL_UNKNOWN_ERR:
    //   An unhandled exception occured. This should never happen.
    // * LOAD_DLL_BADPARAMS:
    //   Bad parameters. (lpfRead == NULL) This error should never
    //   happen. :)
    */

#define LOAD_DLL_OK                     0x00000000
#define LOAD_DLL_READ_ERR               0x00000001
#define LOAD_DLL_INVALID_IMAGE          0x00000002
#define LOAD_DLL_MEMORY_ERR             0x00000003
#define LOAD_DLL_RELOC_ERR              0x00000004
#define LOAD_DLL_BAD_RELOC              0x00000005
#define LOAD_DLL_IMPORT_ERR             0x00000006
#define LOAD_DLL_BAD_IMPORT             0x00000007
#define LOAD_DLL_BOUND_IMPORT_ERR       0x00000008
#define LOAD_DLL_PROTECT_ERR            0x00000009
#define LOAD_DLL_DLLENTRY_ERR           0x0000000A
#define LOAD_DLL_FILE_NOT_FOUND         0xFFFFFFFD
#define LOAD_DLL_UNKNOWN_ERR            0xFFFFFFFE
#define LOAD_DLL_BADPARAMS              0xFFFFFFFF

    /*
    // GetProcAddress functions.
    */

    FARPROC MyGetProcAddress (LOAD_DLL_INFO *lpInfo, LPCSTR lpProcName);
    FARPROC MyGetProcAddress (HMODULE hModule, LPCSTR lpProcName);
    FARPROC MyGetProcAddress (DWORD dwExportDirRVA, DWORD dwImageBase, LPCSTR lpProcName);


#endif /* load_dll_h */
