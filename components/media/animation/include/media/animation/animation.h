#ifndef MEDIALIB_ANIMATION_ANIMATION_HEADER
#define MEDIALIB_ANIMATION_ANIMATION_HEADER

#include <typeinfo>

namespace media
{

namespace animation
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
//????
class Channel
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Channel  ();
    Channel  (const Channel&);
    ~Channel ();
    
    template <class TrackType> Channel (const char* name, const TrackType& track);
    
    Channel& operator = (const Channel&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const std::type_info& TrackType () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class TrackType> const TrackType* Track () const;
    template <class TrackType>       TrackType* Track ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class TrackType> void SetTrack (const TrackType&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Channel&);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Channel&, Channel&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Animation
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
  //???
  
  private:
    struct Impl;
    Impl* impl;
};

}

}

#endif
