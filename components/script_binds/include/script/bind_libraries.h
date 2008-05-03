#ifndef SCRIPTBINDS_BIND_LIBRARIES_HEADER
#define SCRIPTBINDS_BIND_LIBRARIES_HEADER

namespace script
{

//forward declarations
class Environment;

/*
    Регистрация библиотек
*/

void bind_math_library (Environment&);
void bind_scene_graph_library (Environment&);
void bind_bv_library (Environment&);

}

#endif
