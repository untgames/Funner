#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    Конструктор
*/

ContextObject::ContextObject (const ContextManager& in_context_manager)
  : context_manager (in_context_manager)
  {}

/*
    Проверка совместимости
*/

bool ContextObject::IsCompatible (const ContextManager& manager) const
{
  return context_manager.IsCompatible (manager);
}

bool ContextObject::IsCompatible (const ContextObject& object) const
{
  return context_manager.IsCompatible (object.context_manager);
}

/*
    Активация текущего контекста
*/

void ContextObject::MakeContextCurrent () const
{
  context_manager.MakeContextCurrent ();
}

bool ContextObject::IsContextCurrent () const
{
  return context_manager.IsContextCurrent ();
}

/*
    Работа с таблицей локальных данных контекста
*/

void ContextObject::SetContextData (ContextDataTable table_id, size_t element_id, size_t value)
{
  context_manager.SetContextData (table_id, element_id, value);
}

size_t ContextObject::GetContextData (ContextDataTable table_id, size_t element_id) const
{
  return context_manager.GetContextData (table_id, element_id);
}

void ContextObject::ClearContextData (ContextDataTable table_id)
{
  context_manager.ClearContextData (table_id);
}

void ContextObject::ClearContextData ()
{
  context_manager.ClearContextData ();
}

/*
    Определение поддержки расширения контекстом
*/

bool ContextObject::IsSupported (size_t context_id, GlExtension extension_id) const
{
  return context_manager.IsSupported (context_id, extension_id);
}

bool ContextObject::IsSupported (GlExtension extension_id) const
{
  return context_manager.IsSupported (extension_id);
}

/*
    Протоколирование
*/

void ContextObject::LogPrintf  (const char* format, ...) const
{
  va_list args;
  
  va_start (args, format);

  context_manager.LogVPrintf (format, args);
}

void ContextObject::LogVPrintf (const char* format, va_list args) const
{
  context_manager.LogVPrintf (format, args);
}

/*
    Проверка ошибок OpenGL
*/

void ContextObject::CheckErrors (const char* source) const
{
  context_manager.CheckErrors (source);
}

void ContextObject::RaiseError (const char* source) const
{
  context_manager.RaiseError (source);
}
