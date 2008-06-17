#include "shared.h"

using namespace media::collada;
using namespace common;

/*
    Описание реализации InstanceController
*/

typedef stl::vector<size_t> StringOffsetArray;

struct InstanceController::Impl: public xtl::reference_counter
{
  stl::string                   controller;        //имя контроллера
  media::collada::MaterialBinds binds;             //присоединённые материалы
  stl::string                   search_root_names; //имена корней поиска соединений
  StringOffsetArray             search_roots;      //смещения в search_root_names
};

/*
    Конструкторы / деструктор / присваивание
*/

InstanceController::InstanceController ()
  : impl (new Impl, false)
  {}

InstanceController::InstanceController (Impl* in_impl)
  : impl (in_impl, false)
  {}

InstanceController::InstanceController (const InstanceController& ic)
  : impl (ic.impl)
  {}

InstanceController::~InstanceController ()
{
}

InstanceController& InstanceController::operator = (const InstanceController& ic)
{
  impl = ic.impl;

  return *this;
}

/*
    Создание копии
*/

InstanceController InstanceController::Clone () const
{
  return InstanceController (new Impl (*impl));
}

/*
    Контроллер
*/

const char* InstanceController::Controller () const
{
  return impl->controller.c_str ();
}

void InstanceController::SetController (const char* controller_id)
{
  if (!controller_id)
    throw xtl::make_null_argument_exception ("media::collada::InstanceController::SetController", "controller_id");
    
  impl->controller = controller_id;
}

/*
    Определение имени базового меша
*/

const char* InstanceController::FindBaseMesh (Model& model) const
{
  const char* controller_id = impl->controller.c_str ();

  Morph* morph = model.Morphs ().Find (controller_id);
  
  if (morph)
    return morph->BaseMesh ();
  
  Skin* skin = model.Skins ().Find (controller_id);

  if (!skin)
    return 0;

  if (model.Meshes ().Find (skin->BaseMesh ()))
    return skin->BaseMesh ();

  morph = model.Morphs ().Find (skin->BaseMesh ());

  if (morph)
    return morph->BaseMesh ();

  return 0;
}

/*
    Присоединённые материалы
*/

media::collada::MaterialBinds& InstanceController::MaterialBinds ()
{
  return impl->binds;
}

const media::collada::MaterialBinds& InstanceController::MaterialBinds () const
{
  return impl->binds;
}

/*
    Ссылки на корни поиска узлов для скининга
*/

size_t InstanceController::JointSearchRootsCount () const
{
  return impl->search_roots.size ();
}

const char* InstanceController::JointSearchRoot (size_t root_index) const
{
  if (root_index >= impl->search_roots.size ())
    throw xtl::make_range_exception ("media::collada::InstanceController::JointSearchRoot", "root_index", root_index, impl->search_roots.size ());
    
  return impl->search_root_names.c_str () + impl->search_roots [root_index];
}

size_t InstanceController::InsertJointSearchRoot (const char* node_id)
{
  if (!node_id)
    throw xtl::make_null_argument_exception ("media::collada::InstanceController::InsertJointSearchRoot", "node_id");
    
  size_t offset = impl->search_root_names.size ();
  
  try
  {    
    impl->search_root_names += node_id;
    impl->search_root_names += '\0';
  
    impl->search_roots.push_back (offset);
  }
  catch (...)
  {
    impl->search_root_names.erase (offset);
    throw;
  }

  return impl->search_roots.size () - 1;
}

void InstanceController::RemoveJointSearchRoot (size_t root_index)
{
  if (root_index >= impl->search_roots.size ())
    return;
    
  impl->search_roots.erase (impl->search_roots.begin () + root_index);
}

void InstanceController::RemoveAllJointSearchRoots ()
{
  impl->search_roots.clear ();
  impl->search_root_names.clear ();
}
