#ifndef SCRIPTBINDS_SHARED_HEADER
#define SCRIPTBINDS_SHARED_HEADER

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/implicit_cast.h>

#include <bv/axis_aligned_box.h>

#include <common/component.h>

#include <script/bind.h>
#include <script/environment.h>
#include <script/library_manager.h>

namespace engine
{

void bind_file_library        (script::Environment&);
void bind_common_string_tree  (script::Environment&);
void bind_common_var_registry (script::Environment&);

}

#endif
