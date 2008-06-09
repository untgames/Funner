/*
    XTL exception
*/

#ifndef XTL_EXCEPTION_HEADER
#define XTL_EXCEPTION_HEADER

#include <exception>
#include <cstdarg>

namespace xtl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class exception: public std::exception
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� � ��������� ������������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void vtouch (const char* format, va_list args) throw () {} //no default implementation
    virtual void touch  (const char* format, ...) throw () {} //no default implementation
};

}

#endif
