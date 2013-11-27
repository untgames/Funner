struct TechniquePrivateData::ITypeInfo
{
  virtual ~ITypeInfo () {}
 
  virtual void destroy (void* data) = 0;
  virtual void clone   (const void* src, void* dst) = 0;
};

template <class T>
struct TechniquePrivateData::TypeInfoImpl: public ITypeInfo
{
  void destroy (void* data)
  {
    stl::destroy (reinterpret_cast<T*> (data));
  }

  void clone (const void* src, void* dst)
  {
    new (dst) T (*reinterpret_cast<const T*> (src));
  }

  static TypeInfoImpl instance;
};

template <class T> TechniquePrivateData::TypeInfoImpl<T> TechniquePrivateData::TypeInfoImpl<T>::instance;

inline TechniquePrivateData::TechniquePrivateData ()
  : type_info ()
{
  memset (buffer, 0, sizeof (buffer));
}

inline TechniquePrivateData::TechniquePrivateData (const TechniquePrivateData& data)
  : type_info (data.type_info)
{
  if (type_info)
    type_info->clone (data.buffer, buffer);
}

inline TechniquePrivateData::~TechniquePrivateData ()
{  
  if (type_info)
    type_info->destroy (buffer);
}

inline TechniquePrivateData& TechniquePrivateData::operator = (const TechniquePrivateData& data)
{
  if (type_info)
    type_info->destroy (buffer);

  type_info = 0;

  if (data.type_info)
  {
    data.type_info->clone (data.buffer, buffer);

    type_info = data.type_info;
  }

  return *this;
}

template <class T> inline T& TechniquePrivateData::Get ()
{
  static ITypeInfo* const this_type_info = &TypeInfoImpl<T>::instance;

  if (type_info == this_type_info)
    return *reinterpret_cast<T*> (buffer);

  if (sizeof (T) > sizeof (buffer))
    RaiseSizeError (typeid (T), sizeof (T));

  if (type_info)
  {
    type_info->destroy (buffer);

    type_info = 0;
  }

  T* result = new (buffer) T;

  type_info = this_type_info;

  return *result;
}

template <class T> inline const T& TechniquePrivateData::Get () const
{
  return const_cast<TechniquePrivateData&> (*this).template Get<T> ();
}
