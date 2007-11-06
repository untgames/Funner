#include <stl/vector>
#include <xtl/intrusive_ptr.h>
#include <xtl/shared_ptr.h>
#include <xtl/ref.h>
#include <xtl/iterator.h>
#include <xtl/connection.h>
#include <xtl/bind.h>
#include <xtl/any.h>
#include <script/bind.h>
#include <script/lua.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

using namespace script;
using namespace script::lua;

class LuaStack: public IStack
{
  public:
    LuaStack () : size (0) {}

    size_t Size () { return size; }

    float       GetFloat   (size_t index) { return stack.Get <float> (index); }
    int         GetInteger (size_t index) { return stack.Get <int> (index); }
    void*       GetPointer (size_t index) { return stack.Get <void*> (index); }
    const char* GetString  (size_t index) { return stack.Get <const char*> (index); }
    const char* GetSymbol  (size_t index) { return stack.Get <const char*> (index); }
    xtl::any&   GetVariant (size_t index) 
    {
      xtl::any a = stack.Get <void*> (index);  
      return a;
    }

    void Push (float value)        { stack.Push (value); size++; }
    void Push (int value)          { stack.Push (value); size++; }
    void Push (void* pointer)      { stack.Push (pointer); size++; }
    void Push (const char* string) { stack.Push (string); size++; }
    void Push (const xtl::any& a)  { stack.Push (a); size++; }

    void PushSymbol (const char* string) { LuaStack::Push (string); }

    void Pop (size_t arguments_count)
    {
      if (arguments_count > size)
        arguments_count = size;

      stack.Pop (arguments_count);
      size -= arguments_count;
    }
    
    void Dump (size_t index)
    {
      try
      {
        stl::string buffer;      
      
        to_string (buffer, GetVariant (index));

        printf ("'%s'", buffer.c_str ());
      }
      catch (std::exception& exception)
      {
        printf ("dump failed; reason: %s", exception.what ());
      }    
    }
    
    void Dump ()
    {
      printf ("stack (size=%u):\n", Size ());

      for (size_t i=0; i<Size (); i++)
      {
        printf ("  item[%u]: ", i);
        Dump   (i);
        printf ("\n");
      }
    }

    void SetState (lua_State* state) {stack.SetState (state);}

  private:
    script::lua::detail::Stack stack;
    size_t                     size;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интерпретатор LUA
///////////////////////////////////////////////////////////////////////////////////////////////////
class LuaInterpreter: public IInterpreter
{
  public:
    LuaInterpreter (const InvokerRegistry& registry);
    
    const char* Name () { return "LuaInterpreter"; }
    IStack&     Stack () { return stack; }

    void DoCommands (const char* buffer_name, const void* buffer, size_t buffer_size, const LogFunction& log) {environment.DoBuffer (buffer_name, buffer, buffer_size, log);}

    bool HasFunction (const char* name) {return environment.HasFunction (name);}
    
    void Invoke (size_t arguments_count, size_t results_count) {environment.InvokeCore (arguments_count, results_count);}

    void AddR   ef  () {}
    void Release () {}

    void InvokeFunction (const char* name) {(*registry.Find (name)) (stack);}
    
  private:
    Environment            environment;
    LuaStack               stack;
    const InvokerRegistry& registry;
};

LuaInterpreter::LuaInterpreter (const InvokerRegistry& in_registry)
  : registry (in_registry)
{
  stack.SetState (environment.State ());
  
  for (InvokerRegistry::Iterator i = ((InvokerRegistry)registry).CreateIterator (); i; ++i)
    environment.RegisterFunction (registry.InvokerId (i), &LuaInterpreter::InvokeFunction);
}

namespace script
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание интерпретатора lua
///////////////////////////////////////////////////////////////////////////////////////////////////
xtl::com_ptr<IInterpreter> create_lua_interpreter (const InvokerRegistry& registry)
{
  return xtl::com_ptr<IInterpreter> (new LuaInterpreter (registry), false); 
}

xtl::com_ptr<IInterpreter> create_lua_interpreter (const char* name, const InvokerRegistry& registry)
{
  return xtl::com_ptr<IInterpreter> (new LuaInterpreter (registry), false); 
}

}

