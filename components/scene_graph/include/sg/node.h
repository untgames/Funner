#ifndef SCENE_GRAPH_NODE_HEADER
#define SCENE_GRAPH_NODE_HEADER

#include <mathlib.h>
#include <xtl/functional_fwd>
#include <xtl/dynamic_cast_root.h>
#include <xtl/intrusive_ptr.h>

namespace scene_graph
{

//forward declarations
class Scene;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режим присоединения
///////////////////////////////////////////////////////////////////////////////////////////////////
enum NodeBindMode
{
  NodeBindMode_AddRef,  //при присоединении количество ссылок на родителя увеличивается на 1
  NodeBindMode_Capture, //при присоединении количество ссылок на родителя не увеличивается
  
  NodeBindMode_Default = NodeBindMode_AddRef
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режим поиска потомков
///////////////////////////////////////////////////////////////////////////////////////////////////
enum NodeSearchMode
{
  NodeSearchMode_OnNextSublevel, //поиск потомка только на следующем подуровне
  NodeSearchMode_OnAllSublevels, //поиск потомка на всех подуровнях
  
  NodeSearchMode_Default = NodeSearchMode_OnAllSublevels
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Пространство преобразований узла
///////////////////////////////////////////////////////////////////////////////////////////////////
enum NodeTransformSpace
{
  NodeTransformSpace_Local,  //преобразования в локальной системе координат узла
  NodeTransformSpace_Parent, //преобразования в системе координат родительского узла
  NodeTransformSpace_World   //преобразования в мировой системе координат
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режим обхода узлов
///////////////////////////////////////////////////////////////////////////////////////////////////
enum NodeTraverseMode
{
  NodeTraverseMode_TopToBottom, //обход узлов от родителей к потомкам
  NodeTraverseMode_BottomToTop, //обход узлов от потомков к родителям
  
  NodeTraverseMode_Default = NodeTraverseMode_TopToBottom
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///События узла
///////////////////////////////////////////////////////////////////////////////////////////////////
enum NodeEvent
{
  NodeEvent_AfterUpdate,       //срабатывает после обновления состояния узла
  NodeEvent_BeforeDestroy,     //срабатывает перед удалением узла
  NodeEvent_AfterDestroy,      //срабатывает после удаления узла
  NodeEvent_AfterBind,         //срабатывает после присоединения узла к родителю
  NodeEvent_BeforeUnbind,      //срабатывает перед отсоединением узла от родителя
  NodeEvent_AfterSceneAttach,  //срабатывает после присоединения объекта к сцене
  NodeEvent_BeforeSceneDetach, //срабатывает перед отсоединением объекта от сцены
  NodeEvent_AfterSceneChange,  //срабатывает после изменения сцены

  NodeEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///События узлов-потомков
///////////////////////////////////////////////////////////////////////////////////////////////////
enum NodeSubTreeEvent
{
  NodeSubTreeEvent_AfterBind,    //срабатывает после присоединения к узлу прямого или косвенного потомка
  NodeSubTreeEvent_BeforeUnbind, //срабатывает перед отсоединением потомка от узла прямого или косвенного потомка
  
  NodeSubTreeEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Оси узла
///////////////////////////////////////////////////////////////////////////////////////////////////
enum NodeOrt
{
  NodeOrt_X,   //ось Ox
  NodeOrt_Y,   //ось Oy
  NodeOrt_Z,   //ось Oz
  
  NodeOrt_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Узел сцены
///////////////////////////////////////////////////////////////////////////////////////////////////
  //добавить NodeIterator / NodeConstIterator
  //добавить блокировку на удаление объектов при оповещениях (скорее всего в Notify)!!!
class Node: public xtl::dynamic_cast_root
{
  friend class Scene;
  public: 
    typedef xtl::com_ptr <Node> Pointer;
    typedef xtl::com_ptr <const Node> ConstPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание узла
///////////////////////////////////////////////////////////////////////////////////////////////////  
    static Pointer Create ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сцена, которой принадлежит объект
///////////////////////////////////////////////////////////////////////////////////////////////////
          scene_graph::Scene* Scene ();
    const scene_graph::Scene* Scene () const;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя узла
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name    () const;
    void        SetName (const char* new_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   AddRef   () const;   //увеличить число ссылок на узел на 1
    void   Release  () const;   //уменьшить число ссылок на узел на 1
    size_t UseCount () const;   //количество активных ссылок на узел

///////////////////////////////////////////////////////////////////////////////////////////////////
///Связи для перемещения по данному уровню иерархии
///////////////////////////////////////////////////////////////////////////////////////////////////
    Pointer      Parent     ();
    ConstPointer Parent     () const;
    Pointer      FirstChild ();
    ConstPointer FirstChild () const;
    Pointer      LastChild  ();
    ConstPointer LastChild  () const;
    Pointer      PrevChild  ();
    ConstPointer PrevChild  () const;
    Pointer      NextChild  ();
    ConstPointer NextChild  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Настройка иерархических связей
///////////////////////////////////////////////////////////////////////////////////////////////////
      //установка соединения
    void BindToParent (Node&              parent,                                      //родительский узел
                       NodeBindMode       mode = NodeBindMode_Default,                 //режим присоединения
                       NodeTransformSpace invariant_space = NodeTransformSpace_Local); //инвариантное пространство преобразований

      //присоединение к сцене
    void BindToScene (scene_graph::Scene& scene,                                       //сцена
                      NodeBindMode        mode = NodeBindMode_Default,                 //режим присоединения
                      NodeTransformSpace  invariant_space = NodeTransformSpace_Local); //инвариантное пространство преобразований

      //разрыв соединения
    void Unbind (NodeTransformSpace invariant_space = NodeTransformSpace_Local);

      //отсоединение потомка
    void UnbindChild (const char* name, NodeTransformSpace invariant_space = NodeTransformSpace_Local);
    void UnbindChild (const char* name, NodeSearchMode find_mode, NodeTransformSpace invariant_space = NodeTransformSpace_Local);

      //отсоединение всех потомков    
    void UnbindAllChildren ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск потомка по имени
///////////////////////////////////////////////////////////////////////////////////////////////////
    Pointer      FindChild (const char* name, NodeSearchMode mode = NodeSearchMode_Default);
    ConstPointer FindChild (const char* name, NodeSearchMode mode = NodeSearchMode_Default) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Посещение узла с динамической диспетчеризацией по типу (применение паттерна Visitor)
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::basic_visitor<void> Visitor;

    void Accept (Visitor&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обход потомков
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (Node&)>       TraverseFunction;
    typedef xtl::function<void (const Node&)> ConstTraverseFunction;

    void Traverse  (const TraverseFunction&, NodeTraverseMode = NodeTraverseMode_Default);
    void Traverse  (const ConstTraverseFunction&, NodeTraverseMode = NodeTraverseMode_Default) const;
    void VisitEach (Visitor&, NodeTraverseMode = NodeTraverseMode_Default) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Положение узла
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetPosition   (const math::vec3f&);
    void               SetPosition   (float x, float y, float z);
    void               ResetPosition ();
    const math::vec3f& Position      () const;
    const math::vec3f& WorldPosition () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Ориентация узла
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetOrientation   (const math::quatf&);
    void SetOrientation   (float angle_in_degrees, float axis_x, float axis_y, float axis_z);
    void SetOrientation   (float pitch_in_degrees, float yaw_in_degrees, float roll_in_degrees); //углы Эйлера
    void SetOrientation   (const math::vec3f& local_axis,   //направление локальной оси
                           const math::vec3f& local_up,     //направление оси "вверх"
                           const math::vec3f& target_point, //целевая точка
                           NodeTransformSpace space = NodeTransformSpace_Local); //пространство, в котором находится целевая точка
    void ResetOrientation ();

    const math::quatf& Orientation      () const;
    const math::quatf& WorldOrientation () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Масштаб узла
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetScale   (const math::vec3f&);
    void               SetScale   (float sx, float sy, float sz);
    void               ResetScale ();
    const math::vec3f& Scale      () const;
    const math::vec3f& WorldScale () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Позиционирование узла
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LookTo (const math::vec3f& target_point, NodeTransformSpace = NodeTransformSpace_Local);
    void LookTo (const math::vec3f& target_point, const math::vec3f& up_vector, NodeTransformSpace = NodeTransformSpace_Local);
    void LookTo (const math::vec3f& target_point, NodeOrt direction, NodeOrt invariant, NodeTransformSpace = NodeTransformSpace_Local);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление наследованием преобразований
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetOrientationInherit  (bool state); //установка флага наследования родительской ориентации
    void SetScaleInherit        (bool state); //установка флага наследования родительского масштаба
    bool IsOrientationInherited () const;     //наследуется ли родительская ориентация
    bool IsScaleInherited       () const;     //наследуется ли родительский масштаб

///////////////////////////////////////////////////////////////////////////////////////////////////
///Кумулятивные преобразования
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Translate (const math::vec3f&, NodeTransformSpace = NodeTransformSpace_Parent);
    void Translate (float offset_x, float offset_y, float offset_z, NodeTransformSpace = NodeTransformSpace_Parent);
    void Rotate    (const math::quatf&, NodeTransformSpace = NodeTransformSpace_Parent);
    void Rotate    (float angle_in_degrees, float axis_x, float axis_y, float axis_z, NodeTransformSpace = NodeTransformSpace_Parent);
    void Rotate    (float pitch_in_degrees, float yaw_in_degrees, float roll_in_degrees, NodeTransformSpace = NodeTransformSpace_Parent);
    void Scale     (const math::vec3f&);
    void Scale     (float sx, float sy, float sz);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение матриц преобразования узла
///////////////////////////////////////////////////////////////////////////////////////////////////
    const math::mat4f& TransformationMatrix (NodeTransformSpace) const;

      //сокращённые версии
    const math::mat4f& LocalTM  () const { return TransformationMatrix (NodeTransformSpace_Local); }
    const math::mat4f& WorldTM  () const { return TransformationMatrix (NodeTransformSpace_World); }
    const math::mat4f& ParentTM () const { return TransformationMatrix (NodeTransformSpace_Parent); }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение осей
///////////////////////////////////////////////////////////////////////////////////////////////////
    math::vec3f Ort (NodeOrt ort, NodeTransformSpace space) const;

      //сокращённые версии
    math::vec3f LocalOrtX  () const { return Ort (NodeOrt_X, NodeTransformSpace_Local); }
    math::vec3f LocalOrtY  () const { return Ort (NodeOrt_Y, NodeTransformSpace_Local); }
    math::vec3f LocalOrtZ  () const { return Ort (NodeOrt_Z, NodeTransformSpace_Local); }
    math::vec3f WorldOrtX  () const { return Ort (NodeOrt_X, NodeTransformSpace_World); }
    math::vec3f WorldOrtY  () const { return Ort (NodeOrt_Y, NodeTransformSpace_World); }
    math::vec3f WorldOrtZ  () const { return Ort (NodeOrt_Z, NodeTransformSpace_World); }
    math::vec3f ParentOrtX () const { return Ort (NodeOrt_X, NodeTransformSpace_Parent); }
    math::vec3f ParentOrtY () const { return Ort (NodeOrt_Y, NodeTransformSpace_Parent); }
    math::vec3f ParentOrtZ () const { return Ort (NodeOrt_Z, NodeTransformSpace_Parent); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение матрицы преобразования узла object в системе координат данного узла
///////////////////////////////////////////////////////////////////////////////////////////////////
    math::mat4f ObjectTM (Node&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подписка на события Node
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (Node& sender, NodeEvent event)> EventHandler;
    typedef xtl::function<void (Node& sender, Node& child, NodeSubTreeEvent event)> SubTreeEventHandler;

    xtl::connection RegisterEventHandler (NodeEvent event, const EventHandler& handler) const;
    xtl::connection RegisterEventHandler (NodeSubTreeEvent event, const SubTreeEventHandler& handler) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление транзакциями обновления
///  Во время транзакции оповещения не посылаются их обработчикам, а аккумулируются и посылаются
///  при завершении транзакции
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BeginUpdate ();
    void EndUpdate   ();
    bool IsInUpdateTransaction () const;

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
            Node  ();
    virtual ~Node ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Оповещение об изменении узла
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateNotify () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Попытка посещения. Результат: обработано ли посещение
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> static bool TryAccept (T&, Visitor&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Метод, вызываемый при посещении данного узла
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AcceptCore (Visitor&);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///События
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AfterUpdateWorldTransformEvent () {} //после изменения положения объекта
    virtual void AfterSceneAttachEvent () {}          //после присоединения объекта к сцене
    virtual void BeforeSceneDetachEvent () {}         //перед отсоединением объекта от сцены

  private:
    static void DestroyNode (Node*);
    void SetScene (scene_graph::Scene* in_scene);

  private:
    Node (const Node&); //no impl
    Node& operator = (const Node&); //no impl

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вспомогательный класс, для автоматизации блокирования оповещений об обновлениях
///////////////////////////////////////////////////////////////////////////////////////////////////
class NodeUpdateLock
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    NodeUpdateLock  ();
    NodeUpdateLock  (Node&);
    NodeUpdateLock  (const NodeUpdateLock&);
    ~NodeUpdateLock ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    NodeUpdateLock& operator = (const NodeUpdateLock&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (NodeUpdateLock&);

  private:
    Node* node;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (NodeUpdateLock&, NodeUpdateLock&);

#include <sg/detail/node.inl>

}

#endif
