#ifndef OPENGL_ES_SUPPORT

#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    Конструктор / деструктор
*/

AsyncPredicate::AsyncPredicate  (const ContextManager& manager)
  : ContextObject (manager)
{
    //установка текущего контекста

  MakeContextCurrent ();

    //создание запроса

  if (glGenQueries) glGenQueries    (1, &query);
  else              glGenQueriesARB (1, &query);

    //проверка ошибок

  CheckErrors ("render::low_level::opengl::AsyncPredicate::AsyncPredicate");
}

AsyncPredicate::~AsyncPredicate ()
{
  try
  {
      //установка текущего контекста

    MakeContextCurrent ();

      //удаление запроса

    if (glDeleteQueries) glDeleteQueries    (1, &query);
    else                 glDeleteQueriesARB (1, &query);

      //проверка ошибок

    CheckErrors ("");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::AsyncPredicate::~AsyncPredicate");

    LogPrintf ("%s", exception.what ());
  }
  catch (std::exception& exception)
  {
    LogPrintf ("%s", exception.what ());
  }
  catch (...)
  {
    //подавляем все исключения
  }
}

/*
    Указание границ запроса
*/

void AsyncPredicate::Begin ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::AsyncPredicate::Begin";

    //проверка возможности открытия запроса

  const size_t current_is_in_ranges = GetContextCacheValue (CacheEntry_IsInQueryRanges);

  if (current_is_in_ranges)
    throw xtl::format_operation_exception (METHOD_NAME, "Begin already called without end call");

    //установка текущего контекста

  MakeContextCurrent ();

    //открытие запроса

  if (glBeginQuery) glBeginQuery    (GL_SAMPLES_PASSED, query);
  else              glBeginQueryARB (GL_SAMPLES_PASSED, query);

    //проверка ошибок

  CheckErrors (METHOD_NAME);

    //установка кэш-переменных

  SetContextCacheValue (CacheEntry_IsInQueryRanges, 1);
}

void AsyncPredicate::End ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::AsyncPredicate::End";

    //проверка возможности закрытия запроса

  const size_t current_is_in_ranges = GetContextCacheValue (CacheEntry_IsInQueryRanges);

  if (!current_is_in_ranges)
    throw xtl::format_operation_exception (METHOD_NAME, "There was not begin call");

    //установка текущего контекста

  MakeContextCurrent ();

    //зкрытие запроса

  if (glEndQuery) glEndQuery    (GL_SAMPLES_PASSED);
  else            glEndQueryARB (GL_SAMPLES_PASSED);

    //проверка ошибок

  CheckErrors (METHOD_NAME);

    //установка кэш-переменных

  SetContextCacheValue (CacheEntry_IsInQueryRanges, 0);
}

/*
    Получение результата отрисовки
      (операция может привести к остановке выполнения нити до завершения отрисовки)
*/

bool AsyncPredicate::GetResult ()
{
    //установка текущего контекста

  MakeContextCurrent ();

    //получение значения результата запроса

  size_t count = 0;

  if (glGetQueryObjectuiv) glGetQueryObjectuiv    (query, GL_QUERY_RESULT, &count);
  else                     glGetQueryObjectuivARB (query, GL_QUERY_RESULT, &count);

    //проверка ошибок

  CheckErrors ("render::low_level::opengl::AsyncPredicate::GetResult");

  return count != 0;
}

/*
    Получение доступности результата отрисовки
*/

bool AsyncPredicate::IsResultAvailable ()
{
    //установка текущего контекста

  MakeContextCurrent ();

    //проверка готовности результата

  GLint available = 0;

  if (glGetQueryObjectiv) glGetQueryObjectiv    (query, GL_QUERY_RESULT_AVAILABLE, &available);
  else                    glGetQueryObjectivARB (query, GL_QUERY_RESULT_AVAILABLE, &available);

    //проверка ошибок

  CheckErrors ("render::low_level::opengl::AsyncPredicate::IsResultAvailable");

  return available != 0;
}

#endif
