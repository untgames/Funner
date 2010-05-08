#ifndef MEDIALIB_ANIMATION_EVENT_QUEUE_HEADER
#define MEDIALIB_ANIMATION_EVENT_QUEUE_HEADER

#include <cstddef>

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
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    EventTrack Clone () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Id () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    size_t Capacity () const;
    void   Reserve  (size_t events_count);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� / ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t AddEvent  (float delay, float period, const char* event);
    void   SetDelay  (size_t event_index, float delay);
    void   SetPeriod (size_t event_index, float period);
    void   SetEvent  (size_t event_index, float delay, float period, const char* event);
    void   SetEvent  (size_t event_index, const char* event);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetEvent  (size_t event_index) const;
    float       GetDelay  (size_t event_index) const;
    float       GetPeriod (size_t event_index) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ �������, ��������� � ��������� ���������� �������. ������ �������� Sort
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (float time, const char* event)> EventHandler;

    void GetEvents (float previous_time, float current_time, const EventHandler& handler) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Remove (size_t event_index);
    void Clear  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (EventTrack&);

  private:
    struct Impl;
    
    EventTrack (Impl*);
    
  private:
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (EventTrack&, EventTrack&);

}

}

#endif
