#if 0
#include <cstdio>
#include <exception>

#include <stl/string>

#include <xtl/shared_ptr.h>

#include <math/vector.h>
#include <math/matrix.h>

#include <common/property_bindings.h>

using namespace common;

class A
{
  public:
    A (int in_id)
      : id (in_id)
      , int_value (0)
      , float_value (0.0)
    {
      printf ("A::A(%d)\n", id);
    }

    ~A () { printf ("A::~A(%d)\n", id); }
    
    short               IntValue       () const { return int_value; }    
    double              FloatValue     () const { return float_value; }
    const char*         StringValue    () const { return string_value.c_str (); }
    const math::vec3f&  VectorValue    () const { return vector_value; }
    const math::mat3f&  MatrixValue    () const { return matrix_value; }
    void                SetIntValue    (short x) { int_value = x; }
    void                SetFloatValue  (double x) { float_value = x; }
    void                SetStringValue (const char* x) { string_value = x; }    
    void                SetVectorValue (const math::vec3f& x) { vector_value = x; }
    void                SetMatrixValue (const math::mat3f& x) { matrix_value = x; }

  private:
    int         id;
    short       int_value;
    double      float_value;
    stl::string string_value;
    math::vec3f vector_value;
    math::mat3f matrix_value;
};

typedef xtl::shared_ptr<A> APtr;

int main ()
{
  printf ("Results of properties_bindings_test:\n");
  
  try
  {
    PropertyMap properties;
    
    APtr a (new A (12));
    
    bind (properties, "Int", a, &A::SetIntValue, &A::IntValue);
    bind (properties, "Float", a, &A::SetFloatValue, &A::FloatValue);
    bind (properties, "String", a, &A::SetStringValue, &A::StringValue);
    bind (properties, "Vector", a, &A::SetVectorValue, &A::VectorValue);
    bind (properties, "Matrix", a, &A::SetMatrixValue, &A::MatrixValue);    
    bind (properties, "Unavailable", a, &A::SetMatrixValue, &A::MatrixValue);        
    
    a->SetStringValue ("Hello world");
    
    printf ("check PropertyMap::UpdateBindings\n");
    
    properties.UpdateBindings ();
    
    printf ("  string value is '%s'\n", properties.GetString ("String"));
    
    printf ("check PropertyMap::CommitBindings\n");
    
    properties.SetProperty ("Vector", math::vec4f (1.0f, 2.0f, 3.0f, 4.0f));
    
    properties.CommitBindings ();    
    
    math::vec3f v = a->VectorValue ();    
    
    printf ("  vector value is [%.2f %.2f %.2f]\n", v.x, v.y, v.z);
    
    printf ("destroy test\n");
    
    a.reset ();
    
    printf ("exit\n");
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }
}
#endif