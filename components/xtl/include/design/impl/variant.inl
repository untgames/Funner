/*
    bad_variant_cast
*/

inline bad_variant_cast::bad_variant_cast (const std::type_info& _source,const std::type_info& _target)
  : source (_source), target (_target)
  { }
  
inline const std::type_info& bad_variant_cast::source_type () const
{
  return source;
}

inline const std::type_info& bad_variant_cast::target_type () const
{
  return target;
}

/*
    bad_variant_assign
*/

inline bad_variant_assign::bad_variant_assign (const std::type_info& _left,const std::type_info& _right)
  : left (_left), right (_right)
  { }
  
inline const std::type_info& bad_variant_assign::left_type () const
{
  return left;
}

inline const std::type_info& bad_variant_assign::right_type () const
{
  return right;
}

/*
    variant_cast
*/

template <class TargetType,class SourceType>
inline TargetType custom_type_caster<TargetType,SourceType>::cast (const SourceType&)
{
  throw bad_variant_cast (typeid (const SourceType),typeid (TargetType));  
}

template <class TargetType,class SourceType>
inline TargetType variant_cast_internal (const SourceType& value,TargetType*,stl::true_type)
{
  return static_cast<TargetType> (value);
}

template <class TargetType,class SourceType>
inline TargetType variant_cast_internal (const SourceType& value,TargetType*,stl::false_type)
{
  return custom_type_caster<TargetType,SourceType>::cast (value);
}

template <class TargetType,class SourceType>
inline TargetType variant_cast (const SourceType& value)
{
  return variant_cast_internal (value,(TargetType*)0,
         typename type_manip::select<type_manip::conversion_exists<SourceType,TargetType>::result,true_type,false_type>::result ());
}

template <class TargetType,class SourceType>
inline void variant_assign (const SourceType& value,TargetType& result)
{
  result = variant_cast<TargetType> (value);
}

template <class TargetType,class SourceType>
inline void variant_assign (const SourceType& value,const TargetType& result)
{
  throw bad_variant_assign (typeid (const SourceType),typeid (const TargetType));
}

/*
    variant_pointer_holder - используется для хранения указателей на базовый тип варианта
*/

template <class Pointer,class Base,bool Dynamic>
class variant_pointer_holder: public Base
{
  public:
    variant_pointer_holder (const Pointer& _ptr) : ptr (_ptr) { }

    template <class T> void set (const T& value) { variant_assign (value,*ptr); }
    template <class T> T    get () const         { return variant_cast<const T> (*ptr); }

  private:
    Pointer ptr;
};

template <class Pointer,class Base>
class variant_pointer_holder<Pointer,Base,true>: public Base
{
  public:
    variant_pointer_holder  (const Pointer& _ptr) : ptr (new Pointer (_ptr)) { }
    ~variant_pointer_holder () { delete ptr; }

    template <class T> void set (const T& value) { variant_assign (value,**ptr); }
    template <class T> T    get () const         { return variant_cast<const T> (**ptr); }

  private:
    Pointer* ptr;
};

/*
    variant_caster
*/

template <class Base,class T>
struct variant_caster_impl: public Base
{
  template <class T1> variant_caster_impl (const T1& arg) : Base (arg) { }

  void operator = (const T& value) { set (value); }
       operator T () const         { return get<T> (); }
};

/*
    variant_reference
*/

template <class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8> template <class Ptr>
inline variant_reference<T1,T2,T3,T4,T5,T6,T7,T8>::variant_reference (Ptr ptr)
{
  typedef variant_pointer_holder<Ptr,caster_type,false> static_holder_type;
  typedef variant_pointer_holder<Ptr,caster_type,sizeof (buffer) < sizeof (static_holder_type)> holder_type;
  typedef typename type_manip::generate_linear_hierarchy<type_list,variant_caster_impl,holder_type>::result object_caster_type;

  caster = new object_caster_type (ptr);
}

template <class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
inline variant_reference<T1,T2,T3,T4,T5,T6,T7,T8>::~variant_reference ()
{
  delete caster;
}

template <class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8> template <class T>
inline variant_reference<T1,T2,T3,T4,T5,T6,T7,T8>& variant_reference<T1,T2,T3,T4,T5,T6,T7,T8>::operator = (const T& value)
{
  *caster = value;
  return *this;
}

template <class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8> template <class T>
inline variant_reference<T1,T2,T3,T4,T5,T6,T7,T8>::operator T () const
{  
  return static_cast<T> (*caster);
}
