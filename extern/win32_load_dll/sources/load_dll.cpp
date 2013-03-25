/*****************************************************************************/
/* load_dll.cpp                                            (c) XeNotRoN 2002 */
/*---------------------------------------------------------------------------*/
/* Use these functions to load simple DLL files that have only data and code */
/* in their sections and do not take any advantage of other DLL              */   
/* functionality (TLS, resources) that require a windows HMODULE. You can    */
/* import and export functions. Use the MyGetProcAddress() function to       */
/* get the function addresses of a DLL that was loaded by these functions.   */
/* See the load_dll.h file for detailed descriptions about the functions.    */
/*****************************************************************************/

#include <windows.h>
#include <winnt.h>
#include <stdlib.h>
#include <string.h>

#include "load_dll.h"




#define RAISE_EXCEPTION (RaiseException (0xE0000000, 0, 0, NULL))
#define RAISE_EXCEPTION_RESULT(return_result) (result = (return_result), RAISE_EXCEPTION)

#define READ(lpBuff,dwPos,dwSize) __try { \
            if (!lpfRead ((lpBuff), (dwPos), (dwSize), (lpvParam))) RAISE_EXCEPTION; \
        } __except (EXCEPTION_EXECUTE_HANDLER) { RAISE_EXCEPTION_RESULT (LOAD_DLL_READ_ERR); }

//------------------------------------------------------------------------------

DWORD LoadDLL (
    LOAD_DLL_READPROC   lpfRead,    // Address of our read procedure.
    void                *lpvParam,  // A parameter that will be passed to lpfRead()
    DWORD               dwFlags,    // Zero or combination of DLL_XXX flags.
    LOAD_DLL_INFO       *lpInfo     // Pointer to structure that receives DLL info
    )                               // on success. It can be NULL.
{
    DWORD                   result = LOAD_DLL_UNKNOWN_ERR;

    IMAGE_DOS_HEADER        dos_hdr;
    IMAGE_NT_HEADERS        hdr;
    IMAGE_SECTION_HEADER    *sect, *s;

    __try
    {
        if (!lpfRead) RAISE_EXCEPTION_RESULT (LOAD_DLL_BADPARAMS);

        /*
        // Read and check the DOS header...
        */

        READ (&dos_hdr, 0, sizeof (dos_hdr));
        if (dos_hdr.e_magic != IMAGE_DOS_SIGNATURE || !dos_hdr.e_lfanew) 
            RAISE_EXCEPTION_RESULT (LOAD_DLL_INVALID_IMAGE);

        /*
        // Read and check the NT header...
        */

        READ (&hdr, dos_hdr.e_lfanew, sizeof (hdr));
        if (hdr.Signature != IMAGE_NT_SIGNATURE ||
            hdr.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC ||
            !hdr.FileHeader.NumberOfSections)
            RAISE_EXCEPTION_RESULT (LOAD_DLL_INVALID_IMAGE);

        /*
        // Allocating memory and reading section headers.
        */

        DWORD size_section_headers = hdr.FileHeader.NumberOfSections * sizeof (IMAGE_SECTION_HEADER);
        sect = (IMAGE_SECTION_HEADER*)malloc (size_section_headers);
        if (!sect) RAISE_EXCEPTION_RESULT (LOAD_DLL_MEMORY_ERR);

        __try
        {
            DWORD file_offset_section_headers = dos_hdr.e_lfanew + sizeof (DWORD) + 
                sizeof (IMAGE_FILE_HEADER) + hdr.FileHeader.SizeOfOptionalHeader;

            READ (sect, file_offset_section_headers, size_section_headers);

            /*
            // We do not trust the value of hdr.OptionalHeader.SizeOfImage so we
            // calculate our own SizeOfImage. This is the size of the continuous
            // memory block that can hold the headers and all sections.
            // We will search the lowest and highest RVA among the section boundaries.
            // if we must load the header then its RVA will be the lowest (zero)
            // and its size will be (file_offset_section_headers + size_section_headers).
            */

            DWORD   RVAlow = (dwFlags & DLL_NO_HEADERS) ? 0xFFFFFFFF : 0, RVAhigh = 0;
            WORD    i;

            for (i=0,s=sect; i<hdr.FileHeader.NumberOfSections; i++,s++)
            {
                if (!s->Misc.VirtualSize) continue;
                if (s->VirtualAddress < RVAlow) RVAlow = s->VirtualAddress;
                if ((s->VirtualAddress + s->Misc.VirtualSize) > RVAhigh)
                    RVAhigh = s->VirtualAddress + s->Misc.VirtualSize;
            }

            /*
            // RVAlow and RVA high holds the boundaries of the memory block
            // needed to load the sections & header of the image. Now we allocate
            // the memory block to load the sections and additionally the headers.
            // We will try to allocate the block at the preferred image base
            // to avoid doing the base relocations.
            */

            LPVOID image = VirtualAlloc (
                (LPVOID)(hdr.OptionalHeader.ImageBase + RVAlow),
                RVAhigh - RVAlow,
                MEM_COMMIT | MEM_RESERVE,
                PAGE_EXECUTE_READWRITE
                );


            DWORD dwImageBase = hdr.OptionalHeader.ImageBase;

            /*
            // Note: image may differ from the address
            // (LPVOID)(hdr.OptionalHeader.ImageBase + RVAlow)
            // cause windows rounds down addresses to next page
            // boundary.
            */

            if (!image)
            {
                /*
                // If allocation at preferred imagebase failed then we let
                // windows to allocate memory at arbitrary location.
                // Unfortunately we can not skip doing base relocations so
                // we are in crap if the relocation info is stripped from
                // the DLL. :(
                */

                if (hdr.FileHeader.Characteristics & IMAGE_FILE_RELOCS_STRIPPED)
                    RAISE_EXCEPTION_RESULT (LOAD_DLL_RELOC_ERR);

                image = VirtualAlloc (
                    NULL, 
                    RVAhigh - RVAlow,
                    MEM_COMMIT | MEM_RESERVE, 
                    PAGE_EXECUTE_READWRITE);

                dwImageBase = (DWORD)image - RVAlow;
            }

            if (!image) RAISE_EXCEPTION_RESULT (LOAD_DLL_MEMORY_ERR);

            __try
            {
                /*
                // Loading the header if required.
                */

                if (!(dwFlags & DLL_NO_HEADERS))
                    READ ((LPVOID)dwImageBase, 0, file_offset_section_headers + size_section_headers);

                /*
                // Loading the sections.
                */

                for (i=0,s=sect; i<hdr.FileHeader.NumberOfSections; i++,s++)
                {
                    DWORD dwSectionSize = min (s->Misc.VirtualSize, s->SizeOfRawData);

                    if (dwSectionSize) READ (
                        (LPVOID)(s->VirtualAddress + dwImageBase),
                        s->PointerToRawData,
                        dwSectionSize
                        );
                }

                /*
                // Do we need to do the base relocations?
                // Are there any relocatable items?
                */

                if (dwImageBase != hdr.OptionalHeader.ImageBase &&
                    hdr.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress &&
                    hdr.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size)
                {
                    __try
                    {
                        DWORD dwDiff = dwImageBase - hdr.OptionalHeader.ImageBase;
                        IMAGE_BASE_RELOCATION *reloc, *r = (IMAGE_BASE_RELOCATION*)(dwImageBase 
                            + hdr.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);

                        reloc = (IMAGE_BASE_RELOCATION*)((DWORD)r
                            + hdr.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size
                            - sizeof (IMAGE_BASE_RELOCATION));

                        /*
                        // r: current position inside base relocation data.
                        // reloc: We should not start another block if (r >= reloc)
                        */
                   
                        for (; r<reloc; r=(IMAGE_BASE_RELOCATION*)((DWORD)r + r->SizeOfBlock))
                        {
                            WORD *reloc_item = (WORD*)(r + 1);

                            for (DWORD i = 0, dwNumItems = (r->SizeOfBlock - sizeof (IMAGE_BASE_RELOCATION)) >> 1;
                                 i < dwNumItems;
                                 i++, reloc_item++)
                            {
                                switch (*reloc_item >> 12)
                                {
                                case IMAGE_REL_BASED_ABSOLUTE:
                                    break;

                                case IMAGE_REL_BASED_HIGHLOW:
                                    *(DWORD*)(dwImageBase + r->VirtualAddress + (*reloc_item & 0xFFF)) += dwDiff;
                                    break;

                                default:
                                    RAISE_EXCEPTION;
                                }
                            }
                        }
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER)
                    {
                        RAISE_EXCEPTION_RESULT (LOAD_DLL_BAD_RELOC);
                    }
                }

                /*
                // Resolving the imports of the module (if any xD)...
                */

                if (hdr.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress &&
                    hdr.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size)
                {

                    // Return code in case of an unwanted exception...
                    result = LOAD_DLL_BAD_RELOC;


                    IMAGE_IMPORT_DESCRIPTOR *import_desc = (IMAGE_IMPORT_DESCRIPTOR*)(dwImageBase
                        + hdr.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

                    for (; import_desc->Name; import_desc++)
                    {
                        HMODULE hDLL = LoadLibrary ((char*)(dwImageBase + import_desc->Name));
                        if (!hDLL) RAISE_EXCEPTION_RESULT (LOAD_DLL_IMPORT_ERR);

                        /*
                        // We do not trust bound imports. If FirstThunk is not bound then
                        // we use that to find the function ordinals/names. If its bound
                        // we must use OriginalFirstThunk. If FirstThunk is bound and
                        // OriginalFirstThunk is not present then we return with error.
                        */

                        DWORD *src_iat, *dest_iat = (DWORD*)(dwImageBase + import_desc->FirstThunk);

                        if (import_desc->TimeDateStamp)
                        {
                            if (!import_desc->OriginalFirstThunk)
                                RAISE_EXCEPTION_RESULT (LOAD_DLL_BOUND_IMPORT_ERR);

                            src_iat = (DWORD*)(dwImageBase + import_desc->OriginalFirstThunk);
                        }
                        else
                        {
                            src_iat = dest_iat;
                        }
                            

                        for (; *src_iat; src_iat++,dest_iat++)
                        {
                            *dest_iat = (DWORD)GetProcAddress (hDLL, (LPCSTR)
                                ((*src_iat & IMAGE_ORDINAL_FLAG32) ? IMAGE_ORDINAL32(*src_iat) : dwImageBase + *src_iat + 2));

                            if (!*dest_iat) RAISE_EXCEPTION_RESULT (LOAD_DLL_IMPORT_ERR);
                        }
                    }

                }


                /*
                // Setting the protection of memory pages. We leave the protection
                // of header PAGE_EXECUTE_READWRITE.
                // Note: We must convert the section characterictics flag into
                // memory page protection flag of VirtualProtect().
                // In the characterictics flags of a section there are 4 bits that are
                // important for us now:
                //
                // IMAGE_SCN_CNT_CODE       0x00000020
                // IMAGE_SCN_MEM_EXECUTE    0x20000000
                // IMAGE_SCN_MEM_READ       0x40000000
                // IMAGE_SCN_MEM_WRITE      0x80000000
                //
                // IMAGE_SC_CNT_CODE is equivalent to (IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ)
                // so we have only 3 flags that means 8 cases.
                */

                __try
                {
                    for (i=0,s=sect; i<hdr.FileHeader.NumberOfSections; i++,s++)
                    {
                        if (s->Characteristics & IMAGE_SCN_CNT_CODE)
                            s->Characteristics |= IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ;

                        DWORD   dwProtection;

                        /*
                        // We rotate the upper 3 important bits down so the resulting value
                        // is in the range 0-7.
                        */
                        switch ((DWORD)s->Characteristics >> (24+5))
                        {
                            /*
                            // meaning of bits:
                            // 1: execute, 2: read, 4: write
                            */

                        case 1: dwProtection = PAGE_EXECUTE; break;

                        case 0: // case 0: should we use PAGE_NOACCESS? What is it good for?
                        case 2: dwProtection = PAGE_READONLY; break;
            
                        case 3: dwProtection = PAGE_EXECUTE_READ; break;
            
                        case 4:
                        case 6: dwProtection = PAGE_READWRITE; break;

                        case 5:
                        default: dwProtection = PAGE_EXECUTE_READWRITE; break;
                        }

                        if (!VirtualProtect (
                                (LPVOID)(dwImageBase + s->VirtualAddress),
                                s->Misc.VirtualSize,
                                dwProtection,
                                &dwProtection
                                ))
                                RAISE_EXCEPTION;
                    }
                }
                __except (EXCEPTION_EXECUTE_HANDLER)
                {
                    RAISE_EXCEPTION_RESULT (LOAD_DLL_PROTECT_ERR);
                }


                /*
                // Do we have to call the entrypoint of the DLL?
                // If yes then we do the DLL_PROCESS_ATTACH notification but
                // if it returns FALSE we unload the whole image immediately.
                */

                LPDLLMAIN lpfDllMain = NULL;
                if (hdr.OptionalHeader.AddressOfEntryPoint)
                    lpfDllMain = (LPDLLMAIN)(hdr.OptionalHeader.AddressOfEntryPoint + dwImageBase);

                if (lpfDllMain && !(dwFlags & DLL_NO_ENTRY_CALL))
                {
                    BOOL bDllEntryResult;

                    __try
                    {
                        bDllEntryResult = lpfDllMain (dwImageBase, DLL_PROCESS_ATTACH, NULL);
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER)
                    {
                        bDllEntryResult = FALSE;
                    }

                    if (!bDllEntryResult) RAISE_EXCEPTION_RESULT (LOAD_DLL_DLLENTRY_ERR);
                }

                /*
                // We finished!!! :)
                // Filling in the callers info structure...
                */

                if (lpInfo)
                {
                    __try
                    {
                        lpInfo->dwSize = sizeof (LOAD_DLL_INFO);
                        lpInfo->dwFlags = dwFlags;
                        lpInfo->dwImageBase = dwImageBase;
                        lpInfo->lpvMemBlock = image;
                        lpInfo->lpfDllMain = lpfDllMain;
                        lpInfo->dwExportDirRVA = 
                          hdr.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) 
                    {
                        RAISE_EXCEPTION_RESULT (LOAD_DLL_BADPARAMS);
                    }
                }

                result = LOAD_DLL_OK;
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                VirtualFree (image, 0, MEM_RELEASE);
            }
        }
        __finally
        {
            free (sect);
        }

    }
    __except (EXCEPTION_EXECUTE_HANDLER) {}

    return result;
}


//------------------------------------------------------------------------------

BOOL UnloadDLL (
    LOAD_DLL_INFO       *lpInfo,    // Idetify the module to unload.
    DWORD dwFlags                   // Zero or combination of DLL_XXX flags.
    )
{
    BOOL result = TRUE;

    __try
    {
        if (!lpInfo || lpInfo->dwSize != sizeof LOAD_DLL_INFO ||
            !lpInfo->dwImageBase || !lpInfo->lpvMemBlock)
            return FALSE;

        /*
        // Should we call DllMain() with DLL_PROCESS_DETACH?
        */

        if (!(dwFlags & DLL_NO_ENTRY_CALL) && lpInfo->lpfDllMain)
        {
            __try
            {
                result = lpInfo->lpfDllMain (lpInfo->dwImageBase, DLL_PROCESS_DETACH, NULL);
            }
            __except (EXCEPTION_EXECUTE_HANDLER) 
            {
                result = FALSE;
            }
        }

        /*
        // Free the memory block.
        */

        VirtualFree (lpInfo->lpvMemBlock, 0, MEM_RELEASE);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) 
    {
        result = FALSE;
    }

    return result;
}




//------------------------------------------------------------------------------

    /*
    // LoadDLLFromFile...
    */

    typedef struct _LOAD_DLL_FROM_FILE_STRUCT {
        FILE    *f;
        DWORD   dwDLLOffset;
        DWORD   dwDLLSize;
    } LOAD_DLL_FROM_FILE_STRUCT;

static BOOL LoadDLLFromFileCallback (void *lpBuff, DWORD dwPosition, 
                    DWORD dwSize, LOAD_DLL_FROM_FILE_STRUCT *lpvParam)
{
    if (!dwSize) return TRUE;
    if ((dwPosition + dwSize) > lpvParam->dwDLLSize) return FALSE;
    fseek (lpvParam->f, lpvParam->dwDLLOffset + dwPosition, SEEK_SET);
    return fread (lpBuff, 1, dwSize, lpvParam->f) == dwSize;
}


DWORD LoadDLLFromFile (FILE *f, DWORD dwDLLOffset, DWORD dwDLLSize,
    DWORD dwFlags, LOAD_DLL_INFO *lpInfo)
{
    if (dwDLLSize == DLL_SIZE_UNK) dwDLLSize -= dwDLLOffset;
    LOAD_DLL_FROM_FILE_STRUCT ldffs = { f, dwDLLOffset, dwDLLSize };
    return LoadDLL ((LOAD_DLL_READPROC)&LoadDLLFromFileCallback, &ldffs, dwFlags, lpInfo);
}


DWORD LoadDLLFromFile (char *lpszFileName, DWORD dwDLLOffset, DWORD dwDLLSize,
    DWORD dwFlags, LOAD_DLL_INFO *lpInfo)
{
    FILE *f = fopen (lpszFileName, "rb");
    if (!f) return LOAD_DLL_FILE_NOT_FOUND;

    DWORD res;

    __try
    {
        res = LoadDLLFromFile (f, dwDLLOffset, dwDLLSize, dwFlags, lpInfo);
    }
    __finally
    {
        fclose (f);
    }

    return res;
}


    /*
    // LoadDLLFromMemory stuff...
    */

    typedef struct _LOAD_DLL_FROM_MEMORY_STRUCT {
        void    *lpvDLLData;
        DWORD   dwDLLSize;
    } LOAD_DLL_FROM_MEMORY_STRUCT;


static BOOL LoadDLLFromMemoryCallback (void *lpBuff, DWORD dwPosition, 
                        DWORD dwSize, LOAD_DLL_FROM_MEMORY_STRUCT*lpvParam)
{
    if (!dwSize) return TRUE;
    if ((dwPosition + dwSize) > lpvParam->dwDLLSize) return FALSE;
    memcpy (lpBuff, (char*)lpvParam->lpvDLLData + dwPosition, dwSize);
    return TRUE;
}


DWORD LoadDLLFromMemory (void *lpvDLLData, DWORD dwDLLSize,
    DWORD dwFlags, LOAD_DLL_INFO *lpInfo)
{
    if (dwDLLSize == DLL_SIZE_UNK) dwDLLSize -= (DWORD)lpvDLLData;
    LOAD_DLL_FROM_MEMORY_STRUCT ldfms = { lpvDLLData, dwDLLSize };
    return LoadDLL ((LOAD_DLL_READPROC)&LoadDLLFromMemoryCallback, &ldfms, dwFlags, lpInfo);
}


//------------------------------------------------------------------------------

    /*
    // GetProcAddress functions:
    */


FARPROC MyGetProcAddress (LOAD_DLL_INFO *lpInfo, LPCSTR lpProcName)
{
    return MyGetProcAddress (lpInfo->dwExportDirRVA, lpInfo->dwImageBase, lpProcName);
}

FARPROC MyGetProcAddress (HMODULE hModule, LPCSTR lpProcName)
{
    __try
    {
        if (((IMAGE_DOS_HEADER*)hModule)->e_magic != IMAGE_DOS_SIGNATURE) return NULL;
        IMAGE_NT_HEADERS *hdr = (IMAGE_NT_HEADERS*)((DWORD)hModule 
            + ((IMAGE_DOS_HEADER*)hModule)->e_lfanew);

        if (hdr->Signature != IMAGE_NT_SIGNATURE ||
            hdr->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
            return NULL;

        return MyGetProcAddress (
            hdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress,
            (DWORD)hModule,
            lpProcName
            );
    }
    __except (EXCEPTION_EXECUTE_HANDLER) 
    {
        return NULL;
    }
}


FARPROC MyGetProcAddress (DWORD dwExportDirRVA, DWORD dwImageBase, LPCSTR lpProcName)
{
    if (!dwExportDirRVA) return NULL;
    IMAGE_EXPORT_DIRECTORY *exp = (IMAGE_EXPORT_DIRECTORY*)(dwImageBase + dwExportDirRVA);
    DWORD dwOrd = (DWORD)lpProcName;

    __try
    {
        if (dwOrd < 0x10000)
        {
            /*
            // Search for ordinal;
            */

            if (dwOrd < exp->Base) return NULL;
            dwOrd -= exp->Base;
        }
        else
        {
            /*
            // Search for name:
            */

            for (DWORD i=0; i<exp->NumberOfNames; i++)
                if ( !strcmp( (char*)(((DWORD*)(exp->AddressOfNames + dwImageBase))[i] + dwImageBase),
                              lpProcName) )
            {
                dwOrd = ((WORD*)(exp->AddressOfNameOrdinals + dwImageBase))[i];
                break;    
            }

        }

        if (dwOrd >= exp->NumberOfFunctions) return NULL;
        return (FARPROC)(((DWORD*)(exp->AddressOfFunctions 
            + dwImageBase))[dwOrd] + dwImageBase);
    
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return NULL;
    }
}
