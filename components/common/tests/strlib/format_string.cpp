#include <common/strlib.h>
#include <xtl/function.h>
#include <stdio.h>

void process_replacement (const char* prefix, const char* replacement_tag)
{
  printf ("prefix: '%s'; replacement_tag: '%s'\n", prefix, replacement_tag);
}

int main ()
{
  printf ("Results of format_string_test:\n");
  
  const char* format_string = "\t\r\n A123\t=12345 {5asd4}{i}{}  'B ' = 567 CC C=\"{{double_open} {double_close}} D  \" E='asdasd\"' \"z'z\"{end}";
  const char* format_string2 = "{";
  
  common::parse_format_string (format_string, &process_replacement);
  common::parse_format_string (format_string2, &process_replacement);

  return 0;
}
