namespace detail
{

///базовый класс контейнера функции анимационного канала
class IEvaluatorBase : public xtl::reference_counter
{
  public:
    virtual const std::type_info& TrackType () = 0; //тип трека
    virtual const std::type_info& ValueType () = 0; //тип значения результата
    
    virtual ~IEvaluatorBase () {}
};

///шаблонный интерфейс функции анимационного канала
template <class T> class IEvaluator : public IEvaluatorBase
{
  public:
    virtual void Eval (float time, T& value) = 0;
    
    const std::type_info& ValueType ()
    {
      return typeid (T);
    }
};

///реализация функции анимационного канала
template <class Fn> class TrackImpl : public IEvaluator<typename TrackResultType<Fn>::Type> 
{
  public:
    typedef typename TrackResultType<Fn>::Type ValueType;
    
    TrackImpl (Fn in_fn)
      : fn (in_fn)
    {
    }

    void Eval (float time, ValueType& value)
    {
      fn (time, value);
    }
    
    Fn& Function ()
    {
      return fn;
    }

    const std::type_info& TrackType ()
    {
      return typeid (Fn);
    }

  private:
    Fn fn;
};

}

/*
    class Evaluator<T>
*/

/*
   Конструкторы / деструктор / присваивание
*/

template <class T>
Evaluator<T>::Evaluator (const detail::IEvaluator<T>& evaluator)
  : impl (const_cast<detail::IEvaluator<T>*> (&evaluator))
{
  addref (impl);
}

template <class T>
Evaluator<T>::Evaluator (const Evaluator& source)
  : impl (source.impl)
{
  addref (impl);
}

template <class T>
Evaluator<T>::~Evaluator ()
{
  release (impl);
}

template <class T>
Evaluator<T>& Evaluator<T>::operator = (const Evaluator<T>& source)
{
  Evaluator (source).Swap (*this);
  
  return *this;  
}

/*
    Обмен
*/

template <class T>
void Evaluator<T>::Swap (Evaluator& source)
{
  stl::swap (impl, source.impl);
}

template <class T>
void swap (Evaluator<T>& evaluator1, Evaluator<T>& evaluator2)
{
  evaluator1.Swap (evaluator2);
}

/*
   Вычисление значений
*/

namespace detail
{

template <class Ret> struct ResultValue
{
  Ret value;
  
  ResultValue () {}
};

}

template <class T>
typename Evaluator<T>::ValueType Evaluator<T>::operator () (float time) const
{
  detail::ResultValue<T> result;
  
  impl->Eval (time, result.value);
  
  return result.value;
}

template <class T>
void Evaluator<T>::operator () (float time, ValueType& value)
{
  impl->Eval (time, value);
}

/*
    class Channel
*/

/*
    Вычисление значений по времени
*/

template <class Ret> Ret Channel::Eval (float time) const
{
  return Evaluator<Ret> () (time);
}

template <class Ret> void Channel::Eval (float time, Ret& result) const
{
  Evaluator<Ret> () (time, result);
}

/*
    Сплайн
*/

template <class Fn> void Channel::SetTrack (const Fn& fn)
{
  SetTrackCore (new detail::TrackImpl<Fn> (fn));
}

template <class Fn> const Fn* Channel::Track () const
{
  if (&typeid (Fn) != &TrackType ())
    return 0;
  
  return &static_cast<detail::TrackImpl<Fn>*> (TrackCore ())->Function ();
}

template <class Fn> Fn* Channel::Track ()
{
  return const_cast<Fn*> (const_cast<const Channel&> (*this).Track<Fn> ());
}

/*
    Получение функции вычисления значений
*/

template <class T> animation::Evaluator<T> Channel::Evaluator () const
{
  if (&typeid (T) != &ValueType ())
    RaiseTypeError ("media::animation::Channel::Evaluator", typeid (T));
    
  return media::animation::Evaluator<T> (*static_cast<detail::IEvaluator<T>*> (TrackCore ()));
}
