#include "shared.h"

using namespace scene_graph;

/*
    Описание реализации контроллера
*/

struct Controller::Impl: public xtl::instance_counter<Controller>
{
  ControllerEntry      entry;            //вхождение контроллера в список узла
  size_t               ref_count;        //счётчик ссылок
  ControllerOwnerMode  owner_mode;       //режим владения
  scene_graph::Node*   node;             //узел, с которым связан контроллер
  stl::string          name;             //имя контроллера
  ControllerTimeMode   time_mode;        //режим управления временем
  TimeValue            min_delta;        //минимальное значение дельты при обновлении
  TimeValue            last_update_time; //время последнего обновления
  bool                 never_update;     //контрлллер не обновлялся

///Конструктор
  Impl (ControllerTimeMode in_time_mode)
    : ref_count (1)
    , owner_mode (ControllerOwnerMode_None)
    , node (0)
    , time_mode (in_time_mode)
    , never_update (true)
  {
  }
};

/*
    Конструктор / деструктор
*/

Controller::Controller (scene_graph::Node& node, bool updatable)
  : impl (new Impl (ControllerTimeMode_Default))
{
  try
  {
    impl->entry.controller = this;
    impl->entry.updatable  = updatable;

    node.AttachController (impl->entry);

    impl->node = &node;
    
    switch (ControllerOwnerMode_Default)
    {
      case ControllerOwnerMode_NodeOwnsController:
        NodeOwnsController ();
        break;
      case ControllerOwnerMode_ControllerOwnsNode:
        ControllerOwnsNode ();
        break;
      default:
        break;      
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::Controller::Controller");
    throw;
  }
}

Controller::Controller (scene_graph::Node& node, ControllerTimeMode time_mode)
  : impl (new Impl (time_mode))
{
  try
  {
    impl->entry.controller = this;
    impl->entry.updatable  = true;

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
    Имя
*/

void Controller::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("scene_graph::Controller::SetName", "name");
    
  impl->name = name;
}

const char* Controller::Name () const
{
  return impl->name.c_str ();
}

/*
    Режим управления временем
*/

void Controller::SetTimeMode (ControllerTimeMode mode)
{
  switch (mode)
  {
    case ControllerTimeMode_Absolute:
    case ControllerTimeMode_Delta:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::Controller::SetTimeMode", "mode", mode);
  }
  
  impl->time_mode = mode;
}

ControllerTimeMode Controller::TimeMode () const
{
  return impl->time_mode;
}

/*
    Минимальная дельта (работает в обоих режимах управления временем)
*/

void Controller::SetMinDelta (const TimeValue& value)
{
  impl->min_delta = value;
}

const TimeValue& Controller::MinDelta () const
{
  return impl->min_delta;
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

void Controller::UpdateState (const TimeValue& time)
{
  TimeValue delta = impl->never_update ? TimeValue () : time - impl->last_update_time;
  
  impl->last_update_time = time;
  impl->never_update     = false;

  if (delta < impl->min_delta)
    return;

  xtl::com_ptr<Controller> lock_this (this);    
  
  switch (impl->time_mode)
  {
    case ControllerTimeMode_Absolute:
      Update (time);
      break;
    case ControllerTimeMode_Delta:
      Update (delta);
      break;
    default:
      break;
  }  
}

void Controller::Update (const TimeValue&)
{
}

/*
   Перебор контроллеров
*/

Controller::Pointer Controller::NextController ()
{
  if (!impl->entry.next)
    return Controller::Pointer ();

  return impl->entry.next->controller;
}

Controller::Pointer Controller::PrevController ()
{
  if (!impl->entry.prev)
    return Controller::Pointer ();

  return impl->entry.prev->controller;
}

Controller::ConstPointer Controller::NextController () const
{
  return const_cast<Controller&> (*this).NextController ();
}

Controller::ConstPointer Controller::PrevController () const
{
  return const_cast<Controller&> (*this).PrevController ();
}
