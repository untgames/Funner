/*
    Описание array
*/

/*
    Поддержка итераторов
*/
array::iterator array::begin ()
{
  return elems;
}

array::const_iterator array::begin () const 
{
  return elems;
}

array::iterator array::end ()
{
  return elems+N;
}

array::const_iterator array::end () const
{
  return elems+N;
}

/*
    Поддержка реверсных итераторов
*/
array::reverse_iterator array::rbegin ()
{
  return reverse_iterator (end ());
}

array::const_reverse_iterator array::rbegin() const
{
  return const_reverse_iterator (end ());
}

array::reverse_iterator array::rend ()
{
  return reverse_iterator (begin ());
}

array::const_reverse_iterator array::rend () const
{
  return const_reverse_iterator (begin ());
}
    
/*
    Доступ к элементу (operator[])
*/
array::reference array::operator[] (size_type i) 
{ 
  return elems [i];
}

array::const_reference array::operator[](size_type i) const 
{     
  return elems [i]; 
}
    
/*
    Доступ к элементу с проверкой диапазона (at())
*/
array::reference array::at (size_type i)
{
  rangecheck (i);
  return elems [i];
}

array::const_reference array::at (size_type i) const
{
  rangecheck (i);
  return elems [i];
}

/*
    Доступ к первому (front) и последнему (back) элементам
*/
array::reference array::front () 
{ 
  return elems [0]; 
}

array::const_reference array::front () const 
{
  return elems [0];
}

array::reference array::back () 
{ 
  return elems [N-1]; 
}

array::const_reference array::back () const 
{ 
  return elems [N-1]; 
}

/*
    Информация о размере
*/
static array::size_type array::size ()
{
  return N;
}

static bool array::empty ()
{
  return false;
}

static array::size_type array::max_size ()
{
  return N;
}

/*
   Обмен (за линейное время)
*/
void array::swap (array::array<T,N>& y)
{
  stl::swap_ranges (begin (), end (), y.begin ());
}

/*
    Прямой доступ к данным (только чтение)
*/
const T* array::data () const
{
  return elems;
}

T* array::data ()
{ 
  return elems;
}

/*
    Использование как С массива (чтение/запись)
*/
T* array::c_array ()
{ 
  return elems;
}

/*
    Присваивание c преобразованием типа
*/
template <typename T2>
array<T,N>& array::operator= (const array<T2,N>& rhs)
{
    stl::copy (rhs.begin (),rhs.end (), begin ());
    return *this;
}

/*
    Присваивание одного значения для всех элементов
*/
void array::assign (const T& value)
{
    stl::fill_n (begin (), size (), value);
}

/*
    Проверка диапазона
*/
static void array::rangecheck (array::size_type i)
{
  if (i >= size ())
  {
      throw stl::range_error ("array<>: index out of range");
  }
}
