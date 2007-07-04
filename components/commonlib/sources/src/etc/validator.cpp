#include <common/etc.h>
#include <common/strlib.h>
#include <stl/algorithm>
#include <stl/string>
#include <stl/vector>
#include "validator.h"

using namespace common;
using namespace stl;
using namespace math;

/*
    Селекторы значений переменной
*/

template <class T> struct VarSelectTraits        { typedef const T& return_type; };
template <>        struct VarSelectTraits<int>   { typedef int      return_type; };
template <>        struct VarSelectTraits<float> { typedef float    return_type; };

inline VarSelectTraits<int>::return_type   _select (Var& var,int*)   { return var.geti  (); }
inline VarSelectTraits<float>::return_type _select (Var& var,float*) { return var.getf  (); }
inline VarSelectTraits<vec2i>::return_type _select (Var& var,vec2i*) { return var.get2i (); }
inline VarSelectTraits<vec3i>::return_type _select (Var& var,vec3i*) { return var.get3i (); }
inline VarSelectTraits<vec4i>::return_type _select (Var& var,vec4i*) { return var.get4i (); }
inline VarSelectTraits<vec2f>::return_type _select (Var& var,vec2f*) { return var.get2f (); }
inline VarSelectTraits<vec3f>::return_type _select (Var& var,vec3f*) { return var.get3f (); }
inline VarSelectTraits<vec4f>::return_type _select (Var& var,vec4f*) { return var.get4f (); }

template <class T> typename VarSelectTraits<T>::return_type select (Var& var)
{
  return _select (var,(T*)0);
}

/*
    Валидатор диапозона значений переменной
*/

template <class T>
class VarRangeValidator: public VarValidator
{
  public:
    VarRangeValidator (const T& min,const T& max) : min_value (min), max_value (max) {}

    void validate (Var& var) {      
      typename VarSelectTraits<T>::return_type value = select<T> (var);
      
      T new_value = min (max_value,max (min_value,value));

      if (value != new_value)
        var.set (new_value);
    }

  private:
    T min_value, max_value;
};

/*
    Валидатор строковых значений
*/

class VarStringValidator: public VarValidator
{
  public:
    VarStringValidator (size_t _count,const char** values)
    {
      count = _count;
      
      size_t* hashes = (size_t*)data;
      
      for (size_t i=0;i<count;i++)
        hashes [i] = strihash (values [i]);
      
      strcpy (defvalue (),*values);
      
      sort (hashes,hashes+count);
    }
    
    VarStringValidator (const vector<string>& values)
    {
      count = values.size ();
      
      size_t* hashes = (size_t*)data;
      
      for (size_t i=0;i<count;i++)
        hashes [i] = strihash (values [i].c_str ());
      
      strcpy (defvalue (),values [0].c_str ());
      
      sort (hashes,hashes+count);
    }    
    
    void validate (Var& var) {
      size_t*               hashes = (size_t*)data;      
      pair<size_t*,size_t*> pos    = equal_range (hashes,hashes+count,strihash (var.gets ()));

      if (pos.first == pos.second)
        var.set (defvalue ());
    }

    static VarStringValidator* create (size_t count,const char** values)
    {
      if (!count)
        return NULL;
      
      void* buf = ::operator new (sizeof (VarStringValidator)+count*sizeof (size_t)+strlen (*values)-1);
      
      return new (buf) VarStringValidator (count,values);
    }
    
    static VarStringValidator* create (const vector<string>& values)
    {
      if (!values.size ())
        return NULL;
      
      void* buf = ::operator new (sizeof (VarStringValidator)+values.size ()*sizeof (size_t)+values [0].size ()-1);
      
      return new (buf) VarStringValidator (values);
    }
    
  private:    
    char* defvalue () { return data+count*sizeof (size_t); }
  
  private:
    size_t count;
    char   data [1];
};

/*
    VarValidator
*/

VarValidator::VarValidator ()
  : ref_count (1)
  { }

void VarValidator::release ()
{
  if (!--ref_count)
    delete this;
}

/*
    ValidatorFactory
*/

VarValidatorFactory::~VarValidatorFactory ()
{
}

VarValidator* VarValidatorFactory::create (int min,int max)
{
  return new VarRangeValidator<int> (min,max);
}

VarValidator* VarValidatorFactory::create (float min,float max)
{
  return new VarRangeValidator<float> (min,max);
}

VarValidator* VarValidatorFactory::create (const vec2i& min,const vec2i& max)
{
  return new VarRangeValidator<vec2i> (min,max);
}

VarValidator* VarValidatorFactory::create (const vec3i& min,const vec3i& max)
{
  return new VarRangeValidator<vec3i> (min,max);
}

VarValidator* VarValidatorFactory::create (const vec4i& min,const vec4i& max)
{
  return new VarRangeValidator<vec4i> (min,max);
}

VarValidator* VarValidatorFactory::create (const vec2f& min,const vec2f& max)
{
  return new VarRangeValidator<vec2f> (min,max);
}

VarValidator* VarValidatorFactory::create (const vec3f& min,const vec3f& max)
{
  return new VarRangeValidator<vec3f> (min,max);
}

VarValidator* VarValidatorFactory::create (const vec4f& min,const vec4f& max)
{
  return new VarRangeValidator<vec4f> (min,max);
}

VarValidator* VarValidatorFactory::create (size_t count,const char** values)
{
  return VarStringValidator::create (count,values);
}

VarValidator* VarValidatorFactory::create (const vector<string>& values)
{
  return VarStringValidator::create (values);
}
