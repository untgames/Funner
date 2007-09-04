/*
    Вспомогательные массивы
*/

template <bool dummy> struct bitset_arrays
{
  static unsigned char bit_count [256]; //количество битов в байте
  static unsigned char first_one [256]; //первый бит в байте
};

template <bool dummy> unsigned char bitset_arrays<dummy>::bit_count [] = {
  0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5, //0x00 - 0x1F
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6, //0x20 - 0x3F
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6, //0x40 - 0x5F
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7, //0x60 - 0x7F
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6, //0x80 - 0x9F
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7, //0xA0 - 0xBF
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7, //0xC0 - 0xDF
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8  //0xE0 - 0xFF
};

template <bool dummy> unsigned char bitset_arrays<dummy>::first_one [] = {
  0,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0, //0x00 - 0x1F
  5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0, //0x20 - 0x3F
  6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0, //0x40 - 0x5F
  5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0, //0x60 - 0x7F
  7,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0, //0x80 - 0x9F
  5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0, //0xA0 - 0xBF
  6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0, //0xC0 - 0xDF
  5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0  //0xE0 - 0xFF
};

/*
    Конструкторы
*/

template <size_t bits>
inline bitset<bits>::bitset ()
{
  reset ();  
}

template <size_t bits>
bitset<bits>::bitset (unsigned long x)
{
  reset ();
  
  const size_t count = min (sizeof (unsigned long) * CHAR_BIT,bits);
  
  for (size_t i=0;i<count;++i,x>>=1)
    if (x&0x1)
      get_word (i) |= get_mask (i);
}

template <size_t bits> template <class T,class Traits,class Allocator> 
inline bitset<bits>::bitset 
 (const basic_string<T,Traits,Allocator>&              s,
  typename basic_string<T,Traits,Allocator>::size_type pos)
{
  if (pos>s.size ()) 
    raise_out_of_range ("bitset");
      
  copy_from_string (s.begin()+pos,s.size ()-pos);
}

template <size_t bits> template <class T,class Traits,class Allocator>
inline bitset<bits>::bitset 
 (const basic_string<T,Traits,Allocator>&               s,
  typename basic_string<T,Traits,Allocator>::size_type  pos,
  typename basic_string<T,Traits,Allocator>::size_type  count,
  T                                                     e0)
{
  if (pos>s.size ())
    raise_length_error ("bitset");

  if (s.size () - pos < count)
    count = s.size ()-pos;    
    
  copy_from_string (s.begin()+pos,count-pos,e0);
}

template <size_t bits> template <class T>
inline bitset<bits>::bitset (const T* s,size_t pos,size_t count,T e0)
{
  size_t len = char_traits<T>::length (s);
  
  if (pos > len)
    raise_length_error ("bitset",pos); ////переделать!!!!

  if (len-pos < count)
    count = len-pos;
    
  copy_from_string (s+pos,count-pos,e0);
}

/*
    Вспомогательные функции
*/

template <size_t bits>
inline size_t bitset<bits>::which_word (size_t pos)
{
  return pos / BITS_PER_WORD;
}

template <size_t bits>
inline size_t bitset<bits>::which_byte (size_t pos)
{
  return (pos % BITS_PER_WORD) / CHAR_BIT;
}

template <size_t bits>
inline size_t bitset<bits>::which_bit (size_t pos)
{
  return pos % BITS_PER_WORD;
}

template <size_t bits>
inline typename bitset<bits>::word_type bitset<bits>::get_mask (size_t pos)
{
  return (word_type)(word_type (1) << which_bit (pos));
}

template <size_t bits>
inline typename bitset<bits>::word_type& bitset<bits>::get_word (size_t pos)
{
  return word [which_word (pos)];  
}

template <size_t bits>
inline typename bitset<bits>::word_type bitset<bits>::get_word (size_t pos) const
{
  return word [which_word (pos)];  
}

template <size_t bits>
void bitset<bits>::raise_out_of_range (const char* name) const
{
  fprintf (stderr,"out_of_range bitset::%s\n",name);
  abort ();
}

template <size_t bits>
void bitset<bits>::raise_length_error (const char* name,size_t size) const
{
  fprintf (stderr,"length_error bitset::%s length=%u\n",name,size);
  abort ();
}

template <size_t bits>
void bitset<bits>::raise_invalid_argument (const char* name) const
{
  fprintf (stderr,"invalid_argument bitset::%s\n",name);
  abort ();
}

template <size_t bits>
void bitset<bits>::raise_overflow_error (const char* name) const
{
  fprintf (stderr,"overflow bitset::%s\n",name);
  abort ();
}

/*
    Операции доступа
*/

template <size_t bits>
inline bool bitset<bits>::operator [] (size_t pos) const
{
  return unchecked_test (pos);
}

template <size_t bits>
inline typename bitset<bits>::reference bitset<bits>::operator [] (size_t pos)
{
  return reference (&get_word (pos),get_mask (which_bit (pos)));
}

/*
    Установка / сброс / инверсия бита
*/

template <size_t bits>
inline bitset<bits>& bitset<bits>::set (size_t pos)
{
  if (pos >= bits)
    raise_length_error ("set",pos);

  get_word (pos) |= get_mask (pos);
  
  return *this;
}

template <size_t bits>
inline bitset<bits>& bitset<bits>::reset (size_t pos)
{
  if (pos >= bits)
    raise_length_error ("reset",pos);

  get_word (pos) &= ~get_mask (pos);
  
  return *this;
}

template <size_t bits>
inline bitset<bits>& bitset<bits>::set (size_t pos,bool value)
{
  return value ? set (pos) : reset (pos);
}

template <size_t bits>
inline bitset<bits>& bitset<bits>::flip (size_t pos)
{
  if (pos >= bits)
    raise_length_error ("flip",pos);

  get_word (pos) ^= get_mask (pos);
  
  return *this;
}

template <size_t bits>
inline void bitset<bits>::trim ()
{
  if (bits % BITS_PER_WORD)
    word [WORDS-1] &= ~((~static_cast<word_type>(0)) << bits % BITS_PER_WORD);
}

template <size_t bits>
bitset<bits>& bitset<bits>::set ()
{
  for (size_t i=0;i<WORDS;i++)
    word [i] = ~word_type (0);
    
  trim ();
  
  return *this;
}

template <size_t bits>
bitset<bits>& bitset<bits>::reset ()
{
  for (size_t i=0;i<WORDS;i++)
    word [i] = word_type (0);
    
  trim ();

  return *this;    
}

template <size_t bits>
bitset<bits>& bitset<bits>::flip ()
{
  for (int i=0;i<WORDS;i++)
    word [i] = ~word [i];
    
  return *this;
}

/*
    Тестирование бита / подсчёт количества битов
*/    

template <size_t bits>
inline bool bitset<bits>::unchecked_test (size_t pos) const
{
  return (get_word (pos) & get_mask (pos)) != 0;
}

template <size_t bits>
inline bool bitset<bits>::test (size_t pos) const
{
  if (pos >= bits)
    raise_length_error ("test",pos);

  return unchecked_test (pos);
}

template <size_t bits>
bool bitset<bits>::any () const
{
  for (size_t i=0;i<WORDS;i++) 
    if (word [i])
      return true;

  return false;  
}

template <size_t bits>
inline bool bitset<bits>::none () const
{
  return !any ();
}

template <size_t bits>
size_t bitset<bits>::count () const
{
  size_t result = 0;
  
  const unsigned char *byte_ptr = (const unsigned char*)word, 
                      *end_ptr  = (const unsigned char*)(word+WORDS);

  while (byte_ptr!=end_ptr) 
    result += bitset_arrays<true>::bit_count [*byte_ptr++];

  return result;
}

/*
    Побитовые операции
*/

template <size_t bits>
bitset<bits>& bitset<bits>::operator &= (const bitset& x)
{
  for (int i=0;i<WORDS;i++)
      word [i] &= x.word [i];
      
  return *this;
}

template <size_t bits>
bitset<bits>& bitset<bits>::operator |= (const bitset& x)
{
  for (int i=0;i<WORDS;i++)
      word [i] |= x.word [i];
      
  return *this;
}

template <size_t bits>
bitset<bits>& bitset<bits>::operator ^= (const bitset& x)
{
  for (int i=0;i<WORDS;i++)
      word [i] ^= x.word [i];
      
  return *this;
}

template <size_t bits>
bitset<bits>& bitset<bits>::operator <<= (size_t shift)
{
  if (!shift)
    return *this;

  const size_t word_shift = shift / BITS_PER_WORD,
               offset     = shift % BITS_PER_WORD,
               sub_offset = BITS_PER_WORD - offset;               
               
  size_t i = WORDS - 1;
  
  for (;i>word_shift;--i) word [i] = (word [i-word_shift] << offset) | (word [i-word_shift-1] >> sub_offset);
  
  if (i == word_shift)
    word [i] = word [0] << offset;
    
  for (size_t j=0;j<i;++j) word [j] = static_cast<word_type> (0);

  trim ();  
  
  return *this;
}

template <size_t bits>
bitset<bits>& bitset<bits>::operator >>= (size_t shift)
{    
  if (!shift)
    return *this;
  
  const size_t word_shift = shift / BITS_PER_WORD,
               offset     = shift % BITS_PER_WORD,
               sub_offset = BITS_PER_WORD - offset,
               limit      = WORDS - word_shift - 1;               
                
  for (size_t i=0;i<limit;++i) word [i] = (word [i+word_shift] >> offset) | (word [i+word_shift+1] << sub_offset);
  
  word [limit] = word [WORDS-1] >> offset;
  
  for (size_t i=limit+1;i<WORDS;++i) word [i] = static_cast<word_type> (0);    

  trim ();
  
  return *this;
}

template <size_t bits>
inline bitset<bits> bitset<bits>::operator & (const bitset& x) const
{
  return bitset (*this) &= x;
}

template <size_t bits>
inline bitset<bits> bitset<bits>::operator | (const bitset& x) const
{
  return bitset (*this) |= x;
}

template <size_t bits>
inline bitset<bits> bitset<bits>::operator ^ (const bitset& x) const
{
  return bitset (*this) ^= x;
}

template <size_t bits>
inline bitset<bits> bitset<bits>::operator << (size_t shift) const
{
  return bitset (*this) <<= shift;
}

template <size_t bits>
inline bitset<bits> bitset<bits>::operator >> (size_t shift) const
{
  return bitset (*this) >>= shift;
}

template <size_t bits>
inline bitset<bits> bitset<bits>::operator ~ () const
{
  return bitset (*this).flip ();
}

/*
    Поиск
*/

template <size_t bits>
size_t bitset<bits>::find_first () const
{
  for (size_t i=0;i<WORDS;i++)
  {
    word_type w = word [i];    
    
    if (w) 
    {
      for (size_t j=0;j<sizeof (word_type);j++,w>>=CHAR_BIT) 
      {
        unsigned char x = static_cast<unsigned char> (w & (~(unsigned char)0));
        
        if (x)
          return i*BITS_PER_WORD + j*CHAR_BIT + bitset_arrays<true>::first_one [x];
      }
    }
  }
  
  return bits;
}

template <size_t bits>
size_t bitset<bits>::find_next (size_t prev) const
{
  ++prev;

  if (prev >= bits) //check out of bounds
    return bits;

    // search first word
  size_t    i = which_word (prev);  
  word_type w = word [i] & (~word_type (0)) << which_bit (prev);

  if (w) 
  { 
    w >>= which_byte (prev) * CHAR_BIT;
    
    for (size_t j=which_byte (prev);j<sizeof (word_type);j++,w>>=CHAR_BIT) 
    {
      unsigned char x = static_cast<unsigned char> (w & (~(unsigned char)0));
      
      if (x)
        return i*BITS_PER_WORD + j*CHAR_BIT + bitset_arrays<true>::first_one [x];
    }
  }

  // check subsequent words    
  
  for (i++;i<WORDS;i++) 
  {
    word_type w = word [i];
    
    if (w) 
    {
      for (size_t j=0;j<sizeof (word_type);j++,w>>=CHAR_BIT) 
      {
        unsigned char x = static_cast<unsigned char> (w & (~(unsigned char)0));
        
        if (x)
          return i*BITS_PER_WORD + j*CHAR_BIT + bitset_arrays<true>::first_one [x];
      }
    }
  }

  return bits;
}

/*
    Преобразования
*/

template <size_t bits>
unsigned long bitset<bits>::to_ulong () const
{
  if (sizeof (word_type) >= sizeof (unsigned long))
  {
    for (size_t i=1;i<WORDS;++i)
      if (word [i])
        raise_overflow_error ("to_ulong");

    const word_type mask = static_cast<word_type>(static_cast<unsigned long>(-1));
    
    if (word [0] & ~mask) 
      raise_overflow_error ("to_ulong");      

    return static_cast<unsigned long> (word [0] & mask);
  }
  else 
  {
    const size_t nwords = (sizeof (unsigned long) + sizeof (word_type) - 1) / sizeof (word_type);

    size_t min_nwords = nwords;
    
    if (WORDS > nwords)
    {
      for (size_t i=nwords;i<WORDS;++i) 
        if (word [i]) 
          raise_overflow_error ("to_ulong");
    }
    else min_nwords = WORDS;
      
    // If unsigned long is 8 bytes and _WordT is 6 bytes, then an unsigned
    // long consists of all of one word plus 2 bytes from another word.
    
    const size_t part = sizeof (unsigned long) % sizeof (word_type);

    if (part && nwords <= WORDS && (word [min_nwords-1] >> ((sizeof (word_type)-part)*CHAR_BIT)))
      raise_overflow_error ("bitset");

    unsigned long result = 0;    
    
    for (size_t i=0;i<min_nwords;++i) 
      result |= static_cast<unsigned long> (word [i]) << (i * sizeof (word_type)*CHAR_BIT);

    return result;
  }
}

template <size_t bits> template <class T,class Traits,class Allocator> 
inline basic_string<T,Traits,Allocator> bitset<bits>::to_string (T e0) const
{
  basic_string<T,Traits,Allocator> s;

  return to_string (s,e0);
}

template <size_t bits> template <class T,class Traits> 
inline basic_string<T,Traits> 
bitset<bits>::to_string (T e0) const
{
  return to_string<T,Traits,typename basic_string<T,Traits>::allocator_type> (e0);
}

template <size_t bits> template <class T> 
inline basic_string<T,char_traits<T> > 
bitset<bits>::to_string (T e0) const
{
  return to_string<T,char_traits<T>,typename basic_string<T>::allocator_type> (e0);
}

template <size_t bits>
inline string bitset<bits>::to_string (char e0) const
{
  return to_string<char,char_traits<char>,typename basic_string<char>::allocator_type> (e0);
}

template <size_t bits> template <class T,class Traits,class Allocator> 
basic_string<T,Traits,Allocator>& bitset<bits>::to_string (basic_string<T,Traits,Allocator>& s,T e0) const
{ 
  s.assign (bits,e0);
      
  for (typename basic_string<T,Traits,Allocator>::size_type i=0;i<bits;++i) 
    if (unchecked_test (i))
      s [bits-1-i] = (T)(e0+1);
            
  return s;
}

template <size_t bits> template <class T>
T* bitset<bits>::to_string (T* buf,T e0) const
{  
  T* s = buf+bits-1;

  for (size_t i=0;i<bits;++i,--s) 
    *s = unchecked_test (i) ? (T)(e0+1) : e0;
    
  return buf;
}

/*
    Копирование из строки
*/

template <size_t bits> template <class T>
void bitset<bits>::copy_from_string (const T* s,size_t len,T e0)
{
  reset ();  
  
  len = min (bits,len);
  
  for (size_t i=0;len--;s++,i++)
    if      (*s == (T)(e0+1)) set (i);
    else if (*s != e0)        raise_invalid_argument ("copy_from_string");
}
