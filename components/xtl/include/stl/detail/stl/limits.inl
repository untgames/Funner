/*
    number_limits_base
*/

template <class Number>
inline Number numeric_limits_base<Number>::min () throw ()
{
  return Number ();
}
  
template <class Number>  
inline Number numeric_limits_base<Number>::max () throw ()
{
  return Number ();
}

template <class Number>    
inline Number numeric_limits_base<Number>::epsilon () throw ()
{
  return Number ();
}
  
template <class Number>  
inline Number numeric_limits_base<Number>::round_error () throw ()
{
  return Number ();
}
  
template <class Number>  
inline Number numeric_limits_base<Number>::infinity () throw ()
{
  return Number ();
}
  
template <class Number>  
inline Number numeric_limits_base<Number>::quiet_NaN () throw ()
{
  return Number ();
}
  
template <class Number>  
inline Number numeric_limits_base<Number>::signaling_NaN () throw ()
{
  return Number ();
}
  
template <class Number>  
inline Number numeric_limits_base<Number>::denorm_min () throw ()
{
  return Number ();
}

/*
    floating_limits
*/

template <class Number,int Digits,int Digits10,int MinExp,int MaxExp,int MinExp10,int MaxExp10,
          unsigned int InfinityWord,unsigned int QNaNWord,unsigned int SNaNWord,
          bool IsIEC559,float_round_style RoundStyle>
inline Number 
floating_limits<Number,Digits,Digits10,MinExp,MaxExp,MinExp10,MaxExp10,InfinityWord,
                QNaNWord,SNaNWord,IsIEC559,RoundStyle>::infinity () throw () 
{
  static const unsigned int inf [sizeof (Number)/sizeof (int)] = { InfinityWord };
  return *reinterpret_cast<const Number*> (&inf);
}

template <class Number,int Digits,int Digits10,int MinExp,int MaxExp,int MinExp10,int MaxExp10,
          unsigned int InfinityWord,unsigned int QNaNWord,unsigned int SNaNWord,
          bool IsIEC559,float_round_style RoundStyle>
inline Number 
floating_limits<Number,Digits,Digits10,MinExp,MaxExp,MinExp10,MaxExp10,InfinityWord,
                QNaNWord,SNaNWord,IsIEC559,RoundStyle>::quiet_NaN () throw () 
{
  static const unsigned int nan [sizeof (Number)/sizeof (int)] = { QNaNWord };
  return *reinterpret_cast<const Number*> (&nan);
}

template <class Number,int Digits,int Digits10,int MinExp,int MaxExp,int MinExp10,int MaxExp10,
          unsigned int InfinityWord,unsigned int QNaNWord,unsigned int SNaNWord,
          bool IsIEC559,float_round_style RoundStyle>
inline Number 
floating_limits<Number,Digits,Digits10,MinExp,MaxExp,MinExp10,MaxExp10,InfinityWord,
                QNaNWord,SNaNWord,IsIEC559,RoundStyle>::signaling_NaN () throw () 
{
  static const unsigned int nan [sizeof (Number)/sizeof (int)] = { SNaNWord };
  return *reinterpret_cast<const Number*> (&nan);
}
