#ifndef MEDIALIB_ANIMATION_EVENT_TRACK_HEADER
#define MEDIALIB_ANIMATION_EVENT_TRACK_HEADER

#include <xtl/functional_fwd>

namespace media
{

namespace animation
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class EventTrack
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    EventTrack  ();
    EventTrack  (const EventTrack&);
    ~EventTrack ();

    EventTrack& operator = (const EventTrack&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    size_t Capacity () const;
    void   Reserve  (size_t keys_count);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� / ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t SetEvent (float time, const char* event);
    void   SetTime  (size_t event_index, float time);
    void   SetEvent (size_t event_index, float time, const char* event);
    void   SetEvent (size_t event_index, const char* event);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetEvent (size_t event_index) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ �������, ��������� � ��������� ���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (float time, const char* event)> EventHandler;

    void GetEvents (float min_time, float max_time, const EventHandler& handler) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����� (�� ����������� �������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Sort ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Remove (size_t event_index);
    void Clear  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    int FindLower (float time) const; //����� ����� <= time
    int FindUpper (float time) const; //����� ����� >= time
    int FindEqual (float time) const; //����� ����� == time

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (EventTrack&);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (EventTrack&, EventTrack&);

}

}

#endif
