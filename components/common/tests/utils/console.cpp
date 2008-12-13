#include <cstdio>

#include <xtl/bind.h>
#include <xtl/connection.h>
#include <xtl/function.h>

#include <common/console.h>

using namespace common;

void print_line (const char* message)
{
  printf ("new line: '%s'\n", message);
}

int main ()
{
  printf ("Results of console test:\n");
  
  Console::RegisterEventHandler (ConsoleEvent_OnPrintLine, &print_line);
  
  Console::SetMaxLinesCount (2);

  Console::Print ("Console::Print message\n");
  Console::Printf ("Console::Printf arg1=%d arg2='%s'\n", 42, "arg2");
  Console::Printf ("Yet one");
  Console::Printf (" console message\n");
  
  printf ("Console has %u messages:\n", Console::LinesCount ());
  
  for (size_t i=0; i<Console::LinesCount (); i++)
    printf ("  '%s'\n", Console::Message (i));
    
  Console::SetMaxLinesCount (0);
  
  for (int i=0; i<10; i++)
    Console::Printf ("%d", i);
    
  Console::Printf ("\n");
  
  printf ("Console has %u messages:\n", Console::LinesCount ());
  
  for (size_t i=0; i<Console::LinesCount (); i++)
    printf ("  '%s'\n", Console::Message (i));  

  return 0;
}
