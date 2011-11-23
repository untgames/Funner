#include "win32wmi.h"

bool runWmiQuery(std::string query, std::map< std::string, uint64 > & wmiMap)
{
  return false;
}

bool getWmiClass(const char * wmiClass, std::map< std::string, std::string > & wmiMap)
{
  return false;
}

bool getWmiClass(const char * wmiClass, std::map< std::string, uint64 > & wmiMap)
{
  return false;
}

bool getWmiSmbios(uint8 ** data, uint32 * length)
{
  return false;
}
