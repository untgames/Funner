/*
	smbios.h

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

#ifndef INC_SMBIOS_H
#define INC_SMBIOS_H

#include "platform.h"
#include "types.h"

#include <map>
#include <set>
#include <string>
#include <vector>

namespace geekinfo {

typedef enum {
  kStructureBIOS = 0,
  kStructureSystem = 1,
  kStructureBaseboard = 2,
  kStructureChassis = 3,
  kStructureProcessor = 4,
  kStructureMemoryController = 5,
  kStructureMemoryModule = 6,
  kStructureCache = 7,
  kStructurePortConnector = 8,
  kStructureSystemSlots = 9,
  kStructureOnBoardDevices = 10,
  kStructureOEMString = 11,
  kStructureSystemConfiguration = 12,
  kStructureBIOSLanguage = 13,
  kStructureGroupAssociations = 14,
  kStructureSystemEventLog = 15,
  kStructurePhysicalMemoryArray = 16,
  kStructureMemoryDevice = 17,
  kStructure32BitMemoryErrorInformation = 18, 
  kStructureMemoryArrayMappedAddress = 19,
  kStructureMemoryDeviceMappedAddress = 20,
  kStructurePointingDevice = 21,
  kStructurePortableBattery = 22,
  kStructureSystemReset = 23,
  kStructureHardwareSecurity = 24,
  kStructureSystemPowerControls = 25,
  kStructureVoltageProbe = 26,
  kStructureCoolingDevice = 27,
  kStructureTemperatureProbe = 28,
  kStructureElectricalCurrentProbe = 29,
  kStructureOutOfBandRemoteAccess = 30,
  kStructureBootIntegrityServices = 31,
  kStructureSystemBootInformation = 32,
  kStructure64BitMemoryErrorInformation = 33,
  kStructureManagementDevice = 34,
  kStructureManagementDeviceComponent = 35,
  kStructureManagementDeviceThresholdData = 36,
  kStructureMemoryChannel = 37,
  kStructureIPMIDeviceInformation = 38,
  kStructureSystemPowerSupply = 39,
  kStructureAdditionalInformation = 40,
  kStructureOnboardDevicesExtendedInformation = 41
} StructureType;

struct BIOSStructure {
  std::string         vendor;
  std::string         version;
  std::string         releaseDate;
  uint8               majorRelease;
  uint8               minorRelease;
};

struct BaseboardStructure {
  std::string   manufacturer;
  std::string   name;
  std::string   version;
  std::string   serial;
};

struct PhysicalMemoryStructure {
  uint8 location;
  uint8 use;
  uint32 maximum_capacity;  
};

struct MemoryDeviceStructure {
  uint16        size;
  uint8         form_factor;
  std::string   device_locator;
  std::string   bank_locator;
  uint8         memory_type;
  uint16        type_detail;
  uint16        speed;
  std::string   manufacturer;
};

struct SMBIOS {
  BIOSStructure                       bios;
  BaseboardStructure                  baseboard;
  std::vector<MemoryDeviceStructure*> memory_devices;
};

struct Structure {
  StructureType                 type;
  uint32                        length;
  uint32                        handle;
};


/// Returns a pointer to an SMBIOS instance.
const SMBIOS* fetch_smbios();

/// Returns the raw SMBIOS data, useful for debugging problems with the SMBIOS
/// parsing code.
bool fetch_raw_smbios(unsigned char** data, unsigned int* length);

/// Prints the raw SMBIOS data to the output stream stream.
void print_raw_smbios(std::ostream& stream);

} // namespace geekinfo

#endif
