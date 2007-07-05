#include "test.h"
#include <stl/set>

using namespace stl;
using namespace math;

const char* RESULTS_FILE_NAME = "data/wxf_serializer_results.wxf";

typedef set<string> MySet;

int main ()
{
  printf ("Results of wxf_serializer_test:\n");

  try
  {
    int   array [] = {1,2,3,4,5,6,7,8,9,10};
    MySet set;
    
    set.insert ("world");    
    set.insert ("sunny");
    set.insert ("hello");
    
    WxfSerializer serializer (RESULTS_FILE_NAME,2,8);
    
    serializer.WriteComment ("This file is auto generated\nDo not modify it by hand");
    serializer.BeginFrame   ("test1");
    serializer.WriteComment ("Scalar types serialization");
    serializer.BeginFrame   ("scalar");
    serializer.Write        ("int",-1);    
    serializer.Write        ("size_t",(size_t)-1);
    serializer.Write        ("float",(float)M_PI);
    serializer.Write        ("double",(double)M_PI);
    serializer.Write        ("long_double",(long double)M_PI);
    serializer.Write        ("string","Hello world");
    serializer.Write        ("char",'x');
    serializer.EndFrame     ();
    serializer.WriteComment ("Mathlib types serialization");
    serializer.BeginFrame   ("mathlib");
    serializer.Write        ("vec3f",vec3f (1.1f,2.2f,3.3f));
    serializer.Write        ("mat4f",mat4f (1.0f));
    serializer.Write        ("quatd",quatd (M_PI));
    serializer.EndFrame     ();
    serializer.WriteComment ("Intervals serialization");
    serializer.BeginFrame   ("intervals");
    serializer.Write        ("int_array",sizeof (array)/sizeof (*array),array);
    serializer.Write        ("char_fill",(int)5,'x');
    serializer.Write        ("stl_set",set.begin (),set.end ());
    serializer.EndFrame     ();
    serializer.EndFrame     ();
    serializer.BeginFrame   ("test2",vec4f (1.0f));
    serializer.WriteComment ("This frame need to test block-comments in subframes\nDon't worry about it:)");
    serializer.Write        ("bool_flag");
    serializer.Write        ("mat4f_fill",3,mat4f (2.0f));
//    serializer.EndFrame     (); //это сделано специально!

    serializer.CloseFile ();    
    
    PrintFileInfo (RESULTS_FILE_NAME);    
    
    printf ("Lines count:      %u\n",serializer.GetLinesCount ());
    
    FileSystem::Remove (RESULTS_FILE_NAME);
    
    serializer.Write ("wrong tag");        
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }

  return 0;
}
