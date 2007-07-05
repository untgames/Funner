#include <common/parser.h>
#include <stdio.h>

using namespace common;

const char* file_name = "media/tests/parser/test1.wxf";

size_t EnumNodes (Parser::Iterator p,size_t level=0)
{
  size_t nodes = 1;

  for (size_t j=0;j<level;j++)
    printf ("  ");

  printf ("%s",p->Tag ());

  if (p->AttributesCount ())
  {
    printf (":  (");

    for (size_t i=0;i<p->AttributesCount ();i++)
      printf (" '%s'",p->Attribute (i));
      
    printf (" )");
  }    

  printf ("\n");

  for (Parser::Iterator i=p->First ();i;++i)
    nodes += EnumNodes (i,level+1);

  return nodes;
}

int main ()
{
  printf ("Results of wxf1_test:\n");
  
  try
  {
    Parser::Log log;
    Parser parser (log,file_name);
      
    printf ("Parse successfull\n");

    size_t nodes_count = EnumNodes (parser.Root ());
    
    printf ("Total nodes count: %u\n",nodes_count);
    
    if (log.HasErrors () || log.HasWarnings ()) 
    {
      printf ("Parse log:\n");  

      for (size_t i=0;i<log.MessagesCount ();i++)
        printf ("%s\n",log.Message (i));
    }
    else 
      printf ("No errors and warnings detected\n");  
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n", exception.what ());
  }
  
  return 0;  
}
