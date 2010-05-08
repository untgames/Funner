#include "shared.h"

using namespace scene_graph;

/*
    Конструктор / деструктор
*/

SceneObject::SceneObject (scene_graph::Entity& in_entity)
  : entity (in_entity), space (0), prev_object (0), next_object (0)
{
}

SceneObject::~SceneObject ()
{
  BindToSpace (0);
}

/*
    Связывание объекта с пространством
*/

void SceneObject::BindToSpace (SceneSpace* new_space)
{
    //удаление объекта из пространства    

  if (space)
  {
    space->objects_count--;    
    
      //удаление объекта из цепочки объектов пространства
    
    prev_object->next_object = next_object;
    next_object->prev_object = prev_object;
    
    if (this == space->first_object)
    {
      if (space->objects_count) space->first_object = next_object;
      else                      space->first_object = 0;
    }    
  }
  
    //установка нового пространства
    
  space = new_space;  

  if (!new_space)
    return;
  
    //добавление объекта в пространство
    
  if (space->first_object)
  {
    next_object              = space->first_object;  
    prev_object              = next_object->prev_object;
    next_object->prev_object = prev_object->next_object = this;
  }
  else space->first_object = next_object = prev_object = this;
  
  space->objects_count++;
}
