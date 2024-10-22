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
///Тип нити для очереди
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ActionThread
{
  ActionThread_Current,    //текущая нить
  ActionThread_Main,       //главная нить приложения
  ActionThread_Background, //фоновая нить
  
  ActionThread_Num  
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///События очереди
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ActionQueueEvent
{
  ActionQueueEvent_OnPushAction, //срабатывает при добавлении новой задачи в очередь
  ActionQueueEvent_OnPopAction,  //срабатывает при удалении задачи из очереди
  
  ActionQueueEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Объект состояния выполнения действия (thread safe)
///////////////////////////////////////////////////////////////////////////////////////////////////
class Action
{
  friend struct ActionImpl;
  public:
    typedef xtl::function<bool (Action&, size_t milliseconds)> WaitCompleteHandler;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    Action  ();
    Action  (const Action&);
    ~Action ();
    
    Action& operator = (const Action&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///В какой нити создано действие / какой из нитей принадлежит событие
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t       CreatorThreadId () const;
    ActionThread ThreadType      () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Пустое ли действие / сброшено ли действие / ожидание завершения действия
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsEmpty     () const;
    bool IsCanceled  () const;
    bool IsCompleted () const;
    bool Wait        (size_t milliseconds);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Выполнить действие
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Perform ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Отменить действие
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Cancel ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установить функтор ожидания завершения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                       SetWaitHandler (const WaitCompleteHandler&);
    const WaitCompleteHandler& WaitHandler    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Action&);
    
  private:
    Action (ActionImpl*);

  private:
    ActionImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Action&, Action&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Очередь действий (thread safe)
///////////////////////////////////////////////////////////////////////////////////////////////////
class ActionQueue
{
  public:
    typedef Timer::time_t                  time_t;  
    typedef xtl::function<void (Action&)>  ActionHandler;
    typedef xtl::function<void ()>         CallbackHandler;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Постановка действия в очередь
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Action PushAction (const ActionHandler& action, ActionThread thread = ActionThread_Current, const time_t& delay = 0);
    static Action PushAction (const ActionHandler& action, ActionThread thread, const time_t& delay, const time_t& period);
    static Action PushAction (const ActionHandler& action, ActionThread thread, const time_t& delay, Timer& timer);
    static Action PushAction (const ActionHandler& action, ActionThread thread, const time_t& delay, const time_t& period, Timer& timer);
    static Action PushAction (const ActionHandler& action, const CallbackHandler& complete_callback, ActionThread thread = ActionThread_Current, const time_t& delay = 0);
    static Action PushAction (const ActionHandler& action, const CallbackHandler& complete_callback, ActionThread thread, const time_t& delay, Timer& timer);
    static Action PushAction (const ActionHandler& action, size_t thread_id, const time_t& delay = 0);
    static Action PushAction (const ActionHandler& action, size_t thread_id, const time_t& delay, const time_t& period);
    static Action PushAction (const ActionHandler& action, size_t thread_id, const time_t& delay, Timer& timer);
    static Action PushAction (const ActionHandler& action, size_t thread_id, const time_t& delay, const time_t& period, Timer& timer);
    static Action PushAction (const ActionHandler& action, const CallbackHandler& complete_callback, size_t thread_id, const time_t& delay = 0);
    static Action PushAction (const ActionHandler& action, const CallbackHandler& complete_callback, size_t thread_id, const time_t& delay, Timer& timer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение действий из очереди
///////////////////////////////////////////////////////////////////////////////////////////////////
    static size_t ActionsCount (ActionThread thread);
    static size_t ActionsCount (size_t threadId);
    static Action PopAction    (ActionThread thread);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Очистка структур данных, связанных с текущей нитью
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void CleanupCurrentThread ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подписка на события
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (ActionThread thread, Action& action)> EventHandler;

    static xtl::connection RegisterEventHandler (ActionQueueEvent event, const EventHandler& handler);        
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание диспетчера функции обратного вызова в указанной нити
///////////////////////////////////////////////////////////////////////////////////////////////////
ActionQueue::CallbackHandler make_callback_wrapper (ActionThread thread, const ActionQueue::CallbackHandler& handler);

}

#endif
