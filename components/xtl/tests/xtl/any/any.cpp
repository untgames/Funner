#include "shared.h"

template<typename string_type, typename function_type>
struct test
{
    string_type   name;
    function_type action;

    static test make(string_type name, function_type action)
    {
        test result; // MSVC aggreggate initializer bugs
        result.name   = name;
        result.action = action;
        return result;
    }
};

class failure : public stl::exception
{
public: // struction (default cases are OK)

    failure(const stl::string & why) throw()
      : reason(why)
    {
    }

          ~failure() throw() {}

public: // usage

    virtual const char * what() const throw()
    {
        return reason.c_str();
    }

private: // representation

    stl::string reason;

};

inline void check(bool condition, const stl::string & description)
{
    if(!condition)
    {
        throw failure(description);
    }
}

inline void check_true(bool value, const stl::string & description)
{
    check(value, "expected true: " + description);
}

inline void check_false(bool value, const stl::string & description)
{
    check(!value, "expected false: " + description);
}

template<typename lhs_type, typename rhs_type>
void check_equal(
    const lhs_type & lhs, const rhs_type & rhs,
    const stl::string & description)
{
    check(lhs == rhs, "expected equal values: " + description);
}

template<typename lhs_type, typename rhs_type>
void check_unequal(
    const lhs_type & lhs, const rhs_type & rhs,
    const stl::string & description)
{
    check(lhs != rhs, "expected unequal values: " + description);
}

inline void check_null(const void * ptr, const stl::string & description)
{
    check(!ptr, "expected null pointer: " + description);
}

inline void check_non_null(const void * ptr, const stl::string & description)
{
    check(ptr != 0, "expected non-null pointer: " + description);
}

void test_default_ctor()
{
    const any value;

    check_true(value.empty(), "empty");
    check_null(any_cast<int>(&value), "any_cast<int>");
    check_equal(value.type(), typeid(void), "type");
}

void test_converting_ctor()
{
    stl::string text = "test message";
    any value = text;

    check_false(value.empty(), "empty");
    check_equal(value.type(), typeid(stl::string), "type");
    check_null(any_cast<int>(&value), "any_cast<int>");
    check_non_null(any_cast<stl::string>(&value), "any_cast<stl::string>");
    check_equal(
        any_cast<stl::string>(value), text,
        "comparing cast copy against original text");
    check_unequal(
        any_cast<stl::string>(&value), &text,
        "comparing address in copy against original text");
}

void test_copy_ctor()
{
    stl::string text = "test message";
    any original = text, copy = original;

    check_false(copy.empty(), "empty");
    check_equal(original.type(), copy.type(), "type");
    check_equal(
        any_cast<stl::string>(original), any_cast<stl::string>(copy),
        "comparing cast copy against original");
    check_equal(
        text, any_cast<stl::string>(copy),
        "comparing cast copy against original text");
    check_unequal(
        any_cast<stl::string>(&original),
        any_cast<stl::string>(&copy),
        "comparing address in copy against original");
}

void test_copy_assign()
{
    stl::string text = "test message";
    any original = text, copy;
    any * assign_result = &(copy = original);

    check_false(copy.empty(), "empty");
    check_equal(original.type(), copy.type(), "type");
    check_equal(
        any_cast<stl::string>(original), any_cast<stl::string>(copy),
        "comparing cast copy against cast original");
    check_equal(
        text, any_cast<stl::string>(copy),
        "comparing cast copy against original text");
    check_unequal(
        any_cast<stl::string>(&original),
        any_cast<stl::string>(&copy),
        "comparing address in copy against original");
    check_equal(assign_result, &copy, "address of assignment result");
}

void test_converting_assign()
{
    stl::string text = "test message";
    any value;
    any * assign_result = &(value = text);

    check_false(value.empty(), "type");
    check_equal(value.type(), typeid(stl::string), "type");
    check_null(any_cast<int>(&value), "any_cast<int>");
    check_non_null(any_cast<stl::string>(&value), "any_cast<stl::string>");
    check_equal(
        any_cast<stl::string>(value), text,
        "comparing cast copy against original text");
    check_unequal(
        any_cast<stl::string>(&value),
        &text,
        "comparing address in copy against original text");
    check_equal(assign_result, &value, "address of assignment result");
}

void test_bad_cast()
{
    stl::string text = "test message";
    any value = text;
    
    try
    {
      any_cast<const char*> (value);
    }
    catch (bad_any_cast& exception)
    {
      printf ("exception: %s\n", exception.what ());
    }
}

void test_swap()
{
    stl::string text = "test message";
    any original = text, swapped;
    stl::string * original_ptr = any_cast<stl::string>(&original);
    any * swap_result = &original.swap(swapped);

    check_true(original.empty(), "empty on original");
    check_false(swapped.empty(), "empty on swapped");
    check_equal(swapped.type(), typeid(stl::string), "type");
    check_equal(
        text, any_cast<stl::string>(swapped),
        "comparing swapped copy against original text");
    check_non_null(original_ptr, "address in pre-swapped original");
    check_equal(
        original_ptr,
        any_cast<stl::string>(&swapped),
        "comparing address in swapped against original");
    check_equal(swap_result, &original, "address of swap result");
}

void test_null_copying()
{
    const any null;
    any copied = null, assigned;
    assigned = null;

    check_true(null.empty(), "empty on null");
    check_true(copied.empty(), "empty on copied");
    check_true(assigned.empty(), "empty on copied");
}

int main()
{
  printf ("Results of any_test:\n");
  
  try
  {  
    printf                 ("default construction\n");
    test_default_ctor      ();
    printf                 ("single argument construction\n");
    test_converting_ctor   ();
    printf                 ("copy construction\n");
    test_copy_ctor         ();
    printf                 ("copy assignment operator\n");
    test_copy_assign       ();
    printf                 ("converting assignment operator\n");
    test_converting_assign ();
    printf                 ("failed custom keyword cast\n");
    test_bad_cast          ();
    printf                 ("swap member function\n");
    test_swap              ();
    printf                 ("copying operations on a null\n");
    test_null_copying      ();
  }
  catch (std::exception& exception)
  {
    printf ("fail with exception: %s\n", exception.what ());
  }

  return 0;
}
