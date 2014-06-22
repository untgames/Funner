#ifndef SCRIPTBINDS_SHARED_HEADER
#define SCRIPTBINDS_SHARED_HEADER

#include <stl/hash_set>
#include <stl/numeric>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/implicit_cast.h>
#include <xtl/lexical_cast.h>
#include <xtl/shared_ptr.h>
#include <xtl/trackable_ptr.h>

#include <bv/axis_aligned_box.h>

#include <common/async.h>
#include <common/component.h>
#include <common/console.h>
#include <common/file.h>
#include <common/log.h>
#include <common/property_map.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <math/basic_spline.h>
#include <math/matrix.h>
#include <math/quat.h>
#include <math/utility.h>
#include <math/vector.h>

#include <media/image.h>
#include <media/font_library.h>

#include <sg/scene_manager.h>
#include <sg/controllers/sync_physics_to_node.h>
#include <sg/controllers/animation.h>

#include <input/scene_input.h>

#include <physics/manager.h>

#include <push_notifications/push_notifications_center.h>

#include <social/session.h>

#include <store/store.h>

#include <script/bind.h>
#include <script/environment.h>
#include <script/library_manager.h>

namespace engine
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Узел дерева строк
///////////////////////////////////////////////////////////////////////////////////////////////////
class StringNode: public xtl::dynamic_cast_root
{
  public:
    typedef xtl::com_ptr<StringNode> Pointer;

  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer Create ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание копии
///////////////////////////////////////////////////////////////////////////////////////////////////
    Pointer Clone ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение имени/переименование
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name    () const;
    void        SetName (const char* new_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с атрибутами
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t      AttributesCapacity  () const;
    size_t      AttributesCount     () const;
    void        ReserveAttributes   (size_t new_size);
    const char* Attribute           (size_t index) const;
    void        SetAttribute        (size_t index, const char* value);
    size_t      AddAttribute        (const char* value);
    void        AddAttribute        (size_t index, const char* value);
    void        RemoveAttribute     (size_t index);
    void        RemoveAllAttributes ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с детьмя
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t  ChildrenCapacity  () const;
    size_t  ChildrenCount     () const;
    void    ReserveChildren   (size_t new_size);
    Pointer Child             (size_t index) const;
    size_t  AddChild          (StringNode& new_child);
    void    AddChild          (size_t index, StringNode& new_child);
    void    RemoveChild       (size_t index);
    void    RemoveAllChildren ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка/сохранение
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer LoadXml            (const char* file_name);
    static Pointer LoadXmlFromFile    (const char* file_name);
    static Pointer LoadXmlFromString  (const char* string);
    static Pointer LoadJsonFromFile   (const char* file_name);
    static Pointer LoadJsonFromString (const char* string);
           void    SaveXml            (const char* file_name);
           void    SafeSaveXml        (const char* file_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск узла
///////////////////////////////////////////////////////////////////////////////////////////////////
    Pointer FindNode (const char* name_to_find, const char* value, bool create_if_not_exist);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение значения нулевого атрибута дочернего узла с именем name
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Get (const char* name);
    const char* Get (const char* name, const char* default_value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  () const;
    void Release () const;

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор/деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    StringNode  ();
    ~StringNode ();

  private:
    StringNode (const StringNode&);              //no impl
    StringNode& operator = (const StringNode&);  //no impl

  private:
    struct Impl;
    Impl* impl;
};

void bind_common_background_copy_state (script::Environment&);
void bind_common_file_library          (script::Environment&);
void bind_common_string_tree           (script::Environment&);
void bind_common_signals               (script::Environment&);
void bind_common_action                (script::Environment&);
void bind_common_timer                 (script::Environment&);
void bind_common_async_library         (script::Environment&);
void bind_common_property_map          (script::Environment&);
void bind_common_crypto                (script::Environment&);
void bind_common_base64                (script::Environment&);
void bind_math_splines_library         (script::Environment&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Декларация преобразования типов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Derived, class Base>
class decl_sg_cast
{
  public:
    decl_sg_cast ()
    {
      xtl::singleton_default<xtl::declcast<Derived*, Base*, xtl::reinterpret_caster>, false>::instance ();
      xtl::singleton_default<xtl::declcast<Base*, Derived*, xtl::reinterpret_caster>, false>::instance ();
    }
};

}

#endif
