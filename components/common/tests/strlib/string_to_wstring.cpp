#include <stdio.h>
#include <string.h>

#include <stl/string>

#include <common/utf_converter.h>

using namespace common;

int main()
{
  printf("Results of string_to_wstring:\n");
  try
  {
    stl::string str = "Hello World!!!";

    printf("char->wchar %S",to_wstring_from_utf8 (str).c_str());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  return 0;
}