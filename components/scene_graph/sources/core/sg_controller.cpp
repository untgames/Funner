#include "shared.h"

using namespace scene_graph;

/*
    Описание реализации контроллера
*/

struct Controller::Impl
{
  ControllerEntry      entry;              //вхождение контроллера в список узла
  size_t               ref_count;          //счётчик ссылок
  ControllerOwnerMode  owner_mode;         //режим владения
  scene_graph::Node*   node;               //узел, с которым связан контроллер

///Конструктор
  Impl ()
    : ref_count (1)
    , owner_mode (ControllerOwnerMode_None)
    , node (0)
  {
  }
};

/*
    Конструктор / деструктор
*/

Controller::Controller (scene_graph::Node& node, bool updatable)
  : impl (new Impl)
{
  try
  {
    impl->entry.controller = this;
    impl->entry.updatable  = updatable;
    
    node.AttachController (impl->entry);
    
    impl->node = &node;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::Controller::Controller");
    throw;
  }
}

Controller::~Controller ()
{
}

/*
    Подсчёт ссылок
*/

void Controller::AddRef () const
{
  if (impl->ref_count)
    impl->ref_count++;
}

void Controller::Release () const
{
  if (!impl->ref_count)
    return;

  if (!--impl->ref_count)
  {
    const_cast<Controller&> (*this).Detach ();
    delete this;
  }
}

size_t Controller::UseCount () const
{
  return impl->ref_count;
}

/*
    Узел, с которым связан контроллер
*/

scene_graph::Node* Controller::AttachedNode ()
{
  return impl->node;
}

const scene_graph::Node* Controller::AttachedNode () const
{
  return impl->node;
}

/*
    Состояние обновления контроллера
*/
void Controller::SetUpdatable (bool state)
{
  if (impl->entry.updatable == state)
    return;

  impl->entry.updatable = state;
  
  if (impl->node)
    impl->node->UpdateControllerList ();
}

bool Controller::IsUpdatable () const
{
  return impl->entry.updatable;
}

/*
    Обработчик события отсоединения от узла
*/

void Controller::OnNodeDetached ()
{
}

/*
    Режим владения
*/

void Controller::SetOwnerMode (ControllerOwnerMode mode)
{
  if (mode == impl->owner_mode)
    return;

  switch (mode)
  {
    case ControllerOwnerMode_None:
    case ControllerOwnerMode_ControllerOwnsNode:
    case ControllerOwnerMode_NodeOwnsController:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::Controller::SetOwnerMode", "mode", mode);
  }
  
  if (impl->node)
    impl->node->AddRef ();
    
  AddRef ();
 
  switch (impl->owner_mode)
  {
    default:
    case ControllerOwnerMode_None:
      break;
    case ControllerOwnerMode_ControllerOwnsNode:
      if (impl->node)
        impl->node->Release ();
      break;
    case ControllerOwnerMode_NodeOwnsController:
      Release ();
      break;
  }
  
  impl->owner_mode = mode;
  
  switch (impl->owner_mode)
  {
    default:
    case ControllerOwnerMode_None:
      break;
    case ControllerOwnerMode_ControllerOwnsNode:
      if (impl->node)
        impl->node->AddRef ();
      break;
    case ControllerOwnerMode_NodeOwnsController:
      AddRef ();
      break;
  }  

  if (impl->node)
    impl->node->Release ();

  Release ();
}

ControllerOwnerMode Controller::OwnerMode () const
{
  return impl->owner_mode;
}

/*
    Отсоединение контроллера от узла
*/

void Controller::Detach ()
{
  if (!impl->node)
    return;    
    
  AddRef ();    
    
  OnNodeDetached ();    

  Node* node = impl->node;
  
  impl->node = 0;    
  
  node->DetachController (impl->entry);  
  
  switch (impl->owner_mode)
  {
    case ControllerOwnerMode_ControllerOwnsNode:
      node->Release ();
      break;      
    case ControllerOwnerMode_NodeOwnsController:
      Release ();
      break;
    default:
      break;
  }

  Release ();
}

/*
    Обновление
*/

void Controller::UpdateState (float dt)
{
  xtl::com_ptr<Controller> lock_this (this);
  
  Update (dt);
}

void Controller::Update (float)
{
}
