/*
	smbios.cpp

  Copyright (c) 2008 Primate Labs

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

#include "smbios.h"
#include "platform.h"

#if defined(PLATFORM_MACOSX)
	#include <mach/mach.h>
	#include <CoreFoundation/CoreFoundation.h>
	#include <IOKit/IOKitLib.h>
#endif

#if defined(PLATFORM_WINDOWS)
	#include "win32wmi.h"
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

namespace geekinfo {
  
void print_raw_smbios(std::ostream& stream)
{
  uint8 * data;
  uint32 length;
  
  if (!fetch_raw_smbios(&data, &length)) {
    return;
  }
  
  for (uint32 i = 0; i < length; i += 16) {
    uint32              l = std::min(16u, length - i);
    uint8 *             p = data + i;
    std::ostringstream  off;
    std::ostringstream  hex;
    std::ostringstream  chr;
      
    for (unsigned int j = 0; j < l; j++) {
      hex << std::setw(2) << std::setfill('0') << std::setbase(16) << static_cast<uint32>(p[j]) << " ";
      if (isalnum(p[j])) {
        chr << p[j];
      } else {
        chr << ".";
      }
    }
    off << std::setw(4) << std::setfill('0') << std::setbase(16) << i;
    stream.setf(std::ios_base::left);
    stream << off.str() << " " << std::setw(48) << hex.str() << " " << chr.str() << std::endl;
  }
  stream << std::endl;
  
  delete [] data;
}

#define SMBIOS_BYTE(offset, member) if ((o + offset + 1) <= (o + structure.length)) member = *((uint8 *)(o + offset)); else member = 0;
#define SMBIOS_WORD(offset, member) if ((o + offset + 2) <= (o + structure.length)) member = *((uint16 *)(o + offset)); else member = 0;
#define SMBIOS_QUAD(offset, member) if ((o + offset + 4) <= (o + structure.length)) member = *((uint32 *)(o + offset)); else member = 0;
#define SMBIOS_STRING(offset, member) \
  if ((o + offset + 1) <= (o + structure.length)) { \
    stringmap[*((uint8 *)(o + offset)) - 1] = &(member); \
  }

const SMBIOS* parse_smbios(uint8 * data, uint32 length)
{
  SMBIOS *  smbios = new SMBIOS();
  uint8 *   p = data;
  
  while (p < data + length) {
    Structure   structure;
    std::vector<std::string>          stringpool;
    std::map<unsigned, std::string*>  stringmap;
    uint8 *     o = p;

    structure.type = static_cast<StructureType>(*p++); 
    structure.length = *p++; 
    structure.handle = *(uint16 *)p; p += 2; 

    switch(structure.type) {
      case kStructureBIOS:
        SMBIOS_STRING(0x04, smbios->bios.vendor);
        SMBIOS_STRING(0x05, smbios->bios.version);
        SMBIOS_STRING(0x08, smbios->bios.releaseDate);
        SMBIOS_BYTE(0x14, smbios->bios.majorRelease);
        SMBIOS_BYTE(0x15, smbios->bios.minorRelease);
        break;
        
#if 0
      case kStructureSystem:
        SMBIOS_STRING(0x04, kPropertyManufacturer);
        SMBIOS_STRING(0x05, kPropertyProductName);
        SMBIOS_STRING(0x06, kPropertyVersion);
        SMBIOS_STRING(0x07, kPropertySerialNumber);
        break;
#endif

      case kStructureBaseboard:
        SMBIOS_STRING(0x04, smbios->baseboard.manufacturer);
        SMBIOS_STRING(0x05, smbios->baseboard.name);
        SMBIOS_STRING(0x06, smbios->baseboard.version);
        SMBIOS_STRING(0x07, smbios->baseboard.serial);
        break;

#if 0        
      case kStructureProcessor:
        SMBIOS_STRING(0x04, kPropertySocket);
        SMBIOS_BYTE(0x05, kPropertyProcessorType);
        SMBIOS_BYTE(0x06, kPropertyProcessorFamily);
        SMBIOS_STRING(0x07, kPropertyProcessorManufacturer);
        SMBIOS_QUAD(0x08, kPropertyProcessorID);
        SMBIOS_STRING(0x10, kPropertyProcessorVersion);
        SMBIOS_BYTE(0x11, kPropertyProcessorVoltage);
        SMBIOS_WORD(0x12, kPropertyExternalSpeed);
        SMBIOS_WORD(0x14, kPropertyMaximumSpeed);
        SMBIOS_WORD(0x16, kPropertyCurrentSpeed);
        SMBIOS_BYTE(0x23, kPropertyCoreCount);
        SMBIOS_BYTE(0x24, kPropertyCoreEnabled);
        SMBIOS_BYTE(0x25, kPropertyThreadCount);
        break;
        
      case kStructureCache:
        SMBIOS_STRING(0x04, kPropertySocket);
        SMBIOS_WORD(0x05, kPropertyCacheConfiguration);
        SMBIOS_WORD(0x07, kPropertyMaximumCacheSize);
        SMBIOS_WORD(0x09, kPropertyInstalledSize);
        SMBIOS_WORD(0x0b, kPropertySupportedSRAMType);
        SMBIOS_WORD(0x0d, kPropertyCurrentSRAMType);
        SMBIOS_BYTE(0x0f, kPropertyCacheSpeed);
        SMBIOS_BYTE(0x10, kPropertyErrorCorrectionType);
        SMBIOS_BYTE(0x11, kPropertySystemCacheType);
        SMBIOS_BYTE(0x12, kPropertyAssociativity);
        break;
      
      case kStructureMemoryDevice:
        SMBIOS_WORD(0x08, kPropertyTotalWidth);
        SMBIOS_WORD(0x0a, kPropertyDataWidth);
        SMBIOS_WORD(0x0c, kPropertySize);
        SMBIOS_BYTE(0x0e, kPropertyFormFactor);
        SMBIOS_BYTE(0x0f, kPropertyDeviceSet);
        SMBIOS_STRING(0x10, kPropertyDeviceLocator);
        SMBIOS_STRING(0x11, kPropertyBankLocator);
        SMBIOS_BYTE(0x12, kPropertyMemoryType);
        SMBIOS_WORD(0x13, kPropertyTypeDetail);
        SMBIOS_WORD(0x15, kPropertySpeed);
        SMBIOS_STRING(0x17, kPropertyManufacturer);
        SMBIOS_STRING(0x18, kPropertySerialNumber);
        break;
#endif

      case kStructureMemoryDevice: {
        MemoryDeviceStructure* memory = new MemoryDeviceStructure;
        
        SMBIOS_WORD(0x0c, memory->size);
        SMBIOS_BYTE(0x0e, memory->form_factor);
        SMBIOS_STRING(0x10, memory->device_locator);
        SMBIOS_STRING(0x11, memory->bank_locator);
        SMBIOS_BYTE(0x12, memory->memory_type);
        SMBIOS_WORD(0x13, memory->type_detail);
        SMBIOS_WORD(0x15, memory->speed);
        SMBIOS_STRING(0x17, memory->manufacturer);
        
        smbios->memory_devices.push_back(memory);
      } break;      

      default:
        break;
    }

    // Skip to the strings at the end of the SMBIOS structure.
    p = o + structure.length;
    
    while (true) {
      if (*p != '\0') {
        
        // Strip out non-printable and non-ASCII characters from the string.
        for (uint8 *q = p; *q != '\0'; q++) {
          if (*q < 32 || *q > 127) {
            *q = '?';
          }          
        }
        
        std::string s = (char *)p;
        stringpool.push_back(s);
        p += s.length() + 1;
        if (*(p) == '\0') {
          p += 1;
          break;
        } 
      } else {
        p += 2;
        break;
      }
    }
    
    for (unsigned i = 0; i < stringpool.size(); i++) { 
      if (stringmap.count(i) > 0) {
        *(stringmap[i]) = stringpool[i]; 
      }
    }
  }
  
  return smbios;
}

const SMBIOS* fetch_smbios()
{
  static const SMBIOS* smbios = NULL;
  
	if (smbios != NULL) {
    return smbios;
  }
  
  unsigned char* data = NULL;
  unsigned int length = 0;
  
  if (fetch_raw_smbios(&data, &length)) {
    smbios = parse_smbios(data, length);
  }
  
  delete [] data;
  
  return smbios;
}

#if defined(PLATFORM_WINDOWS)

bool fetch_raw_smbios(unsigned char** data, unsigned int* length)
{
	return getWmiSmbios(data, length);
}

#elif defined(PLATFORM_MACOSX)

bool fetch_raw_smbios(unsigned char** data, unsigned int* length)
{
  CFDataRef               value = NULL;
  io_object_t             device;
  io_iterator_t           objectIterator;
  CFMutableDictionaryRef  properties = NULL;
  mach_port_t             master_port;
  bool                    result = false;
  
  if (kIOReturnSuccess == IOMasterPort(MACH_PORT_NULL, &master_port)) {
    if (kIOReturnSuccess == IOServiceGetMatchingServices(master_port, IOServiceMatching("AppleSMBIOS"), &objectIterator)) {
      while ((device = IOIteratorNext(objectIterator))) {
        if (kIOReturnSuccess == IORegistryEntryCreateCFProperties(device, &properties, kCFAllocatorDefault, kNilOptions)) {
          if (CFDictionaryGetValueIfPresent(properties, CFSTR("SMBIOS"), (const void **)&value)) {
            *length = CFDataGetLength(value);
            *data = new uint8[*length];
            memcpy(*data, (uint8 *)CFDataGetBytePtr(value), *length);
            result = true;
            break;
          }
          CFRelease(properties);
        }
        IOObjectRelease(device);
      }
      IOObjectRelease(objectIterator);
    }
    mach_port_deallocate(mach_task_self(), master_port);
  }
  return result;
}

#else

bool fetch_raw_smbios(unsigned char** data, unsigned int* length)
{
  *data = 0;
  *length = 0;
  return false;
}

#endif

} // namespace geekinfo
