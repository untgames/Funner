#include <common/parser.h>
#include <common/streams.h>
#include <stl/vector>
#include <stdio.h>

using namespace common;
using namespace stl;
using namespace math;

const char* SRC_FILE_NAME = "data/test3.wxf";
const char* DST_FILE_NAME = "/io/stdout";

class Test
{
  public:
    Test (const char* in_file_name,const char* out_file_name);
    
    template <class T> void Translate      (const char* type_name,const char* tag,const T& default_value = T ());
    template <class T> void TranslateArray (const char* type_name,const char* tag);
    template <class T> void TranslateArray (const char* type_name,const char* tag,size_t start,size_t step=0);
    
  private:
    template <class T> void Print (const char* type_name,const char* tag,size_t count,const T* array);
  
  private:
    Parser::Log      log;    
    Parser           parser;
    OutputTextStream stream;
    TextSerializer   serializer;
};

Test::Test (const char* in_file_name,const char* out_file_name)
  : parser (log,in_file_name), stream (out_file_name), serializer (stream,", ")
{ 
  if (log.HasErrors () || log.HasWarnings ())
  {
    printf ("Parse log:\n");
    
    for (size_t i=0;i<log.MessagesCount ();i++)
      printf ("%s\n",log.Message (i));
  }
}

template <class T> 
void Test::Print (const char* type_name,const char* tag,size_t count,const T* array)
{
  stream.Printf ("%12s %28s = {",type_name,format ("<%s>[%u]",tag,count).c_str ());

  if (count)
  {    
    for (size_t i=0;i<count-1;i++)
    {
      stream.Print ("\"");
      serializer.Write (array [i]);
      stream.Print ("\"");
      serializer.WriteSeparator ();
    }

    stream.Print ("\"");
    serializer.Write (array [count-1]);
    stream.Print ("\"");
  }

  stream.Print ("}\n");
}

template <class T>
void Test::Translate (const char* type_name,const char* tag,const T& default_value)
{
  T object;
  
  parser.Root ()->Read (tag,object,default_value);
  
  stream.Printf ("%12s %28s = \"",type_name,format ("<%s>",tag).c_str ());
  serializer.Write (object);
  stream.Print ("\"\n");
}

template <class T> 
void Test::TranslateArray (const char* type_name,const char* tag,size_t start,size_t step)
{
  vector<T> v;

  parser.Root ()->ReadArray (tag,back_inserter (v),(size_t)-1,start,step);
  Print (type_name,tag,v.size (),&v [0]);
}

template <class T>
void Test::TranslateArray (const char* type_name,const char* tag)
{
  vector<T> v;

  parser.Root ()->ReadArray (tag,back_inserter (v));  
  Print (type_name,tag,v.size (),&v [0]);
}

int main ()
{
  printf ("Results of read-attributes_test:\n");

  try
  {
    Test test (SRC_FILE_NAME,DST_FILE_NAME);
    
    printf ("Dump:\n");

    test.Translate<int>         ("int","int_value");
    test.Translate<const char*> ("const char*","string_value");
    test.Translate<string>      ("string","string_value");
    test.Translate<float>       ("float","float_value");
    test.Translate<char>        ("char","char_value");
    test.Translate<short>       ("short","hex_value");
    test.Translate<int>         ("int","frame.subframe.int_value");
    test.Translate<vec3f>       ("vec3f","vec3f_value");
    test.Translate<vec3f>       ("vec3f","vec3f_wrong",vec3f (-1.0f));
    test.Translate<mat3f>       ("mat3f","mat3f_value");
    test.TranslateArray<float>  ("float","mat3f_value");
    test.TranslateArray<int>    ("int","mat3f_value",3,2);
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n",exception.what ());
  }
  
  return 0;  
}
