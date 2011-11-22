/*
	basesystem.cpp

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

#include "basesystem.h"
#include "platform.h"
#include "smbios.h"
#include "x86processor.h"

#if defined(PLATFORM_LINUX)
  #include "linuxsystem.h"
#elif defined(PLATFORM_MACOSX)
  #include "macosxsystem.h"
#elif defined(PLATFORM_WINDOWS)
  #include "win32system.h"
#elif defined(PLATFORM_SOLARIS)
  #include "solarissystem.h"
#else
  #error Unsupported platform
#endif

#include <sstream>

std::string BaseSystem::platform()
{
  std::ostringstream	s;

  #if defined(PLATFORM_MACOSX)
    s << "Mac OS X";
  #elif defined(PLATFORM_LINUX)
    s << "Linux";
  #elif defined(PLATFORM_WINDOWS)
    s << "Windows";
  #elif defined(PLATFORM_SOLARIS)
    s << "Solaris";
  #else
    #error Unknown platform
  #endif

  #if defined(ARCH_X86)
    s << " x86";
  #elif defined(ARCH_PPC)
    s << " PPC";
  #elif defined(ARCH_SPARC)
    s << " SPARC";
  #else
    #error Unsupported architecture
  #endif

  #if defined(ARCH_64BIT)
    s << " (64-bit)";
  #elif defined(ARCH_32BIT) 
    s << " (32-bit)";
  #else
    #error Unsupported architecture
  #endif

  return s.str();
}

std::string BaseSystem::compiler()
{
  #if defined(PLATFORM_SOLARIS)
    return "Sun Studio 12";
  #elif defined(PLATFORM_UNIX)
    return "GCC " __VERSION__;
  #elif defined(PLATFORM_WINDOWS)
    #if _MSC_VER >= 1500
      return "Visual C++ 2008";
    #elif _MSC_VER >= 1400
      return "Visual C++ 2005";
    #elif _MSC_VER >= 1310
      return "Visual C++ 2003";
    #elif _MSC_VER > 1300
      return "Visual C++ 2002";
    #endif
  #else
    #error Unknown compiler
  #endif
}

std::string BaseSystem::bios()
{
  const geekinfo::SMBIOS* smbios = geekinfo::fetch_smbios();
  
  if (smbios != NULL) {
    return smbios->bios.vendor + " " + smbios->bios.version;
  }
  return "N/A";
}

std::string BaseSystem::cpu_model()
{
#if defined(ARCH_X86)
  return getX86Processor(cpu(), cpuid());
#else
  return cpu();
#endif
}

BaseSystem * system_factory()
{
  #if defined(PLATFORM_MACOSX)
    return new MacOSXSystem();
  #elif defined(PLATFORM_LINUX)
    return new LinuxSystem();
  #elif defined(PLATFORM_WINDOWS)
    return new Win32System();
  #elif defined(PLATFORM_SOLARIS)
    return new SolarisSystem();
  #else
    #error system_factory() return undefined.
    return NULL;
  #endif
}
