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

void calculate_license_hash (const StringArray& check_files_list, const PropertyMap& properties, unsigned char hash [16]);

}

#endif
