#include "shared.h"

using namespace render::low_level::opengl;

/*
    Объект, оповещающий клиентов при удалении
*/

Trackable::Trackable ()
{
}

Trackable::~Trackable ()
{
  try
  {
    destroy_signal ();
  }
  catch (...)
  {
    //подавляем все исключения
  }
}

/*
    Регистрация обработчика события удаления объекта
*/

xtl::connection Trackable::RegisterDestroyHandler (xtl::slot<void ()>& slot)
{
  return destroy_signal.connect (slot);
}
