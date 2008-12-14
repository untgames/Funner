/*
    Увеличение / уменьшение количества ссылок
*/

inline void reference_counter::increment ()
{
  atomic_conditional_increment ((volatile int&)counter);
}

inline bool reference_counter::decrement ()
{
  return atomic_conditional_decrement ((volatile int&)counter) == 1;
}
