#ifndef RENDER_SCENE_INTERCHANGE_PROPERTY_SYNCHRONIZER_HEADER
#define RENDER_SCENE_INTERCHANGE_PROPERTY_SYNCHRONIZER_HEADER

#include <stl/auto_ptr.h>

#include <common/property_map.h>

#include <render/scene/interchange/streams.h>

namespace render
{

namespace scene
{

namespace interchange
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Слушатель синхронизатора карты свойств
///////////////////////////////////////////////////////////////////////////////////////////////////
class IPropertyMapWriterListener
{
  public:
    virtual void OnPropertyMapRemoved    (object_id_t id) = 0;
    virtual void OnPropertyLayoutRemoved (object_id_t id) = 0;

  protected:
    virtual ~IPropertyMapWriterListener () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Синхронизатор карт свойств
///////////////////////////////////////////////////////////////////////////////////////////////////
class PropertyMapWriter: public xtl::noncopyable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    PropertyMapWriter  (IPropertyMapWriterListener* listener = 0);
    ~PropertyMapWriter ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Запись карты свойств
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Write (OutputStream&, const common::PropertyMap&);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Автоматический синхронизатор карт свойств
///////////////////////////////////////////////////////////////////////////////////////////////////
class PropertyMapAutoWriter: public xtl::noncopyable
{
  struct MapDesc;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Синхронизатор
///////////////////////////////////////////////////////////////////////////////////////////////////
    class Synchronizer
    {
      friend class PropertyMapAutoWriter;
      public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
        Synchronizer  ();
        Synchronizer  (const Synchronizer&);
        ~Synchronizer ();

        Synchronizer& operator = (const Synchronizer&);

      private:
        Synchronizer (MapDesc*);

      private:
        MapDesc* impl;
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    PropertyMapAutoWriter  (IPropertyMapWriterListener* listener = 0);
    ~PropertyMapAutoWriter ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Слежение за картой свойств
///////////////////////////////////////////////////////////////////////////////////////////////////
    Synchronizer CreateSynchronizer (const common::PropertyMap&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Запись карты свойств (вручную)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Write (OutputStream&, const common::PropertyMap&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Синхронизация
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Write (OutputStream&);

  private:
    struct Impl;
    struct MapDescList;

  private:
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Приемник карт свойств
///////////////////////////////////////////////////////////////////////////////////////////////////
class PropertyMapReader: public xtl::noncopyable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    PropertyMapReader  ();
    ~PropertyMapReader ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение карты свойств
///////////////////////////////////////////////////////////////////////////////////////////////////
    common::PropertyMap GetProperties (object_id_t id) const;
    bool                HasProperties (object_id_t id) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обновление карты
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Read (InputStream&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Удаление карты свойств и лэйаута
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RemoveProperties (object_id_t id);
    void RemoveLayout     (object_id_t id);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}

#endif
