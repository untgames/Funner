#ifndef SCENE_GRAPH_MESH_HEADER
#define SCENE_GRAPH_MESH_HEADER

#include <sg/visual_model.h>

namespace scene_graph
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Статический меш
///////////////////////////////////////////////////////////////////////////////////////////////////
class StaticMesh: public VisualModel
{
  public:
    typedef xtl::com_ptr <StaticMesh> Pointer;
    typedef xtl::com_ptr <const StaticMesh> ConstPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание модели
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer Create ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка имени меша модели
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetMeshName  (const char* name);
    const char* MeshName     () const;
    size_t      MeshNameHash () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка ограничивающий тел
///////////////////////////////////////////////////////////////////////////////////////////////////
    using VisualModel::SetBoundBox;
    using VisualModel::SetInfiniteBounds;

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    StaticMesh  ();
    ~StaticMesh ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Метод, вызываемый при посещении данного объекта
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AcceptCore (Visitor&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Связывание свойств узла с методами узла
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BindProperties (common::PropertyBindingMap& bindings);    

  private:
    struct Impl;
    Impl* impl;
};

}

#endif
