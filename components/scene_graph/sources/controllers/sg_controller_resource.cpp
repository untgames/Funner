#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::controllers;

/*
    Описание реализации контрллера ресурсов
*/

struct ResourceController::Impl: public xtl::instance_counter<ResourceController>
{
  media::rms::Binding binding; //связывание с группой ресурсов
  
  Impl (media::rms::Binding& in_binding) 
    : binding (in_binding)
  {
  }
};

/*
    Конструктор / деструктор
*/

ResourceController::ResourceController (Node& node, media::rms::Binding& binding)
  : Controller (node, false)
  , impl (new Impl (binding))
{
  NodeOwnsController ();
}

ResourceController::~ResourceController ()
{
}

/*
    Создание контроллера
*/

ResourceController::Pointer ResourceController::Create (Node& node, media::rms::Binding& binding)
{
  return Pointer (new ResourceController (node, binding), false);
}

/*
    Связывание с группой ресурсов
*/

void ResourceController::SetBinding (media::rms::Binding& binding)
{
  impl->binding = binding;
}

media::rms::Binding& ResourceController::Binding () const
{
  return impl->binding;
}
