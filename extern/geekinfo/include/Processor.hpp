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

#ifndef INC_GEEKINFO_PROCESSOR_HPP
#define INC_GEEKINFO_PROCESSOR_HPP

#include <string>

namespace geekinfo {
  
/// Processor provides information about the processor (or processors) present
/// in the system.  Processor represents every processor present; seperate 
/// instances are not required for each processor. 

class Processor {
public:
  enum Architecture {
    kArchitectureX86,
    kArchitecturePowerPC,
    kArchitectureUnknown
  };
  
  enum Vendor {
    kVendorAMD,
    kVendorIBM,
    kVendorIntel,
    kVendorMotorola,
    kVendorVIA,
    kVendorUnknown
  };
  
  Processor();
  ~Processor();

  /// Returns the processor architecture of the system.
  Architecture get_architecture() const { return m_architecture; }
  
  /// Returns the processor manufacturer.
  Vendor get_vendor() const { return m_vendor; }
  
  /// Returns the processor manufacturer as a string.
  const std::string& get_vendor_string() { return m_vendor_string; }
  
  /// Returns the processor id string.
  const std::string& get_id() const { return m_id; }
  
  /// Returns the processor brand string.
  const std::string& get_brand() const { return m_brand; }
  
  /// Returns the processor name.
  ///
  /// Under X86, the processor name is determined using the id and brand strings.
  /// Under PowerPC, the processor name is the same as the processor brand.  
  const std::string& get_name() const { return m_name; }
  
  /// Returns the processor's package.
  const std::string& get_package() const { return m_package; }

  /// Returns the processor's clock speed in MHz.
  unsigned int get_clock_speed() const { return m_clock; }
  
  /// Returns the processor's bus multiplier.
  unsigned int get_multiplier() const { return m_multiplier; }
  
  /// Returns the processor's bus speed in MHz.
  unsigned int get_bus_speed() const { return m_bus; }

  /// Returns the number of execution threads in the system.
  unsigned int get_thread_count() const { return m_threads; }
  
  /// Returns the number of processor cores in the system.
  unsigned int get_core_count() const { return m_cores; }
  
  /// Returns the number of processors in the system.
  unsigned int get_processor_count() const { return m_processors; }
  
  /// Returns the size of the L1 instruction cache in KB.
  unsigned int get_L1_instruction_cache_size() const { return m_l1i_cache; }
  
  /// Returns the size of the L1 data cache size in KB.
  unsigned int get_L1_data_cache_size() const { return m_l1d_cache; }
  
  /// Returns the size of the L2 cache in KB.
  unsigned int get_L2_cache_size() const { return m_l2_cache; }
  
  /// Returns the size of the L3 cache in KB.
  unsigned int get_L3_cache_size() const { return m_l3_cache; }
  
private:
  Architecture m_architecture;
  Vendor m_vendor;
  std::string m_vendor_string;
  
  std::string m_id;
  std::string m_brand;
  std::string m_name;
  std::string m_package;
  
  unsigned int m_clock;
  unsigned int m_multiplier;
  unsigned int m_bus;
  
  unsigned int m_threads;
  unsigned int m_cores;
  unsigned int m_processors;
  
  unsigned int m_l1i_cache;
  unsigned int m_l1d_cache;
  unsigned int m_l2_cache;
  unsigned int m_l3_cache;
  
  void gather_information();
};

} // namespace geekinfo

#endif
