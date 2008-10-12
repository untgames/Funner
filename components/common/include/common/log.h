#ifndef COMMONLIB_LOG_HEADER
#define COMMONLIB_LOG_HEADER

#include <cstdarg>
#include <cstddef>

#include <xtl/functional_fwd>

namespace common
{

//forward declarations
class LogImpl;
class LogFilterImpl;
class File;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Log
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Log  (const char* name);
    Log  (const Log&);
    ~Log ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Log& operator = (const Log&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Print   (const char* message) const;
    void Printf  (const char* message, ...) const;
    void VPrintf (const char* message, va_list list) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Log&);

  private:
    LogImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Log&, Log&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class LogFilter
{
  public:
    typedef xtl::function<void (const char* log_name, const char* message)> LogHandler;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    LogFilter  (const char* log_name_mask, const LogHandler& handler);
    LogFilter  (const LogFilter&);
    ~LogFilter ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    LogFilter& operator = (const LogFilter&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Mask () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (LogFilter&);

  private:
    LogFilterImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (LogFilter&, LogFilter&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������� � ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class LogFile
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    LogFile  ();
    LogFile  (const common::File& file);
    ~LogFile ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� �������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                SetFile (const common::File& file);
    const common::File& File    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������� � ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Print (const char* log_name, const char* message);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/�������� �������� ������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t AddFilter        (const char* replace_mask, const char* replacement, size_t sort_order=~0);
    void   SetFilter        (size_t filter_index, const char* replace_mask, const char* replacement, size_t sort_order=~0);
    void   RemoveFilter     (size_t filter_index);
    void   RemoveAllFilters ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Flush ();

  private:
    LogFile (const LogFile&);                   //no impl
    const LogFile& operator = (const LogFile&); //no impl

  private:
    struct Impl;
    Impl* impl;
};

}

#endif
