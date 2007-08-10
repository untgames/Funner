#include "scene_object.h"

using namespace scene_graph;

/*
    Конструктор / деструктор
*/

SceneObject::SceneObject (scene_graph::Entity& in_entity)
  : entity (in_entity)
{
  prev_object = next_object = 0;
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
      //удаление объекта из цепочки объектов пространства
    
    prev_object->next_object = next_object;
    next_object->prev_object = prev_object;
    
    if (this == space->first_object)
    {
      if (prev_object != next_object) space->first_object = next_object;
      else                            space->first_object = 0;
    }
    
    space->objects_count--;
  }
  
    //установка нового пространства
    
  space = new_space;  

  if (!new_space)
    return;
  
    //добавление объекта в пространство
    
  if (space->first_object)
  {
    next_object = space->first_object;  
    prev_object = next_object->prev_object;
    next_object->prev_object = prev_object->next_object = this;
  }
  else space->first_object = next_object = prev_object = this;
  
  space->objects_count++;
}
