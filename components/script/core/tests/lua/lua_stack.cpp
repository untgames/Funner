#include "shared.h"

using namespace script;

template <class T> const char* get_type_name ();

template <> const char* get_type_name<int>         () { return "int"; }
template <> const char* get_type_name<float>       () { return "float"; }
template <> const char* get_type_name<const char*> () { return "const char*"; } 
template <> const char* get_type_name<stl::string> () { return "stl::string"; } 

template <class DstT, class SrcT>
void test_dispatch (IStack& stack, const SrcT& source)
{
  printf ("%s -> %s: ", get_type_name<SrcT> (), get_type_name<DstT> ());

  try
  {
    stack.Pop (stack.Size ());    

    detail::push_argument (stack, source);

    detail::get_argument<DstT> (stack, 1);

    printf ("ok\n");
  }
  catch (std::exception& exception)
  {
    printf ("fail (%s)\n", exception.what ());
  }
}

template <class DstT>
void test (IStack& stack)
{
  static const char* STRING = "Hello world";

  test_dispatch<DstT> (stack, int (0));
  test_dispatch<DstT> (stack, float (0));
  test_dispatch<DstT> (stack, STRING);
  test_dispatch<DstT> (stack, stl::string (STRING));
}

int main ()
{
  try
  {
    printf ("Results of lua_stack_test:\n");
    
    xtl::shared_ptr<Environment> env (new Environment);
    
    Shell shell ("lua", env);
    
    xtl::com_ptr<IInterpreter> interpreter (shell.Interpreter ());
    script::IStack&            stack = interpreter->Stack ();
    
    test<int> (stack);
    test<float> (stack);
    test<const char*> (stack);
    test<stl::string> (stack);
  }
  catch (std::exception& exception)
  {      
    printf ("exception: %s\n",exception.what ());
  }                                               

  return 0;
}
