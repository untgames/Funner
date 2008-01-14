#include <common/strlib.h>
#include <xtl/function.h>
#include <stdio.h>

void add_property (const char* property, const char* value)
{
  printf ("  '%s'='%s'\n", property, value);
}

int main ()
{
  printf ("Results of init_string_test:\n");
  
  const char* init_string = "\t\r\n A123\t=12345   'B ' = 567 CC C=\"  D  \" E='asdasd\"' \"z'z\"";
  
  common::parse_init_string (init_string, &add_property);

  return 0;
}
