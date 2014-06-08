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
    size_t Size   () const;
    void   Resize (size_t size);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Максимальный размер буфера
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Capacity () const;
    void   Reserve  (size_t size);

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
    void SyncBuffers (low_level::IDevice& device, size_t first = 0);

  private:
    typedef xtl::uninitialized_storage<Item> ItemBuffer;

  private:
    ItemBuffer           src_buffer;
    LowLevelBufferPtr    dst_buffer;
    size_t               dst_buffer_size;
    low_level::UsageMode usage_mode;
    low_level::BindFlag  bind_flags;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Синхронизация буферов
///////////////////////////////////////////////////////////////////////////////////////////////////
void sync_buffers (low_level::IDevice& device, low_level::UsageMode usage_mode, low_level::BindFlag bind_flags, size_t offset, const void* src_data, size_t src_data_size, 
  size_t buffer_capacity, size_t& dst_buffer_size, LowLevelBufferPtr& dst_buffer);

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
inline size_t DynamicPrimitiveBuffer<T>::Size () const
{
  return src_buffer.size ();
}

template <class T>
inline void DynamicPrimitiveBuffer<T>::Resize (size_t size)
{
  src_buffer.resize (size);
}

template <class T>
inline size_t DynamicPrimitiveBuffer<T>::Capacity () const
{
  return src_buffer.capacity ();
}

template <class T>
inline void DynamicPrimitiveBuffer<T>::Reserve (size_t count)
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
inline void DynamicPrimitiveBuffer<T>::SyncBuffers (low_level::IDevice& device, size_t first)
{
  size_t offset = first * sizeof (Item);

  sync_buffers (device, usage_mode, bind_flags, offset, src_buffer.data () + offset, src_buffer.size () * sizeof (Item) - offset, src_buffer.capacity () * sizeof (Item),
    dst_buffer_size, dst_buffer);
}
