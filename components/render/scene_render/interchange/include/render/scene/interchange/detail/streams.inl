/*
    RawArray
*/

template <class T>
inline RawArray<T>::RawArray (const T* in_data, size_t in_size)
  : size (in_size)
  , data (in_data)
{
}

/*
    OutputStream
*/

/*
    Конструктор / деструктор
*/

inline OutputStream::OutputStream ()
{
  Reset (buffer);
}

inline OutputStream::~OutputStream ()
{
}

/*
    Сброс
*/

inline void OutputStream::Reset (const CommandBuffer& in_buffer)
{
  buffer        = in_buffer;
  pos           = reinterpret_cast<char*> (buffer.Data ());
  command_start = pos;
  buffer_start  = pos;
  buffer_end    = pos + buffer.Size ();
}

/*
    Обмен
*/

inline void OutputStream::Swap (CommandBuffer& in_buffer)
{
  CommandBuffer old_buffer = buffer;

  size_t size = pos - reinterpret_cast<char*> (buffer.Data ());

  Reset (in_buffer);

  old_buffer.Resize (size);

  in_buffer = old_buffer;
}

namespace detail
{

template <size_t Size> inline size_t aligned_size ()
{
  return (Size + DATA_ALIGN - 1) & ~(DATA_ALIGN - 1);
}

inline size_t aligned_size (size_t size)
{
  return (size + DATA_ALIGN - 1) & ~(DATA_ALIGN - 1);
}

}

/*
    Сериализация заголовка и конца команды
*/

inline void OutputStream::BeginCommand (command_id_t id)
{
  static const size_t size = detail::aligned_size<sizeof (Command)> ();

  EnsureSpaceAvailable (size);

  command_start  = pos;
  pos           += size;

  Command* command = reinterpret_cast<Command*> (command_start);

  command->command_id = id;
}

inline void OutputStream::EndCommand ()
{
  Command* command = reinterpret_cast<Command*> (command_start);

  command->command_size = pos - command_start;
}

/*
    Подготовка достаточного места в буфере для записи
*/

inline void OutputStream::EnsureSpaceAvailable (size_t size)
{
  if (size_t (buffer_end - pos) < size)
    Resize (buffer.Size () + size);
}

inline void OutputStream::Resize (size_t new_size)
{
  size_t pos_offset           = pos - buffer_start,
         command_start_offset = command_start - buffer_start;

  buffer.Resize (new_size, true);

  buffer_start  = reinterpret_cast<char*> (buffer.Data ());
  pos           = buffer_start + pos_offset;
  command_start = buffer_start + command_start_offset;
  buffer_end    = buffer_start + buffer.Size ();
}

/*
    Запись блока данных
*/

inline void OutputStream::WriteData (const void* data, size_t size)
{
  const size_t aligned_size = detail::aligned_size (size);

  EnsureSpaceAvailable (aligned_size);
  WriteDataUnsafe      (data, size);

  pos += aligned_size - size;
}

inline void OutputStream::WriteDataUnsafe (const void* data, size_t size)
{
  memcpy (pos, data, size);

  pos += size;
}

template <class T> inline void OutputStream::Write (const T& value)
{
  static const size_t aligned_size = detail::aligned_size<sizeof (T)> ();

  EnsureSpaceAvailable (aligned_size);
  WriteDataUnsafe      (&value, sizeof (T));

  pos += aligned_size - sizeof (T);
}

/*
    Пропуск
*/

inline void OutputStream::Skip (size_t size)
{
  if (!size)
    return;

  EnsureSpaceAvailable (size);

  pos += size;
}

/*
    Позиция потока
*/

inline void OutputStream::SetPosition (size_t position)
{
  if (position > buffer.Size ())
    throw xtl::format_operation_exception ("render::scene::interchange::OutputStream::SetPosition", "Can't set output stream position to %u because current stream size is %u", position, buffer.Size ());

  pos = buffer_start + position;
}

inline size_t OutputStream::Position () const
{
  return pos - buffer_start;
}

/*
    InputStream
*/

/*
    Конструктор / деструктор
*/

inline InputStream::InputStream ()
{
  Reset (buffer);
}

inline InputStream::~InputStream ()
{
}

/*
    Сброс
*/

inline void InputStream::Reset (const CommandBuffer& in_buffer)
{
  buffer       = in_buffer;
  pos          = reinterpret_cast<const char*> (buffer.Data ());
  buffer_start = pos;
  buffer_end   = pos + buffer.Size ();
}

/*
    Количество доступных для считывания данных
*/

inline size_t InputStream::Available () const
{
  return buffer_end - pos;
}

/*
    Чтение блока данных
*/

inline void InputStream::ReadData (void* data, size_t size)
{
  const size_t aligned_size = detail::aligned_size (size);

  if (aligned_size <= Available ())
  {
    ReadDataUnsafe (data, size);

    pos += aligned_size - size;
  }
  else
  {
    throw xtl::format_operation_exception ("render::scene::interchange::InputStream::ReadData(void*,size_t)", "Can't read %u bytes from input stream with %u bytes available", aligned_size, Available ());
  }
}

inline void InputStream::ReadDataUnsafe (void* data, size_t size)
{
  memcpy (data, pos, size);

  pos += size;
}

inline const void* InputStream::ReadData (size_t size)
{
  const size_t aligned_size = detail::aligned_size (size);

  if (aligned_size <= Available ())
  {
    const void* result = pos;

    pos += aligned_size;

    return result;
  }
  else
  {
    throw xtl::format_operation_exception ("render::scene::interchange::InputStream::ReadData(size_t)", "Can't read %u bytes from input stream with %u bytes available", aligned_size, Available ());
  }
}

template <class T> inline const T& InputStream::Read ()
{
  static const size_t aligned_size = detail::aligned_size<sizeof (T)> ();

  if (aligned_size <= Available ())
  {
    const void* result = pos;

    pos += aligned_size;

    return *reinterpret_cast<const T*> (result);
  }
  else
  {
    throw xtl::format_operation_exception ("render::scene::interchange::InputStream::Read<T>", "Can't read %u bytes from input stream with %u bytes available", aligned_size, Available ());
  }
}

/*
    Пропуск
*/

inline void InputStream::Skip (size_t size)
{
  if (size <= Available ())
  {
    pos += size;
  }
  else
  {
    throw xtl::format_operation_exception ("render::scene::interchange::InputStream::Skip", "Can't skip %u bytes from input stream with %u bytes available", size, Available ());
  }
}

/*
    Позиция потока
*/

inline void InputStream::SetPosition (size_t position)
{
  if (position > buffer.Size ())
    throw xtl::format_operation_exception ("render::scene::interchange::InputStream::SetPosition", "Can't set input stream position to %u because current stream size is %u", position, buffer.Size ());

  pos = buffer_start + position;
}

inline size_t InputStream::Position () const
{
  return pos - buffer_start;
}

/*
    Сериализация базовых типов
*/

inline void write (OutputStream& s, bool value)                  { s.Write (static_cast<bool8> (value)); }
inline void write (OutputStream& s, int64 value)                 { s.Write (value); }
inline void write (OutputStream& s, uint64 value)                { s.Write (value); }
inline void write (OutputStream& s, int32 value)                 { s.Write (value); }
inline void write (OutputStream& s, uint32 value)                { s.Write (value); }
inline void write (OutputStream& s, int16 value)                 { s.Write (value); }
inline void write (OutputStream& s, uint16 value)                { s.Write (value); }
inline void write (OutputStream& s, int8 value)                  { s.Write (value); }
inline void write (OutputStream& s, uint8 value)                 { s.Write (value); }
inline void write (OutputStream& s, float32 value)               { s.Write (value); }
inline void write (OutputStream& s, NodeType value)              { write (s, static_cast<int32> (value)); }
inline void write (OutputStream& s, const LightParams& value)    { s.Write (value); }
inline void write (OutputStream& s, const PageCurlParams& value) { s.Write (value); }

inline       bool            read (InputStream& s, xtl::type<bool>)           { return s.Read<bool8> () != 0; }
inline const int64&          read (InputStream& s, xtl::type<int64>)          { return s.Read<int64> (); }
inline const uint64&         read (InputStream& s, xtl::type<uint64>)         { return s.Read<uint64> (); }
inline const int32&          read (InputStream& s, xtl::type<int32>)          { return s.Read<int32> (); }
inline const uint32&         read (InputStream& s, xtl::type<uint32>)         { return s.Read<uint32> (); }
inline const int16&          read (InputStream& s, xtl::type<int16>)          { return s.Read<int16> (); }
inline const uint16&         read (InputStream& s, xtl::type<uint16>)         { return s.Read<uint16> (); }
inline const int8&           read (InputStream& s, xtl::type<int8>)           { return s.Read<int8> (); }
inline const uint8&          read (InputStream& s, xtl::type<uint8>)          { return s.Read<uint8> (); }
inline const float32&        read (InputStream& s, xtl::type<float32>)        { return s.Read<float32> (); }
inline const Command&        read (InputStream& s, xtl::type<Command>)        { return s.Read<Command> (); }
inline const NodeType&       read (InputStream& s, xtl::type<NodeType>)       { return *reinterpret_cast<const NodeType*> (&read (s, xtl::type<int32> ())); }
inline const LightParams&    read (InputStream& s, xtl::type<LightParams>)    { return s.Read<LightParams> (); }
inline const PageCurlParams& read (InputStream& s, xtl::type<PageCurlParams>) { return s.Read<PageCurlParams> (); }

template <class T>
inline const T& read (InputStream& s, xtl::type<T&>)
{
  return read (s, xtl::type<T> ());
}

template <class T>
inline const T& read (InputStream& s, xtl::type<const T&>)
{
  return read (s, xtl::type<T> ());
}

inline void write (OutputStream& s, const char* str)
{
  if (!str)
    str = "";

  int32 length = strlen (str);

  write (s, length);

  s.WriteData (str, length + 1);
}

inline const char* read (InputStream& s, xtl::type<const char*>)
{
  int32 length = s.Read<int32> ();

  return reinterpret_cast<const char*> (s.ReadData (length + 1));
}

template <class T, unsigned int Size>
inline void write (OutputStream& s, const math::vector<T, Size>& value)
{
  s.Write (value);
}

template <class T, unsigned int Size>
inline void write (OutputStream& s, const math::matrix<T, Size>& value)
{
  s.Write (value);
}

template <class T>
inline void write (OutputStream& s, const math::quat<T>& value)
{
  s.Write (value);
}

template <class T>
inline void write (OutputStream& s, const math::angle<T>& value)
{
  s.Write (value);
}

template <class T>
inline void write (OutputStream& s, const bound_volumes::axis_aligned_box<T>& value)
{
  s.Write (value);
}

template <class T, unsigned int Size>
inline const math::vector<T, Size>& read (InputStream& s, xtl::type<math::vector<T, Size> >)
{
  return s.Read<math::vector<T, Size> > ();
}

template <class T, unsigned int Size>
inline const math::matrix<T, Size>& read (InputStream& s, xtl::type<math::matrix<T, Size> >)
{
  return s.Read<math::matrix<T, Size> > ();
}

template <class T>
inline const math::quat<T>& read (InputStream& s, xtl::type<math::quat<T> >)
{
  return s.Read<math::quat<T> > ();
}

template <class T>
inline const math::angle<T>& read (InputStream& s, xtl::type<math::angle<T> >)
{
  return s.Read<math::angle<T> > ();
}

template <class T>
const bound_volumes::axis_aligned_box<T>& read (InputStream& s, xtl::type<bound_volumes::axis_aligned_box<T> >)
{
  return s.Read<bound_volumes::axis_aligned_box<T> > ();  
}

template <class T>
inline void write (OutputStream& s, const RawArray<T>& value)
{
  write (s, value.size);

  s.WriteData (value.data, value.size * sizeof (T));
}

template <class T>
inline RawArray<T> read (InputStream& s, xtl::type<RawArray<T> >)
{
  const uint32& count = read (s, xtl::type<uint32> ());

  s.Skip (detail::aligned_size (count * sizeof (T)));

  return RawArray<T> (reinterpret_cast<T*> (reinterpret_cast<unsigned char*> (&const_cast<uint32&> (count)) + detail::aligned_size<sizeof (uint32)> ()), count);
}
