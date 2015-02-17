///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание примитива рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> 
class DynamicPrimitiveBuffer: public xtl::noncopyable
{
  public:
    typedef T Item;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    DynamicPrimitiveBuffer (low_level::UsageMode usage_mode, low_level::BindFlag flags);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Низкроуровневый буфер
///////////////////////////////////////////////////////////////////////////////////////////////////
    const LowLevelBufferPtr& LowLevelBuffer () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Текущий размер буфера
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int Size   () const;
    void         Resize (unsigned int size);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Максимальный размер буфера
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int Capacity () const;
    void         Reserve  (unsigned int size);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение данных
///////////////////////////////////////////////////////////////////////////////////////////////////
          Item* Data ();
    const Item* Data () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Очистка буфера
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Синхронизация буферов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SyncBuffers (low_level::IDevice& device, unsigned int first = 0, bool sync_data = true);

  private:
    typedef xtl::uninitialized_storage<Item> ItemBuffer;

  private:
    ItemBuffer           src_buffer;
    LowLevelBufferPtr    dst_buffer;
    unsigned int         dst_buffer_size;
    low_level::UsageMode usage_mode;
    low_level::BindFlag  bind_flags;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Синхронизация буферов
///////////////////////////////////////////////////////////////////////////////////////////////////
void sync_buffers (low_level::IDevice& device, low_level::UsageMode usage_mode, low_level::BindFlag bind_flags, unsigned int offset, const void* src_data, unsigned int src_data_size,
  unsigned int buffer_capacity, unsigned int& dst_buffer_size, LowLevelBufferPtr& dst_buffer);

/*
    Реализация
*/

template <class T>
inline DynamicPrimitiveBuffer<T>::DynamicPrimitiveBuffer (low_level::UsageMode mode, low_level::BindFlag flags)
  : dst_buffer_size ()
  , usage_mode (mode)
  , bind_flags (flags)
{
}

template <class T> 
inline const LowLevelBufferPtr& DynamicPrimitiveBuffer<T>::LowLevelBuffer () const
{
  return dst_buffer;
}

template <class T> 
inline unsigned int DynamicPrimitiveBuffer<T>::Size () const
{
  return (unsigned int)src_buffer.size ();
}

template <class T>
inline void DynamicPrimitiveBuffer<T>::Resize (unsigned int size)
{
  src_buffer.resize (size);
}

template <class T>
inline unsigned int DynamicPrimitiveBuffer<T>::Capacity () const
{
  return (unsigned int)src_buffer.capacity ();
}

template <class T>
inline void DynamicPrimitiveBuffer<T>::Reserve (unsigned int count)
{
  src_buffer.reserve (count);
}

template <class T>
inline typename DynamicPrimitiveBuffer<T>::Item* DynamicPrimitiveBuffer<T>::Data ()
{
  return src_buffer.data ();
}

template <class T>
inline const typename DynamicPrimitiveBuffer<T>::Item* DynamicPrimitiveBuffer<T>::Data () const
{
  return src_buffer.data ();
}

template <class T>
inline void DynamicPrimitiveBuffer<T>::Clear ()
{
  src_buffer.resize (0, false);
}

template <class T>
inline void DynamicPrimitiveBuffer<T>::SyncBuffers (low_level::IDevice& device, unsigned int first, bool sync_data)
{
  unsigned int offset = first * sizeof (Item);

  sync_buffers (device, usage_mode, bind_flags, offset, sync_data ? src_buffer.data () + offset : (Item*)0, (unsigned int)(src_buffer.size () * sizeof (Item) - offset), (unsigned int)(src_buffer.capacity () * sizeof (Item)),
    dst_buffer_size, dst_buffer);
}
