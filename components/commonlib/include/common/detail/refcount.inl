/*
    Конструкторы
*/

inline ReferenceCounter::ReferenceCounter ()
  : ref_count (1)
  { }

inline ReferenceCounter::ReferenceCounter (size_t init_ref_count)
  : ref_count (init_ref_count)
  { }

inline ReferenceCounter::ReferenceCounter (const ReferenceCounter&)
  : ref_count (1)
  { }

/*
    Количество ссылок на объект
*/

inline size_t ReferenceCounter::UseCount () const
{
  return ref_count;
}

/*
    Увеличение / уменьшене числа ссылок
*/

inline void ReferenceCounter::AddRef ()
{
  ref_count++;

  if (ref_count == 1)
    OnRestoreReferences ();
}

inline void ReferenceCounter::Release ()
{
  if (!ref_count)
    throw stl::logic_error ("attempt to release reference-count object with no active references");

  if (!--ref_count)
    OnLostReferences ();
}

/*
    Реализация событий (поведение по умолчанию)
*/

inline void ReferenceCounter::OnLostReferences ()
{
  delete this;
}

inline void ReferenceCounter::OnRestoreReferences ()
{
  throw stl::logic_error ("attempt to restore reference-count object with no active references");
}
