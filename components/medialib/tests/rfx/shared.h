#ifndef MEDIALIB_RFX_TESTS_SHARED_HEADER
#define MEDIALIB_RFX_TESTS_SHARED_HEADER

#include <stdio.h>
#include <media/material.h>
#include <xtl/visitor.h>

using namespace medialib;
using namespace medialib::rfx;
using namespace math;

//тестовый шейдер
class TestShader: public Shader
{
  public:
    typedef xtl::com_ptr<TestShader>       Pointer;
    typedef xtl::com_ptr<const TestShader> ConstPointer;
    
    static Pointer Create () { return Pointer (new TestShader, false); }
    
  private:
    Shader* CloneCore () const { return new TestShader (*this); }
    
    void AcceptCore (Visitor& visitor)
    {
      if (!TryAccept (*this, visitor))
        Shader::AcceptCore (visitor);
    }
};

#endif
