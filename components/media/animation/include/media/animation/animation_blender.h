#ifndef MEDIALIB_ANIMATION_BLENDER_HEADER
#define MEDIALIB_ANIMATION_BLENDER_HEADER

#include <media/animation/animation.h>
#include <media/animation/target_blender.h>

namespace xtl
{

///forward declaration
template <class T> class iterator;

}

namespace media
{

namespace animation
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum AnimationBlenderEvent
{
  AnimationBlenderEvent_OnTargetAdded,   //��������� ����� ������������ ����
  AnimationBlenderEvent_OnTargetRemoved, //������� ������������ ����
  
  AnimationBlenderEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class AnimationBlender
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    AnimationBlender  ();
    AnimationBlender  (const AnimationBlender&);
    ~AnimationBlender ();
    
    AnimationBlender& operator = (const AnimationBlender&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������� ���������� / ����� ���������� �������-����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t SourcesCount        () const;
    size_t SourceChannelsCount () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    AnimationState AddSource        (const Animation&);
    AnimationState AddSource        (const Animation&, const PropertyNameMap&);
    void           RemoveTarget     (const char* target_name);
    void           RemoveAllSources ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t TargetsCount () const;        

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::iterator<TargetBlender>       TargetIterator;
    typedef xtl::iterator<const TargetBlender> ConstTargetIterator;    
    
    TargetIterator      CreateTargetIterator ();
    ConstTargetIterator CreateTargetIterator () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* TargetId (const ConstTargetIterator&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ �������, ��������� � ��������� ���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetEvents (float previous_time, float current_time, const EventTrack::EventHandler& handler) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Update ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ������� �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (AnimationBlenderEvent event, const char* target_name, TargetBlender&)> EventHandler;

    xtl::connection RegisterEventHandler (AnimationBlenderEvent event, const EventHandler& handler);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (AnimationBlender&);
    
  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (AnimationBlender&, AnimationBlender&);

}

}

#endif