#ifndef XTL_CONNECTION_HEADER
#define XTL_CONNECTION_HEADER

#include <cstddef>
#include <new>

#include <xtl/singleton_default.h>
#include <xtl/reference_counter.h>

namespace xtl
{

namespace detail
{

//implementation forwards
class connection_impl;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������������� ��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class connection
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    connection  ();
    connection  (const connection&);
    ~connection ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    connection& operator = (const connection&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool connected () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void disconnect ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void block   ();
    void unblock ();
    bool blocked () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const connection&) const;
    bool operator != (const connection&) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void swap (connection&);

  protected:
    connection (detail::connection_impl*);

  private:  
    detail::connection_impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (connection&, connection&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������� ������������� ����� ����� �� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class auto_connection: public connection
{
  public:
    auto_connection  () {}
    auto_connection  (const connection&);    
    ~auto_connection ();

    auto_connection& operator = (const auto_connection&);    
    auto_connection& operator = (const connection&);
};

#include <xtl/detail/connection.inl>

}

#endif
