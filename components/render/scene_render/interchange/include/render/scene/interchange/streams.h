#ifndef RENDER_SCENE_INTERCHANGE_STREAMS_HEADER
#define RENDER_SCENE_INTERCHANGE_STREAMS_HEADER

#include <xtl/common_exceptions.h>
#include <xtl/type.h>

#include <math/matrix.h>
#include <math/quat.h>
#include <math/vector.h>

#include <bv/axis_aligned_box.h>

#include <render/scene/interchange/command_buffer.h>

namespace render
{

namespace scene
{

namespace interchange
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class OutputStream
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    OutputStream  ();
    ~OutputStream ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Reset (const CommandBuffer&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (CommandBuffer&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� � ����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BeginCommand (command_id_t);
    void EndCommand   ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������ ����� � ������ ��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void EnsureSpaceAvailable (size_t size);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void WriteData       (const void* data, size_t size);
    void WriteDataUnsafe (const void* data, size_t size);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> void Write (const T& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetPosition (size_t position);
    size_t Position    () const;

  private:
    OutputStream (const OutputStream&); //no implementation
    OutputStream& operator = (const OutputStream&); //no implementation

    void Resize (size_t size);

  private:
    char*         command_start; //��������� �� ������ �������
    char*         buffer_start;  //��������� �� ������ ������
    char*         buffer_end;    //��������� �� ����� ������
    char*         pos;           //������� ��������� ������
    CommandBuffer buffer;        //�����
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class InputStream
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    InputStream  ();
    ~InputStream ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Reset (const CommandBuffer&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� ��� ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Available () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ReadData       (void* data, size_t size);
    void ReadDataUnsafe (void* data, size_t size);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
                       const void* ReadData (size_t size);
    template <class T> const T&    Read     ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Skip (size_t size);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
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
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct RawArray
{
  uint32   size; //���������� ���������
  const T* data; //������

  RawArray (const T* data, size_t size);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ������� �����
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

template <class T, unsigned int Size> void write (OutputStream&, const math::vector<T, Size>&);
template <class T, unsigned int Size> void write (OutputStream&, const math::matrix<T, Size>&);
template <class T>                    void write (OutputStream&, const math::quat<T>&);
template <class T>                    void write (OutputStream&, const math::angle<T>&);
template <class T>                    void write (OutputStream&, const bound_volumes::axis_aligned_box<T>&);

template <class T> void write (OutputStream&, const RawArray<T>&);

template <class T> const T& read (InputStream&, xtl::type<T&>);
template <class T> const T& read (InputStream&, xtl::type<const T&>);

bool               read (InputStream&, xtl::type<bool>);
const int64&       read (InputStream&, xtl::type<int64>);
const uint64&      read (InputStream&, xtl::type<uint64>);
const int32&       read (InputStream&, xtl::type<int32>);
const uint32&      read (InputStream&, xtl::type<uint32>);
const int16&       read (InputStream&, xtl::type<int16>);
const uint16&      read (InputStream&, xtl::type<uint16>);
const int8&        read (InputStream&, xtl::type<int8>);
const uint8&       read (InputStream&, xtl::type<uint8>);
const float32&     read (InputStream&, xtl::type<float32>);
const Command&     read (InputStream&, xtl::type<Command>);
const char*        read (InputStream&, xtl::type<const char*>);
const NodeType&    read (InputStream&, xtl::type<NodeType>);
const LightParams& read (InputStream&, xtl::type<LightParams>);

template <class T, unsigned int Size> const math::vector<T, Size>& read (InputStream&, xtl::type<math::vector<T, Size> >);
template <class T, unsigned int Size> const math::matrix<T, Size>& read (InputStream&, xtl::type<math::matrix<T, Size> >);
template <class T>                    const math::quat<T>&         read (InputStream&, xtl::type<math::quat<T> >);
template <class T>                    const math::angle<T>&        read (InputStream&, xtl::type<math::angle<T> >);

template <class T> const bound_volumes::axis_aligned_box<T>& read (InputStream&, xtl::type<bound_volumes::axis_aligned_box<T> >);

template <class T> const RawArray<T>& read (InputStream&, xtl::type<RawArray<T> >);

#include <render/scene/interchange/detail/streams.inl>

}

}

}

#endif
