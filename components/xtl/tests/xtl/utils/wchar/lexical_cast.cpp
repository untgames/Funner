#include <xtl/lexical_cast.h>
#include <stl/limits>
#include <math.h>

#include "../shared.h"

#define EPS 1e-06
 
#define TEST_CLOSE(X,Y) printf ((fabs((X)-(Y))<EPS)?"expression '%s==%s' is true\n":"failed expression '%s==%s'\n",#X,#Y)

void test_conversion_from_wchar_t()
{
    TEST (1 == lexical_cast<int>(L'1'));
    TEST_THROW (lexical_cast<int>(L'A'), bad_lexical_cast);

    TEST (123 == lexical_cast<int>(L"123"));
    TEST_THROW (lexical_cast<int>(L""), bad_lexical_cast);
    TEST_THROW (lexical_cast<int>(L"Test"), bad_lexical_cast);

    TEST (1.0 == lexical_cast<double>(L'1'));
    TEST_THROW (lexical_cast<double>(L'A'), bad_lexical_cast);

    TEST_CLOSE (1.23, lexical_cast<double>(L"1.23"));
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
}

int main ()
{
  printf ("Results of wchar_lexical_cast_test:\n");
  
  try
  {  
    printf ("conversion from wchar_t:\n");  
    test_conversion_from_wchar_t ();

    printf("conversion to wchar_t:\n");
    test_conversion_to_wchar_t ();

    printf ("conversion from wstring:\n");
    test_conversion_from_wstring ();
    
    printf ("conversion to wstring:\n");
    test_conversion_to_wstring ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
