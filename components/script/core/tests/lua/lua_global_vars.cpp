#include "shared.h"

using namespace script;

const char* lua_f = 
"function test ()\nMyVar.Value = MyEnum.One\nprint (\"Test MyVar.Value: \" .. MyVar.Value)\n"
"MyVar.Value = MyVar.ReadOnlyValue\nprint (\"Test MyVar.Value: \" .. MyVar.Value)\nend\n"
;

enum MyEnum
{
  MyEnum_Zero,
  MyEnum_One
};

MyEnum my_value = MyEnum_Zero;

MyEnum get_value ()
{
  return my_value;
}

void set_value (MyEnum value)
{
  my_value = value;
}

void log_function (const char* s)
{
  printf ("%s\n", s);
}

int main ()
{
  try
  {
    printf ("Results of lua_global_vars_test:\n");
    
    xtl::shared_ptr<Environment> env (new Environment);

    InvokerRegistry& enum_registry = env->CreateLibrary ("MyEnum");
    InvokerRegistry& var_registry  = env->CreateLibrary ("MyVar");

    Shell shell ("lua", env);
    
    xtl::com_ptr<IInterpreter> interpreter (shell.Interpreter ());

    enum_registry.Register ("get_Zero",  make_const (MyEnum_Zero));
    enum_registry.Register ("get_One",   make_const (MyEnum_One));
    var_registry.Register  ("get_Value", make_invoker (&get_value));
    var_registry.Register  ("set_Value", make_invoker (&set_value));
    var_registry.Register  ("get_ReadOnlyValue", make_const (12));

    interpreter->DoCommands ("lua_f", lua_f, strlen (lua_f), log_function);

    printf ("invoke\n");

    invoke<void> (*interpreter, "test");
  }
  catch (std::exception& exception)
  {      
    printf ("exception: %s\n",exception.what ());
  }
  catch (...)
  {
    printf ("unknown exception\n");
  }

  return 0;
}
