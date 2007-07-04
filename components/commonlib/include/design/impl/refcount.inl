/*
    Конструкторы
*/

inline reference_counter::reference_counter ()
  : ref_count (1)
  { }

inline reference_counter::reference_counter (size_t init_ref_count)
  : ref_count (init_ref_count)
  { }

inline reference_counter::reference_counter (const reference_counter&)
  : ref_count (1)
  { }

/*
    Количество ссылок на объект
*/

inline size_t reference_counter::use_count () const
{
  return ref_count;
}

/*
    Увеличение / уменьшение числа ссылок
*/

inline void reference_counter::addref ()
{
  ref_count++;

  if (ref_count == 1)
    on_restore_references ();
}

inline void reference_counter::release ()
{
  if (!ref_count || !--ref_count)
    on_lost_references ();
}

/*
    Реализация событий (поведение по умолчанию)
*/

inline void reference_counter::on_lost_references ()
{
  delete this;
}

/*
    Функции позволяющие использовать reference_counter* совместно с intrusive_ptr    
*/

inline void intrusive_ptr_addref (reference_counter* rc)
{
  rc->addref ();
}

inline void intrusive_ptr_release (reference_counter* rc)
{
  rc->release ();
}
