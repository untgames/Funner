#include "shared.h"

using namespace render::scene::interchange;

/*
    Constants
*/

const size_t SPIN_WAIT_TIME_MS = 0; //do not use spin lock for this queue now, because in most cases (render or logic thread is significantly faster than vsync interval) we just consumes CPU time without performance gain

/*
   Command queue implementation
*/

typedef syslib::SharedQueue<CommandQueueItem> Queue;

struct CommandQueue::Impl: public xtl::reference_counter
{
  Queue queue; //buffers queue

/// Constructor
  Impl (size_t max_queue_size)
    : queue (max_queue_size, SPIN_WAIT_TIME_MS)
  {
    if (!max_queue_size)
      throw xtl::make_null_argument_exception ("", "max_queue_size");
  }
};

/*
    Constructor / destructor
*/

CommandQueue::CommandQueue (size_t max_queue_size)
  : impl (new Impl (max_queue_size))
{
}

CommandQueue::CommandQueue (const CommandQueue& queue)
  : impl (queue.impl)
{
  addref (impl);
}

CommandQueue::~CommandQueue ()
{
  release (impl);
}

CommandQueue& CommandQueue::operator = (const CommandQueue& queue)
{
  CommandQueue tmp (queue);

  stl::swap (impl, tmp.impl);

  return *this;
}

/*
    Queue size / check for emptiness
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
    Push element to queue / pop from queue
*/

void CommandQueue::Push (const CommandQueueItem& command)
{
  impl->queue.Push (command);
}

bool CommandQueue::Pop (CommandQueueItem& command, size_t timeout)
{
  return impl->queue.TryPop (command, timeout);
}
