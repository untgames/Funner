#include <common/parser.h>
#include <common/strlib.h>
#include <stl/vector>
#include <mathlib.h>
#include <stdio.h>

using namespace common;
using namespace stl;
using namespace math;

const char* SRC_FILE_NAME = "data/test3.wxf";

class Test
{
  public:
    Test (const char* in_file_name);
    
    template <class T> void Translate      (const char* type_name,const char* tag,const T& default_value = T ());
    template <class T> void TranslateArray (const char* type_name,const char* tag);
    template <class T> void TranslateArray (const char* type_name,const char* tag,size_t start,size_t step=0);
    
  private:
    template <class T> void Print (const char* type_name,const char* tag,size_t count,const T* array);    
        
    void Write (int value)            { printf ("%d", value); }
    void Write (short value)          { printf ("%d", value); }
    void Write (const char* s)        { printf ("%s", s); }    
    void Write (float value)          { printf ("%g", value); }
    void Write (const stl::string& s) { Write (s.c_str ()); }
    void Write (char value)           { printf ("%c", value); }
    void Write (const math::vec3f& v) { printf ("%g, %g, %g", v [0], v [1], v [2]); }
    void Write (const math::mat3f& m) { Write (m [0]); printf (", "); Write (m [1]); printf (", "); Write (m [2]); }
  
  private:
    Parser::Log log;    
    Parser      parser;    
};

Test::Test (const char* in_file_name)
  : parser (log,in_file_name)
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
  printf ("%12s %28s = {",type_name,format ("<%s>[%u]",tag,count).c_str ());

  if (count)
  {    
    for (size_t i=0;i<count-1;i++)
    {
      printf ("\"");
      Write  (array [i]);
      printf ("\", ");
    }
    
    printf ("\"");
    Write  (array [count-1]);
    printf ("\"");
  }

  printf ("}\n");
}

template <class T>
void Test::Translate (const char* type_name,const char* tag,const T& default_value)
{
  T object;
  
  read (parser.Root (),tag,object,default_value);
  
  printf ("%12s %28s = \"",type_name,format ("<%s>",tag).c_str ());
  Write  (object);
  printf ("\"\n");
}

template <class T> 
void Test::TranslateArray (const char* type_name,const char* tag,size_t start,size_t step)
{
  vector<T> v;

  read_range (parser.Root (),tag,back_inserter (v),(size_t)-1,start,step);
  Print (type_name,tag,v.size (),&v [0]);
}

template <class T>
void Test::TranslateArray (const char* type_name,const char* tag)
{
  vector<T> v;

  read_range (parser.Root (),tag,back_inserter (v));  
  Print (type_name,tag,v.size (),&v [0]);
}

int main ()
{
  printf ("Results of read-attributes_test:\n");

  try
  {
    Test test (SRC_FILE_NAME);
    
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
