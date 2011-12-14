#ifndef MEDIALIB_RFX_TESTS_SHARED_HEADER
#define MEDIALIB_RFX_TESTS_SHARED_HEADER

#include <cstdio>
#include <exception>

#include <math/utility.h>

#include <media/rfx.h>

#include <xtl/visitor.h>
#include <xtl/function.h>

using namespace media;
using namespace media::rfx::obsolete;
using namespace math;

//�������� ��������
class TestMaterial: public Material
{
  public:
    typedef xtl::com_ptr<TestMaterial>       Pointer;
    typedef xtl::com_ptr<const TestMaterial> ConstPointer;
    
    static Pointer Create () { return Pointer (new TestMaterial, false); }
    
  private:
    Material* CloneCore () const { return new TestMaterial (*this); }
    
    void AcceptCore (Visitor& visitor)
    {
      if (!TryAccept (*this, visitor))
        Material::AcceptCore (visitor);
    }
};

#endif
