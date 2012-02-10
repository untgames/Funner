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
  size_t             channels_count = ChannelsCount ();
  const ChannelDesc* channels       = Channels ();
  
    //расчёт коэффициента нормализации весов

  float total_weight = 0.0f;
  
  for (size_t i=0; i<channels_count; i++)
    total_weight += channels [i].state->Weight ();
    
  float weight_multiplier = 0;
  
  static const float EPS = 1e-06f;
    
  if (total_weight < EPS)
    return;
  
  weight_multiplier = 1.0f / total_weight;
  
    //смешивание
  
  for (size_t i=0; i<channels_count; i++)
  {
    detail::ResultValue<ValueType> channel_value;
    
    static_cast<detail::IEvaluator<T>*> (channels [i].evaluator)->Eval (channels [i].state->Time (), channel_value.value);
  
    blend (channel_value.value, channels [i].state->Weight () * weight_multiplier, value);
  }
}

/*
    Смешивание
*/

template <class T>
inline void blend (const T& src_value, float weight, T& result_value)
{
  result_value += src_value * weight;
}

template <class T> T blend_init (xtl::type<T>)
{
  return T ();
}

template <class T, size_t Size> math::matrix<T, Size> blend_init (xtl::type<math::matrix<T, Size> >)
{
  return math::matrix<T, Size> (T ());
}
