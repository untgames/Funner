#include "shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

using namespace media::rms;

namespace
{

/*
    Константы
*/

const size_t MAX_OPERATION_STEP_DURATION = 50;                //максимальное время выполнения шага операций в миллисекундах
const char*  LOG_NAME                    = "media.rms.async"; //имя протокола

/*
    Интерфейс операции
*/

class IAsyncOperationHandler
{
  public:
    virtual void DoStep  () = 0;
    virtual void AddRef  () = 0;
    virtual void Release () = 0;
  
  protected:
    virtual ~IAsyncOperationHandler () {}
};

/*
    Менеджер асинхронных операций
*/

class AsyncOperationManager
{
  public:
///Конструктор
    AsyncOperationManager ()
      : log (LOG_NAME)
      , action_handler (xtl::bind (&AsyncOperationManager::DoTimedStep, this))
    {
    }
    
    ~AsyncOperationManager ()
    {
      operations.clear ();
    }
  
///Добавление операции
    void AddOperation (IAsyncOperationHandler* operation)
    {            
      if (!operation)
        return;        
      
      operations.push_back (operation);
      
      try
      {
        if (action.IsEmpty ())
        {
          action = common::ActionQueue::PushAction (action_handler, common::ActionThread_Current, 0, MAX_OPERATION_STEP_DURATION / 1000.0);
        }
      }
      catch (...)
      {
        operations.remove (operation);
        throw;
      }
    }
    
///Удаление операции
    void RemoveOperation (IAsyncOperationHandler* operation)
    {
      if (!operation)
        return;
        
      for (OperationList::iterator iter=operations.begin (); iter!=operations.end (); ++iter)
        if (iter->get () == operation)
        {              
          operations.erase (iter);
      
          if (operations.empty () && !action.IsEmpty ())
          {
            action.Cancel ();
            action = common::Action ();
          }
          
          break;
        }
    }
    
///Выполнение операций
    void DoTimedStep ()
    {
      size_t end_time = common::milliseconds () + MAX_OPERATION_STEP_DURATION;      
      
      while (!operations.empty () && common::milliseconds () < end_time)
        DoStep (true);
    }

    bool DoStep (bool ignore_exceptions)
    {
      static const char* METHOD_NAME = "media::rms::AsyncOperationManager::DoStep";
      
      if (operations.empty ())
        return false;        
      
      try
      {
        operations.front ()->DoStep ();
      }
      catch (std::exception& e)
      {
        if (ignore_exceptions)
        {
          log.Printf ("%s\n    at %s", e.what (), METHOD_NAME);
        }
        else throw;          
      }
      catch (...)
      {
        if (ignore_exceptions)
        {
          log.Printf ("Unknown exception\n    at %s", METHOD_NAME);
        }
        else throw;
      }
      
      return true;
    }

  private:
    typedef stl::list<xtl::com_ptr<IAsyncOperationHandler> > OperationList;

  private:
    OperationList                      operations;
    common::Log                        log;
    common::ActionQueue::ActionHandler action_handler;
    common::Action                     action;
};

typedef common::Singleton<AsyncOperationManager> AsyncOperationManagerSingleton;

}

/*
    Описание реализации асинхронной операции
*/

typedef stl::list<AsyncOperation::TaskHandler> TaskList;

struct AsyncOperation::Impl: public IAsyncOperationHandler
{
  AsyncOperation& owner;                //ссылка на владельца
  size_t          ref_count;            //счётчик ссылок
  ProgressState   state;                //текущее состояние
  size_t          estimate_steps_count; //количество шагов для выполнения операции
  size_t          finished_steps_count; //количество выполненных шагов
  stl::string     resource_name;        //имя текущего обрабатываемого ресурса
  stl::string     stage_name;           //имя этапа загрузки
  ProgressHandler progress_handler;     //обработчик оповещения о выполнении операции
  TaskList        tasks;                //задачи

///Конструктор
  Impl (AsyncOperation& in_owner, const ProgressHandler& in_progress_handler)
    : owner (in_owner)
    , ref_count (1)
    , state (ProgressState_Processing)
    , estimate_steps_count (0)
    , finished_steps_count (0)
    , progress_handler (in_progress_handler)
  {
    AsyncOperationManagerSingleton::Instance ()->AddOperation (this);
  }
  
///Деструктор
  ~Impl ()
  {
    RemoveOperation ();
  }
  
///Удаление операции из очереди обработки менеджера асинхронных операций
  void RemoveOperation ()
  {
    try
    {
      AsyncOperationManagerSingleton::Instance ()->RemoveOperation (this);
    }
    catch (...)
    {
    }    
  }
  
///Обновление прогресса
  void UpdateProgress ()
  {
    try
    {
      Progress progress (&owner);

      progress_handler (progress);
    }
    catch (...)
    {
    }    
  }
  
///Выполнение шага операции  
  void DoStep ()
  {
    if (state != ProgressState_Processing || tasks.empty ())
    {
      RemoveOperation ();

      return;
    }
    
    TaskHandler task_handler = tasks.front ();
    
    try
    {
      bool result = task_handler (owner);      
      
      if (result)
      {
        tasks.erase (tasks.begin ());
        
        if (tasks.empty () && state == ProgressState_Processing)
        {
          state = ProgressState_Processed;

          UpdateProgress ();
        }
      }
    }
    catch (...)
    {
      state = ProgressState_Failed;

      UpdateProgress ();

      throw;
    }
  }

///Подсчёт ссылок  
  void AddRef ()
  {
    ref_count++;
  }
  
  void Release ()
  {
    if (!--ref_count)
      delete &owner;
  }  
};

/*
    Конструктор / деструктор
*/

AsyncOperation::AsyncOperation (const ProgressHandler& progress_handler)
{
  try
  {
    impl = new Impl (*this, progress_handler);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::rms::AsyncOperation::AsyncOperation");
    throw;
  }
}

AsyncOperation::~AsyncOperation ()
{
}

/*
    Создание операции
*/

AsyncOperation::Pointer AsyncOperation::Create (const ProgressHandler& progress_handler)
{
  return Pointer (new AsyncOperation (progress_handler), false);
}

namespace
{

void default_progress_handler (Progress& progress)
{
  progress.SetState (ProgressState_Processing);
}

}

AsyncOperation::Pointer AsyncOperation::Create ()
{
  return Pointer (new AsyncOperation (&default_progress_handler), false);
}

/*
    Добавление задач
*/

void AsyncOperation::AddTask (const TaskHandler& task_handler)
{
  impl->tasks.push_back (task_handler);
}

/*
    Состояние операции
*/

void AsyncOperation::SetState (ProgressState state)
{
  switch (state)
  {
    case ProgressState_Processing:
    case ProgressState_Processed:
    case ProgressState_Failed:
      break;
    default:
      throw xtl::make_argument_exception ("media::rms::AsyncOperation::SetState", "state", state);
  }
  
  impl->state = state;
}

ProgressState AsyncOperation::State ()
{
  return impl->state;
}

/*
    Оценка количества шагов выполнения операции
*/

void AsyncOperation::AddEstimateSteps (size_t steps_count)
{
  impl->estimate_steps_count += steps_count;
}

void AsyncOperation::AddFinishedSteps (size_t steps_count)
{
  impl->finished_steps_count += steps_count;
}

/*
    Прогресс выполнения операции
*/

float AsyncOperation::Value ()
{
  if (!impl->estimate_steps_count)
    return 0.0f;
    
  if (impl->finished_steps_count >= impl->estimate_steps_count)
    return 1.0f;

  return impl->finished_steps_count / (float)impl->estimate_steps_count;
}

/*
    Имя текущего обрабатываемого ресурса
*/

const char* AsyncOperation::Resource ()
{
  return impl->resource_name.c_str ();
}

void AsyncOperation::SetResource (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::rms::AsyncOperation::SetResource", "name");
    
  impl->resource_name = name;
}

/*
    Имя этапа обработки
*/

const char* AsyncOperation::Stage ()
{
  return impl->stage_name.c_str ();
}

void AsyncOperation::SetStage (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::rms::AsyncOperation::SetStage", "name");

  impl->stage_name = name;
}

/*
   Оповещение об обновлении
*/

void AsyncOperation::UpdateProgress ()
{
  impl->UpdateProgress ();
}

/*
    Подсчёт ссылок
*/

void AsyncOperation::AddRef ()
{
  impl->AddRef ();
}

void AsyncOperation::Release ()
{
  impl->Release ();
}

/*
    Выполнение операции из очереди
*/

bool AsyncOperation::DoStep ()
{
  return AsyncOperationManagerSingleton::Instance ()->DoStep (false);
}
