#ifndef MEDIALIB_COLLADA_LIBRARY_IMPL_HEADER
#define MEDIALIB_COLLADA_LIBRARY_IMPL_HEADER

namespace media
{

namespace collada
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация библиотеки ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Item> class LibraryImpl: public ILibrary<Item>
{
  typedef ILibrary<Item> Base;
  public:
    typedef typename Base::Iterator      Iterator;
    typedef typename Base::ConstIterator ConstIterator;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество элементов в библиотеке / проверка на пустоту
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size    () const { return impl.Size (); }
    bool   IsEmpty () const { return impl.IsEmpty (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Доступ к элементу по имени (в случае отсутствия - исключение)
///////////////////////////////////////////////////////////////////////////////////////////////////
    const Item& operator [] (const char* id) const
    {
      if (!id)
        common::raise_null_argument ("media::collada::ILibrary::operator []", "id");
        
      const Item* item = impl.Find (id);
      
      if (!item)
        common::raise_invalid_argument ("media::collada::ILibrary::operator []", "id", id);
        
      return *item;
    }

    Item& operator [] (const char* id)
    { 
      return const_cast<Item&> (const_cast<const LibraryImpl&> (*this).LibraryImpl::operator [] (id));
    }    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск элемента по имени (в случае отсутствия возвращает 0)
///////////////////////////////////////////////////////////////////////////////////////////////////
    Item*       Find (const char* id)       { return impl.Find (id); }
    const Item* Find (const char* id) const { return impl.Find (id); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение итератора
///////////////////////////////////////////////////////////////////////////////////////////////////
    Iterator      CreateIterator ()       { return impl.CreateIterator (); }
    ConstIterator CreateIterator () const { return impl.CreateConstIterator (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение имени хранимого ресурса
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* ItemId (const ConstIterator& i) const { return ResourceLibrary<Item>::ItemId (i); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление / удаление элементов в библиотеку / очистка библиотеки
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Insert (const char* id, Item& item) { impl.Insert (id, item); }
    void Remove (const char* id)             { impl.Remove (id); }
    void Clear  ()                           { impl.Clear (); }

  private:
    typedef ResourceLibrary<Item> Impl;
    
    Impl impl;
};

}

}

#endif
