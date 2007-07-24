#include <media/collada/controller.h>

using namespace medialib::collada;

/*
    Реализация библиотеки
*/

struct Controller::Impl
{
  ControllerLibrary&        library; //библиотека
  medialib::collada::Skin&  skin;    //скин, связанный с контроллером
  medialib::collada::Morph& morph;   //морф, связанный с контроллером
  medialib::collada::ControllerType type;

  Impl (collada::ControllerType in_type, collada::Morph& in_morph, collada::Skin&  in_skin, ControllerLibrary& in_library) : type (in_type), 
  skin (skin), morph (morph), library (in_library) {}
};

/*
    Конструктор / деструктор
*/

Controller::Controller (medialib::collada::ControllerType type, medialib::collada::Morph& morph, medialib::collada::Skin&  skin,
                        ControllerLibrary& library, const char* id)
  : Entity (library.Owner (), id),
    impl (new Impl (type, morph, skin, library))
    {}

Controller::~Controller ()
{
  delete impl;
}

/*
    Библиотека
*/

ControllerLibrary& Controller::Library () const
{
  return impl->library;
}

/*
    Тип контроллера
*/
ControllerType Controller::Type () const
{
  return impl->type;
}

/*
    Морф / скин
*/

const medialib::collada::Morph& Controller::Morph () const
{
  return impl->morph;
}

medialib::collada::Morph& Controller::Morph ()
{
  return impl->morph;
}

const medialib::collada::Skin& Controller::Skin () const
{
  return impl->skin;
}

medialib::collada::Skin& Controller::Skin ()
{
  return impl->skin;
}
