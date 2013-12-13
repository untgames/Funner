/*
    PropertySelector
*/

///Конструктор по умолчанию
inline PropertySelector::PropertySelector ()
  : index (~0u)
  , name (0)
{
}

///Конструктор по индексу
inline PropertySelector::PropertySelector (size_t in_index)
  : index (in_index)
  , name (0)
{
}

///Конструктор по имени
inline PropertySelector::PropertySelector (const char* in_name)
  : index (~0u)
  , name (in_name)
{
}

///Наличие имени
inline bool PropertySelector::HasName () const
{
  return name != 0;
}

///Наличие индекса
inline bool PropertySelector::HasIndex () const
{
  return index != ~0u;
}

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

template <class Intermediate, class T, unsigned int Size>
inline void copy_vector_from_helper (const PropertyMap& map, size_t property_index, math::vector<T, Size>& value)
{
  Intermediate src;
  
  map.GetProperty (property_index, src);

  value = src;
}

template <class T, unsigned int Size>
inline void copy_vector_from (const PropertyMap& map, size_t property_index, math::vector<T, Size>& value)
{
  copy_vector_from_helper<math::vector<float, 4> > (map, property_index, value);
}

inline void copy_vector_from (const PropertyMap& map, size_t property_index, math::vector<float, 4>& value)
{
  map.GetProperty (property_index, value);
}

template <class Intermediate, class T, unsigned int Size>
inline void copy_matrix_from_helper (const PropertyMap& map, size_t property_index, math::matrix<T, Size>& value)
{
  Intermediate src;
  
  map.GetProperty (property_index, src);

  value = src;
}

template <class T, unsigned int Size>
inline void copy_matrix_from (const PropertyMap& map, size_t property_index, math::matrix<T, Size>& value)
{
  copy_matrix_from_helper<math::matrix<float, 4> > (map, property_index, value);
}

inline void copy_matrix_from (const PropertyMap& map, size_t property_index, math::matrix<float, 4>& value)
{
  map.GetProperty (property_index, value);
}

}

inline void copy_from (const PropertyMap& map, size_t property_index, stl::string& value)
{
  map.GetProperty (property_index, value);
}

inline void copy_from (const PropertyMap& map, size_t property_index, const char*& value)
{
  value = map.GetString (property_index);
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

inline void copy_from (const PropertyMap& map, size_t property_index, bool& value)
{
  value = map.GetInteger (property_index) != 0;
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

template <class T>
inline void copy_from (const PropertyMap& map, size_t property_index, math::angle<T>& value)
{
  struct Value
  {
    T value;
    
    Value () : value () {}
  };

  Value tmp;
  
  copy_from (map, property_index, tmp.value);
  
  value = math::degree (tmp.value);
}

/*
    Установка свойств по селектору
*/

template <class T>
inline void set_property (PropertyMap& map, const PropertySelector& selector, const T& value)
{
  if      (selector.HasIndex ()) map.SetProperty (selector.index, value);
  else if (selector.HasName ())  map.SetProperty (selector.name, value);
}

/*
    Copy from value to property map
*/

namespace detail
{

template <class CastT, class T>
inline void copy_value_to (const T& value, PropertyMap& map, const PropertySelector& property_selector)
{
  set_property (map, property_selector, static_cast<CastT> (value));
}

}

inline void copy_to (const char* value, PropertyMap& map, const PropertySelector& property_selector)
{
  set_property (map, property_selector, value);
}

inline void copy_to (signed char value, PropertyMap& map, const PropertySelector& property_selector)
{
  detail::copy_value_to<int> (value, map, property_selector);
}

inline void copy_to (unsigned char value, PropertyMap& map, const PropertySelector& property_selector)
{
  detail::copy_value_to<int> (value, map, property_selector);
}

inline void copy_to (short value, PropertyMap& map, const PropertySelector& property_selector)
{
  detail::copy_value_to<int> (value, map, property_selector);
}

inline void copy_to (unsigned short value, PropertyMap& map, const PropertySelector& property_selector)
{
  detail::copy_value_to<int> (value, map, property_selector);
}

inline void copy_to (int value, PropertyMap& map, const PropertySelector& property_selector)
{
  detail::copy_value_to<int> (value, map, property_selector);
}

inline void copy_to (unsigned int value, PropertyMap& map, const PropertySelector& property_selector)
{
  detail::copy_value_to<int> (value, map, property_selector);
}

inline void copy_to (long value, PropertyMap& map, const PropertySelector& property_selector)
{
  detail::copy_value_to<int> (value, map, property_selector);
}

inline void copy_to (unsigned long value, PropertyMap& map, const PropertySelector& property_selector)
{
  detail::copy_value_to<int> (value, map, property_selector);
}

inline void copy_to (bool value, PropertyMap& map, const PropertySelector& property_selector)
{
  detail::copy_value_to<int> (value, map, property_selector);
}

inline void copy_to (float value, PropertyMap& map, const PropertySelector& property_selector)
{
  detail::copy_value_to<float> (value, map, property_selector);
}

inline void copy_to (double value, PropertyMap& map, const PropertySelector& property_selector)
{
  detail::copy_value_to<float> (value, map, property_selector);
}

inline void copy_to (long double value, PropertyMap& map, const PropertySelector& property_selector)
{
  detail::copy_value_to<float> (value, map, property_selector);
}

template <class T>
inline void copy_to (const math::vector<T, 2>& value, PropertyMap& map, const PropertySelector& property_selector)
{
  detail::copy_value_to<math::vector<float, 4> > (value, map, property_selector);
}

template <class T>
inline void copy_to (const math::vector<T, 3>& value, PropertyMap& map, const PropertySelector& property_selector)
{
  detail::copy_value_to<math::vector<float, 4> > (value, map, property_selector);
}

template <class T>
inline void copy_to (const math::vector<T, 4>& value, PropertyMap& map, const PropertySelector& property_selector)
{
  detail::copy_value_to<math::vector<float, 4> > (value, map, property_selector);
}

template <class T>
inline void copy_to (const math::matrix<T, 2>& value, PropertyMap& map, const PropertySelector& property_selector)
{
  detail::copy_value_to<math::matrix<float, 4> > (value, map, property_selector);
}

template <class T>
inline void copy_to (const math::matrix<T, 3>& value, PropertyMap& map, const PropertySelector& property_selector)
{
  detail::copy_value_to<math::matrix<float, 4> > (value, map, property_selector);
}

template <class T>
inline void copy_to (const math::matrix<T, 4>& value, PropertyMap& map, const PropertySelector& property_selector)
{
  detail::copy_value_to<math::matrix<float, 4> > (value, map, property_selector);
}

template <class Intermediate, class T>
inline void copy_to_helper (const math::quat<T>& value, PropertyMap& map, const PropertySelector& property_selector)
{
  Intermediate v = reinterpret_cast<const math::vector<T, 4>&> (value);

  set_property (map, property_selector, v);
}

template <class T>
inline void copy_to (const math::quat<T>& value, PropertyMap& map, const PropertySelector& property_selector)
{
  copy_to_helper<math::vector<float, 4> > (value, map, property_selector);
}

inline void copy_to (const math::quat<float>& value, PropertyMap& map, const PropertySelector& property_selector)
{
  set_property (map, property_selector, reinterpret_cast<const math::vector<float, 4>&> (value));
}

template <class T>
inline void copy_to (const math::angle<T>& value, PropertyMap& map, const PropertySelector& property_selector)
{
  copy_to (degree (value), map, property_selector);
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
    
    virtual void CopyTo (common::PropertyMap& map, const PropertySelector& selector) = 0;
};

template <class T> struct SetterValueType             { typedef T Type; };
template <class T> struct SetterValueType<T&>         { typedef typename SetterValueType<T>::Type Type; };
template <class T> struct SetterValueType<const T>    { typedef typename SetterValueType<T>::Type Type; };
template <class T> struct SetterValueType<volatile T> { typedef typename SetterValueType<T>::Type Type; };

template <class Fn>            struct SetterArgType             { typedef typename SetterValueType<typename Fn::arg1_type>::Type Type; };
template <class Ret, class T>  struct SetterArgType<Ret (*)(T)> { typedef T Type; };

template <class Ret, class FRet, class A1, class A2, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct SetterArgType<xtl::detail::binder<Ret, FRet (*)(A1, A2), T1, T2, T3, T4, T5, T6, T7, T8, T9> >
{
  typedef typename SetterValueType<A2>::Type Type;
};

template <class Ret, class C, class FRet, class A1, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct SetterArgType<xtl::detail::binder<Ret, FRet (C::*)(A1), T1, T2, T3, T4, T5, T6, T7, T8, T9> >
{
  typedef typename SetterValueType<A1>::Type Type;
};

template <class Ret, class C, class FRet, class A1, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct SetterArgType<xtl::detail::binder<Ret, FRet (C::*)(A1) const, T1, T2, T3, T4, T5, T6, T7, T8, T9> >
{
  typedef typename SetterValueType<A1>::Type Type;
};

template <class Ret, class C, class FRet, class A1, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct SetterArgType<xtl::detail::binder<Ret, FRet (C::*)(A1) volatile, T1, T2, T3, T4, T5, T6, T7, T8, T9> >
{
  typedef typename SetterValueType<A1>::Type Type;
};

template <class Ret, class C, class FRet, class A1, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct SetterArgType<xtl::detail::binder<Ret, FRet (C::*)(A1) const volatile, T1, T2, T3, T4, T5, T6, T7, T8, T9> >
{
  typedef typename SetterValueType<A1>::Type Type;
};

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
  
    void CopyTo (common::PropertyMap& map, const PropertySelector& selector)
    {
      copy_to (fn (), map, selector);
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
