#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    онструктор / деструктор
*/

AsyncPredicate::AsyncPredicate  (const ContextManager& manager)
  : ContextObject (manager), parent_context (manager.GetContextId ())
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
    MakeCurrentContextParent ();
   
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
    //подавл€ем все исключени€
  }
}

/*
   ”казание границ запроса
*/

void AsyncPredicate::Begin ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::AsyncPredicate::Begin";

  MakeCurrentContextParent ();   

  size_t &is_in_ranges = GetContextDataTable (Stage_QueryManager)[QueryManagerCache_IsInRanges];

  if (is_in_ranges)
    throw xtl::format_operation_exception (METHOD_NAME, "Begin already called without end call");

  if (glBeginQuery) glBeginQuery    (GL_SAMPLES_PASSED, query);
  else              glBeginQueryARB (GL_SAMPLES_PASSED, query);

  CheckErrors (METHOD_NAME);

  is_in_ranges = 1;
}

void AsyncPredicate::End ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::AsyncPredicate::End";

  MakeCurrentContextParent ();   

  size_t &is_in_ranges = GetContextDataTable (Stage_QueryManager)[QueryManagerCache_IsInRanges];

  if (!is_in_ranges)
    throw xtl::format_operation_exception (METHOD_NAME, "There was not begin call");

  if (glEndQuery) glEndQuery    (GL_SAMPLES_PASSED);
  else            glEndQueryARB (GL_SAMPLES_PASSED);

  CheckErrors (METHOD_NAME);

  is_in_ranges = 0;
}

/*
   ѕолучение результата отрисовки
     (операци€ может привести к остановке выполнени€ нити до завершени€ отрисовки)
*/

bool AsyncPredicate::GetResult ()
{
  MakeCurrentContextParent ();   

  size_t count = 0;

  if (glGetQueryObjectuiv) glGetQueryObjectuiv    (query, GL_QUERY_RESULT, &count);
  else                     glGetQueryObjectuivARB (query, GL_QUERY_RESULT, &count);

  CheckErrors ("render::low_level::opengl::AsyncPredicate::GetResult");

  return count != 0;
}

/*
   ѕолучение доступности результата отрисовки
*/

bool AsyncPredicate::IsResultAvailable ()
{
  MakeCurrentContextParent ();   

  int available = 0;

  if (glGetQueryObjectiv) glGetQueryObjectiv    (query, GL_QUERY_RESULT_AVAILABLE, &available);
  else                    glGetQueryObjectivARB (query, GL_QUERY_RESULT_AVAILABLE, &available);

  CheckErrors ("render::low_level::opengl::AsyncPredicate::IsResultAvailable");

  return available != 0;
}

/*
   ”становка текущим первоначального контекста, если активен другой
*/

void AsyncPredicate::MakeCurrentContextParent ()
{
  ContextManager& manager = GetContextManager ();

  if (manager.GetContextId () != parent_context)
    manager.SetContext (parent_context, 0, 0); 

  MakeContextCurrent ();
}
