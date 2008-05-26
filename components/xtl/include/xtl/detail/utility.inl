/*
    ”правл€ющие шаблоны
*/

//выбор из двух типов condition == true -> TrueType, false -> FalseType
template <bool condition, class TrueType, class FalseType> struct select
{
  typedef TrueType type;
};

template <class TrueType, class FalseType> struct select<false, TrueType, FalseType>
{
  typedef FalseType type;
};

//compile-time assertion
template <bool condition> struct compile_time_assert;
template <>               struct compile_time_assert<true> { enum { value = 1 }; };

/*
    ¬раперы
*/

template <class T>
inline const T& make_const_ref (const T& x)
{
  return x;
}
