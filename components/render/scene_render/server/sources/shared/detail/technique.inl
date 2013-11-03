/*
    Context
*/

inline Context::Context (manager::Frame& in_frame)
  : frame (in_frame)
{
}

/*
    TechniquePrivateData
*/

inline TechniquePrivateData::TechniquePrivateData ()
{
  memset (buffer, 0, sizeof (buffer));
}

template <class T> inline T& TechniquePrivateData::Get ()
{
  if (sizeof (T) > sizeof (data))
    RaiseSizeError (typeid (T), sizeof (T));

  return *reinterpret_cast<T*> (data);
}

template <class T> inline const T& TechniquePrivateData::Get () const
{
  return const_cast<TechniquePrivateData&> (*this).template Get<T> ();
}
