namespace detail
{

/*
    Функции получения свойств различных типов
*/

template <class Int>
inline void get_int_property (const PropertyMap& map, size_t property_index, Int& result)
{
  result = static_cast<Int> (map.GetInteger (property_index));
}

template <class Float>
inline void get_float_property (const PropertyMap& map, size_t property_index, Float& result)
{
  result = static_cast<Float> (map.GetFloat (property_index));
}

inline void get_property (const PropertyMap& map, size_t property_index, const char*& result)
{
  result = map.GetString (property_index);
}

inline void get_property (const PropertyMap& map, size_t property_index, stl::string& result)
{
  result = map.GetString (property_index);
}

inline void get_property (const PropertyMap& map, size_t property_index, unsigned char& result)
{
  get_int_property<unsigned char> (map, property_index, result);
}

inline void get_property (const PropertyMap& map, size_t property_index, signed char& result)
{
  get_int_property<signed char> (map, property_index, result);
}

inline void get_property (const PropertyMap& map, size_t property_index, unsigned short& result)
{
  get_int_property<unsigned short> (map, property_index, result);
}

inline void get_property (const PropertyMap& map, size_t property_index, signed short& result)
{
  get_int_property<signed short> (map, property_index, result);
}

inline void get_property (const PropertyMap& map, size_t property_index, unsigned int& result)
{
  get_int_property<unsigned int> (map, property_index, result);
}

inline void get_property (const PropertyMap& map, size_t property_index, signed int& result)
{
  get_int_property<signed int> (map, property_index, result);
}

inline void get_property (const PropertyMap& map, size_t property_index, unsigned long& result)
{
  get_int_property<unsigned long> (map, property_index, result);
}

inline void get_property (const PropertyMap& map, size_t property_index, signed long& result)
{
  get_int_property<signed long> (map, property_index, result);
}

inline void get_property (const PropertyMap& map, size_t property_index, float& result)
{
  get_float_property<float> (map, property_index, result);
}

inline void get_property (const PropertyMap& map, size_t property_index, double& result)
{
  get_float_property<double> (map, property_index, result);
}

inline void get_property (const PropertyMap& map, size_t property_index, long double& result)
{
  get_float_property<long double> (map, property_index, result);
}

template <class T, unsigned int Size>
inline void get_property (const PropertyMap& map, size_t property_index, math::vector<T, Size>& result)
{
  result = map.GetVector (property_index);
}

template <class T, unsigned int Size>
inline void get_property (const PropertyMap& map, size_t property_index, math::matrix<T, Size>& result)
{
  math::mat4f tmp;
  
  map.GetProperty (property_index, tmp);

  for (unsigned int i=0; i<(4 < Size ? 4 : Size); i++)
    result [i] = tmp [i];

  for (unsigned int i=4; i<Size; i++)
    result [i] = math::vector<T, Size> ();      
}

/*
    Функции установки свойств различных типов
*/

template <class T1, class Key, class T2>
inline void set_casted_property (PropertyMap& map, Key property_key, const T2& value)
{
  map.SetProperty (property_key, static_cast<T1> (value));
}

template <class Key>
inline void set_property (PropertyMap& map, Key property_key, const char* value)
{
  map.SetProperty (property_key, value);
}

template <class Key>
inline void set_property (PropertyMap& map, Key property_key, const stl::string& value)
{
  map.SetProperty (property_key, value.c_str ());
}

template <class Key>
inline void set_property (PropertyMap& map, Key property_key, unsigned char value)
{
  set_casted_property<int> (map, property_key, value);
}

template <class Key>
inline void set_property (PropertyMap& map, Key property_key, signed char value)
{
  set_casted_property<int> (map, property_key, value);
}

template <class Key>
inline void set_property (PropertyMap& map, Key property_key, unsigned short value)
{
  set_casted_property<int> (map, property_key, value);
}

template <class Key>
inline void set_property (PropertyMap& map, Key property_key, signed short value)
{
  set_casted_property<int> (map, property_key, value);
}

template <class Key>
inline void set_property (PropertyMap& map, Key property_key, unsigned int value)
{
  set_casted_property<int> (map, property_key, value);
}

template <class Key>
inline void set_property (PropertyMap& map, Key property_key, signed int value)
{
  set_casted_property<int> (map, property_key, value);
}

template <class Key>
inline void set_property (PropertyMap& map, Key property_key, unsigned long value)
{
  set_casted_property<int> (map, property_key, value);
}

template <class Key>
inline void set_property (PropertyMap& map, Key property_key, signed long value)
{
  set_casted_property<int> (map, property_key, value);
}

template <class Key>
inline void set_property (PropertyMap& map, Key property_key, float value)
{
  set_casted_property<float> (map, property_key, value);
}

template <class Key>
inline void set_property (PropertyMap& map, Key property_key, double value)
{
  set_casted_property<float> (map, property_key, value);
}

template <class Key>
inline void set_property (PropertyMap& map, Key property_key, long double value)
{
  set_casted_property<float> (map, property_key, value);
}

template <class Key, class T, unsigned int Size>
inline void set_property (PropertyMap& map, Key property_key, const math::vector<T, Size>& value)
{
  math::vec4f tmp (value);

  map.SetProperty (property_key, tmp);
}

template <class Key, class T, unsigned int Size>
inline void set_property (PropertyMap& map, Key property_key, const math::matrix<T, Size>& value)
{
  math::mat4f tmp;
  
  for (unsigned int i=0; i< (4 < Size ? 4 : Size); i++)
    tmp [i] = value [i];

  map.SetProperty (property_key, tmp);
}

/*
    Обёртки для связывания свойств
*/

//вспомогательный класс для избежания предупреждений об отсутствии инициализации переменной
template <class Value> struct ResultHolder
{
  Value value;

  ResultHolder () {}    
};

template <class Value> struct ResultHolder<Value&>:               public ResultHolder<Value> {};
template <class Value> struct ResultHolder<const Value>:          public ResultHolder<Value> {};
template <class Value> struct ResultHolder<volatile Value>:       public ResultHolder<Value> {};
template <class Value> struct ResultHolder<const volatile Value>: public ResultHolder<Value> {};

//реализация обёртки над функтором установки свойства
template <class Value, class Fn> class PropertyFunctionSetter: public IPropertySetter
{
  public:
    PropertyFunctionSetter (const Fn& in_fn) : fn (in_fn) {}     
    
    void Update (const PropertyMap& map, size_t property_index)
    {
      ResultHolder<Value> result;
    
      get_property (map, property_index, result.value);
      
      fn (result.value);
    }

  private:
    Fn fn;
};

//реализация обёртки над функтором получения свойства
template <class Fn> class PropertyFunctionGetter: public IPropertyGetter
{
  public:
    PropertyFunctionGetter (const Fn& in_fn) : fn (in_fn) {}     
    
    void Update (PropertyMap& map, size_t property_index)
    {
      set_property (map, property_index, fn ());
    }
    
    void Update (PropertyMap& map, const char* property_name)
    {
      set_property (map, property_name, fn ());
    }    

  private:
    Fn fn;
};

//реализация обёртки над методом установки свойства
template <class Value, class ObjectPtr, class Fn> class PropertyMethodSetter: public IPropertySetter
{
  public:
    PropertyMethodSetter (const ObjectPtr& in_ptr, const Fn& in_fn) : ptr (in_ptr), fn (in_fn) {}

    void Update (const PropertyMap& map, size_t property_index)
    {
      ResultHolder<Value> result;

      get_property (map, property_index, result.value);

      ((*ptr).*fn)(result.value);
    }

  private:
    ObjectPtr ptr;
    Fn        fn;
};

//реализация обёртки над методом получения свойства
template <class ObjectPtr, class Fn> class PropertyMethodGetter: public IPropertyGetter
{
  public:
    PropertyMethodGetter (const ObjectPtr& in_ptr, const Fn& in_fn) : ptr (in_ptr), fn (in_fn) {}

    void Update (PropertyMap& map, size_t property_index)
    {
      set_property (map, property_index, ((*ptr).*fn)());
    }
    
    void Update (PropertyMap& map, const char* property_name)
    {
      set_property (map, property_name, ((*ptr).*fn)());
    }    

  private:
    ObjectPtr ptr;
    Fn        fn;
};

}

template <class Value>
void bind (PropertyMap& map, const char* property_name, xtl::function<void (Value)>& set_fn)
{
  map.BindProperty (property_name, new detail::PropertyFunctionSetter<Value, xtl::function<void (Value)> > (set_fn));
}

template <class Value>
void bind (PropertyMap& map, const char* property_name, xtl::function<Value ()>& get_fn)
{
  map.BindProperty (property_name, new detail::PropertyFunctionGetter<xtl::function<Value ()> > (get_fn));
}

template <class Value>
void bind (PropertyMap& map, const char* property_name, xtl::function<void (Value)>& set_fn, xtl::function<Value ()>& get_fn)
{
  bind (map, property_name, set_fn);

  try
  {
    bind (map, property_name, get_fn);
  }
  catch (...)
  {
    map.UnbindProperty (property_name);
    throw;
  }
}

template <class ObjectPtr, class ObjectType, class Value>
void bind (PropertyMap& map, const char* property_name, const ObjectPtr& ptr, Value (ObjectType::*fn)())
{
  map.BindProperty (property_name, new detail::PropertyMethodGetter<ObjectPtr, Value (ObjectType::*)()> (ptr, fn));
}

template <class ObjectPtr, class ObjectType, class Value>
void bind (PropertyMap& map, const char* property_name, const ObjectPtr& ptr, Value (ObjectType::*fn)() const)
{
  map.BindProperty (property_name, new detail::PropertyMethodGetter<ObjectPtr, Value (ObjectType::*)() const> (ptr, fn));
}

template <class ObjectPtr, class ObjectType, class Value>
void bind (PropertyMap& map, const char* property_name, ObjectPtr& ptr, void (ObjectType::*fn)(Value))
{
  map.BindProperty (property_name, new detail::PropertyMethodSetter<Value, ObjectPtr, void (ObjectType::*)(Value)> (ptr, fn));
}

template <class ObjectPtr, class ObjectType, class Value>
void bind (PropertyMap& map, const char* property_name, ObjectPtr& ptr, void (ObjectType::*set_fn)(Value), Value (ObjectType::*get_fn)() const)
{
  bind (map, property_name, ptr, set_fn);
  
  try
  {
    bind (map, property_name, ptr, get_fn);
  }
  catch (...)
  {
    map.UnbindProperty (property_name);
    throw;
  }
}

template <class ObjectPtr, class ObjectType, class Value>
void bind (PropertyMap& map, const char* property_name, ObjectPtr& ptr, void (ObjectType::*set_fn)(Value), Value (ObjectType::*get_fn)())
{
  bind (map, property_name, ptr, set_fn);
  
  try
  {
    bind (map, property_name, ptr, get_fn);
  }
  catch (...)
  {
    map.UnbindProperty (property_name);
    throw;
  }
}
