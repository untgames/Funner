/*
	win32wmi.h

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

#ifndef INC_WMI_H
#define INC_WMI_H

#include <string>
#include <map>
#include "types.h"

typedef std::map< std::string, std::string >	WmiStrMap;
typedef std::map< std::string, uint64 >			  WmiIntMap;

bool runWmiQuery(std::string query, WmiIntMap & wmiMap);
bool getWmiClass(const char * wmiClass, WmiStrMap & wmiMap);
bool getWmiClass(const char * wmiClass, WmiIntMap & wmiMap);
bool getWmiSmbios(uint8 ** data, uint32 * length);

#endif
