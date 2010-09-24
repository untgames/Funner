#include "scene_graph.h"

namespace engine
{

namespace scene_graph_script_binds
{

/*
    Создание слушателя
*/

Listener::Pointer create_listener ()
{
  return Listener::Create ();
}

/*
   Регистрация библиотеки работы с слушателями
*/

void bind_listener_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_LISTENER_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_listener));

    //регистрация операций

  lib.Register ("set_Gain", make_invoker (&Listener::SetGain));
  lib.Register ("get_Gain", make_invoker (&Listener::Gain));

    //регистрация типов данных

  environment.RegisterType<Listener> (SCENE_LISTENER_LIBRARY);
}

/*
    Создание источника звука
*/

SoundEmitter::Pointer create_sound_emitter (const char* sound_declaration_name)
{
  return SoundEmitter::Create (sound_declaration_name);
}

/*
   Регистрация библиотеки работы с источниками звука
*/

void bind_sound_emitter_library (Environment& environment)
{
  InvokerRegistry& lib = environment.CreateLibrary (SCENE_SOUND_EMITTER_LIBRARY);

    //наследование

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (&create_sound_emitter));

    //регистрация операций

  lib.Register ("set_Gain", make_invoker (&SoundEmitter::SetGain));
  lib.Register ("get_Gain", make_invoker (&SoundEmitter::Gain));

  lib.Register ("get_SoundDeclarationName", make_invoker (&SoundEmitter::SoundDeclarationName));

  lib.Register ("Play", make_invoker (&SoundEmitter::Play));
  lib.Register ("Stop", make_invoker (&SoundEmitter::Stop));

    //регистрация типов данных

  environment.RegisterType<SoundEmitter> (SCENE_SOUND_EMITTER_LIBRARY);
}

}

}

