/*
	solarissystem.h

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


#ifndef INC_SOLARISSYSTEM_H
#define INC_SOLARISSYSTEM_H

#include "geekinfo.h"
#include "basesystem.h"
#include <sstream>

#include <unistd.h>
#include <sys/utsname.h>
#include <sys/systeminfo.h>
#include <kstat.h>
#include <set>
#include <iostream>
#include <smbios.h>

static std::string trim(const std::string s)
{
  std::string::size_type  p;
  std::string::size_type  q;

  p = s.find_first_not_of(' ');
  if (p == std::string::npos) {
    p = 0;
  }
  q = s.find_last_not_of(' ');
  if (q == std::string::npos) {
    q = s.length() - 1;
  }
  return s.substr(p, q - p + 1);
}


class SolarisSystem : public BaseSystem {
public:
  SolarisSystem() : BaseSystem() {
  }

  virtual ~SolarisSystem() {
  }

  virtual std::string os() {
    std::ostringstream s;
    struct utsname name;

    uname(&name);

    s << name.sysname << " " << name.release << " " << name.machine;
    return s.str();

  }

  virtual std::string model() {
    std::ostringstream s;

#if defined(ARCH_X86)
    smbios_hdl_t * shp;
    smbios_struct_t ss;
    smbios_info_t i;
    int err;

    shp = smbios_open(NULL, SMB_VERSION, 0, &err);
    err = smbios_lookup_id(shp, SMB_TYPE_SYSTEM, &ss);
    err = smbios_info_common(shp, ss.smbstr_id, &i);
    s << trim(i.smbi_manufacturer) << " " <<  trim(i.smbi_product);
    smbios_close(shp);
#elif defined(ARCH_SPARC)
    char buffer[256];
    sysinfo(SI_PLATFORM, buffer, sizeof(buffer));
    s << buffer;
#endif

    return s.str();
  }

  virtual std::string motherboard() {
    std::ostringstream s;

#if defined(ARCH_X86)
    smbios_hdl_t * shp;
    smbios_struct_t ss;
    smbios_info_t i;
    int err;

    shp = smbios_open(NULL, SMB_VERSION, 0, &err);
    err = smbios_lookup_id(shp, SMB_TYPE_BASEBOARD, &ss);
    err = smbios_info_common(shp, ss.smbstr_id, &i);
    s << trim(i.smbi_manufacturer) << " " <<  trim(i.smbi_product);
    smbios_close(shp);
#endif

    return s.str();
  }

  virtual std::string cpu() {
#if defined(ARCH_X86)
    kstat_ctl_t *kc;
    kstat_t *ksp;
    kstat_io_t kio;
    kstat_named_t *knp;

    kc = kstat_open();
    ksp = kstat_lookup(kc, (char *)"cpu_info", -1, NULL);
    kstat_read(kc, ksp, NULL);
    knp = (kstat_named_t *)kstat_data_lookup(ksp, (char *)"brand");

    return knp->value.string.addr.ptr;
#else
    return "";
#endif
  }

  virtual std::string cpuid() {
#if defined(ARCH_X86)
    kstat_ctl_t *kc;
    kstat_t *ksp;
    kstat_io_t kio;
    kstat_named_t *knp;

    kc = kstat_open();
    ksp = kstat_lookup(kc, (char *)"cpu_info", -1, NULL);
    kstat_read(kc, ksp, NULL);
    knp = (kstat_named_t *)kstat_data_lookup(ksp, (char *)"implementation");

    return knp->value.string.addr.ptr;
#else
    return "";
#endif
  }

  virtual uint64 cpu_logical_count() {
    kstat_ctl_t    *kc;
    kstat_t        *ksp;
    kstat_io_t     kio;

    int count = 0;

    kc = kstat_open();
    for (ksp = kc->kc_chain; ksp != NULL; ksp = ksp->ks_next) {
      if (strcmp(ksp->ks_module, "cpu_info") == 0) {
        count += 1;
      }
    }
    return count;
  }

  virtual uint64 cpu_core_count() {
    // TODO: Is this the correct thing to do for Solaris?
    return cpu_logical_count();
  }


  virtual uint64 cpu_physical_count() {
    kstat_ctl_t    *kc;
    kstat_t        *ksp;
    kstat_io_t      kio;
    kstat_named_t *knp;
    std::set<int>   physicalid;

    kc = kstat_open();
    for (ksp = kc->kc_chain; ksp != NULL; ksp = ksp->ks_next) {

      if (strcmp(ksp->ks_module, "cpu_info") == 0) {
        kstat_read(kc, ksp, NULL);
        knp = (kstat_named_t *)kstat_data_lookup(ksp, (char *)"chip_id");
        physicalid.insert(knp->value.i32);
      }
    }
    return physicalid.size();
  }

  virtual uint64 cpu_frequency() {
    kstat_ctl_t *kc;
    kstat_t *ksp;
    kstat_io_t kio;
    kstat_named_t *knp;

    kc = kstat_open();
    ksp = kstat_lookup(kc, (char *)"cpu_info", -1, NULL);
    kstat_read(kc, ksp, NULL);
    knp = (kstat_named_t *)kstat_data_lookup(ksp, (char *)"clock_MHz");

    return knp->value.i32 * 1000000;
  }
  
  virtual uint64 cpu_l1_inst_cache() { return 0; }
  virtual uint64 cpu_l1_data_cache() { return 0; }
  virtual uint64 cpu_l2_cache() { return 0; }
  virtual uint64 cpu_l3_cache() { return 0; }
  
  virtual uint64 bus_frequency() {
    return 0;
  }

  virtual uint64 memory_size() {
    return sysconf(_SC_PAGESIZE) * sysconf(_SC_PHYS_PAGES);
  }

  virtual uint64 simd() {
    return false;
  }
};

#endif
