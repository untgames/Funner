#ifndef XTL_TYPE_TRAITS_TEST_HEADER
#define XTL_TYPE_TRAITS_TEST_HEADER

#include <stdio.h>
#include <string.h>
#include <typeinfo>
#include <xtl/type_traits>

using namespace tr1;

//проверка значения выражения
#define CHECK_INTEGRAL_CONSTANT(EXPRESSION, CORRECT_VALUE)  \
{ \
  int result = EXPRESSION, correct_result = CORRECT_VALUE; \
 \
  if (result == correct_result) printf ("expression %s=%s\n", #EXPRESSION, result ? "true" : "false"); \
  else                          printf ("error: %s=%s, must be %s\n", #EXPRESSION, result ? "true" : "false",  \
                                        correct_result ? "true" : "false"); \
}

#define TEST(X) printf ((X)?"expression '%s' is true\n":"failed expression '%s'\n",#X);

#define CHECK_TYPE(EXPECTED_TYPE, TYPE_EXPRESSION) \
{ \
  if (is_same<TYPE_EXPRESSION, EXPECTED_TYPE>::value) \
    printf ("expression %s=%s\n", #TYPE_EXPRESSION, #EXPECTED_TYPE); \
  else \
    printf ("error: expression %s=%s must be %s\n", #TYPE_EXPRESSION, typeid (TYPE_EXPRESSION).name (), #EXPECTED_TYPE); \
}

#define TRANSFORM_CHECK(name, from_suffix, to_suffix)\
   CHECK_TYPE(bool to_suffix, name<bool from_suffix>::type);\
   CHECK_TYPE(char to_suffix, name<char from_suffix>::type);\
   CHECK_TYPE(wchar_t to_suffix, name<wchar_t from_suffix>::type);\
   CHECK_TYPE(signed char to_suffix, name<signed char from_suffix>::type);\
   CHECK_TYPE(unsigned char to_suffix, name<unsigned char from_suffix>::type);\
   CHECK_TYPE(short to_suffix, name<short from_suffix>::type);\
   CHECK_TYPE(unsigned short to_suffix, name<unsigned short from_suffix>::type);\
   CHECK_TYPE(int to_suffix, name<int from_suffix>::type);\
   CHECK_TYPE(unsigned int to_suffix, name<unsigned int from_suffix>::type);\
   CHECK_TYPE(long to_suffix, name<long from_suffix>::type);\
   CHECK_TYPE(unsigned long to_suffix, name<unsigned long from_suffix>::type);\
   CHECK_TYPE(float to_suffix, name<float from_suffix>::type);\
   CHECK_TYPE(long double to_suffix, name<long double from_suffix>::type);\
   CHECK_TYPE(double to_suffix, name<double from_suffix>::type);\
   CHECK_TYPE(UDT to_suffix, name<UDT from_suffix>::type);\
   CHECK_TYPE(enum1 to_suffix, name<enum1 from_suffix>::type);
   
#define DUMMY_MACRO_PARAM /**/

#define DECL_TRANSFORM_TEST(name, type, from, to)\
void name(){ TRANSFORM_CHECK(type, from, to) }
#define DECL_TRANSFORM_TEST3(name, type, from)\
void name(){ TRANSFORM_CHECK(type, from, DUMMY_MACRO_PARAM) }
#define DECL_TRANSFORM_TEST2(name, type, to)\
void name(){ TRANSFORM_CHECK(type, DUMMY_MACRO_PARAM, to) }
#define DECL_TRANSFORM_TEST0(name, type)\
void name(){ TRANSFORM_CHECK(type, DUMMY_MACRO_PARAM, DUMMY_MACRO_PARAM) }   

/*
    Тестовые типы данных
*/

//тестовые перечисления
enum enum_UDT{ one, two, three };

enum enum1
{
   one_,two_
};

enum enum2
{
   three_,four_
};

//тестовое объединение
union union_UDT
{
  int x;
  double y;
  ~union_UDT(){}
};

union POD_union_UDT
{
  int x;
  double y;
};

union empty_union_UDT
{
   ~empty_union_UDT(){}
};

union empty_POD_union_UDT{};

//тип данных с размером не равным 0
struct UDT
{
   UDT(){};
   ~UDT(){};
   UDT(const UDT&);
   UDT& operator=(const UDT&);
   int i;

   void f1();
   int f2();
   int f3(int);
   int f4(int, float);
};

//тип данных с размером равным 0
struct empty_UDT
{
   empty_UDT(){};
   empty_UDT(const empty_UDT&){};
   ~empty_UDT(){};
   empty_UDT& operator=(const empty_UDT&){ return *this; }
   bool operator==(const empty_UDT&)const
   { return true; }
};

struct non_empty
{
   int i;
   
   private:
     non_empty (const non_empty&);
     non_empty& operator = (const non_empty&);
};

//POD тип данных
struct POD_UDT { int x; };

struct nothrow_copy_UDT
{
   nothrow_copy_UDT();
   nothrow_copy_UDT(const nothrow_copy_UDT&)throw();
   ~nothrow_copy_UDT(){};
   nothrow_copy_UDT& operator=(const nothrow_copy_UDT&){ return *this; }
   bool operator==(const nothrow_copy_UDT&)const
   { return true; }
};

struct nothrow_assign_UDT
{
   nothrow_assign_UDT();
   nothrow_assign_UDT(const nothrow_assign_UDT&);
   ~nothrow_assign_UDT(){};
   nothrow_assign_UDT& operator=(const nothrow_assign_UDT&)throw(){ return *this; }
   bool operator==(const nothrow_assign_UDT&)const
   { return true; }
};

struct nothrow_construct_UDT
{
   nothrow_construct_UDT()throw();
   nothrow_construct_UDT(const nothrow_construct_UDT&);
   ~nothrow_construct_UDT(){};
   nothrow_construct_UDT& operator=(const nothrow_construct_UDT&){ return *this; }
   bool operator==(const nothrow_construct_UDT&)const
   { return true; }
};

//абстрактные базовые классы
struct test_abc1
{
   test_abc1();
   virtual ~test_abc1();
   test_abc1(const test_abc1&);
   test_abc1& operator=(const test_abc1&);
   virtual void foo() = 0;
   virtual void foo2() = 0;
};

struct test_abc2
{
   virtual ~test_abc2();
   virtual void foo() = 0;
   virtual void foo2() = 0;
};

struct test_abc3 : public test_abc1
{
   virtual void foo3() = 0;
};

//неопределённый тип
struct incomplete_type;

//используется для проверки того, что is_pointer не возвращает true для классов с ooperator void* ()
struct non_pointer
{
   operator void*(){return this;}
};

struct non_int_pointer
{
   int i;
   operator int*(){return &i;}
};
struct int_constructible
{
   int_constructible(int);
};
struct int_convertible
{
   operator int();
};

class Base { };

class Derived : public Base { };
class Derived2 : public Base { };
class MultiBase : public Derived, public Derived2 {};
class PrivateBase : private Base {};

class NonDerived { };

struct polymorphic_base
{
   virtual ~polymorphic_base();
   virtual void method();
};

struct polymorphic_derived1 : polymorphic_base
{
};

struct polymorphic_derived2 : polymorphic_base
{
   virtual void method();
};

struct virtual_inherit1 : virtual Base { };
struct virtual_inherit2 : virtual_inherit1 { };

struct VB
{
   virtual ~VB(){};
};

struct VD : VB
{
   ~VD(){};
};

//функции
typedef void foo0_t();
typedef void foo1_t(int);
typedef void foo2_t(int&, double);
typedef void foo3_t(int&, bool, int, int);
typedef void foo4_t(int, bool, int*, int[], int, int, int, int, int);

//указатели на функции
typedef void(*f1)();
typedef int(*f2)(int);
typedef int(*f3)(int, bool);
typedef void (UDT::*mf1)();
typedef int (UDT::*mf2)();
typedef int (UDT::*mf3)(int);
typedef int (UDT::*mf4)(int, float);
typedef int (UDT::*mp);
typedef int (UDT::*cmf)(int) const;

#ifdef _MSC_VER
  #pragma warning (push)
  #pragma warning (disable: 4181)
#endif

//ссылочные типы
typedef int& r_type;
typedef const r_type cr_type;

#ifdef _MSC_VER
  #pragma warning (pop)
#endif

struct trivial_except_construct
{
   trivial_except_construct();
   int i;
};

struct trivial_except_destroy
{
   ~trivial_except_destroy();
   int i;
};

struct trivial_except_copy
{
   trivial_except_copy(trivial_except_copy const&);
   int i;
};

struct trivial_except_assign
{
   trivial_except_assign& operator=(trivial_except_assign const&);
   int i;
};

template <class T>
struct wrap
{
   T t;
   int j;
};

#endif
