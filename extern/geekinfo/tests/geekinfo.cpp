/*
  geekinfo.cpp

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

#include "geekinfo.h"
#include "smbios.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

int main()
{
  std::cout.setf(std::ios::left);
  std::cout << "System Information" << std::endl;
  for (int i = (int)kSystemMetricPlatform; i != kSystemMetricCount; i++) {    
    std::ostringstream oss;
    
    oss << systemMetricName((SystemMetricType)i) << ":";
    std::cout << "  " <<  std::setw(24) << oss.str() << systemMetric((SystemMetricType)i) << std::endl;
  }
  
#if 0
  const geekinfo::SMBIOS * smbios = geekinfo::fetch_smbios();
  std::cout << smbios->baseboard.manufacturer << std::endl;
  
  for (int i = 0; i < smbios->memory_devices.size(); i++) {
    std::cout << smbios->memory_devices[i]->size << std::endl;
    std::cout << smbios->memory_devices[i]->speed << std::endl;
    std::cout << smbios->memory_devices[i]->device_locator << " " << smbios->memory_devices[i]->bank_locator << std::endl;
  }
  
  geekinfo::print_raw_smbios(std::cout);
#endif
  
#if 0
  geekinfo::Processor p;
  geekinfo::Memory m;
#endif  
  
  return 0;
}
