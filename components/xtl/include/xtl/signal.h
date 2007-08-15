#ifndef XTL_SIGNAL_HEADER
#define XTL_SIGNAL_HEADER

#include <xtl/tuple>
#include <xtl/function.h>
#include <xtl/connection.h>

namespace stl
{

struct bidirectional_iterator_tag;

}

namespace xtl
{

namespace detail
{

//implementation forwards
template <class Signature> class slot;

}

//аккумулятор по умолчанию для обработки сигналов
template <class Result> struct default_signal_accumulator;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сигнал
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature, class Accumulator = default_signal_accumulator<typename function<Signature>::result_type> >
class signal
{
  struct dummy { void nonnull () {} };
  typedef void (dummy::*safe_bool)();
  typedef function<Signature> function_type;
  public:
    typedef typename function_type::arg1_type      arg1_type;
    typedef typename function_type::arg2_type      arg2_type;
    typedef typename function_type::arg3_type      arg3_type;
    typedef typename function_type::arg4_type      arg4_type;
    typedef typename function_type::arg5_type      arg5_type;
    typedef typename function_type::arg6_type      arg6_type;
    typedef typename function_type::arg7_type      arg7_type;
    typedef typename function_type::arg8_type      arg8_type;
    typedef typename function_type::arg9_type      arg9_type;
    typedef Accumulator                            accumulator_type;
    typedef typename accumulator_type::result_type result_type;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    signal  ();

    ~signal ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка на пустоту
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool empty () const;

    operator safe_bool () const { return empty () ? 0 : &dummy::nonnull; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Соединение / отсоединение обработчиков сигнала
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Fn> connection connect    (Fn);
    template <class Fn> void       disconnect (Fn);

    void disconnect_all ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество слотов
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t num_slots () const; //линейной время

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вызов
///////////////////////////////////////////////////////////////////////////////////////////////////
    result_type operator () () const;
    result_type operator () (arg1_type) const;
    result_type operator () (arg1_type, arg2_type) const;
    result_type operator () (arg1_type, arg2_type, arg3_type) const;
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type) const;
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type) const;
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type) const;
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type) const;
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type) const;
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type, arg9_type) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Accumulator1> void swap (signal<Signature, Accumulator1>&);

  private:
    typedef detail::slot<Signature> slot;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Распространение сигнала
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Tuple> result_type invoke (Tuple&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Копирование обработчиков
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Signature1, class Accumulator1> void clone (const signal<Signature1, Accumulator1>&);
    
  private:
    signal (const signal&); //no impl
    signal& operator = (const signal&); //no impl

  private:
    mutable slot first;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature, class Accumulator1, class Accumulator2>
void swap (signal<Signature, Accumulator1>&, signal<Signature, Accumulator2>&);

#include <xtl/detail/signal.inl>

}

#endif
