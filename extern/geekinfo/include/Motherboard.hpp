/*
	Motherboard.hpp

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

#ifndef INC_GEEKINFO_MOTHERBOARD_HPP
#define INC_GEEKINFO_MOTHERBOARD_HPP

#include <string>

namespace geekinfo {
  
/// Motherboard provides information about the motherboard installed in the 
/// system.
  
class Motherboard {
public:
  Motherboard();
  ~Motherboard();
  
  /// Returns the motherboard model string.
  const std::string& get_model() const { return m_model; }
  
  /// Returns the motherboard manufacturer string.
  const std::string& get_manufacturer() const { return m_manufacturer; }
  
  /// Returns the motherboard name string.
  const std::string& get_name() const { return m_name; }
  
private:
  std::string m_model;
  std::string m_manufacturer;
  std::string m_name;
};
  
} // namespace geekinfo

#endif
