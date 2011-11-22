/*
	linuxsystem.cpp

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

#include "linuxsystem.h"
#include "platform.h"
#include <sstream>	
#include <cstdlib>

#if !defined(PLATFORM_LINUX)
    #error Wrong platform
#endif

#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <regex.h>
#include <fstream>
#include <set>

#if defined(ARCH_PPC)
#include "macosxmodel.h"
#endif

#if defined(ARCH_X86)
#include "x86processor.h"

struct Registers {
	uint32	eax;
	uint32	ebx;
	uint32 	ecx;
	uint32 	edx;
};

static Registers cpuID(uint32 level)
{
	Registers	r;

    asm volatile("cpuid"
        : "=a" (r.eax), "=b" (r.ebx), "=c" (r.ecx), "=d" (r.edx)
        : "a" (level));

	return r;
}

static std::string cpu()
{
  std::string result;
  Registers	r; // r.eax, r.ebx, r.ecx, r.edx;
  
  r = cpuID(0x80000000);
  if (r.eax & 0x80000000 && r.eax >= 0x80000004) {
    uint32 brand[13];
    char * p;

    r = cpuID(0x80000002);
    brand[0] = r.eax;
    brand[1] = r.ebx;
    brand[2] = r.ecx;
    brand[3] = r.edx;
    r = cpuID(0x80000003);
    brand[4] = r.eax;
    brand[5] = r.ebx;
    brand[6] = r.ecx;
    brand[7] = r.edx;
    r = cpuID(0x80000004);
    brand[8] = r.eax;
    brand[9] = r.ebx;
    brand[10] = r.ecx;
    brand[11] = r.edx;
    brand[12] = 0;
    p = (char *)brand;
    while (*p != 0 && *p == ' ') {
        p++;
    }
    result = p;
  }
  return result;
}

static std::string cpuid()
{
    std::ostringstream  s;
    Registers			r;
    uint32              family; //, extendedFamily;
    uint32              model; //, extendedModel;
    uint32              stepping;
    uint32				vendor[4];
    char *				p;

    r = cpuID(1);
    family = (r.eax & 0xf00) >> 8;
    model = (r.eax & 0xf0) >> 4;
    stepping = r.eax & 0xf;
    if (family == 0x06 || family == 0x0f) {
        if (family == 0x0f) {
            family += ((r.eax & 0xff00000) >> 20);
        }
        model += ((r.eax & 0x000f0000) >> 12);
    }
	//std::cout << "!" << std::endl;

    r = cpuID(0);
    vendor[0] = r.ebx;
    vendor[1] = r.edx;
    vendor[2] = r.ecx;
    vendor[3] = 0;
    p = (char *)vendor;
    
    s << p << " Family " << family << " Model " << model << " Stepping " << stepping;
    return s.str();
}

static bool cpusse()
{
	Registers	r;
    
    r = cpuID(0x00000001);
	if (r.edx & 0x02000000) {
		return true;
	} else {
		return false;
	}
}

#else

static std::string cpu() 
{
  return "";
}

static std::string cpuid()
{
  return "";
}

#endif

#define COMPILE_RE(v,r) if (regcomp(v, r, REG_ICASE)) { return false; }

bool LinuxSystem::parseCpuInfo() 
{
  std::ifstream		  cpuinfo("/proc/cpuinfo");
  std::string			  buffer;
	regmatch_t			  match[2];
	std::set< int >		physicalid;
	
	regex_t			processorRE;
	regex_t			physicalidRE;
	regex_t			mhzRE;
	regex_t			cacheRE;

	regex_t			clockRE;
	regex_t			cpuRE;
	regex_t			machineRE;
	regex_t			motherboardRE;
	regex_t altivecRE;
	if (!cpuinfo.is_open()) {
		return false;
	}
	
	COMPILE_RE(&processorRE, "processor[[:space:]]*: \\([[:digit:]]*\\)");
	COMPILE_RE(&physicalidRE, "physical id[[:space:]]*: \\([[:digit:]]*\\)");
	COMPILE_RE(&mhzRE, "cpu MHz[[:space:]]*: \\([[:digit:]]*\\)");
	COMPILE_RE(&cacheRE, "cache size[[:space:]]*: \\([[:digit:]]*\\)");
	COMPILE_RE(&clockRE, "clock[[:space:]]*: \\([[:digit:]]*\\)");
	COMPILE_RE(&cpuRE, "cpu[[:space:]]*: \\([^$]*\\)");
	COMPILE_RE(&machineRE, "machine[[:space:]]*: \\([^$]*\\)");
	COMPILE_RE(&motherboardRE, "motherboard[[:space:]]*: \\([^$]*\\)");
	COMPILE_RE(&altivecRE, "cpu[[:space:]]*: .* altivec supported");
	if (cpuinfo.is_open()) {    	
		while (getline(cpuinfo, buffer)) {
			if (regexec(&processorRE, buffer.c_str(), 2, match, 0) == 0) {
				m_logicalCount += 1;
			}			
			if (regexec(&physicalidRE, buffer.c_str(), 2, match, 0) == 0) {
			  int id = strtol(buffer.substr(match[1].rm_so, match[1].rm_eo).c_str(), NULL, 0);
				physicalid.insert(id);
			}
			if (regexec(&mhzRE, buffer.c_str(), 2, match, 0) == 0) {
			  m_mhz = strtol(buffer.substr(match[1].rm_so, match[1].rm_eo).c_str(), NULL, 0);
			}
			if (regexec(&cacheRE, buffer.c_str(), 2, match, 0) == 0) {
			  m_cacheSize = strtol(buffer.substr(match[1].rm_so, match[1].rm_eo).c_str(), NULL, 0);
			}
			if (regexec(&clockRE, buffer.c_str(), 2, match, 0) == 0) {
			  m_mhz = strtol(buffer.substr(match[1].rm_so, match[1].rm_eo).c_str(), NULL, 0);
			}
			if (regexec(&cpuRE, buffer.c_str(), 2, match, 0) == 0) {
				m_cpu = buffer.substr(match[1].rm_so, match[1].rm_eo);
			}
			if (regexec(&machineRE, buffer.c_str(), 2, match, 0) == 0) {
				m_model = buffer.substr(match[1].rm_so, match[1].rm_eo);
			}
			if (regexec(&motherboardRE, buffer.c_str(), 2, match, 0) == 0) {
				m_motherboard =buffer.substr(match[1].rm_so, match[1].rm_eo);
			}
			if (regexec(&altivecRE, buffer.c_str(), 2, match, 0) == 0) {
			  m_altivec = true;
			}
			
		}
	}
	m_physicalCount = physicalid.size();
	if (m_physicalCount == 0) {
	    m_physicalCount = 1;
	}

	return true;
}

LinuxSystem::LinuxSystem()
{
	m_logicalCount = 0;
	m_physicalCount = 0;
	m_mhz = 0;
	m_cacheSize = 0;
	m_altivec = false;
	
	if (!parseCpuInfo()) {
		m_logicalCount = 1;
		m_physicalCount = 1;
	}
}

LinuxSystem::~LinuxSystem()
{
}

#if defined(ARCH_X86)
static uint32 getL1Dcache()
{
  Registers	r; 
  uint32    result = 0;

  r = cpuID(0x80000000);
  if (r.eax & 0x80000000 && r.eax >= 0x80000005) {
    r = cpuID(0x80000005);
    result = 1024 * (r.ecx >> 24); // drop lower 24 bits, convert to bytes
  }

  return result;
}

static uint32 getL1Ccache()
{
  Registers	r;
  uint32    result = 0;

  r = cpuID(0x80000000);
  if (r.eax & 0x80000000 && r.eax >= 0x80000005) {
    r = cpuID(0x80000005);
    result = 1024 * (r.edx >> 24); // drop lower 24 bits, convert to bytes
  }

  return result;
}

static uint32 getL2cache()
{
  Registers	r;
  uint32    result = 0;

  r = cpuID(0x80000000);
  if (r.eax & 0x80000000 && r.eax >= 0x80000006) {
    r = cpuID(0x80000006);
    result = 1024 * (r.ecx >> 16); // drop lower 16 bits, convert to bytes
  }

  return result;
}

static uint32 getL3cache()
{
  Registers	r;
  uint32    result = 0;

  r = cpuID(0x80000000);
  if (r.eax & 0x80000000 && r.eax >= 0x80000006) {
    r = cpuID(0x80000006);
    result = 524288 * (r.edx >> 18); // drop lower 18 bits, convert from 512KB chunks to bytes
  }

  return result;
}
#endif


std::string LinuxSystem::os()
{
    std::ostringstream s;
    struct utsname name;

    uname(&name);
    s << name.sysname << " " << name.release << " " << name.machine;
    return s.str();
}

std::string LinuxSystem::model()
{
  std::ostringstream s;

#if defined(ARCH_PPC) 
	if (m_model.length()) {
	  return get_model_name(m_model);
	}
#endif

  s << "Linux PC (" << cpu() << ")";
  return s.str();
}

std::string LinuxSystem::motherboard()
{
	if (m_motherboard.length()) {
		return m_motherboard;
	}
    return "Unknown Motherboard";
}

std::string LinuxSystem::cpu()
{
	if (m_cpu.length()) {
		return getX86Processor(m_cpu, cpuid());
	}
  return getX86Processor(::cpu(), cpuid());
}

std::string LinuxSystem::cpuid()
{
  if (m_cpu.length()) {
    return m_cpu;
  }
  return ::cpuid();
}

uint64 LinuxSystem::cpu_logical_count()
{
    return m_logicalCount;
}

uint64 LinuxSystem::cpu_core_count()
{
    // TODO: Is this the correct thing to do for Linux?
    return m_logicalCount;
}

uint64 LinuxSystem::cpu_physical_count()
{
    return m_physicalCount;
}

uint64 LinuxSystem::cpu_frequency()
{
	return m_mhz * 1000000;
}

uint64 LinuxSystem::cpu_l1_inst_cache()
{
  return (uint64)getL1Ccache();
}

uint64 LinuxSystem::cpu_l1_data_cache()
{
  return (uint64)getL1Dcache();
}

uint64 LinuxSystem::cpu_l2_cache()
{
  return (uint64)getL2cache();
}

uint64 LinuxSystem::cpu_l3_cache()
{
  return (uint64)getL3cache();
}

uint64 LinuxSystem::bus_frequency()
{
    return 0;
}

uint64 LinuxSystem::memory_size()
{
    struct sysinfo si;

    if (!sysinfo(&si)) {
        return uint64(si.totalram) * si.mem_unit;
    }
    return 0;
}

uint64 LinuxSystem::memory_free()
{
    struct sysinfo si;

    if (!sysinfo(&si)) {
        return uint64(si.freeram) * si.mem_unit;
    }
    return 0;
}

uint64 LinuxSystem::simd()
{
#if defined(ARCH_X86)
	return cpusse();
#elif defined(ARCH_PPC)
	return m_altivec;
#else
	#error Unsupported architecture.
#endif
}
