#include "scene_graph.h"

using namespace scene_graph;

template class engine::decl_sg_cast<helpers::Helper,  Node>;
template class engine::decl_sg_cast<helpers::Helper,  Entity>;
template class engine::decl_sg_cast<helpers::Helper,  VisualModel>;
template class engine::decl_sg_cast<helpers::Box,     Node>;
template class engine::decl_sg_cast<helpers::Box,     Entity>;
template class engine::decl_sg_cast<helpers::Box,     VisualModel>;
template class engine::decl_sg_cast<helpers::Box,     helpers::Helper>;
//template class engine::decl_sg_cast<helpers::Sphere,  Node>;
//template class engine::decl_sg_cast<helpers::Sphere,  Entity>;
//template class engine::decl_sg_cast<helpers::Sphere,  helpers::Helper>;

namespace engine
{

namespace scene_graph_script_binds
{

/*
    Создание хелперов
*/

helpers::Box::Pointer create_box_helper ()
{
  return helpers::Box::Create ();
}

/*
   Регистрация библиотеки работы с хелперами
*/

void set_bound_box (helpers::Box& entity, const bound_volumes::aaboxf& box)
{
  entity.SetBoundBox (box);
}

void bind_box_helper_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_BOX_HELPER_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_VISUAL_MODEL_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_box_helper));

    //регистрация операций

  lib.Register ("set_BoundBox", make_invoker (&set_bound_box));

    //регистрация типов данных

  environment.RegisterType<helpers::Box> (SCENE_BOX_HELPER_LIBRARY);
}

}

}
