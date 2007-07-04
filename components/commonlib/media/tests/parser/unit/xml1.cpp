#include <common/parser.h>
#include <stdio.h>

using namespace common;

const char* file_name[] = {"data/test1.xml", "data/error_test1.xml"};
const size_t elements = 2;

size_t EnumNodes (ParseNode* p,size_t level=0)
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

  for (ParseNode* i=p->First();i;i=i->Next())
    nodes += EnumNodes (i,level+1);
    
  return nodes;
}

int main ()
{
  for (size_t i=0;i<elements;i++)
  {
    ParseLog log;

    printf ("Results of xml1_test:\n");

    Parser p (log, file_name[i]);
      
    printf ("Parse successfull\n");

    size_t nodes_count = EnumNodes (p.Root ());
    
    printf ("Total nodes count: %u\n",nodes_count);
    
    for (size_t i = 0; i < log.MessagesCount (); i++)
    {
      printf ("%s\n", log.Message(i));
    }
  }
  return 0;  
}
