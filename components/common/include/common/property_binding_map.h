#ifndef COMMON_PROPERTY_BINDING_MAP_HEADER
#define COMMON_PROPERTY_BINDING_MAP_HEADER

#include <stl/string_fwd>

#include <common/property_map.h>

namespace xtl
{

namespace detail
{

//forward declaration
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct binder;

}

}

namespace math
{

//forward declarations
template <class T> class quat;
template <class T> class angle;

math::angle<float>  degree (float);
math::angle<double> degree (double);

}

namespace common
{

namespace detail
{

//implementation forwards
class IPropertySetter;
class IPropertyGetter;

}

//forward declaration
class PropertyBindingMapSynchronizer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Связывание свойства с методами его чтения / записи
///////////////////////////////////////////////////////////////////////////////////////////////////
class PropertyBinding
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    PropertyBinding  ();
    PropertyBinding  (const PropertyBinding&);
    ~PropertyBinding ();
    
    PropertyBinding& operator = (const PropertyBinding&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка setter / getter для свойства
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Fn>          void SetSetter (const Fn& fn); //signature like <void (const T& value)>
    template <class T, class Fn> void SetSetter (const Fn& fn); //signature like <void (const T& value)>    
    template <class Fn>          void SetGetter (const Fn& fn); //signature like <T ()>

    template <class Fn> const Fn* Setter () const;
    template <class Fn> const Fn* Getter () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка наличия setter / getter для свойства
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool HasSetter () const;
    bool HasGetter () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Сброс setter / getter для свойства
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ResetSetter ();
    void ResetGetter ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Синхронизация с картой свойств
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CopyFromPropertyMap (const PropertyMap& map, size_t property_index);     //установка значения свойства из карты свойств по индексу
    void CopyFromPropertyMap (const PropertyMap& map, const char* property_name); //установка значения свойства из карты свойств по индексу    
    void CopyToPropertyMap   (PropertyMap& map, size_t property_index) const;     //установка значения в карте свойств по индексу
    void CopyToPropertyMap   (PropertyMap& map, const char* property_name) const; //установка значения в карте свойств по имени    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (PropertyBinding&);

  private:
    void                     SetSetterCore (detail::IPropertySetter*);
    void                     SetGetterCore (detail::IPropertyGetter*);
    detail::IPropertyGetter* GetterCore    () const;
    detail::IPropertySetter* SetterCore    () const;

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (PropertyBinding&, PropertyBinding&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Слушатель карты связывания свойств
///////////////////////////////////////////////////////////////////////////////////////////////////
enum PropertyBindingMapEvent
{
  PropertyBindingMapEvent_OnLayoutUpdate, //вызывается после обновления расположения карты
  
  PropertyBindingMapEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Карта, сопоставляющая имени свойства методы установки и чтения его значения
///////////////////////////////////////////////////////////////////////////////////////////////////
class PropertyBindingMap
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    PropertyBindingMap  ();
    PropertyBindingMap  (const PropertyBindingMap&);
    ~PropertyBindingMap ();

    PropertyBindingMap& operator = (const PropertyBindingMap&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество свойств в карте
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size () const;  
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск индекса свойства
///////////////////////////////////////////////////////////////////////////////////////////////////
    int  IndexOf   (const char* name) const;
    bool IsPresent (const char* name) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя свойства
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* PropertyName     (size_t index) const;
    size_t      PropertyNameHash (size_t index) const;
    void        SetPropertyName  (size_t index, const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Связывание свойства
///////////////////////////////////////////////////////////////////////////////////////////////////
          common::PropertyBinding& PropertyBinding    (size_t index);
    const common::PropertyBinding& PropertyBinding    (size_t index) const;
          void                     SetPropertyBinding (size_t index, const common::PropertyBinding& binding);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление нового свойства
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t AddProperty (const char* name, const common::PropertyBinding&);
    
    template <class GetterFn>
    size_t AddProperty (const char*     name,    //property name
                        const GetterFn& getter); //signature like <T ()>
    
    template <class GetterFn, class SetterFn>
    size_t AddProperty (const char*     name,    //property name
                        const GetterFn& getter,  //signature like <T ()>
                        const SetterFn& setter); //signature like <void (const T& value)>
    
    template <class Value, class GetterFn, class SetterFn>
    size_t AddProperty (const char*     name,    //property name
                        const GetterFn& getter,  //signature like <T ()>
                        const SetterFn& setter); //signature like <void (const T& value)>

///////////////////////////////////////////////////////////////////////////////////////////////////
///Удаление свойств
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RemoveProperty (size_t index);
    void RemoveProperty (const char* name);
    void Clear          ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Синхронизация с картой свойств
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CopyFromPropertyMap (const PropertyMap& map); //установка значений свойств из карты свойств
    void CopyToPropertyMap   (PropertyMap& map) const; //установка значений в карте свойств
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание объекта синхронизации
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef PropertyBindingMapSynchronizer Synchronizer;

    Synchronizer CreateSynchronizer (PropertyMap&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание карты свойств
///////////////////////////////////////////////////////////////////////////////////////////////////
    PropertyMap CreatePropertyMap () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Подписка на события карты связывания свойств
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (PropertyBindingMapEvent event)> EventHandler;

    xtl::connection RegisterEventHandler (PropertyBindingMapEvent event, const EventHandler& handler) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Объект оповещения об удалении
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::trackable& Trackable () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (PropertyBindingMap&);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (PropertyBindingMap&, PropertyBindingMap&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение объекта оповещения об удалении
///////////////////////////////////////////////////////////////////////////////////////////////////
xtl::trackable& get_trackable (const PropertyBindingMap&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Синхронизатор (для уменьшения числа поисков по имени)
///  захватывает ссылки на обе карты
///////////////////////////////////////////////////////////////////////////////////////////////////
class PropertyBindingMapSynchronizer
{
 public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы/ деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    PropertyBindingMapSynchronizer  (PropertyBindingMap& binding_map, PropertyMap& property_map);
    PropertyBindingMapSynchronizer  (const PropertyBindingMapSynchronizer&);
    ~PropertyBindingMapSynchronizer ();

    PropertyBindingMapSynchronizer& operator = (const PropertyBindingMapSynchronizer&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Синхронизация с картой свойств
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CopyFromPropertyMap () const; //установка значений свойств из карты свойств
    void CopyToPropertyMap   () const; //установка значений в карте свойств

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен    
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (PropertyBindingMapSynchronizer&);
    
  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен    
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (PropertyBindingMapSynchronizer&, PropertyBindingMapSynchronizer&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Копирование значения свойства из PropertyMap в переменную указанного типа
///////////////////////////////////////////////////////////////////////////////////////////////////
void copy_from (const PropertyMap& map, size_t property_index, stl::string& value);
void copy_from (const PropertyMap& map, size_t property_index, const char*& value);
void copy_from (const PropertyMap& map, size_t property_index, signed char& value);
void copy_from (const PropertyMap& map, size_t property_index, unsigned char& value);
void copy_from (const PropertyMap& map, size_t property_index, short& value);
void copy_from (const PropertyMap& map, size_t property_index, unsigned short& value);
void copy_from (const PropertyMap& map, size_t property_index, int& value);
void copy_from (const PropertyMap& map, size_t property_index, unsigned int& value);
void copy_from (const PropertyMap& map, size_t property_index, long& value);
void copy_from (const PropertyMap& map, size_t property_index, unsigned long& value);
void copy_from (const PropertyMap& map, size_t property_index, float& value);
void copy_from (const PropertyMap& map, size_t property_index, double& value);
void copy_from (const PropertyMap& map, size_t property_index, long double& value);
void copy_from (const PropertyMap& map, size_t property_index, bool& value);

template <class T>
void copy_from (const PropertyMap& map, size_t property_index, math::vector<T, 2>& value);

template <class T>
void copy_from (const PropertyMap& map, size_t property_index, math::vector<T, 3>& value);

template <class T>
void copy_from (const PropertyMap& map, size_t property_index, math::vector<T, 4>& value);

template <class T>
void copy_from (const PropertyMap& map, size_t property_index, math::matrix<T, 2>& value);

template <class T>
void copy_from (const PropertyMap& map, size_t property_index, math::matrix<T, 3>& value);

template <class T>
void copy_from (const PropertyMap& map, size_t property_index, math::matrix<T, 4>& value);

template <class T>
void copy_from (const PropertyMap& map, size_t property_index, math::quat<T>& value);

template <class T>
void copy_from (const PropertyMap& map, size_t property_index, math::angle<T>& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вспомогательный селектор свойства в карте
///////////////////////////////////////////////////////////////////////////////////////////////////
struct PropertySelector
{
  size_t      index; //индекс свойства
  const char* name;  //имя свойства
  
///Конструктор по умолчанию
  PropertySelector ();
  
///Конструктор по индексу
  PropertySelector (size_t index);  
  
///Конструктор по имени
  PropertySelector (const char* name);  

///Наличие имени
  bool HasName () const;

///Наличие индекса
  bool HasIndex () const;  
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка свойства по селектору
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> void set_property (PropertyMap&, const PropertySelector& selector, const T& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Копирование значения свойства из переменной указанного типа в PropertyMap
///////////////////////////////////////////////////////////////////////////////////////////////////
void copy_to (const char* value, PropertyMap& map, const PropertySelector& property_selector);
void copy_to (const stl::string& value, PropertyMap& map, const PropertySelector& property_selector);
void copy_to (signed char value, PropertyMap& map, const PropertySelector& property_selector);
void copy_to (unsigned char value, PropertyMap& map, const PropertySelector& property_selector);
void copy_to (short value, PropertyMap& map, const PropertySelector& property_selector);
void copy_to (unsigned short value, PropertyMap& map, const PropertySelector& property_selector);
void copy_to (int value, PropertyMap& map, const PropertySelector& property_selector);
void copy_to (unsigned int value, PropertyMap& map, const PropertySelector& property_selector);
void copy_to (long value, PropertyMap& map, const PropertySelector& property_selector);
void copy_to (unsigned long value, PropertyMap& map, const PropertySelector& property_selector);
void copy_to (float value, PropertyMap& map, const PropertySelector& property_selector);
void copy_to (double value, PropertyMap& map, const PropertySelector& property_selector);
void copy_to (long double value, PropertyMap& map, const PropertySelector& property_selector);
void copy_to (bool value, PropertyMap& map, const PropertySelector& property_selector);

template <class T>
void copy_to (const math::vector<T, 2>& value, PropertyMap& map, const PropertySelector& property_selector);

template <class T>
void copy_to (const math::vector<T, 3>& value, PropertyMap& map, const PropertySelector& property_selector);

template <class T>
void copy_to (const math::vector<T, 4>& value, PropertyMap& map, const PropertySelector& property_selector);

template <class T>
void copy_to (const math::matrix<T, 2>& value, PropertyMap& map, const PropertySelector& property_selector);

template <class T>
void copy_to (const math::matrix<T, 3>& value, PropertyMap& map, const PropertySelector& property_selector);

template <class T>
void copy_to (const math::matrix<T, 4>& value, PropertyMap& map, const PropertySelector& property_selector);

template <class T>
void copy_to (const math::quat<T>& value, PropertyMap& map, const PropertySelector& property_selector);

template <class T>
void copy_to (const math::angle<T>& value, PropertyMap& map, const PropertySelector& property_selector);

#include <common/detail/property_binding_map.inl>

}

#endif
