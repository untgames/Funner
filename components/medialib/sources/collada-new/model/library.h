#ifndef MEDIALIB_COLLADA_LIBRARY_HEADER
#define MEDIALIB_COLLADA_LIBRARY_HEADER

#include <media/collada/utility.h>
#include <common/strlib.h>
#include <stl/hash_map>
#include <xtl/functional>

namespace medialib
{

namespace collada
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имена библиотек
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Item> const char* get_library_name ();

template <> inline const char* get_library_name<Effect>   () { return "library_effects"; }
template <> inline const char* get_library_name<Material> () { return "library_materials"; }
template <> inline const char* get_library_name<Mesh>     () { return "library_meshes"; }
template <> inline const char* get_library_name<Morph>    () { return "library_morphs"; }
template <> inline const char* get_library_name<Skin>     () { return "library_skines"; }
template <> inline const char* get_library_name<Light>    () { return "library_lights"; }
template <> inline const char* get_library_name<Camera>   () { return "library_cameras"; }
template <> inline const char* get_library_name<Node>     () { return "library_nodes"; }
template <> inline const char* get_library_name<Scene>    () { return "library_scenes"; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Базовая библиотека
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Item> class ItemLibrary: public ILibrary<Item>
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ItemLibrary (ModelImpl* in_owner) : owner (in_owner), gen_id (0) {}

    ~ItemLibrary ()
    {
      for (ItemMap::iterator i=items.begin (); i!=items.end (); ++i)
        delete i->second;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Владелец
///////////////////////////////////////////////////////////////////////////////////////////////////
    ModelImpl* Owner () const { return owner; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Идентификатор коллекции
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* EntityId () const { return get_library_name<Item> (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество элементов / проверка на пустоту
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size    () const { return items.size (); }
    bool   IsEmpty () const { return items.empty (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Доступ к элементу по имени
///////////////////////////////////////////////////////////////////////////////////////////////////
    const Item& operator [] (const char* id) const
    {
      if (!id)
        common::RaiseNullArgument ("medialib::collada::Library::operator []", "id");
        
      ItemMap::const_iterator iter = items.find (id);
      
      if (iter == items.end ())
        common::RaiseInvalidArgument ("medialib::collada::Library::operator []", "id", id, "No item with this name in library");
        
      return *iter->second;
    }
    
    Item& operator [] (const char* id)
    {
      return const_cast<Item&> (const_cast<const ItemLibrary&> (*this) [id]);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск элемента по имени
///////////////////////////////////////////////////////////////////////////////////////////////////
    const Item* Find (const char* id) const
    {
      if (!id)
        common::RaiseNullArgument ("medialib::collada::Library::Find", "id");
        
      ItemMap::const_iterator iter = items.find (id);

      if (iter == items.end ())
        return 0;

      return iter->second;
    }
    
    Item* Find (const char* id)
    {
      return const_cast<Item*> (const_cast<const ItemLibrary&> (*this).Find (id));
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перебор элементов библиотеки
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ForEach (const xtl::function<void (Item&)>& fn)
    {
      for (ItemMap::iterator i=items.begin (); i!=items.end (); ++i)
        fn (*i->second);
    }

    void ForEach (const xtl::function<void (const Item&)>& fn) const
    {
      for (ItemMap::const_iterator i=items.begin (); i!=items.end (); ++i)
        fn (*i->second);
    }        

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сброс неиспользуемых элементов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Flush ()
    {
      common::RaiseNotImplemented ("medialib::collada::Library::Flush");
    }

  protected:
    class ContructableItem: public Item
    {
      public:
        template <class T1> ContructableItem  (T1& arg1, ModelImpl* owner, const char* id) : Item (arg1, owner, id) {}
                            ContructableItem  (ModelImpl* owner, const char* id) : Item (owner, id) {}
                            ~ContructableItem () {}
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление новых элементов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Insert (ContructableItem* item)
    {
      try
      {
        items [item->EntityId ()] = item;
      }
      catch (...)
      {
        delete item;
        throw;
      }
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание элементов
///////////////////////////////////////////////////////////////////////////////////////////////////
    Item* CreateCore (const char* id)
    {
      ContructableItem* item = new ContructableItem (owner, id ? id : common::format ("%s.item%u", get_library_name<Item> (), gen_id++).c_str ());
    
      Insert (item);
      
      return item;
    }
    
    template <class T1> Item* CreateCore (T1& arg1, const char* id)
    {
      ContructableItem* item = new ContructableItem (arg1, owner, id ? id : common::format ("%s.item%u", get_library_name<Item> (), gen_id++).c_str ());

      Insert (item);

      return item;
    }    
    
  private:
    typedef stl::hash_map<stl::hash_key<const char*>, ContructableItem*> ItemMap;
  
  private:
    ModelImpl* owner;   //владелец
    ItemMap    items;   //элементы
    size_t     gen_id;  //следующий номер при автогенерации имён
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Библиотека
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Item> class Library: public ItemLibrary<Item>
{
  public:
    Library (ModelImpl* owner) : ItemLibrary<Item> (owner) {}
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание элементов
///////////////////////////////////////////////////////////////////////////////////////////////////
    Item& Create (const char* id) { return *CreateCore (id); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Библиотека материалов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <> class Library<Material>: public ItemLibrary<Material>
{
  public:
    Library (ModelImpl* owner) : ItemLibrary<Material> (owner) {}
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание элементов
///////////////////////////////////////////////////////////////////////////////////////////////////
    Material& Create (Effect& effect, const char* id) { return *CreateCore (effect, id); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Библиотека морферов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <> class Library<Morph>: public ItemLibrary<Morph>
{
  public:
    Library (ModelImpl* owner) : ItemLibrary<Morph> (owner) {}
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание элементов
///////////////////////////////////////////////////////////////////////////////////////////////////
    Morph& Create (Mesh& base_mesh, const char* id) { return *CreateCore (base_mesh, id); }
};

}

}

#endif
