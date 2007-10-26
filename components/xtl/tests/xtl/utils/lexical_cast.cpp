#include <xtl/lexical_cast.h>
#include <stl/limits>
#include <math.h>

#include "shared.h"

#define EPS 1e-06

#define TEST_THROW(X,Y) try { (X); printf ("executed '%s'\n", #X); } catch (Y&) { printf ("exception '%s' accepted\n", #Y); } \
  catch (...) { printf ("wrong exception. must be '%s'\n", #Y); }
  
#define TEST_CLOSE(X,Y) printf ((fabs((X)-(Y))<EPS)?"expression '%s==%s' is true\n":"failed expression '%s==%s'\n",#X,#Y)

void test_conversion_to_char()
{
    TEST ('A' == lexical_cast<char>('A'));
    TEST (' ' == lexical_cast<char>(' '));
    TEST ('1' == lexical_cast<char>(1));
    TEST ('0' == lexical_cast<char>(0));
    TEST_THROW (lexical_cast<char>(123), bad_lexical_cast);
    TEST ('1' == lexical_cast<char>(1.0));
    TEST ('1' == lexical_cast<char>(true));
    TEST ('0' == lexical_cast<char>(false));
    TEST ('A' == lexical_cast<char>("A"));
    TEST (' ' == lexical_cast<char>(" "));
    TEST_THROW (lexical_cast<char>(""), bad_lexical_cast);
    TEST_THROW (lexical_cast<char>("Test"), bad_lexical_cast);
    TEST ('A' == lexical_cast<char>(stl::string("A")));
    TEST (' ' == lexical_cast<char>(stl::string(" ")));
    TEST_THROW (
        lexical_cast<char>(stl::string("")), bad_lexical_cast);
    TEST_THROW (
        lexical_cast<char>(stl::string("Test")), bad_lexical_cast);
}

void test_conversion_to_int()
{
    TEST (1 == lexical_cast<int>('1'));
    TEST (0 == lexical_cast<int>('0'));
    TEST_THROW (lexical_cast<int>('A'), bad_lexical_cast);
    TEST (1 == lexical_cast<int>(1));
    TEST (
        (stl::numeric_limits<int>::max)() == 
        lexical_cast<int>((stl::numeric_limits<int>::max)()));
    TEST (1 == lexical_cast<int>(1.0));

    TEST_THROW (lexical_cast<int>(1.23), bad_lexical_cast);

    TEST_THROW (lexical_cast<int>(1e20), bad_lexical_cast);
    TEST (1 == lexical_cast<int>(true));
    TEST (0 == lexical_cast<int>(false));
    TEST (123 == lexical_cast<int>("123"));
    TEST_THROW (
        lexical_cast<int>(" 123"), bad_lexical_cast);
    TEST_THROW (lexical_cast<int>(""), bad_lexical_cast);
    TEST_THROW (lexical_cast<int>("Test"), bad_lexical_cast);
    TEST (123 == lexical_cast<int>("123"));
    TEST (123 == lexical_cast<int>(stl::string("123")));
    TEST_THROW (
        lexical_cast<int>(stl::string(" 123")), bad_lexical_cast);
    TEST_THROW (
        lexical_cast<int>(stl::string("")), bad_lexical_cast);
    TEST_THROW (
        lexical_cast<int>(stl::string("Test")), bad_lexical_cast);
}

void test_conversion_to_double()
{
    TEST_CLOSE (1.0, lexical_cast<double>('1'));
    TEST_THROW (lexical_cast<double>('A'), bad_lexical_cast);
    TEST_CLOSE (1.0, lexical_cast<double>(1));
    TEST_CLOSE (1.23, lexical_cast<double>(1.23));
    TEST_CLOSE (1.0, lexical_cast<double>(true));
    TEST_CLOSE (0.0, lexical_cast<double>(false));
    TEST_CLOSE (1.23, lexical_cast<double>("1.23"));
    TEST_THROW (lexical_cast<double>(""), bad_lexical_cast);
    TEST_THROW (lexical_cast<double>("Test"), bad_lexical_cast);
    TEST_CLOSE (1.23, lexical_cast<double>(stl::string("1.23")));
    TEST_THROW (
        lexical_cast<double>(stl::string("")), bad_lexical_cast);
    TEST_THROW (
        lexical_cast<double>(stl::string("Test")), bad_lexical_cast);
}

void test_conversion_to_bool()
{
    TEST (true == lexical_cast<bool>('1'));
    TEST (false == lexical_cast<bool>('0'));
    TEST_THROW (lexical_cast<bool>('A'), bad_lexical_cast);
    TEST (true == lexical_cast<bool>(1));
    TEST (false == lexical_cast<bool>(0));
    TEST_THROW (lexical_cast<bool>(123), bad_lexical_cast);
    TEST (true == lexical_cast<bool>(1.0));
    TEST (false == lexical_cast<bool>(0.0));
    TEST (true == lexical_cast<bool>(true));
    TEST (false == lexical_cast<bool>(false));
    TEST (true == lexical_cast<bool>("1"));
    TEST (false == lexical_cast<bool>("0"));
    TEST_THROW (lexical_cast<bool>(""), bad_lexical_cast);
    TEST_THROW (lexical_cast<bool>("Test"), bad_lexical_cast);
    TEST (true == lexical_cast<bool>("1"));
    TEST (false == lexical_cast<bool>("0"));
    TEST (true == lexical_cast<bool>(stl::string("1")));
    TEST (false == lexical_cast<bool>(stl::string("0")));
    TEST_THROW (
        lexical_cast<bool>(stl::string("")), bad_lexical_cast);
    TEST_THROW (
        lexical_cast<bool>(stl::string("Test")), bad_lexical_cast);
}

void test_conversion_to_string()
{
    TEST ("A" == lexical_cast<stl::string>('A'));
    TEST (" " == lexical_cast<stl::string>(' '));
    TEST ("123" == lexical_cast<stl::string>(123));
    TEST ("1.23" == lexical_cast<stl::string>(1.23));
    TEST ("1.111111111" == lexical_cast<stl::string>(1.111111111));
   
    TEST ("1" == lexical_cast<stl::string>(true));
    TEST ("0" == lexical_cast<stl::string>(false));
    TEST ("Test" == lexical_cast<stl::string>("Test"));
    TEST (" " == lexical_cast<stl::string>(" "));
    TEST ("" == lexical_cast<stl::string>(""));
    TEST ("Test" == lexical_cast<stl::string>(stl::string("Test")));
    TEST (" " == lexical_cast<stl::string>(stl::string(" ")));
    TEST ("" == lexical_cast<stl::string>(stl::string("")));
}

void test_conversion_to_pointer()
{
    TEST_THROW (lexical_cast<char *>("Test"), bad_lexical_cast);
    TEST_THROW (lexical_cast<wchar_t *>("Test"), bad_lexical_cast);
}

/*void test_conversion_from_to_wchar_t_alias()
{
    TEST (123u == lexical_cast<unsigned short>("123"));
    TEST (123u == lexical_cast<unsigned int>("123"));
    TEST (123u == lexical_cast<unsigned long>("123"));
    TEST (stl::string("123") ==
        lexical_cast<stl::string>(static_cast<unsigned short>(123)));
    TEST (stl::string("123") == lexical_cast<stl::string>(123u));
    TEST (stl::string("123") == lexical_cast<stl::string>(123ul));
}

void test_conversion_from_wchar_t()
{
    TEST (1 == lexical_cast<int>(L'1'));
    TEST_THROW (lexical_cast<int>(L'A'), bad_lexical_cast);

    TEST (123 == lexical_cast<int>(L"123"));
    TEST_THROW (lexical_cast<int>(L""), bad_lexical_cast);
    TEST_THROW (lexical_cast<int>(L"Test"), bad_lexical_cast);

    TEST (1.0 == lexical_cast<double>(L'1'));
    TEST_THROW (lexical_cast<double>(L'A'), bad_lexical_cast);

    TEST (1.23 == lexical_cast<double>(L"1.23"));
    TEST_THROW (lexical_cast<double>(L""), bad_lexical_cast);
    TEST_THROW (lexical_cast<double>(L"Test"), bad_lexical_cast);

    TEST (true == lexical_cast<bool>(L'1'));
    TEST (false == lexical_cast<bool>(L'0'));
    TEST_THROW (lexical_cast<bool>(L'A'), bad_lexical_cast);

    TEST (true == lexical_cast<bool>(L"1"));
    TEST (false == lexical_cast<bool>(L"0"));
    TEST_THROW (lexical_cast<bool>(L""), bad_lexical_cast);
    TEST_THROW (lexical_cast<bool>(L"Test"), bad_lexical_cast);
}

void test_conversion_to_wchar_t()
{
    TEST (L'1' == lexical_cast<wchar_t>(1));
    TEST (L'0' == lexical_cast<wchar_t>(0));
    TEST_THROW (lexical_cast<wchar_t>(123), bad_lexical_cast);
    TEST (L'1' == lexical_cast<wchar_t>(1.0));
    TEST (L'0' == lexical_cast<wchar_t>(0.0));
    TEST (L'1' == lexical_cast<wchar_t>(true));
    TEST (L'0' == lexical_cast<wchar_t>(false));
    TEST (L'A' == lexical_cast<wchar_t>(L'A'));
    TEST (L' ' == lexical_cast<wchar_t>(L' '));
    TEST (L'A' == lexical_cast<wchar_t>(L"A"));
    TEST (L' ' == lexical_cast<wchar_t>(L" "));
    TEST_THROW (lexical_cast<wchar_t>(L""), bad_lexical_cast);
    TEST_THROW (lexical_cast<wchar_t>(L"Test"), bad_lexical_cast);
    TEST (L'A' == lexical_cast<wchar_t>(stl::wstring(L"A")));
    TEST (L' ' == lexical_cast<wchar_t>(stl::wstring(L" ")));
    TEST_THROW (
        lexical_cast<wchar_t>(stl::wstring(L"")), bad_lexical_cast);
    TEST_THROW (
        lexical_cast<wchar_t>(stl::wstring(L"Test")), bad_lexical_cast);
}

void test_conversion_from_wstring()
{
    TEST (123 == lexical_cast<int>(stl::wstring(L"123")));
    TEST_THROW (
        lexical_cast<int>(stl::wstring(L"")), bad_lexical_cast);
    TEST_THROW (
        lexical_cast<int>(stl::wstring(L"Test")), bad_lexical_cast);

    TEST (true == lexical_cast<bool>(stl::wstring(L"1")));
    TEST (false == lexical_cast<bool>(stl::wstring(L"0")));
    TEST_THROW (
        lexical_cast<bool>(stl::wstring(L"")), bad_lexical_cast);
    TEST_THROW (
        lexical_cast<bool>(stl::wstring(L"Test")), bad_lexical_cast);
}

void test_conversion_to_wstring()
{
    TEST (L"123" == lexical_cast<stl::wstring>(123));
    TEST (L"1.23" == lexical_cast<stl::wstring>(1.23));
    TEST (L"1.111111111" == lexical_cast<stl::wstring>(1.111111111));
    TEST (L"1" == lexical_cast<stl::wstring>(true));
    TEST (L"0" == lexical_cast<stl::wstring>(false));

    TEST (L"A" == lexical_cast<stl::wstring>(L'A'));
    TEST (L" " == lexical_cast<stl::wstring>(L' '));

    TEST (L"Test" == lexical_cast<stl::wstring>(L"Test"));
    TEST (L" " == lexical_cast<stl::wstring>(L" "));
    TEST (L"" == lexical_cast<stl::wstring>(L""));
    TEST (L"Test" == lexical_cast<stl::wstring>(stl::wstring(L"Test")));
    TEST (L" " == lexical_cast<stl::wstring>(stl::wstring(L" ")));
    TEST (L"" == lexical_cast<stl::wstring>(stl::wstring(L"")));
} */

void test_bad_lexical_cast()
{
    try
    {
        lexical_cast<int>(stl::string("Test"));

        TEST (false); // Exception expected
    }
    catch(const bad_lexical_cast &e)
    {
        TEST (&e.source_type() == &typeid(stl::string));
        TEST (&e.target_type() == &typeid(int));
    }
}

void test_no_whitespace_stripping()
{
    TEST_THROW (lexical_cast<int>(" 123"), bad_lexical_cast);
    TEST_THROW (lexical_cast<int>("123 "), bad_lexical_cast);
}

int main ()
{
  printf ("Results of lexical_cast_test:\n");
  
  try
  {  
    printf ("conversion to char:\n");
    test_conversion_to_char ();
    
    printf ("conversion to int:\n");
    test_conversion_to_int ();
    
    printf ("conversion to double:\n");
    test_conversion_to_double ();
    
    printf ("conversion to bool:\n");
    test_conversion_to_bool ();
    
  //  printf ("conversion from to wchar_t:\n");
  //  test_conversion_from_to_wchar_t_alias ();
    
    printf ("conversion to pointer:\n");
    test_conversion_to_pointer ();
    
    printf ("conversion to string:\n");
    test_conversion_to_string ();

  //  printf ("conversion from wchar_t:\n");  
  //  test_conversion_from_wchar_t ();
    
  //  printf("conversion to wchar_t:\n");
  //  test_conversion_to_wchar_t ();
    
  //  printf ("conversion from wstring:\n");
  //  test_conversion_from_wstring ();
    
  //  printf ("conversion to wstring:\n");
  //  test_conversion_to_wstring ();

    printf ("bad_lexical_cast test:\n");
    test_bad_lexical_cast ();
    
    printf ("no whitespace stripping test:\n");
    test_no_whitespace_stripping ();  
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
