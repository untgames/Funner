#ifndef SCRIPTBINDS_BIND_LIBRARIES_HEADER
#define SCRIPTBINDS_BIND_LIBRARIES_HEADER

namespace script
{

//forward declarations
class Environment;

namespace binds
{

/*
    Регистрация библиотек
*/

void bind_bv_library (Environment&);
void bind_common_library (Environment&);
void bind_common_string_tree (Environment&);
void bind_common_var_registry (Environment&);
void bind_math_library (Environment&);
void bind_render_library (Environment&);
void bind_scene_graph_library (Environment&);

}

}

#endif
