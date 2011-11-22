/*
	Memory.cpp

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

#include "Memory.hpp"
#include "smbios.h"

namespace geekinfo {
  
Memory::Memory()
: m_size(0)
{
  gather_information();
}

Memory::~Memory()
{
}

void Memory::gather_information()
{
  const SMBIOS* smbios = fetch_smbios();
  
  if (!smbios) {
    return;
  }

  for (unsigned int i = 0; i < smbios->memory_devices.size(); i++) {
    m_module_sizes.push_back(smbios->memory_devices[i]->size);
    m_module_speeds.push_back(smbios->memory_devices[i]->speed);
    m_module_types.push_back("NYI");
    m_module_locations.push_back(smbios->memory_devices[i]->device_locator + " " + smbios->memory_devices[i]->bank_locator);
  }
}
  
} // namespace geekinfo