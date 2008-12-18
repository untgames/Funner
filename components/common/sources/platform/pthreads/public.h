#ifndef COMMLIB_PTHREADS_PUBLIC_HEADER
#define COMMLIB_PTHREADS_PUBLIC_HEADER

#include <platform/platform.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class PThreadsSystem: public ICustomThreadSystem
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////  
    PThreadsSystem  ();
    ~PThreadsSystem ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� / �������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    thread_t CreateThread (IThreadCallback*);
    void     DeleteThread (thread_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void CancelThread (thread_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void JoinThread (thread_t);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    thread_t GetCurrentThread ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������������ �������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void EnterCriticalSection ();
    void ExitCriticalSection ();    
};

}

#endif
