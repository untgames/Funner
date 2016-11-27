#include "shared.h"

using namespace scene_graph;
using namespace math;

/*
    Описание реализации спрайтовой модели
*/

typedef xtl::signal<void (VisualModel& sender, VisualModelEvent event_id)> VisualModelSignal;
typedef stl::auto_ptr<common::PropertyMap>                                 PropertyMapPtr;

struct VisualModel::Impl: public xtl::instance_counter<VisualModel>
{
  scene_graph::Scissor* scissor;                        //область отсечения
  xtl::auto_connection  on_scissor_destroy_connection;  //соединение с событием удаления области отсечения
  PropertyMapPtr        dynamic_shader_properties;      //динамические свойства шейдера
  PropertyMapPtr        static_shader_properties;       //статические свойства шейдера
  VisualModelSignal     signals [VisualModelEvent_Num]; //сигналы модели
  bool                  need_release_scissor;           //нужно ли освобождать область отсечения

  Impl () : scissor (), need_release_scissor () {}

  ~Impl ()
  {
    if (need_release_scissor)    
      scissor->Release ();
  }

  const char* ScissorName ()
  {
    if (!scissor)
      return "";

    return scissor->Name ();
  }

///Оповещение о событии
  void Notify (VisualModel& sender, VisualModelEvent event_id)
  {
    try
    {
      if (!signals [event_id])
        return;

      signals [event_id] (sender, event_id);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }

/// Отсоединение области вывода
  void UnbindScissor ()
  {
    scissor              = 0;
    need_release_scissor = false;

    on_scissor_destroy_connection.disconnect ();
  }
};

/*
    Конструктор / деструктор
*/

VisualModel::VisualModel ()
  : impl (new Impl)
{
}

VisualModel::~VisualModel ()
{
}

/*
    Область отсечения
*/

void VisualModel::SetScissor (scene_graph::Scissor* scissor, NodeBindMode mode)
{
    //проверяем корректность режима присоединения

  bool need_release_scissor = false;
    
  switch (mode)
  {
    case NodeBindMode_AddRef:
      need_release_scissor = true;
      break;
    case NodeBindMode_WeakRef:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::VisualModel::SetScissor", "mode", mode);
  }

  if (scissor == impl->scissor)
    return;

  scene_graph::Scissor* old_scissor = impl->scissor;

    //установка области отсечения

  xtl::auto_connection on_destroy;

  if (scissor)
  {
    on_destroy = scissor->RegisterEventHandler (NodeEvent_AfterDestroy, xtl::bind (&Impl::UnbindScissor, impl.get ()));
  }

  if (impl->need_release_scissor)
  {
    old_scissor->Release ();

    impl->need_release_scissor = false;
  }

  impl->scissor                       = scissor;
  impl->on_scissor_destroy_connection = on_destroy;

  xtl::connection ().swap (on_destroy);

  if (scissor)
  {  
    impl->need_release_scissor = need_release_scissor;
   
    if (need_release_scissor)
      scissor->AddRef ();
  }

  impl->Notify (*this, VisualModelEvent_AfterScissorUpdate);

  UpdateNotify ();
}

Scissor* VisualModel::Scissor ()
{
  return impl->scissor;
}

const Scissor* VisualModel::Scissor () const
{
  return impl->scissor;
}

/*
    Динамические свойства шейдера (могут быть NULL)
*/

common::PropertyMap* VisualModel::DynamicShaderProperties ()
{
  return impl->dynamic_shader_properties.get ();
}

const common::PropertyMap* VisualModel::DynamicShaderProperties () const
{
  return impl->dynamic_shader_properties.get ();
}

void VisualModel::SetDynamicShaderProperties (common::PropertyMap* properties)
{
  if (properties)
  {
    if (impl->dynamic_shader_properties) *impl->dynamic_shader_properties = *properties;
    else                                 impl->dynamic_shader_properties.reset (new common::PropertyMap (*properties));
  }
  else
  {
    impl->dynamic_shader_properties.reset ();
  }

  UpdateNotify ();
}

void VisualModel::SetDynamicShaderProperties (const common::PropertyMap& properties)
{
  SetDynamicShaderProperties (&const_cast<common::PropertyMap&> (properties));
}

/*
    Статические свойства шейдера (могут быть NULL)
*/

common::PropertyMap* VisualModel::StaticShaderProperties ()
{
  return impl->static_shader_properties.get ();
}

const common::PropertyMap* VisualModel::StaticShaderProperties () const
{
  return impl->static_shader_properties.get ();
}

void VisualModel::SetStaticShaderProperties (common::PropertyMap* properties)
{
  if (properties)
  {
    if (impl->static_shader_properties) *impl->static_shader_properties = *properties;
    else                                 impl->static_shader_properties.reset (new common::PropertyMap (*properties));
  }
  else
  {
    impl->static_shader_properties.reset ();
  }

  UpdateNotify ();
}

void VisualModel::SetStaticShaderProperties (const common::PropertyMap& properties)
{
  SetStaticShaderProperties (&const_cast<common::PropertyMap&> (properties));
}

/*
    Динамическая диспетчеризация
*/

void VisualModel::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}

/*
    Подписка на события модели
*/

xtl::connection VisualModel::RegisterEventHandler (VisualModelEvent event, const EventHandler& handler)
{
  switch (event)
  {
    case VisualModelEvent_AfterScissorUpdate:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::VisualModel::RegisterEventHandler", "event", event);
  }
  
  return impl->signals [event].connect (handler);
}

/*
    Связывание свойств
*/

void VisualModel::BindProperties (common::PropertyBindingMap& bindings)
{
  Entity::BindProperties (bindings);

  bindings.AddProperty ("Scissor", xtl::bind (&VisualModel::Impl::ScissorName, impl.get ()));
}
