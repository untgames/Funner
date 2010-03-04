#ifndef COMMONLIB_ACTION_QUEUE_HEADER
#define COMMONLIB_ACTION_QUEUE_HEADER

#include <common/time.h>

#include <cstddef>
#include <xtl/functional_fwd>

namespace common
{

//implementation forwards
struct ActionImpl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ���� ��� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ActionThread
{
  ActionThread_Current,    //������� ����
  ActionThread_Main,       //������� ���� ����������
  ActionThread_Background, //������� ����
  
  ActionThread_Num  
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ActionQueueEvent
{
  ActionQueueEvent_OnPushAction, //����������� ��� ���������� ����� ������ � �������
  ActionQueueEvent_OnPopAction,  //����������� ��� �������� ������ �� �������
  
  ActionQueueEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Action
{
  friend struct ActionImpl;
  public:
    typedef xtl::function<bool (Action&, size_t milliseconds)> WaitCompleteHandler;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Action  ();
    Action  (const Action&);
    ~Action ();
    
    Action& operator = (const Action&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///� ����� ���� ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t CreaterThreadId () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �� �������� / �������� �� �������� / �������� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsEmpty     () const;
    bool IsCanceled  () const;
    bool IsCompleted () const;
    bool Wait        (size_t milliseconds);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Perform ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Cancel ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� �������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                       SetWaitHandler (const WaitCompleteHandler&);
    const WaitCompleteHandler& WaitHandler    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Action&);
    
  private:
    Action (ActionImpl*);

  private:
    ActionImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Action&, Action&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ActionQueue
{
  public:
    typedef Timer::time_t                  time_t;  
    typedef xtl::function<void (Action&)>  ActionHandler;
    typedef xtl::function<void ()>         CallbackHandler;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Action PushAction (const ActionHandler& action, ActionThread thread = ActionThread_Current, time_t delay = 0.0);
    static Action PushAction (const ActionHandler& action, ActionThread thread, time_t delay, time_t period);
    static Action PushAction (const ActionHandler& action, ActionThread thread, time_t delay, Timer& timer);
    static Action PushAction (const ActionHandler& action, ActionThread thread, time_t delay, time_t period, Timer& timer);
    static Action PushAction (const ActionHandler& action, const CallbackHandler& complete_callback, ActionThread thread = ActionThread_Current, time_t delay = 0.0);
    static Action PushAction (const ActionHandler& action, const CallbackHandler& complete_callback, ActionThread thread, time_t delay, Timer& timer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static size_t ActionsCount (ActionThread thread);
    static Action PopAction    (ActionThread thread);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �������� ������, ��������� � ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void CleanupCurrentThread ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (ActionThread thread, Action& action)> EventHandler;

    static xtl::connection RegisterEventHandler (ActionQueueEvent event, const EventHandler& handler);
};

}

#endif
