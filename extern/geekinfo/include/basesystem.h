#ifndef INC_BASESYSTEM_H
#define INC_BASESYSTEM_H

#include "types.h"
#include <string>

class BaseSystem {
public:
  BaseSystem() {}
  virtual ~BaseSystem() {}

  virtual std::string platform();
  virtual std::string compiler();

  virtual std::string os() { return "N/A"; } 
  virtual std::string model() { return "N/A"; }
  virtual std::string motherboard() { return "N/A"; }
  virtual std::string bios();
  
  virtual std::string system_load() { return "N/A"; }

  virtual std::string cpu() { return "N/A"; }
  virtual std::string cpuid() { return "N/A"; }
  virtual std::string cpu_features() { return "N/A"; }
  virtual std::string cpu_model();

  virtual uint64      cpu_logical_count() { return (uint64)-1; }
  virtual uint64      cpu_core_count() { return (uint64)-1; }
  virtual uint64      cpu_physical_count() { return (uint64)-1; }
  virtual uint64      cpu_frequency() { return (uint64)-1; }

  virtual uint64		  cpu_l1_inst_cache() { return (uint64)-1; }
  virtual uint64		  cpu_l1_data_cache() { return (uint64)-1; }
  virtual uint64		  cpu_l2_cache() { return (uint64)-1; }
  virtual uint64		  cpu_l3_cache() { return (uint64)-1; }

  virtual uint64      bus_frequency() { return (uint64)-1; };
  virtual uint64      memory_size() { return (uint64)-1; };
  virtual std::string memory_type() { return "N/A"; }	
  virtual uint64      memory_free() { return (uint64)-1; };

  virtual std::string display() { return "N/A"; }

  virtual uint64		  simd() { return (uint64)-1; }
};

BaseSystem * system_factory();

#endif
