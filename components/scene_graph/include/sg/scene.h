#ifndef SCENE_GRAPH_SCENE_HEADER
#define SCENE_GRAPH_SCENE_HEADER

#include <sg/entity.h>
#include <bv/plane_list.h>

namespace scene_graph
{

//forward declarations
class SceneObject;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сцена
///////////////////////////////////////////////////////////////////////////////////////////////////
class Scene : public xtl::dynamic_cast_root //Убрать в будущем!!!!
{
  friend class Entity;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Scene  ();
    ~Scene ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя сцены
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetName (const char* name);
    const char* Name    () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Корень сцены
///////////////////////////////////////////////////////////////////////////////////////////////////
          Node& Root ();
    const Node& Root () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество объектов в сцене
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t EntitiesCount () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обход объектов, принадлежащих сцене
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (Entity&)> TraverseFunction;
    typedef xtl::basic_visitor<void>      Visitor;

    void Traverse  (const TraverseFunction&) const;
    void Traverse  (INodeTraverser&) const;
    void VisitEach (Visitor&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обход объектов, принадлежащих сцене и входящих в ограничивающий объём
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Traverse  (const bound_volumes::aaboxf&, const TraverseFunction&) const;
    void Traverse  (const bound_volumes::aaboxf&, INodeTraverser&) const;
    void VisitEach (const bound_volumes::aaboxf&, Visitor&) const;
    void Traverse  (const bound_volumes::plane_listf&, const TraverseFunction&) const;
    void Traverse  (const bound_volumes::plane_listf&, INodeTraverser&) const;
    void VisitEach (const bound_volumes::plane_listf&, Visitor&) const;

  private:    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация объектов сцены
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Attach (SceneObject&);

  private:
    Scene (const Scene&); //no impl
    Scene& operator = (const Scene&); //no impl

  private:
    struct Impl;
    Impl* impl;
};

}

#endif
