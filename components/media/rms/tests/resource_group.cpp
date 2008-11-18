#include "shared.h"

const char* GROUP_MASK = "data/group/*";

void dump (const char* name, const ResourceGroup& group)
{
  printf ("%s (%u items):\n", name, group.Size ());
  
  for (size_t i=0, count=group.Size (); i<count; i++)
    printf ("  '%s'\n", group.Item (i));
}

void print_on_add (const char* resource_name)
{
  printf ("add '%s'\n", resource_name);
}

void print_on_remove (const char* resource_name)
{
  printf ("remove '%s'\n", resource_name);
}

int main ()
{
  printf ("Results of resource_group_tst:\n");
  
  try
  {
    ResourceGroup group1;
    
    group1.RegisterEventHandler (ResourceGroupEvent_OnAdd, &print_on_add);
    group1.RegisterEventHandler (ResourceGroupEvent_OnRemove, &print_on_remove);
    
    add_file_group ("data/group/*", group1);
        
    dump ("group1 initial state", group1);
    
    ResourceGroup group2;
    
    group2.Add ("data/group/test1.txt");

    group1.Remove (group2);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
