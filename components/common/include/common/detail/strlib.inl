/*
    Хэширование
*/

inline size_t strhash (const stl::string& s,size_t init_hash)
{
  return strhash (s.c_str (),init_hash);
}

inline size_t strihash (const stl::string& s,size_t init_hash)
{
  return strihash (s.c_str (),init_hash);
}
