#include "shared.h"

using namespace sound::low_level::openal;

/*
   Менеджер контекстов OpenAL
*/

/*
   Конструктор / деструктор
*/

OpenALContextManagerImpl::OpenALContextManagerImpl ()
  : current_context (0)
  {}

OpenALContextManagerImpl::~OpenALContextManagerImpl ()
{
}

/*
   Установка текущего контекста
*/

void OpenALContextManagerImpl::SetCurrentContext (OpenALContext& context)
{
  if (current_context == &context)
    return;

  context.MakeCurrent ();

  current_context = &context;
}

