#ifndef SCENE_GRAPH_CONTROLLERS_LOOK_TO_NODE_POINT_HEADER
#define SCENE_GRAPH_CONTROLLERS_LOOK_TO_NODE_POINT_HEADER

#include <sg/controller.h>

namespace scene_graph
{

namespace controllers
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Контроллер выравнивания ориентации на точку узла
///////////////////////////////////////////////////////////////////////////////////////////////////
class LookToNodePoint: public Controller
{
  public:
    typedef xtl::com_ptr<LookToNodePoint>       Pointer;
    typedef xtl::com_ptr<const LookToNodePoint> ConstPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание контроллера
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer Create (Node& node);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка/получение функции рассчета углового ускорения узла
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<const math::vec3f& (const math::vec3f& current_velocity, const math::vec3f& distance, float dt)> AccelerationFunction;

    void                        SetAccelerationHandler (const AccelerationFunction& acceleration_function);
    const AccelerationFunction& AccelerationHandler    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Запуск выравнивания
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Start (const Node& node, const math::vec3f& node_space_position, NodeOrt look_axis, NodeOrt rotation_axis);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Остановка выравнивания
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Stop ();

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    LookToNodePoint  (Node& node);
    ~LookToNodePoint ();
    
  private:
    void Update (const TimeValue& dt);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

#endif
