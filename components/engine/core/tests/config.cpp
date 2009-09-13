#include <cstdio>

#include <stl/string>

#include <xtl/connection.h>
#include <xtl/exception.h>
#include <xtl/function.h>

#include <engine/config.h>

using namespace engine;

const char* EVAL_STRING1 = "Var1={Var1}, Var3={Var3} end{}";
const char* EVAL_STRING2 = "{Var1}-Var1, unknown var = {unknown var}, Var3={Var3}";
const char* EVAL_STRING3 = "ComplexVar = {ComplexVar}";

const char* RECURSIVE_EVAL_STRING = "{RecursiveVar3}";

void save_handler ()
{
  printf ("save_handler called\n");
}

void enumerate_handler (const char* var_name, const char* value)
{
  printf ("enumerate_handler called for var '%s' with value '%s'\n", var_name, value);
}

int main ()
{
  printf ("Results of config_test:\n");

  try
  {
    Config config;

    config.RegisterSaveHandler (save_handler);

    printf ("Enumerating variables\n");

    config.EnumerateVars ("*mask*", enumerate_handler);

    printf ("Variables enumerated\n");

    config.SetVar ("Var1", "Var1_Value");
    config.SetVar ("Var2", "Var2_Value");
    config.SetVar ("Var3", "Var3_Value");
    config.SetVar ("MegaVar1", "MegaVar1_Value");
    config.SetVar ("ComplexVar", "ComplexVar_Value&{Var1}");

    printf ("Var2 value is '%s'\n", config.GetVar ("Var2"));

    printf ("Evaluating string '%s': '%s'\n", EVAL_STRING1, config.Eval (EVAL_STRING1).c_str ());
    printf ("Evaluating string '%s': '%s'\n", EVAL_STRING2, config.Eval (EVAL_STRING2).c_str ());
    printf ("Evaluating string '%s': '%s'\n", EVAL_STRING3, config.Eval (EVAL_STRING3).c_str ());

    printf ("Enumerating all variables\n");
    config.EnumerateVars (enumerate_handler);
    printf ("Enumerating *Mega* variables\n");
    config.EnumerateVars ("*Mega*", enumerate_handler);
    printf ("Done\n");

    config.RemoveVars ("Var*");

    printf ("Enumerating all variables\n");
    config.EnumerateVars (enumerate_handler);
    printf ("Done\n");

    config.Save ();

    config.SetVar ("RecursiveVar1", "{RecursiveVar2}");
    config.SetVar ("RecursiveVar2", "{RecursiveVar3}");
    config.SetVar ("RecursiveVar3", "{RecursiveVar1}");

    printf ("Evaluating string '%s': '%s'\n", RECURSIVE_EVAL_STRING, config.Eval (RECURSIVE_EVAL_STRING).c_str ());
  }
  catch (xtl::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
