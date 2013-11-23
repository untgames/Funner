#include "shared.h"

using namespace render::scene;
using namespace render::scene::server;

/*
    Описание реализации отображаемой модели
*/

struct VisualModel::Impl
{
  NodePtr scissor; //область отсечения
};

/*
    Конструкторы / деструктор / присваивание
*/

VisualModel::VisualModel ()
  : impl (new Impl)
{
}

VisualModel::~VisualModel ()
{
}

/*
    Область отсечения
*/

void VisualModel::SetScissor (Node* node)
{
  impl->scissor = node;
}

Node* VisualModel::Scissor () const
{
  return impl->scissor.get ();
}
