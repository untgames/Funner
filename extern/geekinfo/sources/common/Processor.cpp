/*
  Processor.cpp

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

#include "Processor.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "platform.h"

#ifdef __GNUC__
#undef PLATFORM_WINDOWS
#endif

#if defined(PLATFORM_WINDOWS)
#include <intrin.h>
#endif


namespace {
  
#if defined(ARCH_X86) 

struct Registers {
  unsigned int eax;
  unsigned int ebx;
  unsigned int ecx;
  unsigned int edx;
};

Registers call_cpuid(unsigned int level)
{
  Registers r;
  
#if defined(PLATFORM_WINDOWS)
  __cpuid((int*)&r, level);
#else
  asm volatile(
    "mov %%ebx, %%esi\n\t"
    "cpuid\n\t"
    "xchg %%esi, %%ebx"
    : "=a" (r.eax), "=S" (r.ebx), "=c" (r.ecx), "=d" (r.edx)
    : "a" (level)
  );
#endif

  return r;
}

Registers call_cpuid(unsigned int level, unsigned int op)
{
  Registers r;

#if defined(PLATFORM_WINDOWS)

#else
  asm volatile(
    "mov %%ebx, %%esi\n\t"
    "cpuid\n\t"
    "xchg %%esi, %%ebx"
    : "=a" (r.eax), "=S" (r.ebx), "=c" (r.ecx), "=d" (r.edx)
    : "a" (level), "c" (op)
  );
#endif

  return r;
}

std::string parse_vendor_string(const Registers& r)
{
  unsigned int vendor[4];
  
  vendor[0] = r.ebx;
  vendor[1] = r.edx;
  vendor[2] = r.ecx;
  vendor[3] = 0;
  
  return std::string(reinterpret_cast<char *>(vendor));
}

std::string parse_brand_string(const Registers& r)
{
  unsigned int brand[5];
  
  brand[0] = r.eax;
  brand[1] = r.ebx;
  brand[2] = r.ecx;
  brand[3] = r.edx;
  brand[4] = 0;

  return std::string(reinterpret_cast<char *>(brand));
}

#endif

} // anonymous

namespace geekinfo {
  
Processor::Processor()
: m_architecture(kArchitectureUnknown)
, m_vendor(kVendorUnknown)
, m_clock(0)
, m_multiplier(0)
, m_bus(0)
, m_threads(0)
, m_cores(0)
, m_processors(0)
, m_l1i_cache(0)
, m_l1d_cache(0)
, m_l2_cache(0)
, m_l3_cache(0)
{
  gather_information();
}

Processor::~Processor()
{ 
}

#if defined(ARCH_X86)

void Processor::gather_information()
{
  Registers r;
  
  // Determine the processor vendor
  {
    r = call_cpuid(0);
    m_vendor_string = parse_vendor_string(r);
    
    if (m_vendor_string == "AuthenticAMD") {
      m_vendor = kVendorAMD;
    } else if (m_vendor_string == "GenuineIntel") {
      m_vendor = kVendorIntel;
    } else if (m_vendor_string == "CentaurHauls") {
      m_vendor = kVendorVIA;
    } else {
      m_vendor = kVendorUnknown;
    }
  }
  
  // Determine the processor signature
  {
    r = call_cpuid(1);
    
    // TODO: This assumes we're running on an Intel processor; the process
    // for finding out family, model, and stepping might be different on
    // processors from other manufacturers.
    
    unsigned int family = (r.eax & 0xf00) >> 8;
    unsigned int model = (r.eax & 0xf0) >> 4;
    unsigned int stepping = r.eax & 0xf;
    
    if (family == 0x06 || family == 0x0f) {
      model += (r.eax & 0x000f0000) >> 12;
      if (family == 0x0f) {
        family += (r.eax & 0xff00000) >> 20;
      }
    }
    
    std::ostringstream oss;
    
    oss << std::setw(2) << std::setfill('0') << std::setbase(16) << family;
    oss << std::setw(0) << model;
    oss << std::setw(0) << stepping;
    oss << "h";

    m_id = oss.str();
  }
  
  // Determine the processor string
  {
    m_brand = parse_brand_string(call_cpuid(0x80000002));
    m_brand += parse_brand_string(call_cpuid(0x80000003));
    m_brand += parse_brand_string(call_cpuid(0x80000004));
  }
  
  // TODO: Leverage the x86 processor lookup table to determine the name 
  // of the processor.  Alternatively, do something clever that "massages"
  // the processor brand into the name.
  
  // TODO: Determine processor frequency, bus frequency, and bus multiplier.
  
  // TODO: Determine the number of processors/cores/threads
  
  // TODO: Determine the amount of cache
}

#else

void Processor::gather_information()
{
  //throw std::exception("Processor::gather_information() not yet implemented");
}

#endif

} // namespace geekinfo
