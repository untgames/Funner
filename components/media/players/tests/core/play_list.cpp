#include "shared.h"

int main ()
{
  printf ("Results of play_list_test:\n");
  
  try
  {
    PlayList list1;
	
	dump ("list1_default", list1);
    
    printf ("list1 empty: %s\n", list1.IsEmpty () ? "true" : "false");
    
    list1.AddSource ("source1");
    size_t remove_index = list1.AddSource ("source2");
    list1.AddSource ("source3");
    
    dump ("list1_after_add", list1);
    printf ("list1 empty: %s\n", list1.IsEmpty () ? "true" : "false");    
    
    list1.RemoveSource (remove_index);
    
    dump ("list1_after_remove", list1);
    
    PlayList list2 = list1, list3 = list1.Clone ();
    
    dump ("list2_copy", list2);
    
    list1.RemoveSource (0u);
    
    dump ("list2_after_list1_remove", list2);
    dump ("list3_after_list1_remove", list3);
    
    list1.AddSource ("source4");
    list1.AddSource ("source4");
    list1.AddSource ("source5");    
    list1.AddSource ("source4");    
    
    dump ("list1_with_duplicates", list1);
    
    list1.RemoveSource ("source4");
    
    dump ("list1_after_remove_duplicates", list1);
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }
}
