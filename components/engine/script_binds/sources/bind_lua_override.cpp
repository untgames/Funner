#include "shared.h"

using namespace script;

namespace
{

/*
    Константы
*/

const char* COMPONENT_NAME = "script.binds.LuaOverride";
const char* LOG_NAME       = "engine.lua";
const char* BINDER_NAME    = "LuaOverride";

/*
    Функции
*/

struct LuaLogHolder
{
  LuaLogHolder () : log (LOG_NAME) {} 

  common::Log log;
};

typedef common::Singleton<LuaLogHolder> LuaLogSingleton;

void print_override (const char* message)
{
  LuaLogSingleton::Instance ()->log.Printf ("%s", message);
}

void error_override (const char* message)
{
  throw xtl::format_operation_exception ("", message ? "error: %s" : "error", message);
}

void assert_override (bool assertion, const char* message)
{
  if (assertion)
    return;
    
  error_override (message ? message : "assertion failed");
}

void assert_override (bool assertion)
{
  assert_override (assertion, 0);
}

void find_file (IStack& stack, const char* short_file_name, stl::string& full_file_name)
{  
  full_file_name.clear ();

  if (!short_file_name)
    return;

  stl::string search_paths_string, replacement = common::format ("\\1%s\\2", short_file_name);

  try
  {
    search_paths_string = script::invoke<stl::string> (stack.Interpreter (), "searchpaths");
  }
  catch (...)
  {
    full_file_name = short_file_name;
    return;
  }

  common::StringArray search_paths = common::split (search_paths_string.c_str (), ";", " \t");
  
  for (size_t i=0, count=search_paths.Size (); i<count; i++)
  {
    const char* path = search_paths [i];

    static const char* PATTERN = "([^?]*)\\?([^?]*)";

    stl::string file_name = common::replace (path, PATTERN, replacement.c_str (), "g");    

    if (!common::FileSystem::IsFileExist (file_name.c_str ()))
      continue;

    full_file_name = file_name;
    
    return;
  }
  
  full_file_name = short_file_name;
}

struct DoFileOverride
{
  size_t operator () (IStack& stack)
  {
    size_t initial_stack_size = stack.Size ();

    stl::string file_name;
    
    find_file (stack, stack.GetString (1), file_name);

    common::InputFile input_file (file_name.c_str ());

    xtl::uninitialized_storage<char> file_content (input_file.Size ());

    input_file.Read (file_content.data (), file_content.size ());

    stack.Interpreter ().DoCommands (file_name.c_str (), file_content.data (), file_content.size ());

    return stack.Size () - initial_stack_size;
  }
};

typedef stl::hash_set<stl::hash_key<const char*> > ModuleSet;

struct RequireOverride
{
  size_t operator () (IStack& stack)
  {
    size_t initial_stack_size = stack.Size ();

    stl::string file_name;
    
    find_file (stack, stack.GetString (1), file_name);
    
    if (modules.count (file_name.c_str ()))
      return 0;
      
    modules.insert (file_name.c_str ());

    common::InputFile input_file (file_name.c_str ());

    xtl::uninitialized_storage<char> file_content (input_file.Size ());

    input_file.Read (file_content.data (), file_content.size ());

    stack.Interpreter ().DoCommands (file_name.c_str (), file_content.data (), file_content.size ());

    return stack.Size () - initial_stack_size;
  }

  ModuleSet modules;  
};

/*
    Компонент
*/

class Component
{
  public:
    Component ()
    {
      LibraryManager::RegisterLibrary (BINDER_NAME, &Bind);
    }

  private:
    static void Bind (Environment& environment)
    {
      InvokerRegistry& lib = environment.Library ("global");

        //регистрация операций

      lib.Register ("print",  make_invoker (&print_override));
      lib.Register ("error",  make_invoker (&error_override));
      lib.Register ("assert", make_invoker (
                                make_invoker ((void (*)(bool, const char*))&assert_override),
                                make_invoker ((void (*)(bool))&assert_override)));
      lib.Register ("dofile", DoFileOverride ());
      lib.Register ("require", RequireOverride ());      
    }
};

extern "C"
{

common::ComponentRegistrator<Component> LuaOverrideScriptBind (COMPONENT_NAME);

}

}
