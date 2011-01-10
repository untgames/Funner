#ifndef COMMONLIB_LICENSE_MANAGER_SHARED_HEADER
#define COMMONLIB_LICENSE_MANAGER_SHARED_HEADER

#include <stl/string>
#include <stl/vector>

#include <xtl/common_exceptions.h>
#include <xtl/reference_counter.h>

#include <common/component.h>
#include <common/file.h>
#include <common/license_manager.h>
#include <common/parser.h>
#include <common/property_map.h>
#include <common/singleton.h>
#include <common/string.h>
#include <common/strlib.h>

namespace common
{

struct CheckFile
{
  stl::string name;
  size_t      max_hash_size;
  
  CheckFile (const char* in_name, size_t in_max_hash_size = 0)
    : name (in_name)
    , max_hash_size (in_max_hash_size)
  { }
};

void calculate_license_hash (const stl::vector<CheckFile>& check_files_list, const PropertyMap& properties, unsigned char hash [16]);

}

#endif
