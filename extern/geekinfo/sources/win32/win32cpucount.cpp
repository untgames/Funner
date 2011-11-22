// Copyright (c) 2005 Intel Corporation 
// All Rights Reserved
//
// CPUCount.cpp : Detects three forms of hardware multi-threading support across IA-32 platform
//					The three forms of HW multithreading are: Multi-processor, Multi-core, and 
//					HyperThreading Technology.
//					This application enumerates all the logical processors enabled by OS and BIOS,
//					determine the HW topology of these enabled logical processors in the system 
//					using information provided by CPUID instruction.
//					A multi-processing system can support any combination of the three forms of HW
//					multi-threading support. The relevant topology can be identified using a 
//					three level decomposition of the "initial APIC ID" into 
//					Package_id, core_id, and SMT_id. Such decomposition provides a three-level map of 
//					the topology of hardware resources and
//					allow multi-threaded software to manage shared hardware resources in 
//					the platform to reduce resource contention

//					Multicore detection algorithm for processor and cache topology requires
//					all leaf functions of CPUID instructions be available. System administrator
//					must ensure BIOS settings is not configured to restrict CPUID functionalities.
//-------------------------------------------------------------------------------------------------

// Modifications for Win32 geekinfo by Xo W. (GHF):
// -all linux stuff taken out
// -inline assembly not usable with 64-bit MSVC compiler replace with intrinsics where possible and
// -put into a separate file if necessary (or convenient, really)

#include "win32cpucount.h"
#include <intrin.h>

#define HWD_MT_BIT         0x10000000     // EDX[28]  Bit 28 is set if HT or multi-core is supported
#define NUM_LOGICAL_BITS   0x00FF0000     // EBX[23:16] Bit 16-23 in ebx contains the number of logical
                                          // processors per physical processor when execute cpuid with 
                                          // eax set to 1
#define NUM_CORE_BITS      0xFC000000     // EAX[31:26] Bit 26-31 in eax contains the number of cores minus one
                                          // per physical processor when execute cpuid with 
                                          // eax set to 4. 


#define INITIAL_APIC_ID_BITS  0xFF000000  // EBX[31:24] Bits 24-31 (8 bits) return the 8-bit unique 
                                          // initial APIC ID for the processor this code is running on.
                                          


// Status Flag
#define SINGLE_CORE_AND_HT_ENABLED					1
#define SINGLE_CORE_AND_HT_DISABLED					2
#define SINGLE_CORE_AND_HT_NOT_CAPABLE					4
#define MULTI_CORE_AND_HT_NOT_CAPABLE					5
#define MULTI_CORE_AND_HT_ENABLED					6
#define MULTI_CORE_AND_HT_DISABLED					7
#define USER_CONFIG_ISSUE						8


unsigned int  CpuIDSupported(void);
unsigned int  GenuineIntel(void);
unsigned int  HWD_MTSupported(void);
unsigned int  MaxLogicalProcPerPhysicalProc(void);
unsigned int  MaxCorePerPhysicalProc(void);
unsigned int  find_maskwidth(unsigned int);
unsigned char GetAPIC_ID(void);
unsigned char GetNzbSubID(unsigned char,
						  unsigned char,
						  unsigned char);

unsigned char CPUCount(unsigned int *,
					   unsigned int *,
					   unsigned int *);

#include <windows.h>
#include <stdio.h>
#include <assert.h>

char g_s3Levels[2048];

//
// CpuIDSupported will return 0 if CPUID instruction is unavailable. Otherwise, it will return 
// the maximum supported standard function.
//
unsigned int CpuIDSupported(void)
{
//	unsigned int MaxInputValue;
// If CPUID instruction is supported
/*	try
	{
		MaxInputValue = 0;
		// call cpuid with eax = 0
		__asm
		{
			xor eax, eax
			cpuid
			mov MaxInputValue, eax
		}
	}
	catch (...)
	{
		return(0);                   // cpuid instruction is unavailable
	}*/

	return 1;

}


//
// GenuineIntel will return 0 if the processor is not a Genuine Intel Processor 
//
unsigned int GenuineIntel(void)
{
	unsigned int VendorID[4] = {0, 0, 0, 0};
	try    // If CPUID instruction is supported
	{
        /*
		__asm		
		{
			xor eax, eax			
        	cpuid					
			mov VendorID, ebx
			mov VendorID + 4, edx
			mov VendorID + 8, ecx
		}
		*/
		__cpuid((int *)VendorID, 0);
	}		
	catch (...)
	{
		return(0);      		unsigned int MaxInputValue =0;
		             // cpuid instruction is unavailable
	}
	return  ((VendorID[1] == 'uneG') &&
			 (VendorID[3] == 'Ieni') &&
			 (VendorID[2] == 'letn'));
}



//
// Function returns the maximum cores per physical package. Note that the number of 
// AVAILABLE cores per physical to be used by an application might be less than this
// maximum value.
//

unsigned int MaxCorePerPhysicalProc(void)
{
	unsigned int r[4] = {0, 0, 0, 0};   
	unsigned int Regeax        = 0;
	
  if (!HWD_MTSupported()) {
    return (unsigned int) 1;  // Single core
  }

  __cpuid((int *)r, 0);
  if (r[0] >= 4) {
    r[0] = 4; r[2] = 0;
    __cpuid((int *)r, 4);
    Regeax = r[0];
  }
  
  return (unsigned int)((Regeax & NUM_CORE_BITS) >> 26)+1;
}

//
// The function returns 0 when the hardware multi-threaded bit is not set.
//
unsigned int HWD_MTSupported(void)
{
    unsigned int CPUInfo[4] = { 0, 0, 0, 0 };
	unsigned int Regedx      = 0;


	if ((CpuIDSupported() >= 1) && GenuineIntel())
	{
		/*
        __asm
		{
			mov eax, 1
			cpuid
			mov Regedx, edx
		}
        */
		__cpuid((int *)CPUInfo, 1);
	}

	return (CPUInfo[3] & HWD_MT_BIT);  

  
}

//
// Function returns the maximum logical processors per physical package. Note that the number of 
// AVAILABLE logical processors per physical to be used by an application might be less than this
// maximum value.
//
unsigned int MaxLogicalProcPerPhysicalProc(void)
{
    unsigned int CPUInfo[4] = { 0, 0, 0, 0 };

	if (!HWD_MTSupported()) return (unsigned int) 1;
	    /*
		__asm
		{
			mov eax, 1
			cpuid
			mov Regebx, ebx
		}
		*/
		__cpuid((int *)CPUInfo, 1);
		
		return (unsigned int) ((CPUInfo[1] & NUM_LOGICAL_BITS) >> 16);

}


unsigned char GetAPIC_ID(void)
{
    unsigned int CPUInfo[4] = { 0, 0, 0, 0 };
    
    /*
	__asm
	{
		mov eax, 1
		cpuid
		mov Regebx, ebx
	}                             
    */
    
    __cpuid((int *)CPUInfo, 1);
    
	return (unsigned char) ((CPUInfo[1] & INITIAL_APIC_ID_BITS) >> 24);

}

int mostSignificantBitPosition(unsigned int n) {
  int pos = 0;
  int tmp;
#if 0 
  tmp = n >> 32;
  if (tmp != 0) { n = tmp; pos = pos + 32; }
#endif
  tmp = n >> 16;
  if (tmp != 0) { n = tmp; pos = pos + 16; }
  tmp = n >> 8;
  if (tmp != 0) { n = tmp; pos = pos + 8; }
  tmp = n >> 4;
  if (tmp != 0) { n = tmp; pos = pos + 4; }
  tmp = n >> 2;
  if (tmp != 0) { n = tmp; pos = pos + 2; }
  tmp = n >> 1;
  if (tmp != 0) { n = tmp; pos = pos + 1; }
  return pos + n - 1;
} // From Hacker's Delight

//
// Determine the width of the bit field that can represent the value count_item. 
//
unsigned int find_maskwidth(unsigned int CountItem)
{ // this somehow works better than the original
	if (CountItem == 0)
		return 1;
	/*unsigned int MaskWidth,
				 count = CountItem;
	__asm
	{
		mov eax, count
		mov ecx, 0
		mov MaskWidth, ecx
		dec eax
		bsr cx, ax
		jz next
		inc cx
		mov MaskWidth, ecx
next:
		
	}*/

	return 1+mostSignificantBitPosition(--CountItem);
}

//
// Extract the subset of bit field from the 8-bit value FullID.  It returns the 8-bit sub ID value
//
unsigned char GetNzbSubID(unsigned char FullID,
						  unsigned char MaxSubIDValue,
						  unsigned char ShiftCount)
{
	unsigned int MaskWidth;
	unsigned char MaskBits;

	MaskWidth = find_maskwidth((unsigned int) MaxSubIDValue);
	MaskBits  = (0xff << ShiftCount) ^ 
				((unsigned char) (0xff << (ShiftCount + MaskWidth)));

	return (FullID & MaskBits);
}




//
//
//
unsigned char CPUCount(unsigned int *TotAvailLogical,
					   unsigned int *TotAvailCore,
					   unsigned int *PhysicalNum)
{
	unsigned char StatusFlag = 0;
	unsigned int numLPEnabled = 0;
	DWORD dwAffinityMask;
	int j = 0, MaxLPPerCore;
	unsigned char apicID, PackageIDMask;
	unsigned char tblPkgID[256], tblCoreID[256], tblSMTID[256];
	char	tmp[256];
	g_s3Levels[0] = 0;
	*TotAvailCore = 1;
	*PhysicalNum  = 1;

	DWORD_PTR dwProcessAffinity, dwSystemAffinity;
	GetProcessAffinityMask(GetCurrentProcess(), 
						   &dwProcessAffinity,
						   &dwSystemAffinity);
	if (dwProcessAffinity != dwSystemAffinity)  // not all CPUs are enabled
	{
		StatusFlag = USER_CONFIG_ISSUE;		
		return StatusFlag;
	}
	
	// Assumwe that cores within a package have the SAME number of 
	// logical processors.  Also, values returned by
	// MaxLogicalProcPerPhysicalProc and MaxCorePerPhysicalProc do not have
	// to be power of 2.

	MaxLPPerCore = MaxLogicalProcPerPhysicalProc() / MaxCorePerPhysicalProc();
	dwAffinityMask = 1;

	while (dwAffinityMask && dwAffinityMask <= dwSystemAffinity)
	{
		if (SetThreadAffinityMask(GetCurrentThread(), dwAffinityMask))
		{
			Sleep(0);  // Ensure system to switch to the right CPU
			apicID = GetAPIC_ID();


			// Store SMT ID and core ID of each logical processor
			// Shift vlaue for SMT ID is 0
			// Shift value for core ID is the mask width for maximum logical
			// processors per core

			tblSMTID[j]  = GetNzbSubID(apicID, MaxLPPerCore, 0);
			tblCoreID[j] = GetNzbSubID(apicID, 
						   MaxCorePerPhysicalProc(),
						   (unsigned char) find_maskwidth(MaxLPPerCore));

			// Extract package ID, assume single cluster.
			// Shift value is the mask width for max Logical per package

			PackageIDMask = (unsigned char) (0xff << 
							find_maskwidth(MaxLogicalProcPerPhysicalProc()));

			tblPkgID[j] = apicID & PackageIDMask;
			sprintf(tmp,"  AffinityMask = %d; Initial APIC = %d; Physical ID = %d, Core ID = %d,  SMT ID = %d\n",
				    dwAffinityMask, apicID, tblPkgID[j], tblCoreID[j], tblSMTID[j]);
			strcat(g_s3Levels, tmp);

			numLPEnabled ++;   // Number of available logical processors in the system.

		} // if

		j++;
		dwAffinityMask = 1 << j;
	} // while

    // restore the affinity setting to its original state
    SetThreadAffinityMask(GetCurrentThread(), dwProcessAffinity);
	Sleep(0);
	*TotAvailLogical = numLPEnabled;
	
	//
	// Count available cores (TotAvailCore) in the system
	//
	unsigned char CoreIDBucket[256];
	DWORD ProcessorMask, pCoreMask[256];
	unsigned int i, ProcessorNum;

	CoreIDBucket[0] = tblPkgID[0] | tblCoreID[0];
	ProcessorMask = 1;
	pCoreMask[0] = ProcessorMask;

	for (ProcessorNum = 1; ProcessorNum < numLPEnabled; ProcessorNum++)
	{
		ProcessorMask <<= 1;
		for (i = 0; i < *TotAvailCore; i++)
		{
			// Comparing bit-fields of logical processors residing in different packages
			// Assuming the bit-masks are the same on all processors in the system.
			if ((tblPkgID[ProcessorNum] | tblCoreID[ProcessorNum]) == CoreIDBucket[i])
			{
				pCoreMask[i] |= ProcessorMask;
				break;
			}

		}  // for i

		if (i == *TotAvailCore)   // did not match any bucket.  Start a new one.
		{
			CoreIDBucket[i] = tblPkgID[ProcessorNum] | tblCoreID[ProcessorNum];
			pCoreMask[i] = ProcessorMask;

			(*TotAvailCore)++;	// Number of available cores in the system

		}

	}  // for ProcessorNum


	//
	// Count physical processor (PhysicalNum) in the system
	//
	unsigned char PackageIDBucket[256];
	DWORD pPackageMask[256];

	PackageIDBucket[0] = tblPkgID[0];
	ProcessorMask = 1;
	pPackageMask[0] = ProcessorMask;

	for (ProcessorNum = 1; ProcessorNum < numLPEnabled; ProcessorNum++)
	{
		ProcessorMask <<= 1;
		for (i = 0; i < *PhysicalNum; i++)
		{
			// Comparing bit-fields of logical processors residing in different packages
			// Assuming the bit-masks are the same on all processors in the system.
			if (tblPkgID[ProcessorNum]== PackageIDBucket[i])
			{
				pPackageMask[i] |= ProcessorMask;
				break;
			}

		}  // for i

		if (i == *PhysicalNum)   // did not match any bucket.  Start a new one.
		{
			PackageIDBucket[i] = tblPkgID[ProcessorNum];
			pPackageMask[i] = ProcessorMask;

			(*PhysicalNum)++;	// Total number of physical processors in the system

		}

	}  // for ProcessorNum

    //
    // Check to see if the system is multi-core 
    // Check if the system is hyper-threading
    //
    if (*TotAvailCore > *PhysicalNum) 
    {
    	// Multi-core
    	if (MaxLPPerCore == 1)
    		StatusFlag = MULTI_CORE_AND_HT_NOT_CAPABLE;
    	else if (numLPEnabled > *TotAvailCore)
    		StatusFlag = MULTI_CORE_AND_HT_ENABLED;
    		else StatusFlag = MULTI_CORE_AND_HT_DISABLED;
    
    }
    else
    {
    	// Single-core
    	if (MaxLPPerCore == 1)
    		StatusFlag = SINGLE_CORE_AND_HT_NOT_CAPABLE;
    	else if (numLPEnabled > *TotAvailCore)
    		StatusFlag = SINGLE_CORE_AND_HT_ENABLED;
    		else StatusFlag = SINGLE_CORE_AND_HT_DISABLED;
    
    
    }

    return StatusFlag;
}