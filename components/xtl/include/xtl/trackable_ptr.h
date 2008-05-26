#ifndef XTL_TRACKABLE_PTR_HEADER
#define XTL_TRACKABLE_PTR_HEADER

#include <xtl/trackable.h>
#include <xtl/default_cast_type.h>

namespace xtl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///"������" ���������, �������������� ��� �������� ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class trackable_ptr
{
  typedef T* (trackable_ptr::*unspecified_bool_type)() const;
  public:
    typedef T                        element_type;
    typedef T*                       pointer;
    typedef T&                       reference;
    typedef trackable::slot_type     slot_type;
    typedef trackable::function_type function_type;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    trackable_ptr ();
    trackable_ptr (pointer);
    trackable_ptr (const trackable_ptr&);

    template <class T1> trackable_ptr (T1*);
    template <class T1> trackable_ptr (const trackable_ptr<T1>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    trackable_ptr& operator = (pointer);
    trackable_ptr& operator = (const trackable_ptr&);

    template <class T1> trackable_ptr& operator = (T1*);
    template <class T1> trackable_ptr& operator = (const trackable_ptr<T1>&);

    void reset ();
    void reset (pointer);

    template <class T1> void reset (T1*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    pointer   get         () const;
    reference operator *  () const;
    pointer   operator -> () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � ��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    operator unspecified_bool_type () const;
    bool     operator !            () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������/�������� ������������ ��������� ��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    connection connect (const function_type&);
    connection connect (slot_type&);

    template <class Fn> void disconnect (Fn);

    void disconnect_all ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T1> bool operator == (const trackable_ptr<T1>&) const;
    template <class T1> bool operator == (const T1*) const;
    template <class T1> bool operator != (const trackable_ptr<T1>&) const;
    template <class T1> bool operator != (const T1*) const;

  private:
    void update ();

  private:
    typedef signal<void ()> signal_type;

  private:
    pointer     ptr;
    slot_type   on_destroy;
    signal_type notification;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T1, class T2> bool operator == (const T1*, const trackable_ptr<T2>&);
template <class T1, class T2> bool operator != (const T1*, const trackable_ptr<T2>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� trackable-�������
///////////////////////////////////////////////////////////////////////////////////////////////////
trackable& get_trackable (trackable&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> T* get_pointer (trackable_ptr<T>&);

#include <xtl/detail/trackable_ptr.inl>

}

#endif
