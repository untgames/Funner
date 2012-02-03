/*
    Copy from property map to value
*/

namespace detail
{

template <class T>
inline void copy_integer_from (const PropertyMap& map, size_t property_index, T& value)
{
  value = static_cast<T> (map.GetInteger (property_index));
}

template <class T>
inline void copy_float_from (const PropertyMap& map, size_t property_index, T& value)
{
  value = static_cast<T> (map.GetFloat (property_index));
}

template <class T, unsigned int Size>
inline void copy_vector_from (const PropertyMap& map, size_t property_index, math::vector<T, Size>& value)
{
  math::vector<float, 4> src;
  
  map.GetProperty (property_index, src);

  value = src;
}

inline void copy_vector_from (const PropertyMap& map, size_t property_index, math::vector<float, 4>& value)
{
  map.GetProperty (property_index, value);
}

template <class T, unsigned int Size>
inline void copy_matrix_from (const PropertyMap& map, size_t property_index, math::matrix<T, Size>& value)
{
  math::matrix<float, 4> src;
  
  map.GetProperty (property_index, src);

  value = src;
}

inline void copy_matrix_from (const PropertyMap& map, size_t property_index, math::matrix<float, 4>& value)
{
  map.GetProperty (property_index, value);
}

}

inline void copy_from (const PropertyMap& map, size_t property_index, signed char& value)
{
  detail::copy_integer_from (map, property_index, value);
}

inline void copy_from (const PropertyMap& map, size_t property_index, unsigned char& value)
{
  detail::copy_integer_from (map, property_index, value);
}

inline void copy_from (const PropertyMap& map, size_t property_index, short& value)
{
  detail::copy_integer_from (map, property_index, value);
}

inline void copy_from (const PropertyMap& map, size_t property_index, unsigned short& value)
{
  detail::copy_integer_from (map, property_index, value);
}

inline void copy_from (const PropertyMap& map, size_t property_index, int& value)
{
  detail::copy_integer_from (map, property_index, value);
}

inline void copy_from (const PropertyMap& map, size_t property_index, unsigned int& value)
{
  detail::copy_integer_from (map, property_index, value);
}

inline void copy_from (const PropertyMap& map, size_t property_index, long& value)
{
  detail::copy_integer_from (map, property_index, value);
}

inline void copy_from (const PropertyMap& map, size_t property_index, unsigned long& value)
{
  detail::copy_integer_from (map, property_index, value);
}

inline void copy_from (const PropertyMap& map, size_t property_index, float& value)
{
  detail::copy_float_from (map, property_index, value);
}

inline void copy_from (const PropertyMap& map, size_t property_index, double& value)
{
  detail::copy_float_from (map, property_index, value);
}

inline void copy_from (const PropertyMap& map, size_t property_index, long double& value)
{
  detail::copy_float_from (map, property_index, value);
}

template <class T>
inline void copy_from (const PropertyMap& map, size_t property_index, math::vector<T, 2>& value)
{
  detail::copy_vector_from (map, property_index, value);
}

template <class T>
inline void copy_from (const PropertyMap& map, size_t property_index, math::vector<T, 3>& value)
{
  detail::copy_vector_from (map, property_index, value);
}

template <class T>
inline void copy_from (const PropertyMap& map, size_t property_index, math::vector<T, 4>& value)
{
  detail::copy_vector_from (map, property_index, value);
}

template <class T>
inline void copy_from (const PropertyMap& map, size_t property_index, math::matrix<T, 2>& value)
{
  detail::copy_matrix_from (map, property_index, value);
}

template <class T>
inline void copy_from (const PropertyMap& map, size_t property_index, math::matrix<T, 3>& value)
{
  detail::copy_matrix_from (map, property_index, value);
}

template <class T>
inline void copy_from (const PropertyMap& map, size_t property_index, math::matrix<T, 4>& value)
{
  detail::copy_matrix_from (map, property_index, value);
}

template <class T>
inline void copy_from (const PropertyMap& map, size_t property_index, math::quat<T>& value)
{
  detail::copy_vector_from (map, property_index, reinterpret_cast<math::vector<T, 4>&> (value));
}

/*
    Copy from value to property map
*/

namespace detail
{

template <class CastT, class T>
inline void copy_value_to (const T& value, PropertyMap& map, size_t property_index)
{
  map.SetProperty (property_index, static_cast<CastT> (value));
}

}

inline void copy_to (signed char value, PropertyMap& map, size_t property_index)
{
  detail::copy_value_to<int> (value, map, property_index);
}

inline void copy_to (unsigned char value, PropertyMap& map, size_t property_index)
{
  detail::copy_value_to<int> (value, map, property_index);
}

inline void copy_to (short value, PropertyMap& map, size_t property_index)
{
  detail::copy_value_to<int> (value, map, property_index);
}

inline void copy_to (unsigned short value, PropertyMap& map, size_t property_index)
{
  detail::copy_value_to<int> (value, map, property_index);
}

inline void copy_to (int value, PropertyMap& map, size_t property_index)
{
  detail::copy_value_to<int> (value, map, property_index);
}

inline void copy_to (unsigned int value, PropertyMap& map, size_t property_index)
{
  detail::copy_value_to<int> (value, map, property_index);
}

inline void copy_to (long value, PropertyMap& map, size_t property_index)
{
  detail::copy_value_to<int> (value, map, property_index);
}

inline void copy_to (unsigned long value, PropertyMap& map, size_t property_index)
{
  detail::copy_value_to<int> (value, map, property_index);
}

inline void copy_to (float value, PropertyMap& map, size_t property_index)
{
  detail::copy_value_to<float> (value, map, property_index);
}

inline void copy_to (double value, PropertyMap& map, size_t property_index)
{
  detail::copy_value_to<float> (value, map, property_index);
}

inline void copy_to (long double value, PropertyMap& map, size_t property_index)
{
  detail::copy_value_to<float> (value, map, property_index);
}

template <class T>
inline void copy_to (const math::vector<T, 2>& value, PropertyMap& map, size_t property_index)
{
  detail::copy_value_to<math::vector<float, 4> > (value, map, property_index);
}

template <class T>
inline void copy_to (const math::vector<T, 3>& value, PropertyMap& map, size_t property_index)
{
  detail::copy_value_to<math::vector<float, 4> > (value, map, property_index);
}

template <class T>
inline void copy_to (const math::vector<T, 4>& value, PropertyMap& map, size_t property_index)
{
  detail::copy_value_to<math::vector<float, 4> > (value, map, property_index);
}

template <class T>
inline void copy_to (const math::matrix<T, 2>& value, PropertyMap& map, size_t property_index)
{
  detail::copy_value_to<math::matrix<float, 4> > (value, map, property_index);
}

template <class T>
inline void copy_to (const math::matrix<T, 3>& value, PropertyMap& map, size_t property_index)
{
  detail::copy_value_to<math::matrix<float, 4> > (value, map, property_index);
}

template <class T>
inline void copy_to (const math::matrix<T, 4>& value, PropertyMap& map, size_t property_index)
{
  detail::copy_value_to<math::matrix<float, 4> > (value, map, property_index);
}

template <class T>
inline void copy_to (const math::quat<T>& value, PropertyMap& map, size_t property_index)
{
  math::vector<float, 4> v = reinterpret_cast<const math::vector<T, 4>&> (value);

  map.SetProperty (property_index, v);
}

inline void copy_to (const math::quat<float>& value, PropertyMap& map, size_t property_index)
{
  map.SetProperty (property_index, reinterpret_cast<const math::vector<float, 4>&> (value));
}

namespace detail
{

/*
    PropertySetter / PropertyGetter
*/

class IPropertySetter
{
  public:
    virtual ~IPropertySetter () {}
    
    virtual void CopyFrom (const common::PropertyMap& map, size_t property_index) = 0;
};

class IPropertyGetter
{
  public:
    virtual ~IPropertyGetter () {}
    
    virtual void CopyTo (common::PropertyMap& map, size_t property_index) = 0;
};

template <class T> struct SetterValueType             { typedef T Type; };
template <class T> struct SetterValueType<T&>         { typedef typename SetterValueType<T>::Type Type; };
template <class T> struct SetterValueType<const T>    { typedef typename SetterValueType<T>::Type Type; };
template <class T> struct SetterValueType<volatile T> { typedef typename SetterValueType<T>::Type Type; };

template <class Fn>            struct SetterArgType             { typedef typename SetterValueType<typename Fn::arg1_type>::Type Type; };
template <class Ret, class T>  struct SetterArgType<Ret (*)(T)> { typedef T Type; };

template <class Fn>
class PropertySetter: public IPropertySetter
{
  public:
    PropertySetter (const Fn& in_fn) : fn (in_fn) {}

    const Fn& Function () { return fn; }

  private:
    Fn fn;
};

template <class Fn, class T>
class PropertyTypedSetter: public PropertySetter<Fn>
{
  struct Value
  {
    T value;
    
    Value () : value () {}
  };
  
  public:
    PropertyTypedSetter (const Fn& in_fn) : PropertySetter<Fn> (in_fn) {}
    
    using PropertySetter<Fn>::Function;

    void CopyFrom (const common::PropertyMap& map, size_t property_index)
    {
      Value value;

      copy_from (map, property_index, value.value);
      
      Function ()(value.value);
    }
};

template <class Fn>
class PropertyGetter: public IPropertyGetter
{
  public:
    PropertyGetter (const Fn& in_fn) : fn (in_fn) {}
    
    const Fn& Function () { return fn; }
  
    void CopyTo (common::PropertyMap& map, size_t property_index)
    {
      copy_to (fn (), map, property_index);
    }
    
  private:
    Fn fn;
};

}

/*
    PropertyBinding
*/

template <class Fn>
inline void PropertyBinding::SetSetter (const Fn& fn)
{
  SetSetterCore (new detail::PropertyTypedSetter<Fn, typename detail::SetterArgType<Fn>::Type> (fn));
}

template <class T, class Fn>
inline void PropertyBinding::SetSetter (const Fn& fn)
{
  SetSetterCore (new detail::PropertyTypedSetter<Fn, T> (fn));
}

template <class Fn>
inline void PropertyBinding::SetGetter (const Fn& fn)
{
  SetGetterCore (new detail::PropertyGetter<Fn> (fn));
}

template <class Fn>
inline const Fn* PropertyBinding::Setter () const
{
  detail::IPropertySetter* fn = SetterCore ();
  
  if (detail::PropertySetter<Fn>* casted_fn = dynamic_cast<detail::PropertySetter<Fn>*> (fn))
    return &casted_fn->Function ();
    
  return 0;
}

template <class Fn>
inline const Fn* PropertyBinding::Getter () const
{
  detail::IPropertyGetter* fn = GetterCore ();
  
  if (detail::PropertyGetter<Fn>* casted_fn = dynamic_cast<detail::PropertyGetter<Fn>*> (fn))
    return &casted_fn->Function ();
    
  return 0;
}

/*
    PropertyBindingMap
*/

template <class GetterFn>
inline size_t PropertyBindingMap::AddProperty (const char* name, const GetterFn& getter)
{
  common::PropertyBinding binding;
  
  binding.SetGetter (getter);
  
  return AddProperty (name, binding);
}

template <class GetterFn, class SetterFn>
inline size_t PropertyBindingMap::AddProperty (const char* name, const GetterFn& getter, const SetterFn& setter)
{
  common::PropertyBinding binding;

  binding.SetSetter (setter);
  binding.SetGetter (getter);

  return AddProperty (name, binding);
}

template <class Value, class GetterFn, class SetterFn>
inline size_t PropertyBindingMap::AddProperty (const char* name, const GetterFn& getter, const SetterFn& setter)
{
  common::PropertyBinding binding;

  binding.SetSetter<Value> (setter);
  binding.SetGetter (getter);

  return AddProperty (name, binding);
}
