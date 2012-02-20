#ifndef MEDIA_ANIMATION_CHANNEL_BLENDER_HEADER
#define MEDIA_ANIMATION_CHANNEL_BLENDER_HEADER

#include <xtl/type.h>

#include <media/animation/animation_state.h>
#include <media/animation/channel.h>

namespace math
{

//forward declaration
template <class T, unsigned int Size> class matrix;

}

namespace media
{

namespace animation
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ChannelBlenderEvent
{
  ChannelBlenderEvent_OnSourcesAdded,   //��������� ����� ������������ ���������
  ChannelBlenderEvent_OnSourcesRemoved, //������� ������������ ���������
  
  ChannelBlenderEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ��� �������� ������������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ChannelBlenderBase
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ChannelBlenderBase  (const ChannelBlenderBase&);
    ~ChannelBlenderBase ();
    
    ChannelBlenderBase& operator = (const ChannelBlenderBase&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const std::type_info& ValueType () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t SourcesCount () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � �������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddSource        (const animation::AnimationState& state, const animation::Channel& channel);
    void RemoveSources    (const animation::AnimationState& state);
    void RemoveSources    (const animation::Channel& channel);    
    void RemoveAllSources ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ������� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (ChannelBlenderEvent event)> EventHandler;

    xtl::connection RegisterEventHandler (ChannelBlenderEvent event, const EventHandler& handler) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (ChannelBlenderBase&);

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ChannelBlenderBase (const std::type_info& value_type);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������� ��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    struct Source
    {
      detail::IEvaluatorBase* evaluator; //������ �� ����������� ��������� ��������
      AnimationStateImpl*     state;     //��������� ��������
    };
    
    const Source* Sources () const;
    
  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (ChannelBlenderBase&, ChannelBlenderBase&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class ChannelBlender: public ChannelBlenderBase
{
  public:
    typedef T ValueType;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ChannelBlender ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ValueType operator () () const;
    void      operator () (ValueType&) const;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>              T                     blend_init (xtl::type<T>);
template <class T, size_t Size> math::matrix<T, Size> blend_init (xtl::type<math::matrix<T, Size> >);
template <class T>              math::quat<T>         blend_init (xtl::type<math::quat<T> >);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> void blend (const T& src_value, float weight, T& result_value);

#include <media/animation/detail/channel_blender.inl>

}

}

#endif

