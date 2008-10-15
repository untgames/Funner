#include "shared.h"

const char*  TEXT_BUFFER      = "A";
const size_t TEXT_BUFFER_SIZE = strlen (TEXT_BUFFER);

bool my_check (size_t buffer_size, const char* buffer)
{
  printf ("check: buffer_size=%u buffer='%s'\n", buffer_size, buffer);

  if (buffer_size > TEXT_BUFFER_SIZE)
    buffer_size = TEXT_BUFFER_SIZE;    

  return memcmp (buffer, TEXT_BUFFER, buffer_size) == 0;
}

void my_parse (ParseTreeBuilder& builder, ParseLog& log, const char* name, size_t buffer_size, char* buffer)
{
  printf ("parse: name='%s' buffer_size=%u buffer='%s'\n", name, buffer_size, buffer);
  
  builder.BeginNode ("A", 11);
  builder.AddAttribute ("B");
  builder.EndNode ();
  
  log.Error (name, 12, "syntax error");
}

int main ()
{
  printf ("Results of parser_manager_test:\n");

  try
  {
    ParserManager::RegisterParser ("test", &my_parse, &my_check);

    Parser parser ("text1", TEXT_BUFFER_SIZE, TEXT_BUFFER);
    
    printf ("Parse log:\n");
    
    parser.Log ().Print (&print_log);
    
    printf ("Parse result:\n");    

    print (parser.Root (), 0);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
