namespace detail
{

class IEvaluatorBase : public xtl::reference_counter
{
  public:
/*
   Тип трека / тип значения
*/
    virtual const std::type_info& TrackType () = 0;
    virtual const std::type_info& ValueType () = 0;
    
    virtual ~IEvaluatorBase () {};
};

template <class T> class IEvaluator : public IEvaluatorBase
{
  public:
    virtual void Eval (float time, T& value) = 0;
    
    const std::type_info& ValueType ()
    {
      return typeid (T);
    }
};

template <class Fn> class TrackImpl : public IEvaluator<typename TrackResultType<Fn>::Type> 
{
  public:
    typedef typename TrackResultType<Fn>::Type ValueType;
    
    TrackImpl (Fn in_fn)
      : fn (in_fn)
      {}

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
  Evaluator (source.impl).Swap (*this);
  
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

template <class T>
typename Evaluator<T>::ValueType Evaluator<T>::operator () (float time) const
{
  ValueType value;
  
  impl->Eval (time, value);
  
  return value;
}

template <class T>
void Evaluator<T>::operator () (float time, ValueType& value)
{
  impl->Eval (time, value);
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
  if (&typeid (detail::TrackImpl<Fn>) != &TrackType ())
    return 0;
  
  return &static_cast<detail::TrackImpl<Fn>*> (TrackCore ())->Function ();
}

template <class Fn> Fn* Channel::Track ()
{
  return const_cast<Fn*> (const_cast<const Channel&> (*this).Track<Fn> ());
}

/*
   Вычисление значений по времени
*/

template <class Ret> Ret Channel::Eval (float time) const
{
  Ret value;
  
  Eval (time, value);
  
  return value;
}

template <class Ret> void Channel::Eval (float time, Ret& result) const
{
  Evaluator<Ret> () (time, result);
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
