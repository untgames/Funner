#ifndef RENDER_SCENE_INTERCHANGE_STREAMS_HEADER
#define RENDER_SCENE_INTERCHANGE_STREAMS_HEADER

#include <xtl/common_exceptions.h>
#include <xtl/type.h>

#include <math/matrix.h>
#include <math/quat.h>
#include <math/vector.h>

#include <bv/axis_aligned_box.h>

#include <render/scene/interchange/command_buffer.h>

namespace media
{

//forward declarations
class Image;

}

namespace render
{

namespace scene
{

namespace interchange
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Выравнивание данных
///////////////////////////////////////////////////////////////////////////////////////////////////
const size_t DATA_ALIGN = 4;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Выходной поток сериализации
///////////////////////////////////////////////////////////////////////////////////////////////////
class OutputStream
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    OutputStream  ();
    ~OutputStream ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сброс
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Reset (const CommandBuffer&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (CommandBuffer&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сериализация заголовка и конца команды
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BeginCommand (command_id_t);
    void EndCommand   ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подготовка достаточного места в буфере для записи
///////////////////////////////////////////////////////////////////////////////////////////////////
    void EnsureSpaceAvailable (size_t size);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Запись блока данных
///////////////////////////////////////////////////////////////////////////////////////////////////
    void WriteData       (const void* data, size_t size);
    void WriteDataUnsafe (const void* data, size_t size);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Запись значения
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> void Write (const T& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Пропуск
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Skip (size_t size);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Позиция потока
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetPosition (size_t position);
    size_t Position    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Идентификатор владельца для внутрипроцессного обмена
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetInprocessOwnerId (object_id_t id);
    object_id_t InprocessOwnerId    () const;

  private:
    OutputStream (const OutputStream&); //no implementation
    OutputStream& operator = (const OutputStream&); //no implementation

    void Resize (size_t size);

  private:
    char*         command_start;      //указатель на начало команды
    char*         buffer_start;       //указатель на начало буфера
    char*         buffer_end;         //указатель на конец буфера
    char*         pos;                //позиция указателя записи
    CommandBuffer buffer;             //буфер
    object_id_t   inprocess_owner_id; //идентификатор владельца для внутрипроцессного обмена
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Входной поток десериализации
///////////////////////////////////////////////////////////////////////////////////////////////////
class InputStream
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    InputStream  ();
    ~InputStream ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сброс
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Reset (const CommandBuffer&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество доступных для считывания данных
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Available () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Чтение блока данных
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ReadData       (void* data, size_t size);
    void ReadDataUnsafe (void* data, size_t size);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Чтение значения
///////////////////////////////////////////////////////////////////////////////////////////////////
                       const void* ReadData (size_t size);
    template <class T> const T&    Read     ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Пропуск
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Skip (size_t size);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Позиция потока
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetPosition (size_t position);
    size_t Position    () const;

  private:
    InputStream (const InputStream&); //no implementation
    InputStream& operator = (const InputStream&); //no implementation   

  private:
    const char*   pos;
    const char*   buffer_start;
    const char*   buffer_end;
    CommandBuffer buffer;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Массив
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct RawArray
{
  uint32   size; //количество элементов
  const T* data; //данные

  RawArray (const T* data, size_t size);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сериализация базовых типов
///////////////////////////////////////////////////////////////////////////////////////////////////
void write (OutputStream&, bool);
void write (OutputStream&, int64);
void write (OutputStream&, uint64);
void write (OutputStream&, int32);
void write (OutputStream&, uint32);
void write (OutputStream&, int16);
void write (OutputStream&, uint16);
void write (OutputStream&, int8);
void write (OutputStream&, uint8);
void write (OutputStream&, float32);
void write (OutputStream&, const char*);
void write (OutputStream&, NodeType);
void write (OutputStream&, const LightParams&);
void write (OutputStream&, const PageCurlParams&);
void write (OutputStream&, SpriteMode);
void write (OutputStream&, PrimitiveUsage);
void write (OutputStream&, const media::Image&);

template <class T, unsigned int Size> void write (OutputStream&, const math::vector<T, Size>&);
template <class T, unsigned int Size> void write (OutputStream&, const math::matrix<T, Size>&);
template <class T>                    void write (OutputStream&, const math::quat<T>&);
template <class T>                    void write (OutputStream&, const math::angle<T>&);
template <class T>                    void write (OutputStream&, const bound_volumes::axis_aligned_box<T>&);

template <class T> void write (OutputStream&, const RawArray<T>&);

template <class T> const T& read (InputStream&, xtl::type<T&>);
template <class T> const T& read (InputStream&, xtl::type<const T&>);

bool                  read (InputStream&, xtl::type<bool>);
const int64&          read (InputStream&, xtl::type<int64>);
const uint64&         read (InputStream&, xtl::type<uint64>);
const int32&          read (InputStream&, xtl::type<int32>);
const uint32&         read (InputStream&, xtl::type<uint32>);
const int16&          read (InputStream&, xtl::type<int16>);
const uint16&         read (InputStream&, xtl::type<uint16>);
const int8&           read (InputStream&, xtl::type<int8>);
const uint8&          read (InputStream&, xtl::type<uint8>);
const float32&        read (InputStream&, xtl::type<float32>);
const Command&        read (InputStream&, xtl::type<Command>);
const char*           read (InputStream&, xtl::type<const char*>);
const NodeType&       read (InputStream&, xtl::type<NodeType>);
const LightParams&    read (InputStream&, xtl::type<LightParams>);
const PageCurlParams& read (InputStream&, xtl::type<PageCurlParams>);
const SpriteMode&     read (InputStream&, xtl::type<SpriteMode>);
const PrimitiveUsage& read (InputStream&, xtl::type<PrimitiveUsage>);
media::Image          read (InputStream&, xtl::type<media::Image>);

template <class T, unsigned int Size> const math::vector<T, Size>& read (InputStream&, xtl::type<math::vector<T, Size> >);
template <class T, unsigned int Size> const math::matrix<T, Size>& read (InputStream&, xtl::type<math::matrix<T, Size> >);
template <class T>                    const math::quat<T>&         read (InputStream&, xtl::type<math::quat<T> >);
template <class T>                    const math::angle<T>&        read (InputStream&, xtl::type<math::angle<T> >);

template <class T> const bound_volumes::axis_aligned_box<T>& read (InputStream&, xtl::type<bound_volumes::axis_aligned_box<T> >);

template <class T> RawArray<T> read (InputStream&, xtl::type<RawArray<T> >);

#include <render/scene/interchange/detail/streams.inl>

}

}

}

#endif
