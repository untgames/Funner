/*
	Memory.hpp

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

#include <string>
#include <vector>

namespace geekinfo {

/// Memory provides information about the memory modules installed in the 
/// system and the memory slots present on the motherboard.

class Memory {
public:
  Memory();
  ~Memory();
  
  /// Return the size of installed memory in megabytes.
  unsigned int get_memory_size() const { return m_size; }
  
  /// Returns thes size of the memory modules installed in the system in megabytes.
  const std::vector<unsigned int>& get_module_sizes() const { return m_module_sizes; }

  /// Returns thes speed of the memory modules installed in the system in megabytes.
  const std::vector<unsigned int>& get_module_speeds() const { return m_module_speeds; }

  /// Returns the type of modules installed in each memory slot.  
  const std::vector<std::string>& get_module_types() const { return m_module_types; }
  
  /// Returns the locations of all the memory modules in the system.
  const std::vector<std::string>& get_module_locations() const { return m_module_locations; }
  
private:
  unsigned int m_size;
  std::vector<unsigned int> m_module_sizes;
  std::vector<unsigned int> m_module_speeds;
  std::vector<std::string> m_module_types;
  std::vector<std::string> m_module_locations;
  
  void gather_information();
};

} // namespace geekinfo