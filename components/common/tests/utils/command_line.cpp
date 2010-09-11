#include <cstdio>

#include <xtl/bind.h>
#include <xtl/exception.h>
#include <xtl/function.h>

#include <common/command_line.h>

using namespace common;

const char*  ARGV [] = { "program.exe", "param1",  "-h", "-h", "param2", "--switch", "-b", "b_arg", "-b", "b_arg2 b_arg3", "--batch=\"equal_arg1 equal_arg2\"", "param3" };
const size_t ARGC = sizeof (ARGV) / sizeof (*ARGV);

void print_params (const CommandLine& command_line)
{
  size_t params_count = command_line.ParamsCount ();

  printf ("Command line params count = %u\n", params_count);

  for (size_t i = 0; i < params_count; i++)
    printf ("Param %u is '%s'\n", i, command_line.Param (i));
}

void argument_handler (const char* switch_name, const char* switch_arg)
{
  printf ("Switch '%s' occured with arg '%s'\n", switch_name, switch_arg);
}

int main ()
{
  printf ("Results of command_line_test:\n");

  CommandLine command_line;
  
  print_params (command_line);

  try
  {
    command_line.Process (ARGC, ARGV);
  }
  catch (xtl::exception& e)
  {
    printf ("Exception: '%s'\n", e.what ());
  }

  command_line.SetSwitchHandler ("help",   'h', 0, xtl::bind (&argument_handler, "--help", _1));
  command_line.SetSwitchHandler ("switch", 's', 0, xtl::bind (&argument_handler, "--switch", _1));
  command_line.SetSwitchHandler ("batch",  'b', "batch_arg", xtl::bind (&argument_handler, "--batch", _1));

  try
  {
    command_line.Process (ARGC, ARGV);
  }
  catch (xtl::exception& e)
  {
    printf ("Exception: '%s'\n", e.what ());
  }
  
  print_params (command_line);

  return 0;
}
