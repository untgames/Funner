#include "shared.h"

using namespace push_notifications;

/*
   Реализация сообщения
*/

struct Notification::Impl : xtl::reference_counter
{
  common::PropertyMap properties; //свойства
};

/*
   Конструктор / деструктор / копирование
*/

Notification::Notification ()
  : impl (new Impl ())
  {}

Notification::Notification (const Notification& source)
  : impl (source.impl)
{
  addref (impl);
}

Notification::~Notification ()
{
  release (impl);
}

Notification& Notification::operator = (const Notification& source)
{
  Notification(source).Swap (*this);

  return *this;
}

/*
   Cвойства
*/

const common::PropertyMap& Notification::Properties () const
{
  return impl->properties;
}

common::PropertyMap& Notification::Properties ()
{
  return impl->properties;
}

void Notification::SetProperties (const common::PropertyMap& properties)
{
  impl->properties = properties;
}

/*
   Обмен
*/

void Notification::Swap (Notification& source)
{
  stl::swap (impl, source.impl);
}

namespace push_notifications
{

/*
   Обмен
*/

void swap (Notification& notification1, Notification& notification2)
{
  notification1.Swap (notification2);
}

}
