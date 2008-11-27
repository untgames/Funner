#include "shared.h"

using namespace script;

const char* lua_f = 
"function test ()\nA.B.C.MyVar.Value = A.B.C.MyEnum.One\nprint (\"Test A.B.C.MyVar.Value: \" .. A.B.C.MyVar.Value)\n"
"A.B.C.MyVar.HardValue = A.B.C.MyVar.HardValue\nend\n"
;

struct A
{
  A (int in_id) : id (in_id) {}

  int id;
};

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

A get_hard_value ()
{
  return A (12);
}

void set_hard_value (const A& a)
{
  printf ("set hard value: %d\n", a.id);
}

int main ()
{
  try
  {
    printf ("Results of lua_global_vars_test:\n");
    
    xtl::shared_ptr<Environment> env (new Environment);

    env->CreateLibrary ("A");

    InvokerRegistry& enum_registry = env->CreateLibrary ("A.B.C.MyEnum");
    InvokerRegistry& var_registry  = env->CreateLibrary ("A.B.C.MyVar");

    env->RegisterType<A> ("A");    

    Shell shell ("lua", env);
    
    xtl::com_ptr<IInterpreter> interpreter (shell.Interpreter ());

    enum_registry.Register ("get_Zero",  make_const (MyEnum_Zero));
    enum_registry.Register ("get_One",   make_const (MyEnum_One));
    var_registry.Register  ("get_Value", make_invoker (&get_value));
    var_registry.Register  ("set_Value", make_invoker (&set_value));
    var_registry.Register  ("set_HardValue", make_invoker (&set_hard_value));    
    var_registry.Register  ("get_HardValue", make_invoker (&get_hard_value));

    interpreter->DoCommands ("lua_f", lua_f, strlen (lua_f));

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
