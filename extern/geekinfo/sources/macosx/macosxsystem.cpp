/*
  macosxsystem.cpp

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

#include "macosxsystem.h"
#include "platform.h"
#include <sstream>
#include <iomanip>
#if !defined(PLATFORM_MACOSX)
  #error Wrong platform
#endif

#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/utsname.h>
#include <mach/vm_statistics.h>
#include <mach/mach.h>
#include <mach/machine.h>

#include <iostream>
#include "macosxmodel.h"

#include "smbios.h"

#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFString.h>
#include <IOKit/IOKitLib.h>
#include <ApplicationServices/ApplicationServices.h>
#include <IOKit/Graphics/IOFrameBufferShared.h>

extern "C" CFStringRef CFCopySystemVersionString();                  

#define	HOST_VM_INFO_COUNT ((mach_msg_type_number_t) \
(sizeof(vm_statistics_data_t)/sizeof(integer_t)))

bool sysctlbynameuint64(const char * name, uint64_t * value)
{
  size_t  size = sizeof(uint64_t);
  int     result;

  result = sysctlbyname(name, value, &size, NULL, 0);

  if (result != -1) {    
    switch(size) {
      case sizeof(uint64_t):
        break;
      case sizeof(uint32_t):
        *value = *(uint32_t *)value;
        break;
      case sizeof(uint16_t):
        *value = *(uint16_t *)value;
        break;
      case sizeof(uint8_t):
        *value = *(uint8_t *)value;
        break;
    }
    return true;
  } else {
    return false;
  }
}

uint64_t sysctlbynameuint64(const char* name)
{   
  uint64_t    result;

  if (sysctlbynameuint64(name, &result)) {
    return result;
  } else {
    return 0;
  }
}


std::string sysctlbynamestring(const char * name)
{
  std::string result;
  size_t      vsize = 512;
  char        vstr[ 512 ];

  sysctlbyname(name, vstr, &vsize, NULL, 0);
  result = vstr;
  return result;
}

static int GetIntFromDictionaryForKey(CFDictionaryRef desc, CFStringRef key)
{
  CFNumberRef value;
  int num = 0;
  if (!desc || !key)
    return 0;
  if ((value = (CFNumberRef)CFDictionaryGetValue(desc, key)) == NULL || CFGetTypeID(value) != CFNumberGetTypeID()) {
    return 0;
  }
  CFNumberGetValue(value, kCFNumberIntType, &num);
  return num;
}

#if defined(ARCH_PPC)
static std::string __GetRawProcessorID()
{
  static int32 detectedtype = -2L;
  std::string	detectedname;

  // http://www.freescale.com/files/archives/doc/support_info/PPCPVR.pdf

  struct { 
    int32 rid; 
    const char * name; 
  }
  cpuridtable[] = {
    {    0x0001, "601"                 },
    {    0x0003, "603"                 },
    {    0x0004, "604"                 },
    {    0x0006, "603e"                },
    {    0x0007, "603r/603ev"          }, 
    {    0x0008, "G3 (740/750)"        },
    {    0x0009, "604e"                },
    {    0x000A, "604ev"               },
    {    0x000C, "G4 (7400)"           },
    {    0x0020, "403G/403GC/403GCX"   },
    {    0x0039, "G5 (970)"            },
    {    0x003C, "G5 (970FX)"          }, 
    {    0x0044, "G5 (970MP)"          }, 
    {    0x0050, "821"                 },
    {    0x0080, "860"                 },
    {    0x0081, "8240"                },
    {    0x4011, "405GP"               },
    {    0x7000, "750FX"               },
    {    0x8000, "G4 (7441/7450/7451)" },
    {    0x8001, "G4 (7445/7455)"      },
    {    0x8002, "G4 (7447/7457)"      },
    {    0x8003, "G4 (7447A)"          },
    {    0x8004, "G4 (7448)"           },
    {    0x800C, "G4 (7410)"           },
  };

  if (detectedtype == -2L) {
    CFDataRef value = NULL;
    io_object_t device;
    io_iterator_t objectIterator;
    CFMutableDictionaryRef properties = NULL;
    mach_port_t master_port;
    detectedtype = -1L;

    if (kIOReturnSuccess == IOMasterPort(MACH_PORT_NULL, &master_port)) {
      if (kIOReturnSuccess == IOServiceGetMatchingServices(master_port, IOServiceMatching("IOPlatformDevice"), &objectIterator)) {
        while ((device = IOIteratorNext(objectIterator))) {
          if (kIOReturnSuccess == IORegistryEntryCreateCFProperties(device, &properties, kCFAllocatorDefault, kNilOptions)) {
            if (CFDictionaryGetValueIfPresent(properties, CFSTR("cpu-version"), (const void **)&value)) {
              CFDataGetBytes(value, CFRangeMake(0,4/*CFDataGetLength((void *)value)*/), (UInt8 *)&detectedtype);
              detectedtype = (detectedtype>>16)&0xffff;
            }
            CFRelease(properties);
          }
          IOObjectRelease(device);
        }
        IOObjectRelease(objectIterator);
      }
      mach_port_deallocate(mach_task_self(), master_port);
    }
  }

  if (detectedtype > 0) {
    for (unsigned int i = 0; i < (sizeof(cpuridtable)/sizeof(cpuridtable[0])); i++) {
      if (cpuridtable[i].rid == detectedtype) {
        std::ostringstream	s;

        s << "PowerPC " << cpuridtable[i].name;
        detectedname = s.str();
        break;
      }
    }
  }

  return detectedname;
}
#endif

MacOSXSystem::MacOSXSystem()
{
}

MacOSXSystem::~MacOSXSystem()
{
}

std::string MacOSXSystem::os()
{
  std::ostringstream  s;

  CFStringRef         version = CFCopySystemVersionString();
  char                versionBuffer[ 256 ];
  int                 versionSize = 256;

  /*
  FIXME: Make sure versionBuffer starts with "Version " before truncating it.
  */
    CFStringGetCString(version, versionBuffer, versionSize, kCFStringEncodingUTF8);
  s << "Mac OS X " << &(versionBuffer[8]);        

  return s.str();
}

std::string MacOSXSystem::model()
{
#if defined(ARCH_PPC)
  return get_model_name(sysctlbynamestring("hw.model"), __GetRawProcessorID());
#elif defined(ARCH_X86) 
  return get_model_name(sysctlbynamestring("hw.model"), sysctlbynamestring("machdep.cpu.brand_string"));
#endif  
}

std::string MacOSXSystem::motherboard()
{
  const geekinfo::SMBIOS* smbios = geekinfo::fetch_smbios();
  
  if (smbios != NULL) {
    return smbios->baseboard.manufacturer + " " + smbios->baseboard.name + " " + smbios->baseboard.version;
  }

  return sysctlbynamestring("hw.model");
}

std::string MacOSXSystem::system_load()
{
  std::ostringstream  s;
	loadavg             data;
	size_t              size = sizeof(data);
	int                 mib[] = { CTL_VM, VM_LOADAVG };
	
	sysctl(mib, 2, &data, &size, NULL, 0);
  s << std::setprecision(2) 
    << double(data.ldavg[0])/data.fscale << " "
    << double(data.ldavg[1])/data.fscale << " "
    << double(data.ldavg[2])/data.fscale; 
  return s.str();
}

std::string MacOSXSystem::cpu()
{
#if defined(ARCH_PPC)
  return __GetRawProcessorID();
#elif defined(ARCH_X86) 
  return sysctlbynamestring("machdep.cpu.brand_string");  
#else
  #error Unsupported platform
#endif
}

std::string MacOSXSystem::cpuid()
{
  std::ostringstream  s;

#if defined(ARCH_PPC)
  s << sysctlbynameuint64("hw.cputype") << ", " << sysctlbynameuint64("hw.cpusubtype");       
#elif defined(ARCH_X86) 
  s << sysctlbynamestring("machdep.cpu.vendor") << " ";
  s << "Family " << sysctlbynameuint64("machdep.cpu.family") << " ";
  s << "Model " << sysctlbynameuint64("machdep.cpu.model") << " ";
  s << "Stepping " << sysctlbynameuint64("machdep.cpu.stepping");
#else
  #error Unsupported platform
#endif

  return s.str();
}

std::string MacOSXSystem::cpu_features()
{
  #if defined(ARCH_PPC)
    return "N/A";
  #else
  	return sysctlbynamestring("machdep.cpu.features") + 
  	  sysctlbynamestring("machdep.cpu.extfeatures");
  #endif
}

uint64 MacOSXSystem::cpu_logical_count()
{
  uint64  result;

  if (!sysctlbynameuint64("hw.logicalcpu", &result)) {
    if (!sysctlbynameuint64("hw.ncpu", &result)) {
      result = 1;
    }
  }
  return result;
}

uint64 MacOSXSystem::cpu_core_count()
{
  uint64 result = 0;
  if (!sysctlbynameuint64("hw.physicalcpu", &result)) {
    result = cpu_logical_count();
  }
  return result;
}

uint64 MacOSXSystem::cpu_physical_count()
{
  uint64 result = 0;

  if (!sysctlbynameuint64("hw.packages", &result)) {
    uint64 ncpu = 0;
    uint64 nlpp = 0;

    if (sysctlbynameuint64("hw.ncpu", &ncpu) && sysctlbynameuint64("machdep.cpu.logical_per_package", &nlpp)) {
      result = ncpu / nlpp;
    } else {
      if (!sysctlbynameuint64("hw.physicalcpu", &result)) {
        if (!sysctlbynameuint64("hw.ncpu", &result)) {
          result = 1;
        }
      }
    }    
  }

  return result;
}

uint64 MacOSXSystem::cpu_frequency()
{
  return sysctlbynameuint64("hw.cpufrequency");
}

uint64 MacOSXSystem::cpu_l1_inst_cache()
{
  return sysctlbynameuint64("hw.l1icachesize");
}

uint64 MacOSXSystem::cpu_l1_data_cache()
{
  return sysctlbynameuint64("hw.l1dcachesize");
}

uint64 MacOSXSystem::cpu_l2_cache()
{
  return sysctlbynameuint64("hw.l2cachesize");
}

uint64 MacOSXSystem::cpu_l3_cache()
{
  return sysctlbynameuint64("hw.l3cachesize");
}

uint64 MacOSXSystem::bus_frequency()
{
  return sysctlbynameuint64("hw.busfrequency");
}

uint64 MacOSXSystem::memory_size()
{
  return sysctlbynameuint64("hw.memsize");
}

uint64 MacOSXSystem::memory_free()
{
  uint64 result;
  vm_statistics_data_t vm_stat;
  int count = HOST_VM_INFO_COUNT;

  host_statistics(mach_host_self(), HOST_VM_INFO, (integer_t*)&vm_stat, (mach_msg_type_number_t*)&count);
  result = (uint64)(vm_stat.free_count + vm_stat.inactive_count) * sysctlbynameuint64("hw.pagesize");

  return result;
}

std::string MacOSXSystem::display()
{

  std::ostringstream s;

  CFDataRef value = NULL, model = NULL;
  io_object_t device;
  io_iterator_t objectIterator;
  CFMutableDictionaryRef properties = NULL;
  mach_port_t master_port;

  if (kIOReturnSuccess == IOMasterPort(MACH_PORT_NULL, &master_port)) {
    if (kIOReturnSuccess == IOServiceGetMatchingServices(master_port, IOServiceMatching("IOPCIDevice"), &objectIterator)) {
      while ((device = IOIteratorNext(objectIterator))) {
        if (kIOReturnSuccess == IORegistryEntryCreateCFProperties(device, &properties, kCFAllocatorDefault, kNilOptions)) {
          if (CFDictionaryGetValueIfPresent(properties, CFSTR("class-code"), (const void **)&value)) {
            if ((*(UInt32*)CFDataGetBytePtr(value) & 0x00ff0000) == 0x00030000) {
              if (CFDictionaryGetValueIfPresent(properties, CFSTR("model"), (const void **)&model)) {
                s << (char *)CFDataGetBytePtr(model) << " ";
                CFRelease(properties);
                IOObjectRelease(device);
                break;
              }
            }
          }
          CFRelease(properties);
        }
        IOObjectRelease(device);
      }
      IOObjectRelease(objectIterator);
    }
    mach_port_deallocate(mach_task_self(), master_port);
  }

  short				MAXDISPLAYS = 8;
  uint32				vramSize = 0;
  io_service_t		dspPort;
  CGDirectDisplayID   displays[MAXDISPLAYS];
  CGDisplayCount displayCount;
  CFNumberRef			vramStorage = NULL;
  CFMutableDictionaryRef vramProp = NULL;

  CGGetOnlineDisplayList(MAXDISPLAYS, displays, &displayCount);

  dspPort = CGDisplayIOServicePort(displays[0]);

  if (kIOReturnSuccess == IORegistryEntryCreateCFProperties(dspPort, &vramProp, kCFAllocatorDefault, kNilOptions)) {
    if (CFDictionaryGetValueIfPresent(vramProp, CFSTR("IOFBMemorySize"), (const void **)&vramStorage)) {
      //std::cout << vramStorage << std::endl;
      CFNumberGetValue(vramStorage, kCFNumberSInt32Type, (void *)&vramSize);
      s << vramSize / 1048576 << " MB ";
    }
    CFRelease(vramProp);
  }
  IOObjectRelease(dspPort);

  CFDictionaryRef displayModeDict = CGDisplayCurrentMode(kCGDirectMainDisplay);
  int horiRes = 0, vertRes = 0, refreshRate = 0;

  horiRes = GetIntFromDictionaryForKey(displayModeDict, kCGDisplayWidth);
  vertRes = GetIntFromDictionaryForKey(displayModeDict,  kCGDisplayHeight);
  refreshRate = GetIntFromDictionaryForKey(displayModeDict,  kCGDisplayRefreshRate);

  s << horiRes << 'x' << vertRes << '@' << refreshRate << "Hz";

  return s.str();
}

std::string MacOSXSystem::memory_type()
{
  std::ostringstream	s;
  io_registry_entry_t	memory;
  mach_port_t			master_port;
  CFDataRef			data;

  IOMasterPort(bootstrap_port, &master_port);

  memory = IORegistryEntryFromPath(master_port, kIODeviceTreePlane ":/memory");

  if (memory != MACH_PORT_NULL) {
    data = (CFDataRef)IORegistryEntryCreateCFProperty(memory, CFSTR("dimm-speeds"), kCFAllocatorDefault, kNilOptions) ;
    if (data) {
      s << CFDataGetBytePtr(data);
      CFRelease(data);
    }
    data = (CFDataRef)IORegistryEntryCreateCFProperty(memory, CFSTR("dimm-types"), kCFAllocatorDefault, kNilOptions) ;
    if (data) {
      s << " " << CFDataGetBytePtr(data);
      CFRelease(data);
    }
  }
  IOObjectRelease(memory);

  return s.str();
}


uint64 MacOSXSystem::simd()
{
  return sysctlbynameuint64("hw.vectorunit");
}
