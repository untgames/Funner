#include "shared.h"

using namespace media;

int main ()
{
  printf ("Results of charset_manager test:\n");

  try
  {
    const char* charset = CharsetManager::FindCharset ("1");

    printf ("Charset '1' found = %d\n", charset != 0);

    CharsetManager::RegisterCharset ("1", "abcd");

    charset = CharsetManager::FindCharset ("1");

    printf ("Charset '1' found = %d\n", charset != 0);
    printf ("Charset '1' = '%s'\n", charset);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n",exception.what ());
  }
  catch (...)
  {
    printf ("unknown exception\n");
  }

  return 0;
}
