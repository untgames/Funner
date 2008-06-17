#include <exception>

#include <stl/hash_map>
#include <stl/queue>

#include <xtl/function.h>

#include <common/action_queue.h>

using namespace common;

namespace
{

struct Action
{
  size_t                     action_id;
  size_t                     next_time;
  size_t                     period;
  size_t                     count;      //количество в очереди
  bool                       is_deleted;
  ActionQueue::ActionHandler handler;

  Action ()
    : period (0), is_deleted (false) 
    {}

  Action (size_t in_action_id, size_t in_next_time, size_t in_period, const ActionQueue::ActionHandler& in_handler) 
    : action_id (in_action_id), next_time (in_next_time), period (in_period), count (0), is_deleted (false), handler (in_handler) 
    {}
};

void dummy_handler (const char*)
{
}

}

/*
   Описание реализации класса ActionQueue
*/

struct ActionQueue::Impl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Impl () 
      : cur_time (0), action_queue (&cur_time)
      {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление/удаление действий
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetAction (size_t action_id, time_t first_time, time_t period, const ActionHandler& handler)
    {
      ActionMapIterator iter = action_map.find (action_id);

      if (iter == action_map.end ())
        iter = action_map.insert_pair (action_id, Action (action_id, first_time, period, handler)).first;
      else
      {
        iter->second.next_time = first_time;
        iter->second.period    = period;
        iter->second.handler   = handler;
      }

      action_queue.push (ActionQueueElement (iter, first_time));
      iter->second.count++;
      iter->second.is_deleted = false;
    }

    void RemoveAction (size_t action_id)
    {
      ActionMapIterator iter = action_map.find (action_id);

      if (iter == action_map.end ())
        return;

      iter->second.is_deleted = true;
    }

    void Clear ()
    {
      action_map.clear ();
      while (!action_queue.empty ())
        action_queue.pop ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение состояний
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsEmpty () const
    {
      return action_map.empty ();
    }

    bool HasAction (size_t action_id) const
    {
      ActionMap::const_iterator iter = action_map.find (action_id);

      if (iter == action_map.end () || iter->second.is_deleted)
        return false;

      return true;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Инициация действий (no throw)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DoActions (time_t time, const LogHandler& handler)
    {
      time_t previous_time = cur_time, time_diff = time - cur_time;

      while (!action_queue.empty () && (((action_queue.top ().next_time - previous_time) <= time_diff) || action_queue.top ().iterator->second.is_deleted))
      {
        ActionMapIterator iter = action_queue.top ().iterator;

        if (action_queue.top ().next_time != iter->second.next_time)
        {
          Pop ();
        
          continue;
        }

        if (iter->second.is_deleted)
        {
          Pop ();

          continue;
        }

        if ((iter->second.next_time - previous_time) <= time_diff)
        {
          try
          {
            iter->second.handler (iter->second.action_id);
          }
          catch (std::exception& e)
          {
            LogMessage (e.what (), handler);
          }
          catch (...)
          {
            LogMessage ("Unknown exception", handler);
          }

          if (iter->second.period)
            SetAction (iter->second.action_id, iter->second.next_time + iter->second.period, iter->second.period, iter->second.handler);
          else
            iter->second.is_deleted = true;

          Pop ();
        }
      }

      cur_time = time;
    }

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Удаление элемента из начала очереди
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Pop ()
    {
      ActionMapIterator iter = action_queue.top ().iterator;
      
      action_queue.pop ();          
      iter->second.count--;

      if (!iter->second.count)
        action_map.erase (iter);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Логгирование
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LogMessage (const char* message, const LogHandler& handler)
    {
      try
      {
        handler (message);
      }
      catch (...)
      {
      }
    }

  private:
    typedef stl::hash_map<size_t, Action> ActionMap;
    typedef ActionMap::iterator           ActionMapIterator;

    struct ActionQueueElement
    {
      ActionMapIterator iterator;
      time_t            next_time;

      ActionQueueElement (ActionMapIterator& in_iterator, time_t in_next_time)
        : iterator (in_iterator), next_time (in_next_time)
        {}
    };

    struct ActionMapComparator
    {
      ActionMapComparator (time_t *in_cur_time) 
        : cur_time (in_cur_time)
        {}

      bool operator () (const ActionQueueElement& left, const ActionQueueElement& right)
      {
        return ((left.next_time - *cur_time) > (right.next_time - *cur_time));
      }

      time_t* cur_time;
    };

    typedef stl::priority_queue<ActionQueueElement, stl::vector<ActionQueueElement>, ActionMapComparator> ActionQueue;

  private:
    time_t      cur_time;
    ActionMap   action_map;
    ActionQueue action_queue;
};

/*
   Конструкторы / деструктор
*/

ActionQueue::ActionQueue ()
  : impl (new Impl)
  {}

ActionQueue::~ActionQueue ()
{
  delete impl;
}

/*
   Добавление/удаление действий
*/

void ActionQueue::SetAction (size_t action_id, time_t target_time, const ActionHandler& handler)
{
  impl->SetAction (action_id, target_time, 0, handler);
}

void ActionQueue::SetAction (size_t action_id, time_t first_time, time_t period, const ActionHandler& handler)
{
  impl->SetAction (action_id, first_time, period, handler);
}

void ActionQueue::RemoveAction (size_t action_id)
{
  impl->RemoveAction (action_id);
}

void ActionQueue::Clear ()
{
  impl->Clear ();
}

/*
   Получение состояний
*/

bool ActionQueue::IsEmpty () const
{
  return impl->IsEmpty ();
}

bool ActionQueue::HasAction (size_t action_id) const
{
  return impl->HasAction (action_id);
}

/*
   Инициация действий
*/

void ActionQueue::DoActions (time_t time, const LogHandler& handler)
{
  impl->DoActions (time, handler);
}

void ActionQueue::DoActions (time_t time)
{
  impl->DoActions (time, &dummy_handler);
}
