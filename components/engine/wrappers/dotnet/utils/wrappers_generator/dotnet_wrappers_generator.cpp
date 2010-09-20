#include <cstdio>
#include <exception>

#include <xtl/iterator.h>

#include <script/bind.h>
#include <script/environment.h>

using namespace script;

// обработка библиотеки
void process_library (const char* name, InvokerRegistry& lib)
{
  printf ("library '%s':\n", name);
  
  for (InvokerRegistry::Iterator iter=lib.CreateIterator (); iter; ++iter)
  {
    stl::string signature = to_string (*iter);
    
    printf ("  %s%s\n", lib.InvokerId (iter), signature.c_str ());
  }
}

// обработка библиотек
void process_libraries (Environment& env)
{
  for (Environment::Iterator iter=env.CreateIterator (); iter; ++iter)
  {
    process_library (env.LibraryId (iter), *iter);   
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
