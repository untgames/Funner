/*
	win32system.h

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

#ifndef INC_WIN32SYSTEM_H
#define INC_WIN32SYSTEM_H

#include "basesystem.h"
#include "types.h"

class Win32System : public BaseSystem {
public:
  Win32System();
  virtual ~Win32System();
	
  virtual std::string os();
  virtual std::string model();
  virtual std::string motherboard();
  virtual std::string cpu(); 
  virtual std::string cpuid();
  virtual uint64      cpu_logical_count();
  virtual uint64      cpu_core_count();
  virtual uint64      cpu_physical_count();
  virtual uint64      cpu_frequency();

  virtual uint64      cpu_l1_inst_cache();
  virtual uint64      cpu_l1_data_cache();
  virtual uint64      cpu_l2_cache();
  virtual uint64      cpu_l3_cache();

  virtual uint64      bus_frequency();
  virtual uint64      memory_size();
  virtual std::string memory_type();
  virtual uint64      memory_free();
  
  virtual std::string display();

  virtual uint64      simd();
};

#endif
