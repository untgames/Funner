/*
	system_old.cpp

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

#include "geekinfo.h"
#include "basesystem.h"
#include "platform.h"
#include <iomanip>
#include <sstream>
#include <map>


typedef uint64 (BaseSystem::*I)(void)	;
typedef std::string (BaseSystem::*S)(void)	;

struct SystemMeta {
  SystemMetricType				id;
  SystemMetricTypeType		type;
  std::string						  name;
  I								        imethod;
  S								        smethod;
};

struct MetricValue {
  std::string		name;
  std::string		svalue;
  uint64			  ivalue;
};

static std::map< SystemMetricType, MetricValue >	s_systemInformation;
static bool s_systemInformationInitialized = false;

static SystemMeta s_systemMeta[] = {
  { kSystemMetricPlatform,			    kSystemMetricTypeString,	"Platform",					    NULL, &BaseSystem::platform },
  { kSystemMetricCompiler,			    kSystemMetricTypeString,	"Compiler",					    NULL,	&BaseSystem::compiler },
  { kSystemMetricOS,					      kSystemMetricTypeString,	"Operating System",			NULL,	&BaseSystem::os },
  { kSystemMetricModel, 				    kSystemMetricTypeString,	"Model", 					      NULL,	&BaseSystem::model },
  { kSystemMetricMotherboard,			  kSystemMetricTypeString,	"Motherboard", 				  NULL,	&BaseSystem::motherboard },
  { kSystemMetricBIOS,              kSystemMetricTypeString,	"BIOS", 				        NULL,	&BaseSystem::bios },
  { kSystemMetricCPU,					      kSystemMetricTypeString,	"Processor", 				    NULL,	&BaseSystem::cpu },
  { kSystemMetricCPUID,				      kSystemMetricTypeString,	"Processor ID", 			  NULL,	&BaseSystem::cpuid },
  { kSystemMetricCPULogicalCount, 	kSystemMetricTypeCount,		"Logical Processors", 	&BaseSystem::cpu_logical_count,	NULL },
  { kSystemMetricCPUCoreCount, 	    kSystemMetricTypeCount,		"Processor Cores", 	    &BaseSystem::cpu_core_count,	NULL },
  { kSystemMetricCPUPhysicalCount,	kSystemMetricTypeCount,		"Physical Processors", 	&BaseSystem::cpu_physical_count, NULL },
  { kSystemMetricCPUFrequency,		  kSystemMetricTypeHertz,		"Processor Frequency", 	&BaseSystem::cpu_frequency, NULL },
  { kSystemMetricCPUL1ICache,			  kSystemMetricTypeBytes,		"L1 Instruction Cache", &BaseSystem::cpu_l1_inst_cache,	NULL },
  { kSystemMetricCPUL1DCache,			  kSystemMetricTypeBytes,		"L1 Data Cache", 			  &BaseSystem::cpu_l1_data_cache,	NULL },
  { kSystemMetricCPUL2Cache,			  kSystemMetricTypeBytes,		"L2 Cache", 				    &BaseSystem::cpu_l2_cache, NULL },
  { kSystemMetricCPUL3Cache,			  kSystemMetricTypeBytes,		"L3 Cache", 				    &BaseSystem::cpu_l3_cache, NULL },
  { kSystemMetricBusFrequency,		  kSystemMetricTypeHertz,		"Bus Frequency", 			  &BaseSystem::bus_frequency, NULL },
  { kSystemMetricMemorySize,			  kSystemMetricTypeBytes,		"Memory", 					    &BaseSystem::memory_size, NULL },
  { kSystemMetricMemoryType,			  kSystemMetricTypeString,	"Memory Type", 				  NULL,	&BaseSystem::memory_type },
  { kSystemMetricFreeMemory,			  kSystemMetricTypeBytes,		"Free Memory",				  &BaseSystem::memory_free, NULL },
  { kSystemMetricDisplay,				    kSystemMetricTypeString,	"Display",					    NULL, &BaseSystem::display },
  { kSystemMetricSIMD,				      kSystemMetricTypeCount,		"SIMD",						      &BaseSystem::simd, NULL },
  { kSystemMetricCPUModel,          kSystemMetricTypeString,  "Processor Model",      NULL, &BaseSystem::cpu_model },
  { kSystemMetricCPUFeatures,       kSystemMetricTypeString,  "Processor Features",   NULL, &BaseSystem::cpu_features },
  { kSystemMetricSystemLoad,        kSystemMetricTypeString,  "System Load",          NULL, &BaseSystem::system_load },
  { kSystemMetricNone, }
};

std::string formatMetric(uint64 value, SystemMetricTypeType type)
{
  std::ostringstream  s;
  double              divisor;
  static char *		prefixString[] = { "", "K", "M", "G", "T", "P", "E", "Z", "Y" };
  unsigned int		prefix;
  double				result = value;

  switch(type) {
    case kSystemMetricTypeCount:
      s << value;
      break;

    default:
    switch(type) {
      case kSystemMetricTypeBytes:
      divisor = 1024.0;
      break;
      default:
      divisor = 1000.0;
      break;
    }
    for (prefix = 0; prefix < (sizeof(prefixString) / sizeof(prefixString[0])); prefix++) {
      if (result < divisor) {
        break;
      }
      result /= divisor;
    }
    s.setf(std::ios::fixed);
    if (result > 100.0) {
      s << std::setprecision(0);
    } else if (result > 10.0) {
      s << std::setprecision(1);
    } else {
      s << std::setprecision(2);
    }
    s << result << " " << prefixString[ prefix ];
    switch(type) {
      case kSystemMetricTypeHertz:
        s << "Hz";
        break;
      case kSystemMetricTypeBytes:
        s << "B";
        break;
      default:
        break;
    }
    break;
  }
  return s.str();
}

static void initializeSystemInformation()
{
  BaseSystem * 	baseSystem = system_factory();
  MetricValue		value;

  for (int i = 0; s_systemMeta[i].id != kSystemMetricNone; i++) {
    value.name = s_systemMeta[i].name;
    switch(s_systemMeta[i].type) {
      case kSystemMetricTypeString:
        value.svalue = (baseSystem->*(s_systemMeta[i].smethod))();
        value.ivalue = 0;
        break;

      case kSystemMetricTypeCount:
      case kSystemMetricTypeHertz:
      case kSystemMetricTypeBytes: 
        value.ivalue = (baseSystem->*(s_systemMeta[i].imethod))();
        value.svalue = formatMetric(value.ivalue, s_systemMeta[i].type);
        break;
    }
    s_systemInformation[ s_systemMeta[i].id ] = value;
  }

  s_systemInformationInitialized = true;

  delete baseSystem;
}

std::string systemMetric(SystemMetricType metric)
{
  if (!s_systemInformationInitialized) {
    initializeSystemInformation();
  }

  return s_systemInformation[metric].svalue;
}

uint64 systemMetricInteger(SystemMetricType metric)
{
  if (!s_systemInformationInitialized) {
    initializeSystemInformation();
  }

  return s_systemInformation[ metric ].ivalue;
}

std::string systemMetricName(SystemMetricType metric)
{
  if (!s_systemInformationInitialized) {
    initializeSystemInformation();
  }

  return s_systemInformation[ metric ].name;
}
