#include <common/exception.h>
#include <stl/string>
#include <stl/memory>
#include <stl/hash_map>
#include <script.h>
#include <xtl/function.h>

extern "C"
{
  #include <lua.h>
  #include <lualib.h>
  #include <lauxlib.h>
}

//сделать const char*!!
#define RECALL_NAME "recall"
#define HASH_MAP_NAME "funcs_hash_map"

namespace
{

void DebugLogFunction (const char* env_name, const char* message)
{
  printf ("'%s' at environment '%s'\n", message, env_name);
}

}

//вверх, сразу после include
using namespace common;
using namespace stl;
using namespace script;
using lua::Environment; //????????
using lua::detail::Invoker;

struct Environment::Impl
{
  public:
    Impl ();
    ~Impl ();

    static int Recaller (lua_State* l_state);
    static int AtPanic  (lua_State* l_state) {Raise <Exception> ("LuaEnvImpl::AtPanic", "Lua at panic."); return 0;}

    /*
      откомментировать поля, сделать Stack объектом, а не указателем.
      сделать базовый класс, который будет содержать lua_State* и проверять его на 0
      инициализацию делать Impl () : Base (), stack (state) {} //state поле в Base
    */   
    
    lua_State*                  l_state;
    hash_map <string, Invoker*> funcs;
    auto_ptr <lua::Stack>       stack; //???
    string                      str_name;
    Environment::DebugLogFunc   log_function;
};

static int DestroyIUserData (lua_State* state)
{
  lua::IUserData** ptr = (lua::IUserData**)lua_touserdata (state,-1);
  
  if (!ptr) 
    luaL_typerror (state,1,"user_data");    

  delete (*ptr);
  
  return 0;
}

static const luaL_reg iuser_data_meta_table [] = {
  {"__gc", DestroyIUserData},
  {0,0}
};

#include <malloc.h>
#include <common/heap.h>

static size_t allocated_blocks = 0, deallocated_blocks = 0, used_memory = 0, max_mem_used = 0;
static Heap my_heap;

static void* my_alloc (void *ud, void *ptr, size_t osize, size_t nsize) 
{
  (void)ud;
  (void)osize;  /* not used */
  
  if (nsize == 0)
  {
/*    if (ptr)
      used_memory -= _msize (ptr);
    printf ("Deallocating memory at %p (block %u, now used memory = %u)\n", ptr, deallocated_blocks++, used_memory);*/
    
    if (ptr)
      used_memory -= my_heap.Size (ptr);
//    printf ("Deallocating memory at %p (block %u, now used memory = %u)\n", ptr, deallocated_blocks++, used_memory);
    my_heap.Deallocate (ptr);//free(ptr);
    
    return NULL;
  }
  else
  {
/*    void* ret_value = realloc(ptr, nsize);
    used_memory += _msize (ret_value);
    printf ("Allocated %u bytes at %p (block %u, now used memory = %u)\n", nsize, ret_value, allocated_blocks++, used_memory);*/

    void *ret_value = my_heap.Allocate (nsize);
    if (ptr)
    {
      memcpy (ret_value, ptr, min (my_heap.Size (ptr), nsize));
      used_memory -= my_heap.Size (ptr);
      my_heap.Deallocate (ptr);
      deallocated_blocks++;
    }
    used_memory += my_heap.Size (ret_value);
    if (used_memory > max_mem_used) max_mem_used = used_memory;
//    printf ("Allocated %u bytes at %p (block %u, now used memory = %u)\n", nsize, ret_value, allocated_blocks++, used_memory);
    return ret_value; 
  }
}

Environment::Impl::Impl ()
{
/*  l_state = lua_open ();
  if (!l_state)
    Raise <Exception> ("Impl::Impl", "Can't create lua state.");

  lua_setallocf (l_state, &my_alloc, NULL);
  */

  l_state = lua_newstate (&my_alloc, NULL);
  if (!l_state)
    Raise <Exception> ("Impl::Impl", "Can't create lua state.");

  lua_atpanic (l_state, &Impl::AtPanic);
  luaL_openlibs (l_state);

  lua_pushcfunction (l_state, &Impl::Recaller);
  lua_setglobal     (l_state, RECALL_NAME);
  lua_pushlightuserdata (l_state, &funcs);
  lua_setglobal         (l_state, HASH_MAP_NAME);

  luaL_newmetatable (l_state,"iuser_data");
  luaL_openlib      (l_state,0,iuser_data_meta_table,0);

  stack = new lua::Stack (l_state);
}

Environment::Impl::~Impl ()
{
  lua_close (l_state);
  HeapStat stat;
  my_heap.GetStatistics (stat);
  printf ("\n\nFinal stats:\n sys_allocate count = %u\n sys_deallocate count = %u\n sys_allocate size = %u\n sys_deallocate size = %u\n", stat.sys_allocate_count,
            stat.sys_deallocate_count, stat.sys_allocate_size, stat.sys_deallocate_size);
  printf (" allocate count = %u\n deallocate count = %u\n allocate size = %u\n deallocate size = %u\n", stat.allocate_count,
            stat.deallocate_count, stat.allocate_size, stat.deallocate_size);  
  printf (" maximum used memory = %u\n", max_mem_used);

  printf ("Range statistics:\n");
  printf ("\t\t\t\t Blocks\tTotal size\n");
  printf ("--------------------------------------------------------------------------------\n");
  for (size_t i=0;i<stat.ranges_count;i++)
  {
    const HeapStat::Range& range = stat.ranges [i];
    
    printf ("\t[%8u;%8u]: %8u\t%8u\n",range.min_size,range.max_size,range.allocate_count-range.deallocate_count,
            range.allocate_size-range.deallocate_size);
  }

  printf ("\n\n");
}

int Environment::Impl::Recaller (lua_State* l_state)
{
  lua_getglobal (l_state, HASH_MAP_NAME);
    //лучше и проще хранить в луа указатель на Impl либо даже на Environment
  
  hash_map<string, Invoker*>::iterator func = ((hash_map <string, Invoker*>*)lua_touserdata(l_state, -1))->find (string (lua_tostring (l_state, 1)));

  if (func == ((hash_map <string, Invoker*>*)lua_touserdata(l_state, -1))->end ())
    Raise <Exception> ("Impl::Recaller", "Calling unregistered function.");

  return (*(func->second)) (lua::Stack (l_state));
}

Environment::Environment ()
  : impl (new Impl)
{
  SetDebugLog (&DebugLogFunction);
}

Environment::~Environment ()
{
  delete impl;
}

void Environment::SetDebugLog (const DebugLogFunc& new_log_function)
{
  impl->log_function = new_log_function; 
}

const char* Environment::Name () const
{
  return impl->str_name.c_str ();
}

void Environment::Rename (const char* new_name)
{
  impl->str_name = new_name;
}

lua::Stack* Environment::Stack ()
{
  return impl->stack.get();
}

const lua::Stack* Environment::Stack () const
{
  return impl->stack.get();
}

void Environment::DoString (const char* expression)
{
  try
  {
     //сделать отдельный тип исключения
    
    if (luaL_dostring (impl->l_state, expression))
      Raise <Exception> ("EnvironmentImpl::DoString", "Error when executing expression %s", expression);
  }
  catch (std::exception& exception)
  {                                               
    impl->log_function (Name (), exception.what ()); 
    Raise <Exception> ("EnvironmentImpl::DoFile", "Bad script call");
  }
  catch (...)
  {
    impl->log_function (Name (), "Unknown exception have occured."); 
    Raise <Exception> ("EnvironmentImpl::DoFile", "Bad script call");
  }
}

void Environment::DoFile (const char* file_name)
{
  try
  {
       //не верно. нужно читать через common::File
    
    if (luaL_dofile (impl->l_state, file_name))
      Raise <Exception> ("EnvironmentImpl::DoFile", "Error when loading file '%s'", file_name);
  }
  catch (std::exception& exception)
  {                                               
    impl->log_function (Name (), exception.what ()); 
    Raise <Exception> ("EnvironmentImpl::DoFile", "Bad script call");
  }
  catch (...)
  {
    impl->log_function (Name (), "Unknown exception have occured."); 
    Raise <Exception> ("EnvironmentImpl::DoFile", "Bad script call");
  }
}

void Environment::Invoke (size_t args_count, size_t results_count)
{
  if (lua_pcall(impl->l_state, args_count, results_count, 0))
    Raise <Exception> ("invoke", "Error running function: '%s'", lua_tostring(impl->l_state, -1));
}

void Environment::RegisterFunction (const char* name, Invoker* invoker)
{
  impl->funcs.insert_pair (string (name), invoker);

  string generated_function ("\nfunction ");
  string args;
  
    //общие замечания. проще по-моему сделать это через common::format, либо хотя бы сделать string::reserve. иначе 
    //будет много доп. выделений/освобождений памяти. в общем продумать
 
  generated_function += name;
  generated_function += " (";
  for (size_t i = 0; i < invoker->ArgCount (); i++)
  {
    char arg_buf [8];

    sprintf (&arg_buf[0], "arg%u", i);
    args += &arg_buf[0];
    if (i < invoker->ArgCount () - 1)
      args += ", ";
  }
  generated_function += args;
  generated_function += ")\n return ";
  generated_function += RECALL_NAME;
  generated_function += " (\"";
  generated_function += name;
  generated_function += "\"";
  if (invoker->ArgCount ())
  {
    generated_function += ", ";
    generated_function += args;
  }
  generated_function += ")\n end";

  DoString (generated_function.c_str());
}

void Environment::Swap (Environment& lua)
{
  stl::swap (impl, lua.impl);
}

void script::lua::swap (Environment& env1, Environment& env2)
{
  env1.Swap (env2);
}

void script::lua::invoke (Environment& env, const char* fn_name)
{
  Stack* stack = env.Stack ();

  stack->PushFunction(fn_name);

  env.Invoke (0, 0);
}

