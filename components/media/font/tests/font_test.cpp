#include <cstdio>
#include <exception>

#include <media/font.h>

using namespace media;

const char* file_name = "data/my_font.xfont";
const char* RESULT_FILE_NAME = "/io/stdout/my_font.xfont";

int main ()
{
  printf ("Results of font_test:\n");

  try
  {
    Font (file_name).Save (RESULT_FILE_NAME);
  }
  catch (std::exception& exception)
  {                                               
    printf ("exception: %s\n",exception.what ());
    return 1;
  }                                               
  catch (...)
  {
    printf ("unknown exception\n");
    return 1;
  }

  return 0;
}
