#include "shared.h"

using namespace render::scene::interchange;

/*
    Константы
*/

const size_t DESIRED_FPS       = 60;
const size_t SPIN_WAIT_TIME_MS = 1000 / DESIRED_FPS;

/*
    Описание реализации очереди команд
*/

typedef syslib::SharedQueue<CommandQueueItem> Queue;

struct CommandQueue::Impl
{
  Queue queue; //очередь буферов

/// Конструктор
  Impl (size_t max_queue_size)
    : queue (max_queue_size, SPIN_WAIT_TIME_MS)
  {
    if (!max_queue_size)
      throw xtl::make_null_argument_exception ("", "max_queue_size");
  }
};

/*
    Конструктор / деструктор
*/

CommandQueue::CommandQueue (size_t max_queue_size)
  : impl (new Impl (max_queue_size))
{
}

CommandQueue::~CommandQueue ()
{
}

/*
    Размер очереди / проверка на пустоту
*/

size_t CommandQueue::Size () const
{
  return impl->queue.Size ();
}

bool CommandQueue::IsEmpty () const
{
  return impl->queue.IsEmpty ();
}

/*
    Помещение элемента в очередь / извлечение из очереди
*/

void CommandQueue::Push (const CommandQueueItem& command)
{
  impl->queue.Push (command);
}

bool CommandQueue::Pop (CommandQueueItem& command, size_t timeout)
{
  return impl->queue.TryPop (command, timeout);
}
