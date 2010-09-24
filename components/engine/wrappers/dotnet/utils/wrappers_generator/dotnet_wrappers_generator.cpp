#include <cstdio>
#include <exception>

#include <xtl/iterator.h>

#include <script/bind.h>
#include <script/environment.h>

using namespace script;

// поиск имени типа
const char* get_type_name (Environment& env, const xtl::type_info& type)
{
  const char* result = env.FindLibraryId (type.std_type ());
  
  if (result)
    return result;
    
  return type.name ();
}

// обработка библиотеки
void process_library (const char* name, InvokerRegistry& lib, Environment& env)
{
  printf ("library '%s':\n", name);
  
  for (InvokerRegistry::Iterator iter=lib.CreateIterator (); iter; ++iter)
  {
    Invoker invoker = *iter;
    
    for (size_t i=0; i<invoker.OverloadsCount (); i++)
    {
      InvokerSignature s = invoker.OverloadSignature (i);
      
      printf ("  %s %s (", get_type_name (env, s.ResultType ()), lib.InvokerId (iter));
      
      for (size_t j=0; j<s.ParametersCount (); j++)
      {
        if (j)
          printf (", ");
          
        printf ("%s", get_type_name (env, s.ParameterType (j)));
      }
      
      printf (")\n");
    }
  }
}

// обработка библиотек
void process_libraries (Environment& env)
{
  for (Environment::Iterator iter=env.CreateIterator (); iter; ++iter)
  {
    process_library (env.LibraryId (iter), *iter, env);   
  }
}

// точка входа
int main ()
{
  try
  {
      //создание окружения и привязка библиотек
    
    Environment env;
    
    env.BindLibraries ("*");
    
      //перебор библиотек
      
    process_libraries (env);
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
