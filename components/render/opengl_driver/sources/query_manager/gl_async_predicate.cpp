#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор / деструктор
*/

AsyncPredicate::AsyncPredicate  (const ContextManager& manager)
  : ContextObject (manager)
{
  MakeContextCurrent ();

  if (glGenQueries) glGenQueries    (1, &query);
  else              glGenQueriesARB (1, &query);

  CheckErrors ("render::low_level::opengl::AsyncPredicate::AsyncPredicate");
}

AsyncPredicate::~AsyncPredicate ()
{
  try
  {
    MakeContextCurrent ();

    if (glDeleteQueries) glDeleteQueries    (1, &query);
    else                 glDeleteQueriesARB (1, &query);

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

  size_t &is_in_ranges = GetContextDataTable (Stage_QueryManager)[QueryManagerCache_IsInRanges];

  if (is_in_ranges)
    throw xtl::format_operation_exception (METHOD_NAME, "Begin already called without end call");

  MakeContextCurrent ();       

  if (glBeginQuery) glBeginQuery    (GL_SAMPLES_PASSED, query);
  else              glBeginQueryARB (GL_SAMPLES_PASSED, query);

  CheckErrors (METHOD_NAME);

  is_in_ranges = 1;
}

void AsyncPredicate::End ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::AsyncPredicate::End";

  size_t &is_in_ranges = GetContextDataTable (Stage_QueryManager)[QueryManagerCache_IsInRanges];

  if (!is_in_ranges)
    throw xtl::format_operation_exception (METHOD_NAME, "There was not begin call");
    
  MakeContextCurrent ();       

  if (glEndQuery) glEndQuery    (GL_SAMPLES_PASSED);
  else            glEndQueryARB (GL_SAMPLES_PASSED);

  CheckErrors (METHOD_NAME);

  is_in_ranges = 0;
}

/*
   Получение результата отрисовки
     (операция может привести к остановке выполнения нити до завершения отрисовки)
*/

bool AsyncPredicate::GetResult ()
{
  MakeContextCurrent ();   

  size_t count = 0;

  if (glGetQueryObjectuiv) glGetQueryObjectuiv    (query, GL_QUERY_RESULT, &count);
  else                     glGetQueryObjectuivARB (query, GL_QUERY_RESULT, &count);

  CheckErrors ("render::low_level::opengl::AsyncPredicate::GetResult");

  return count != 0;
}

/*
   Получение доступности результата отрисовки
*/

bool AsyncPredicate::IsResultAvailable ()
{
  MakeContextCurrent ();   

  int available = 0;

  if (glGetQueryObjectiv) glGetQueryObjectiv    (query, GL_QUERY_RESULT_AVAILABLE, &available);
  else                    glGetQueryObjectivARB (query, GL_QUERY_RESULT_AVAILABLE, &available);

  CheckErrors ("render::low_level::opengl::AsyncPredicate::IsResultAvailable");

  return available != 0;
}
