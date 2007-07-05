#include "test.h"

struct TestA {};
struct TestB { virtual void foo(void) = 0; };
struct TestC { private: virtual void foo(void) = 0; };
struct TestD : TestA {};
struct TestE : TestB {};
struct TestF : TestC {};
struct TestG : TestB { virtual void foo(void) {} };
struct TestH : TestC { private: virtual void foo(void) {} };
struct TestI : TestB, TestC {};
struct TestJ : TestI { virtual void foo(void) {} };
struct TestK : TestB { virtual void foo(void); virtual void foo2(void) = 0; };
struct TestL : TestK { virtual void foo2(void) {} };
struct TestM : virtual TestB {};
struct TestN : virtual TestC {};
struct TestO : TestM, TestN {};
struct TestP : TestO { virtual void foo(void) {} };
struct TestQ : TestB { virtual void foo(void) = 0; };
struct TestR : TestC { private: virtual void foo(void) = 0; };
struct TestS { virtual void foo(void) {} };
struct TestT { virtual ~TestT(void) {} virtual void foo(void) {} };
struct TestU : TestT { virtual void foo(void) = 0; };
struct TestV : TestT { virtual void foo(void) {} };
struct TestW { virtual void foo1(void) = 0; virtual void foo2(void) = 0; };
struct TestX : TestW { virtual void foo1(void) {}  virtual void foo2(void) {} };
struct TestY { virtual ~TestY(void) = 0; };
struct TestZ { virtual ~TestZ(void) = 0; }; TestZ::~TestZ(void) {}
struct TestAA : TestZ { virtual ~TestAA(void) = 0; }; TestAA::~TestAA(void) {}
struct TestAB : TestAA { virtual ~TestAB(void) {} }; 
struct TestAC { virtual void foo(void) = 0; }; void TestAC::foo(void) {}
struct TestAD : TestAC {};
struct TestAE : TestAD { virtual void foo() {} };
struct TestAF : TestAD { virtual void foo(); }; void TestAF::foo(void) {}
struct TestAG : virtual TestA {};

// template test types:
template <class T> struct TTestA {};
template <class T> struct TTestB { virtual void foo(void) = 0; };
template <class T> struct TTestC { private: virtual void foo(void) = 0; };
template <class T> struct TTestD : TTestA<T> {};
template <class T> struct TTestE : TTestB<T> {};
template <class T> struct TTestF : TTestC<T> {};
template <class T> struct TTestG : TTestB<T> { virtual void foo(void) {} };
template <class T> struct TTestH : TTestC<T> { private: virtual void foo(void) {} };
template <class T> struct TTestI : TTestB<T>, TTestC<T> {};
template <class T> struct TTestJ : TTestI<T> { virtual void foo(void) {} };
template <class T> struct TTestK : TTestB<T> { virtual void foo(void); virtual void foo2(void) = 0; };
template <class T> struct TTestL : TTestK<T> { virtual void foo2(void) {} };
template <class T> struct TTestM : virtual TTestB<T> {};
template <class T> struct TTestN : virtual TTestC<T> {};
template <class T> struct TTestO : TTestM<T>, TTestN<T> {};
template <class T> struct TTestP : TTestO<T> { virtual void foo(void) {} };
template <class T> struct TTestQ : TTestB<T> { virtual void foo(void) = 0; };
template <class T> struct TTestR : TTestC<T> { private: virtual void foo(void) = 0; };
template <class T> struct TTestS { virtual void foo(void) {} };
template <class T> struct TTestT { virtual ~TTestT(void) {} virtual void foo(void) {} };
template <class T> struct TTestU : TTestT<T> { virtual void foo(void) = 0; };
template <class T> struct TTestV : TTestT<T> { virtual void foo(void) {} };
template <class T> struct TTestW { virtual void foo1(void) = 0; virtual void foo2(void) = 0; };
template <class T> struct TTestX : TTestW<T> { virtual void foo1(void) {}  virtual void foo2(void) {} };
template <class T> struct TTestY { virtual ~TTestY(void) = 0; };
template <class T> struct TTestZ { virtual ~TTestZ(void) = 0; }; template <class T> TTestZ<T>::~TTestZ(void) {}
template <class T> struct TTestAA : TTestZ<T> { virtual ~TTestAA(void) = 0; }; template <class T> TTestAA<T>::~TTestAA(void) {}
template <class T> struct TTestAB : TTestAA<T> { virtual ~TTestAB(void) {} }; 
template <class T> struct TTestAC { virtual void foo(void) = 0; }; template <class T> void TTestAC<T>::foo(void) {}
template <class T> struct TTestAD : TTestAC<T> {};
template <class T> struct TTestAE : TTestAD<T> { virtual void foo() {} };
template <class T> struct TTestAF : TTestAD<T> { virtual void foo(); }; template <class T> void TTestAF<T>::foo(void) {}
template <class T> struct TTestAG : virtual TTestA<T> {};

int main ()
{
  printf ("Results of is_abstract_test:\n");

  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestA>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestB>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestC>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestD>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestE>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestF>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestG>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestH>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestI>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestJ>::value), false); // only one method implemented!
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestK>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestL>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestM>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestN>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestO>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestP>::value), false); // ???
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestQ>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestR>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestS>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestT>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestU>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestV>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestW>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestX>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestY>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestZ>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestAA>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestAB>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestAC>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestAD>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestAE>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestAF>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestAG>::value), false);

  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestA>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestB>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestC>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestD>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestE>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestF>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestG>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestH>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestI>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestJ>::value), false); // only one method implemented!
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestK>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestL>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestM>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestN>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestO>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestP>::value), false); // ???
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestQ>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestR>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestS>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestT>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestU>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestV>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestW>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestX>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestY>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestZ>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestAA>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestAB>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestAC>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestAD>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestAE>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestAF>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TestAG>::value), false);

  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestA>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestB>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestC>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestD>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestE>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestF>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestG>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestH>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestI>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestJ>::value), false); // only one method implemented!
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestK>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestL>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestM>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestN>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestO>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestP>::value), false); // ???
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestQ>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestR>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestS>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestT>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestU>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestV>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestW>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestX>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestY>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestZ>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestAA>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestAB>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestAC>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestAD>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestAE>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestAF>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TestAG>::value), false);

  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestA>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestB>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestC>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestD>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestE>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestF>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestG>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestH>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestI>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestJ>::value), false); // only one method implemented!
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestK>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestL>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestM>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestN>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestO>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestP>::value), false); // ???
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestQ>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestR>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestS>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestT>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestU>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestV>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestW>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestX>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestY>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestZ>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestAA>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestAB>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestAC>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestAD>::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestAE>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestAF>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TestAG>::value), false);

  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestA&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestB&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestC&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestD&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestE&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestF&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestG&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestH&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestI&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestJ&>::value), false); // only one method implemented!
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestK&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestL&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestM&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestN&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestO&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestP&>::value), false); // ???
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestQ&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestR&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestS&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestT&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestU&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestV&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestW&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestX&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestY&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestZ&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestAA&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestAB&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestAC&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestAD&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestAE&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestAF&>::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TestAG&>::value), false);
  // MSVC prior to VC7.1 always runs out of swap space trying to
  // compile these, so leave them out for now (the test fails anyway).
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestA<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestB<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestC<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestD<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestE<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestF<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestG<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestH<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestI<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestJ<int> >::value), false); // only one method implemented!
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestK<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestL<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestM<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestN<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestO<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestP<int> >::value), false); // ???
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestQ<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestR<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestS<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestT<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestU<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestV<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestW<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestX<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestY<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestZ<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestAA<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestAB<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestAC<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestAD<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestAE<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestAF<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestAG<int> >::value), false);

  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestA<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestB<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestC<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestD<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestE<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestF<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestG<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestH<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestI<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestJ<int> >::value), false); // only one method implemented!
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestK<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestL<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestM<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestN<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestO<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestP<int> >::value), false); // ???
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestQ<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestR<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestS<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestT<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestU<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestV<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestW<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestX<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestY<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestZ<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestAA<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestAB<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestAC<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestAD<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestAE<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestAF<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<const TTestAG<int> >::value), false);

  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestA<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestB<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestC<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestD<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestE<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestF<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestG<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestH<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestI<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestJ<int> >::value), false); // only one method implemented!
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestK<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestL<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestM<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestN<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestO<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestP<int> >::value), false); // ???
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestQ<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestR<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestS<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestT<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestU<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestV<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestW<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestX<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestY<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestZ<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestAA<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestAB<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestAC<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestAD<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestAE<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestAF<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile TTestAG<int> >::value), false);

  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestA<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestB<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestC<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestD<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestE<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestF<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestG<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestH<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestI<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestJ<int> >::value), false); // only one method implemented!
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestK<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestL<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestM<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestN<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestO<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestP<int> >::value), false); // ???
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestQ<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestR<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestS<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestT<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestU<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestV<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestW<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestX<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestY<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestZ<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestAA<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestAB<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestAC<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestAD<int> >::value), true);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestAE<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestAF<int> >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<volatile const TTestAG<int> >::value), false);

  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestA<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestB<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestC<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestD<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestE<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestF<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestG<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestH<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestI<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestJ<int>& >::value), false); // only one method implemented!
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestK<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestL<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestM<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestN<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestO<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestP<int>& >::value), false); // ???
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestQ<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestR<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestS<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestT<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestU<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestV<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestW<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestX<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestY<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestZ<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestAA<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestAB<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestAC<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestAD<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestAE<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestAF<int>& >::value), false);
  CHECK_INTEGRAL_CONSTANT ((is_abstract<TTestAG<int>& >::value), false);

  return 0;
}
