#include "shared.h"

using namespace scene_graph;
using detail::ISceneAttachment;

/*
    Описание реализации контекста сцены
*/
namespace
{

typedef xtl::shared_ptr<ISceneAttachment>                        AttachmentPtr;
typedef stl::hash_map<stl::hash_key<const char*>, AttachmentPtr> AttachmentMap;

///Реестр присоединенных данных
struct AttachmentRegistry: public xtl::reference_counter
{
  AttachmentPtr anonymous_attachment;
  AttachmentMap named_attachments;
};

typedef xtl::intrusive_ptr<AttachmentRegistry>                      AttachmentRegistryPtr;
typedef stl::hash_map<const std::type_info*, AttachmentRegistryPtr> AttachmentRegistryMap;
typedef xtl::signal<bool (const char*)>                             ExceptionFilterSignal;

}

struct SceneContext::Impl: public xtl::reference_counter
{
  common::PropertyMap   properties;       //свойства контекста
  AttachmentRegistryMap attachments;      //присоединенные данные
  ExceptionFilterSignal exception_filter; //фильтр ислючений
  LogFunction           log_handler;      //обработчик событий протоколирования
};

/*
    Конструкторы / деструктор / присваивание
*/

SceneContext::SceneContext ()
  : impl (new Impl)
{
}

SceneContext::SceneContext (const SceneContext& context)
  : impl (context.impl)
{
  addref (impl);
}

SceneContext::~SceneContext ()
{
  release (impl);
}

SceneContext& SceneContext::operator = (const SceneContext& context)
{
  SceneContext (context).Swap (*this);
  
  return *this;
}

/*
    Свойства
*/

common::PropertyMap SceneContext::Properties () const
{
  return impl->properties;
}

void SceneContext::SetProperties (const common::PropertyMap& properties)
{
  impl->properties = properties;
}

/*
    Работа с присоединенными данными
*/

void SceneContext::RaiseAttachmentError (const char* name, const std::type_info& type)
{
  static const char* METHOD_NAME = "scene_graph::SceneContext::Attachment(const char*)";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  throw xtl::format_operation_exception (METHOD_NAME, "Can't find attachment '%s' of type '%s'", name, type.name ());
}

void SceneContext::RaiseAttachmentError (const std::type_info& type)
{
  throw xtl::format_operation_exception ("scene_graph::SceneContext::Attachment", "Can't find attachment of type '%s'", type.name ());
}

void SceneContext::AttachCore (ISceneAttachment* in_attachment)
{
  static const char* METHOD_NAME = "scene_graph::SceneContext::Attach(T&)";

  if (!in_attachment)
    throw xtl::make_null_argument_exception (METHOD_NAME, "attachment");

  AttachmentPtr attachment (in_attachment);

  AttachmentRegistryMap::iterator iter = impl->attachments.find (&attachment->ValueType ());

  if (iter != impl->attachments.end () && iter->second->anonymous_attachment)
    throw xtl::format_operation_exception (METHOD_NAME, "Attachment of type '%s' has already defined", attachment->ValueType ().name ());
    
  AttachmentRegistryPtr registry;    
    
  if (iter == impl->attachments.end ())
  {
    registry = AttachmentRegistryPtr (new AttachmentRegistry, false);
    
    impl->attachments.insert_pair (&attachment->ValueType (), registry);
  }
  else
  {
    registry = &*iter->second;
  }

  registry->anonymous_attachment = attachment;
}

void SceneContext::AttachCore (const char* name, ISceneAttachment* in_attachment)
{
  static const char* METHOD_NAME = "scene_graph::SceneContext::Attach(const char*,T&)";

  if (!in_attachment)
    throw xtl::make_null_argument_exception (METHOD_NAME, "attachment");

  AttachmentPtr attachment (in_attachment);

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  AttachmentRegistryMap::iterator iter = impl->attachments.find (&attachment->ValueType ());
  
  AttachmentRegistryPtr registry;

  if (iter == impl->attachments.end ())
  {
    registry = AttachmentRegistryPtr (new AttachmentRegistry, false);
    
    impl->attachments.insert_pair (&attachment->ValueType (), registry);
  }
  else
  {
    registry = &*iter->second;
  }
  
  AttachmentMap::iterator iter1 = registry->named_attachments.find (name);

  if (iter1 != registry->named_attachments.end ())
    throw xtl::format_operation_exception (METHOD_NAME, "Attachment '%s' of type '%s' has already defined", name, attachment->ValueType ().name ());
  
  registry->named_attachments.insert_pair (name, attachment);
}

void SceneContext::DetachCore (const std::type_info& type)
{
  AttachmentRegistryMap::iterator iter = impl->attachments.find (&type);
  
  if (iter == impl->attachments.end ())
    return;

  iter->second->anonymous_attachment.reset ();
}

void SceneContext::DetachCore (const char* name, const std::type_info& type)
{
  if (!name)
    return;

  AttachmentRegistryMap::iterator iter = impl->attachments.find (&type);
  
  if (iter == impl->attachments.end ())
    return;

  iter->second->named_attachments.erase (name);
}

ISceneAttachment* SceneContext::FindAttachmentCore (const std::type_info& type) const
{
  AttachmentRegistryMap::iterator iter = impl->attachments.find (&type);
  
  if (iter == impl->attachments.end ())
    return 0;
    
  AttachmentRegistry& registry = *iter->second;
  
  return &*registry.anonymous_attachment;
}

ISceneAttachment* SceneContext::FindAttachmentCore (const char* name, const std::type_info& type) const
{
  if (!name)
    return 0;

  AttachmentRegistryMap::iterator iter = impl->attachments.find (&type);

  if (iter == impl->attachments.end ())
    return 0;

  AttachmentRegistry& registry = *iter->second;
  
  AttachmentMap::iterator iter1 = registry.named_attachments.find (name);
  
  if (iter1 == registry.named_attachments.end ())
    return 0;    
  
  return &*iter1->second;
}

/*
    Отсоединение всех данных
*/

void SceneContext::DetachAllCore (const std::type_info& type)
{
  impl->attachments.erase (&type);
}

void SceneContext::DetachAll ()
{
  impl->attachments.clear ();
}

/*
    Очистка
*/

void SceneContext::Clear ()
{
  DetachAll ();
  
  impl->properties.Clear ();
  
  impl->exception_filter.disconnect_all ();
}

/*
    Обработчик ошибок создания сцены
*/

namespace
{

struct ExceptionFilter
{
  stl::string                    mask;
  SceneContext::ExceptionHandler filter;
  
  ExceptionFilter (const char* in_mask, const SceneContext::ExceptionHandler& in_filter)
    : mask (in_mask)
    , filter (in_filter)
  {
  }
  
  bool operator () (const char* message) const
  {
    if (!message || !common::wcimatch (message, mask.c_str ()))
      return false;
      
    return filter (message);
  }
};

}

xtl::connection SceneContext::RegisterErrorHandler (const char* error_wc_mask, const ExceptionHandler& handler)
{
  if (!error_wc_mask)
    throw xtl::make_null_argument_exception ("scene_graph::SceneContext::RegisterErrorHandler", "error_wc_mask");

  return impl->exception_filter.connect (ExceptionFilter (error_wc_mask, handler));
}

bool SceneContext::FilterError (const char* error_message) const
{
  if (!error_message)
    return false;

  return impl->exception_filter (error_message);
}

/*
    Поток протоколирования
*/

void SceneContext::SetLogHandler (const LogFunction& handler)
{
  impl->log_handler = handler;
}

const SceneContext::LogFunction& SceneContext::LogHandler () const
{
  return impl->log_handler;
}

/*
    Обмен
*/

void SceneContext::Swap (SceneContext& context)
{
  stl::swap (impl, context.impl);
}

namespace scene_graph
{

void swap (SceneContext& context1, SceneContext& context2)
{
  context1.Swap (context2);
}

}
