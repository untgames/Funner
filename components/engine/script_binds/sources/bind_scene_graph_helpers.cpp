#include "scene_graph.h"

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
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_BOX_HELPER_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_box_helper));

    //регистрация операций

  lib.Register ("set_BoundBox", make_invoker (&set_bound_box));

    //регистрация типов данных

  environment.RegisterType<helpers::Box> (SCENE_BOX_HELPER_LIBRARY);
}

}

}
