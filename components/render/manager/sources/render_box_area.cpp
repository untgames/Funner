#include "shared.h"

using namespace render::manager;

/*
    Конструкторы
*/

BoxAreaImpl::BoxAreaImpl ()
{
}

BoxAreaImpl::BoxAreaImpl (const render::manager::Box& in_box)
  : box (in_box)
{
}
    
/*
    Размеры области
*/

void BoxAreaImpl::SetBox (const render::manager::Box& in_box)
{
  box = in_box;
}
