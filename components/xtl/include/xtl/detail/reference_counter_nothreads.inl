/*
    Увеличение / уменьшение количества ссылок
*/

inline void reference_counter::increment ()
{
  if (!counter)
    return;
    
  ++counter;
}

inline bool reference_counter::decrement ()
{
  if (!counter)
    return false;

  return --counter == 0;
}
