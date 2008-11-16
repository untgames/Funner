#include <cstdio>

#include <xtl/bind.h>
#include <xtl/connection.h>
#include <xtl/function.h>

#include <common/console.h>

using namespace common;

void console_handler (const char* message)
{
  printf ("Console handler - new message: %s", message);
}

int main ()
{
  printf ("Results of console test:\n");

  Console::RegisterEventHandler (console_handler);

  Console::Print ("Console::Print message\n");
  Console::Printf ("Console::Printf arg1=%d arg2='%s'\n", 42, "arg2");

  return 0;
}
