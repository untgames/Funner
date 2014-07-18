#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

/*
    Описание реализации области отсечения
*/

struct Scissor::Impl
{
  manager::BoxArea area; //область отсечения
};

/*
    Конструктор / деструктор
*/

Scissor::Scissor ()
  : impl (new Impl)
{
}

Scissor::~Scissor ()
{
}

/*
    Область
*/

const manager::BoxArea& Scissor::Area ()
{
  return impl->area;
}

/*
    Обход
*/

void Scissor::VisitCore (ISceneVisitor& visitor)
{
  visitor.Visit (*this);
}

/*
    Оповещение об обновлении ограничивающего тела
*/

void Scissor::OnUpdateBounds ()
{
  const bound_volumes::aaboxf& box = BoundBox ();

  impl->area.SetBox (box.minimum (), box.maximum ());
}
