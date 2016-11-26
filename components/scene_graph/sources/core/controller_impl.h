#ifndef SCENE_GRAPH_CONTROLLER_IMPL_HEADER
#define SCENE_GRAPH_CONTROLLER_IMPL_HEADER

namespace scene_graph
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вхождение контроллера в списки узла
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ControllerEntry
{
  Controller*      controller; //ссылка на объект контроллера
  ControllerEntry* next;       //следующий контроллер
  ControllerEntry* prev;       //предыдущий контроллер
  bool             updatable;  //является ли контроллер обновляемым
  
  ControllerEntry ()
    : controller ()
    , next ()
    , prev ()
    , updatable ()
  {
  }
};

}

#endif
