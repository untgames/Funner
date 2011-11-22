/*
win32system.cpp

  Copyright (c) 2006-2009 Primate Labs

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include "win32system.h"
#include "platform.h"
#include <intrin.h>
#include <sstream>	

#if !defined(PLATFORM_WINDOWS)
  #error Wrong platform
#endif

extern "C" {
  #include "intel/cputopology.h"
}

#include <windows.h>
#include "win32cpucount.h"
#include "win32wmi.h"

struct Registers {
  uint32	eax;
  uint32	ebx;
  uint32 	ecx;
  uint32 	edx;
};

static Registers cpuID(uint32 level)
{
  Registers	r;

  __cpuid((int *)&r, level);

  return r;
}

static std::string cpu()
{
  std::string result;
  Registers	r; // r.eax, r.ebx, r.ecx, r.edx;

  r = cpuID(0x80000000);
  if (r.eax & 0x80000000 && r.eax >= 0x80000004) {
    uint32 brand[13];
    char * p;

    r = cpuID(0x80000002);
    brand[0] = r.eax;
    brand[1] = r.ebx;
    brand[2] = r.ecx;
    brand[3] = r.edx;
    r = cpuID(0x80000003);
    brand[4] = r.eax;
    brand[5] = r.ebx;
    brand[6] = r.ecx;
    brand[7] = r.edx;
    r = cpuID(0x80000004);
    brand[8] = r.eax;
    brand[9] = r.ebx;
    brand[10] = r.ecx;
    brand[11] = r.edx;
    brand[12] = 0;
    p = (char *)brand;
    while (*p != 0 && *p == ' ') {
      p++;
    }
    result = p;
  }
  return result;
}

static std::string cpuid()
{
  std::ostringstream  s;
  Registers			r;
  uint32              family; //, extendedFamily;
  uint32              model; //, extendedModel;
  uint32              stepping;
  uint32				vendor[4];
  char *				p;

  r = cpuID(1);
  family = (r.eax & 0xf00) >> 8;
  model = (r.eax & 0xf0) >> 4;
  stepping = r.eax & 0xf;
  if (family == 0x06 || family == 0x0f) {
    if (family == 0x0f) {
      family += ((r.eax & 0xff00000) >> 20);
    }
    model += ((r.eax & 0x000f0000) >> 12);
  }
  //std::cout << "!" << std::endl;

  r = cpuID(0);
  vendor[0] = r.ebx;
  vendor[1] = r.edx;
  vendor[2] = r.ecx;
  vendor[3] = 0;
  p = (char *)vendor;

  s << p << " Family " << family << " Model " << model << " Stepping " << stepping;
  return s.str();
}

static bool cpusse()
{
  Registers	r;

  r = cpuID(0x00000001);
  if (r.edx & 0x02000000) {
    return true;
  } else {
    return false;
  }
}

#if 0
static bool cpummx()
{
  uint32      r.eax, r.ebx, r.ecx, r.edx;

  cpuID(0x00000001, &r.eax, &r.ebx, &r.ecx, &r.edx);
  if (r.edx & 0x00800000) {
    return true;
  } else {
    return false;
  }
}
#endif

Win32System::Win32System() : BaseSystem()
{
}

Win32System::~Win32System()
{
}

std::string Win32System::model()
{
  std::ostringstream  s;
  WmiStrMap           wmiMap;

  if (getWmiClass("Win32_ComputerSystem", wmiMap)) {
    std::string	manufacturer = wmiMap[ "Manufacturer" ];
    std::string model = wmiMap[ "Model" ];

    if (manufacturer.length() == 0 && model.length() == 0) {
      s << "Unknown";
    } else {
      if (manufacturer.find("To Be Filled By O.E.M.") != std::string::npos) {
        manufacturer = "";
      }
      if (model.find("To Be Filled By O.E.M.") != std::string::npos) {
        model = "";
      }
      if (model.length() != 0 || model.length() != 0) {
        s << manufacturer << " " << model;
      } else {
        s << "Generic";
      } 
    }
  } else {
    s << "Unknown";
  }
  return s.str();    
}

std::string Win32System::motherboard()
{
  std::ostringstream  s;
  WmiStrMap           wmiMap;

  if (getWmiClass("Win32_BaseBoard", wmiMap)) {
    s << wmiMap[ "Manufacturer" ] << " " << wmiMap[ "Product" ];
  } else {
    s << "Unknown";
  }
  return s.str();
}

std::string Win32System::os()
{
  std::ostringstream  s;
  OSVERSIONINFO       vi;
  std::string         result = "Unknown";
  WmiStrMap           wmiMap;

  if (getWmiClass("Win32_OperatingSystem", wmiMap) && wmiMap[ "Caption" ].length() > 0) {
    result = wmiMap[ "Caption" ];
  } else {
    memset(&vi, 0, sizeof(OSVERSIONINFO));
    vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&vi);

    switch(vi.dwPlatformId) {
      case VER_PLATFORM_WIN32_WINDOWS:
      switch(vi.dwMinorVersion) {
        case 0:
        result = "Microsoft Windows 95";
        break;
        case 10:
        result = "Microsoft Windows 98";
        break;
        case 90:
        result = "Microsoft Windows ME";
        break;
      }
      break;
      case VER_PLATFORM_WIN32_NT: 
      if (vi.dwMajorVersion == 5 && vi.dwMinorVersion == 0) {
        s << "Microsoft Windows 2000 " << vi.szCSDVersion;
        result = s.str();
      } else if (vi.dwMajorVersion == 5 && vi.dwMinorVersion == 1) {
        s << "Microsoft Windows XP " << vi.szCSDVersion;
        result = s.str();
      } else if (vi.dwMajorVersion == 5 && vi.dwMinorVersion == 2) {
        s << "Microsoft Windows 2003 " << vi.szCSDVersion;
        result = s.str();
      } else {
        s << "Microsoft Windows NT " << vi.dwMajorVersion << "." << vi.dwMinorVersion;
        result = s.str();
      }
      break;
    }
  }
  return result;
}

std::string Win32System::cpu()
{    
  return ::cpu();
}

std::string Win32System::cpuid()
{
  return ::cpuid();
}

uint64 Win32System::cpu_logical_count()
{
  return GetSysLogicalProcessorCount();
}

uint64 Win32System::cpu_core_count()
{
  return GetSysProcessorCoreCount();
}

uint64 Win32System::cpu_physical_count()
{
  return GetSysProcessorPackageCount();
}

uint64 Win32System::cpu_frequency()
{
  WmiIntMap           wmiMap;
  uint64				result = 0;

  if (getWmiClass("Win32_Processor", wmiMap)) {
    result = wmiMap[ "MaxClockSpeed" ] * 1000000;
  } 
  return result;
}

static uint32 getL1Dcache()
{
  uint32 result;
  Registers	r; // r.eax, r.ebx, r.ecx, r.edx;

  r = cpuID(0x80000000);
  if (r.eax & 0x80000000 && r.eax >= 0x80000005) {
    r = cpuID(0x80000005);
    result = 1024 * (r.ecx >> 24); // drop lower 24 bits, convert to bytes
  }

  return result;
}

static uint32 getL1Ccache()
{
  uint32 result;
  Registers	r; // r.eax, r.ebx, r.ecx, r.edx;

  r = cpuID(0x80000000);
  if (r.eax & 0x80000000 && r.eax >= 0x80000005) {
    r = cpuID(0x80000005);
    result = 1024 * (r.edx >> 24); // drop lower 24 bits, convert to bytes
  }

  return result;
}

static uint32 getL2cache()
{
  uint32    result;
  Registers	r; // r.eax, r.ebx, r.ecx, r.edx;

  r = cpuID(0x80000000);
  if (r.eax & 0x80000000 && r.eax >= 0x80000006) {
    r = cpuID(0x80000006);
    result = 1024 * (r.ecx >> 16); // drop lower 16 bits, convert to bytes
  }

  return result;
}

static uint32 getL3cache()
{
  uint32    result;
  Registers	r; // r.eax, r.ebx, r.ecx, r.edx;

  r = cpuID(0x80000000);
  if (r.eax & 0x80000000 && r.eax >= 0x80000006) {
    r = cpuID(0x80000006);
    result = 524288 * (r.edx >> 18); // drop lower 18 bits, convert from 512KB chunks to bytes
  }

  return result;
}

uint64 Win32System::cpu_l1_inst_cache()
{
  WmiIntMap wmiMap;
  uint64		result = 0;

  if (runWmiQuery("SELECT InstalledSize FROM Win32_CacheMemory WHERE CacheType = 3 AND Level = 3", wmiMap)) {
    result = wmiMap[ "InstalledSize" ] * 1024;
  }

  if (result == 0) {
    result = (uint64)getL1Ccache();
  }

  if (result == 0) { // On some machines, L1 cache may be recognized as CacheType = 2 (Unknown)
  if (runWmiQuery("SELECT InstalledSize FROM Win32_CacheMemory WHERE Level = 3", wmiMap)) {
    result = wmiMap[ "InstalledSize" ] * 1024; // Something is better than nothing.
  }
}

return result;
}

uint64 Win32System::cpu_l1_data_cache()
{
  WmiIntMap wmiMap;
  uint64		result = 0;

  if (runWmiQuery("SELECT InstalledSize FROM Win32_CacheMemory WHERE CacheType = 4 AND Level = 3", wmiMap)) {
    result = wmiMap[ "InstalledSize" ] * 1024;
  }

  if (result == 0) {
    result = (uint64)getL1Dcache();
  }

  if (result == 0) { // On some machines, L1 cache may be recognized as CacheType = 2 (Unknown)
  if (runWmiQuery("SELECT InstalledSize FROM Win32_CacheMemory WHERE Level = 3", wmiMap)) {
    result = wmiMap[ "InstalledSize" ] * 1024; // Something is better than nothing.
  }
}

return result;
}

uint64 Win32System::cpu_l2_cache()
{
  WmiIntMap           wmiMap;
  uint64				result = 0;
  HMODULE				hMod;

  if (runWmiQuery("SELECT InstalledSize FROM Win32_CacheMemory WHERE Level = 4", wmiMap)) {
    result = wmiMap[ "InstalledSize" ] * 1024;
  } // Newer machines with < Vista report 0, because Microsoft = lazy

  if (result == 0) { 
    result = (uint64)getL2cache();
  }

  return result;
}


uint64 Win32System::cpu_l3_cache()
{
  WmiIntMap           wmiMap;
  uint64				result = 0;

  if (runWmiQuery("SELECT InstalledSize FROM Win32_CacheMemory WHERE Level = 5", wmiMap)) {
    result = wmiMap[ "InstalledSize" ] * 1024;
  } 

  if (result == 0) {
    result = (uint64)getL3cache();
  }

  return result;
}

uint64 Win32System::bus_frequency()
{
  WmiIntMap           wmiMap;
  uint64				result = 0;

  if (getWmiClass("Win32_Processor", wmiMap)) {
    result = wmiMap[ "ExtClock" ] * 1000000;
  } 

  return result;
}

uint64 Win32System::memory_size()
{
  MEMORYSTATUSEX	ms;

  memset(&ms, 0, sizeof(MEMORYSTATUSEX));
  ms.dwLength = sizeof(MEMORYSTATUSEX);
  GlobalMemoryStatusEx(&ms);

  return ms.ullTotalPhys;
}

std::string Win32System::memory_type()
{	
  std::ostringstream	result;
  WmiIntMap           wmiMap;

  if (getWmiClass("Win32_PhysicalMemory", wmiMap)) {
    switch(wmiMap[ "MemoryType" ]) {
      case 17:
      result << "SDRAM ";
      break;	
      case 18:
      result << "SGRAM ";
      break;
      case 19:
      result << "RDRAM ";
      break;
      case 20:
      result << "DDR ";
      break;
      default:
      break;
    }
    result << wmiMap[ "Speed" ] << " MHz";
  }
  return result.str();
}

uint64 Win32System::memory_free()
{
  uint64				result = 0;
  MEMORYSTATUSEX		statex;

  statex.dwLength = sizeof (statex);
  GlobalMemoryStatusEx(&statex);

  result = statex.ullAvailPhys;

  return result;
}

std::string Win32System::display()
{
  std::ostringstream result;
  WmiStrMap wmiStrMap;
  WmiIntMap wmiIntMap;
  
  if (getWmiClass("Win32_VideoController", wmiStrMap) && getWmiClass("Win32_VideoController", wmiIntMap)) {
    std::string adapterName = wmiStrMap[ "Caption" ];
    unsigned int adapterRAM = wmiIntMap[ "AdapterRAM" ];
    unsigned int horizontalResolution = wmiIntMap[ "CurrentHorizontalResolution" ];
    unsigned int verticalResolution = wmiIntMap[ "CurrentVerticalResolution" ];
    unsigned int refreshRate = wmiIntMap[ "CurrentRefreshRate" ]; 
    result << adapterName << " " << adapterRAM / 1048576 << " MB " 
      << horizontalResolution << "x" << verticalResolution << "@" << refreshRate << "Hz";
    return result.str();
  }

  return BaseSystem::display();
}

uint64 Win32System::simd()
{
  return cpusse();
}
