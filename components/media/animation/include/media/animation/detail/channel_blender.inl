/*
    ChannelBlender
*/

template <class T>
inline ChannelBlender<T>::ChannelBlender ()
  : ChannelBlenderBase (typeid (T))
{
}

template <class T>
inline typename ChannelBlender<T>::ValueType ChannelBlender<T>::operator () () const
{
  ValueType result = blend_init (xtl::type<ValueType> ());
  
  (*this)(result);
  
  return result;
}

template <class T>
inline void ChannelBlender<T>::operator () (ValueType& value) const
{
  size_t        sources_count = SourcesCount ();
  const Source* sources       = Sources ();
  
    //расчёт коэффициента нормализации весов

  float total_weight = 0.0f;
  
  for (size_t i=0; i<sources_count; i++)
    total_weight += get_weight (*sources [i].state);
    
  float weight_multiplier = 0;
  
  static const float EPS = 1e-06f;
    
  if (total_weight < EPS)
    return;
  
  weight_multiplier = 1.0f / total_weight;
  
    //смешивание
  
  for (size_t i=0; i<sources_count; i++)
  {
    detail::ResultValue<ValueType> source_value;
    
    static_cast<detail::IEvaluator<T>*> (sources [i].evaluator)->Eval (get_time (*sources [i].state), source_value.value);
  
    blend (source_value.value, get_weight (*sources [i].state) * weight_multiplier, value);
  }
}

/*
    Смешивание
*/

template <class T>
inline void blend (const T& src_value, float weight, T& result_value)
{
  result_value += static_cast<T> (src_value * weight);
}

template <class T> T blend_init (xtl::type<T>)
{
  return T ();
}

template <class T, size_t Size> math::matrix<T, Size> blend_init (xtl::type<math::matrix<T, Size> >)
{
  return math::matrix<T, Size> (T ());
}

template <class T> math::quat<T> blend_init (xtl::type<math::quat<T> >)
{
  return math::quat<T> (T (), T (), T (), T ());
}
