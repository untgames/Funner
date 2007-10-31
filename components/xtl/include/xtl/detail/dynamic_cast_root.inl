/*
    Получение dynamic_cast_root
*/

inline dynamic_cast_root* get_root (dynamic_cast_root& root)
{
  return &root;
}

inline dynamic_cast_root* get_root (dynamic_cast_root* root)
{
  return root;
}

inline const dynamic_cast_root* get_root (const dynamic_cast_root& root)
{
  return &root;
}

inline const dynamic_cast_root* get_root (const dynamic_cast_root* root)
{
  return root;
}
