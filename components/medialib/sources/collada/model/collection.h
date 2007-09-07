#ifndef MEDIALIB_COLLADA_COLLECTION_HEADER
#define MEDIALIB_COLLADA_COLLECTION_HEADER

#include <media/collada/utility.h>
#include <common/strlib.h>
#include <stl/vector>
#include <xtl/functional>

namespace media
{

namespace collada
{

//forwards
class Surface;
class Node;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имена элементов коллекции
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Item> const char* get_collection_subname ();

template <> inline const char* get_collection_subname<Surface>      () { return "surfaces"; }
template <> inline const char* get_collection_subname<Node>         () { return "nodes"; }
template <> inline const char* get_collection_subname<Light>        () { return "lights"; }
template <> inline const char* get_collection_subname<Camera>       () { return "cameras"; }
template <> inline const char* get_collection_subname<InstanceMesh> () { return "meshes"; }
template <> inline const char* get_collection_subname<MorphTarget>  () { return "morph_targets"; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Уничтожение объекта
///////////////////////////////////////////////////////////////////////////////////////////////////
template <bool NeedDestroy> struct Destroyer
{
  template <class T> static void Destroy (T* obj) { delete obj; }
};

template <> struct Destroyer<false>
{
  template <class T> static void Destroy (T* obj) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Базовая коллекция
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Item, class StorableItem=Item, bool NeedDestroy=false>
class Collection: public ICollection<Item>
{
  public:
    enum { DEFAULT_ITEMS_RESERVE = 8 }; //количество элементов резервируемых по умолчанию
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Collection (Entity& in_owner)
      : owner (in_owner), name (common::format ("%s#%s", owner.EntityId (), get_collection_subname<Item> ()))
    {
      items.reserve (DEFAULT_ITEMS_RESERVE);
    }

    ~Collection ()
    {
      Clear ();
    }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Владелец
///////////////////////////////////////////////////////////////////////////////////////////////////
    ModelImpl* Owner () const { return owner.Owner (); }
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Идентификатор коллекции
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* EntityId () const { return name.c_str (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество элементов / проверка на пустоту
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size    () const { return items.size (); }
    bool   IsEmpty () const { return items.empty (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Доступ к элементу по индексу
///////////////////////////////////////////////////////////////////////////////////////////////////
    const Item& operator [] (size_t index) const
    {
      if (index >= items.size ())
        common::RaiseOutOfRange ("media::collada::Collection::operator []", "index", index, items.size ());

      return *items [index];
    }
    
    Item& operator [] (size_t index)
    {
      return const_cast<Item&> (const_cast<const Collection&> (*this) [index]);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск элемента в коллекции
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual int Find (Item& item) const
    {
      for (ItemArray::const_iterator i=items.begin (); i!=items.end (); ++i)
        if (*i == &item)
          return i - items.begin ();
          
      return -1;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление элемента в коллекцию
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t InsertCore (StorableItem& item)
    {
      items.push_back (&item);
      
      return items.size () - 1;
    }

    size_t Insert (Item& item)
    {
      if (item.Owner () != Owner ())
        raise_incompatible ("media::collada::Collection::Insert", item, *this);

      return InsertCore (item);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Удаление элемента из коллекции
///////////////////////////////////////////////////////////////////////////////////////////////////
   void Remove (size_t index)
   {
     if (index >= items.size ())
       common::RaiseOutOfRange ("media::collada::Collection::Remove", "index", index, items.size ());

     Destroyer<NeedDestroy>::Destroy (items [index]);

     items.erase (items.begin () + index);
   }
   
   void Remove (Item& item)
   {
     int index = Find (item);

     if (index != -1)
       Remove (index);
   }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Очистка коллекции
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ()
    {
      if (NeedDestroy)
      {
        for (ItemArray::iterator i=items.begin (); i!=items.end (); ++i)
          Destroyer<NeedDestroy>::Destroy (*i);
      }

      items.clear ();
    }    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перебор элементов библиотеки
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ForEach (const xtl::function<void (Item&)>& fn)
    {
      for (ItemArray::iterator i=items.begin (); i!=items.end (); ++i)
        fn (**i);
    }

    void ForEach (const xtl::function<void (const Item&)>& fn) const
    {
      for (ItemArray::const_iterator i=items.begin (); i!=items.end (); ++i)
        fn (**i);
    }    

  private:
    typedef stl::vector<StorableItem*> ItemArray;

  private:
    Entity&     owner; //владелец
    stl::string name;  //имя коллекции
    ItemArray   items; //элементы
};

}

}

#endif
