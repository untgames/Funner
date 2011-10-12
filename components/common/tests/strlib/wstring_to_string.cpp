#include <stdio.h>
#include <string.h>

#include <stl/string>

#include <common/utf_converter.h>

using namespace common;

int main()
{
  printf("Results of wstring_to_string:\n");
  try
  {
    stl::wstring str = L"Hello World!!!";

    printf ("wchar->char %s\n", to_utf8_string (str).c_str());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  return 0;
}