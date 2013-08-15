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
  buffer_end    = pos + buffer.Size ();
}

/*
    Сериализация заголовка и конца команды
*/

inline void OutputStream::BeginCommand (command_id_t id)
{
  EnsureSpaceAvailable (sizeof (Command));

  command_start  = pos;
  pos           += sizeof (Command);

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
  if (buffer_end - pos < size)
    Resize (buffer.Size () + size);
}

inline void OutputStream::Resize (size_t new_size)
{
  char* buffer_start = reinterpret_cast<char*> (buffer.Data ());

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
  EnsureSpaceAvailable (size);
  WriteDataUnsafe      (data, size);
}

inline void OutputStream::WriteDataUnsafe (const void* data, size_t size)
{
  memcpy (pos, data, size);

  pos += size;
}

template <class T> inline void OutputStream::Write (const T& value)
{
  WriteData (&value, sizeof (T));
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
  buffer     = in_buffer;
  pos        = reinterpret_cast<const char*> (buffer.Data ());
  buffer_end = pos + buffer.Size ();
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
  if (size >= Available ())
  {
    ReadDataUnsafe (data, size);
  }
  else
  {
    throw xtl::format_operation_exception ("render::scene::interchange::InputStream::ReadData(void*,size_t)", "Can't read %u bytes from input stream with %u bytes available", size, Available ());
  }
}

inline void InputStream::ReadDataUnsafe (void* data, size_t size)
{
  memcpy (data, pos, size);
}

inline const void* InputStream::ReadData (size_t size)
{
  if (siz >= Available ())
  {
    void* result = pos;

    pos += siz;

    return result;
  }
  else
  {
    throw xtl::format_operation_exception ("render::scene::interchange::InputStream::ReadData(size_t)", "Can't read %u bytes from input stream with %u bytes available", size, Available ());
  }
}

template <class T> inline const T& InputStream::Read ()
{
  return *reinterpret_cast<const T*> (ReadData (sizeof (T)));
}

/*
    Пропуск
*/

inline void InputStream::Skip (size_t size)
{
  if (size >= Available ())
  {
    pos += size;
  }
  else
  {
    throw xtl::format_operation_exception ("render::scene::interchange::InputStream::Skip", "Can't skip %u bytes from input stream with %u bytes available", size, Available ());
  }
}

/*
    Сериализация базовых типов
*/

inline void write (OutputStream& s, int32 value)   { s.Write (value); }
inline void write (OutputStream& s, uint32 value)  { s.Write (value); }
inline void write (OutputStream& s, int16 value)   { s.Write (value); }
inline void write (OutputStream& s, uint16 value)  { s.Write (value); }
inline void write (OutputStream& s, int8 value)    { s.Write (value); }
inline void write (OutputStream& s, uint8 value)   { s.Write (value); }
inline void write (OutputStream& s, float32 value) { s.Write (value); }

inline int32&   read (InputStream& s, xtl::type<int32>)   { return s.Read<int32> (); }
inline uint32&  read (InputStream& s, xtl::type<uint32>)  { return s.Read<uint32> (); }
inline int16&   read (InputStream& s, xtl::type<int16>)   { return s.Read<int16> (); }
inline uint16&  read (InputStream& s, xtl::type<uint16>)  { return s.Read<uint16> (); }
inline int8&    read (InputStream& s, xtl::type<int8>)    { return s.Read<int8> (); }
inline uint8&   read (InputStream& s, xtl::type<uint8>)   { return s.Read<uint8> (); }
inline float32& read (InputStream& s, xtl::type<float32>) { return s.Read<float32> (); }
inline Command& read (InputStream& s, xtl::type<Command>) { return s.Read<Command> (); }
