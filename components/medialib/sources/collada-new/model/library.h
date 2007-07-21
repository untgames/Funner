#include <media/collada/utility.h>
#include <common/strlib.h>
#include <stl/hash_map>
#include <stl/algorithm>
#include <xtl/functional>

namespace medialib
{

namespace collada
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Имена библиотек
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Item> const char* get_library_name ();

template <> const char* get_library_name<Effect>   () { return "library_effects"; }
template <> const char* get_library_name<Material> () { return "library_materials"; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Библиотека
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Item> class Library: public ILibrary<Item>
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Library (ModelImpl* in_owner) : owner (in_owner), gen_id (0) {}

    ~Library ()
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
    const char* EntityID () const { return get_library_name<Item> (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество элементов / проверка на пустоту
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size  () const { return items.size (); }
    bool   Empty () const { return items.empty (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Доступ к элементу по имени
///////////////////////////////////////////////////////////////////////////////////////////////////
    const Item& operator [] (const char* id) const
    {
      if (!id)
        RaiseNullArgument ("medialib::collada::Library::operator []", "id");
        
      ItemMap::const_iterator iter = items.find (id);
      
      if (iter == items.end ())
        RaiseInvalidArgument ("medialib::collada::Library::operator []", "id", id, "No item with this name in library");
        
      return *iter->second;
    }
    
    Item& operator [] (const char* id)
    {
      return const_cast<Item&> (const_cast<const Library&> (*this) [id]);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск элемента по имени
///////////////////////////////////////////////////////////////////////////////////////////////////
    const Item* Find (const char* id) const
    {
      if (!id)
        RaiseNullArgument ("medialib::collada::Library::Find", "id");
        
      ItemMap::const_iterator iter = items.find (id);

      if (iter == items.end ())
        return 0;

      return iter->second;
    }
    
    Item* Find (const char* id)
    {
      return const_cast<Item*> (const_cast<const Library&> (*this).Find (id));
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
///Создание элементов
///////////////////////////////////////////////////////////////////////////////////////////////////
    Item& Create (const char* id)
    {
      return *CreateCore (id ? id : format ("%s.item%u", get_library_name<Item> (), gen_id++).c_str ());
    }

    Item& Create (Effect& effect, const char* id)
    {
      return *CreateCore (effect, id ? id : format ("%s.item%u", get_library_name<Item> (), gen_id++).c_str ());
    }

    Item* CreateCore (const char* id)
    {
      ContructableItem* item = new ContructableItem (*this, id);
      
      try
      {
        items [id] = item;
        return item;
      }
      catch (...)
      {
        delete item;
        throw;
      }
    }
    
    Item* CreateCore (Effect& effect, const char* id)
    {
      ContructableItem* item = new ContructableItem (effect, *this, id);
      
      try
      {
        items [id] = item;
        return item;
      }
      catch (...)
      {
        delete item;
        throw;
      }
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сброс неиспользуемых элементов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Flush ()
    {
      RaiseNotImplemented ("medialib::collada::Library::Flush");
    }

  private:
    class ContructableItem: public Item
    {
      public:
        ContructableItem  (Library& library, const char* id) : Item (library, id) {}
        ~ContructableItem () {}

        //специализация для материала        
        ContructableItem  (collada::Effect& effect, Library& library, const char* id) : Item (effect, library, id) {}
    };
  
    typedef stl::hash_map<stl::hash_key<const char*>, ContructableItem*> ItemMap;
  
  private:
    ModelImpl* owner;   //владелец
    ItemMap    items;   //элементы
    size_t     gen_id;  //следующий номер при автогенерации имён
};

}

}
