#ifndef MEDIALIB_ANIMATION_CHANNEL_HEADER
#define MEDIALIB_ANIMATION_CHANNEL_HEADER

#include <cstddef>
#include <typeinfo>

#include <xtl/reference_counter.h>

namespace media
{

namespace animation
{

//forward declarations
class Channel;
class ChannelBlenderBase;

namespace detail
{

class IEvaluatorBase;

template <class T> class IEvaluator;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������-����������� �������� ������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class Evaluator
{
  public:
    typedef T ValueType;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Evaluator  (const detail::IEvaluator<T>&);
    Evaluator  (const Evaluator&);
    ~Evaluator ();

    Evaluator& operator = (const Evaluator&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ValueType operator () (float time) const;
    void      operator () (float time, ValueType& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Evaluator&);

  private:
    detail::IEvaluator<ValueType>* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
void swap (Evaluator<T>&, Evaluator<T>&);

template <class Fn> struct TrackResultType
{
  typedef typename Fn::result_type Type;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Channel
{
  friend class ChannelBlenderBase;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Channel  ();
    Channel  (const Channel&);
    ~Channel ();

    Channel& operator = (const Channel&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    Channel Clone () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Id () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ������������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* ParameterName    () const;
    void        SetParameterName (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����� / ��� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const std::type_info& TrackType () const;
    const std::type_info& ValueType () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Fn> void      SetTrack (const Fn&);
    template <class Fn> const Fn* Track    () const;
    template <class Fn> Fn*       Track    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Ret> Ret  Eval (float time) const;
    template <class Ret> void Eval (float time, Ret& result) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> animation::Evaluator<T> Evaluator () const;   

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Channel&);

  private:
    struct Impl;  
  
    Channel (Impl*);
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                    SetTrackCore (detail::IEvaluatorBase*);
    detail::IEvaluatorBase* TrackCore    () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ���������� �������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RaiseTypeError (const char* source, const std::type_info& type) const;

  private:
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Channel&, Channel&);

#include <media/animation/detail/channel.inl>

}

}

#endif
