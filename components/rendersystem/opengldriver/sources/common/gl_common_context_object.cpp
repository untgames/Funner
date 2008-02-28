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

/*
    Работа с таблицами локальных данных текущего контекста
*/

const ContextDataTable& ContextObject::GetContextDataTable (Stage table_id) const
{
  return context_manager.GetContextDataTable (table_id);
}

ContextDataTable& ContextObject::GetContextDataTable (Stage table_id)
{
  return context_manager.GetContextDataTable (table_id);
}

/*
    Определение поддержки расширения контекстом
*/

bool ContextObject::IsSupported (size_t context_id, const Extension& extension) const
{
  return context_manager.IsSupported (context_id, extension);
}

bool ContextObject::IsSupported (const Extension& extension) const
{
  return context_manager.IsSupported (extension);
}

/*
   Поулчение аппаратно поддерживаемых возможностей контекста
*/

const ContextCaps& ContextObject::GetCaps () const
{
  return context_manager.GetCaps ();
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

void ContextObject::SetValidationState (bool state)
{
  context_manager.SetValidationState (state);  
}

bool ContextObject::GetValidationState () const
{
  return context_manager.GetValidationState ();
}

void ContextObject::CheckErrors (const char* source) const
{
  context_manager.CheckErrors (source);
}

void ContextObject::RaiseError (const char* source) const
{
  context_manager.RaiseError (source);
}

void ContextObject::ClearErrors () const
{
  context_manager.ClearErrors ();
}
