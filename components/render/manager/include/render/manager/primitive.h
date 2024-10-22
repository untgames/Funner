#ifndef RENDER_MANAGER_PRIMITIVE_HEADER
#define RENDER_MANAGER_PRIMITIVE_HEADER

//TODO: добавить генерацию стандартных примитивов!!!

#include <math/angle.h>

#include <render/manager/material.h>

namespace media
{

namespace geometry
{

//forward declarations
class VertexStream;
class VertexBuffer;
class IndexBuffer;
class Mesh;
class MeshLibrary;

}

}

namespace render
{

namespace manager
{

//implementation forwards
class PrimitiveBuffersImpl;
class PrimitiveImpl;
class Wrappers;

template <class T> class SimplePrimitiveList;
template <class T> class SimplePrimitiveListImpl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режим использования меша
///////////////////////////////////////////////////////////////////////////////////////////////////
enum MeshBufferUsage
{
  MeshBufferUsage_Static,  //не обновляемый буфер
  MeshBufferUsage_Dynamic, //обновляемый буфер
  MeshBufferUsage_Stream,  //часто обновляемый буфер
  
  MeshBufferUsage_Default = MeshBufferUsage_Dynamic
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режим спрайтов
///////////////////////////////////////////////////////////////////////////////////////////////////
enum SpriteMode
{
  SpriteMode_Billboard         = 1,                                          //up vector is view up,  normal & rotation is ignored
  SpriteMode_Oriented          = 2,                                          //up vector is local up, normal & rotation is used
  SpriteMode_OrientedBillboard = SpriteMode_Billboard | SpriteMode_Oriented, //up vector is view up,  normal & rotation is used
};

enum PrimitiveBufferType
{
  PrimitiveBufferType_IndexBuffer  = 1,
  PrimitiveBufferType_VertexStream = 1 << 1,
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Спрайт
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Sprite
{
  math::vec3f  position;   //положение центра спрайта
  math::vec2f  size;       //размер спрайта
  math::vec4f  color;      //цвет спрайта
  math::vec2f  tex_offset; //смещение начала спрайта в текстуре [0;1]
  math::vec2f  tex_size;   //размер спрайта в текстуре [0;1]
  math::vec3f  normal;     //нормаль
  math::anglef rotation;   //поворот относительно нормали
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Точка линии
///////////////////////////////////////////////////////////////////////////////////////////////////
struct LinePoint
{
  math::vec3f position;   //положение точки в пространстве
  math::vec4f color;      //цвет точки
  math::vec2f tex_offset; //смещение точки в текстуре [0;1]
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Линия
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Line
{
  LinePoint point [2]; //концы линии
};

typedef SimplePrimitiveList<Sprite> SpriteList;
typedef SimplePrimitiveList<Line>   LineList;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Буфер примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
class PrimitiveBuffers
{
  friend class Wrappers;
  public:
    typedef unsigned long long object_id_t;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    PrimitiveBuffers  (const PrimitiveBuffers&);
    ~PrimitiveBuffers ();
    
    PrimitiveBuffers& operator = (const PrimitiveBuffers&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление буферов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Add (const media::geometry::VertexStream& buffer, MeshBufferUsage usage = MeshBufferUsage_Default);
    void Add (const media::geometry::VertexBuffer& buffer, MeshBufferUsage usage = MeshBufferUsage_Default);    
    void Add (const media::geometry::IndexBuffer& buffer, MeshBufferUsage usage = MeshBufferUsage_Default);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обновление буферов
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void Update             (const media::geometry::VertexStream& buffer);
    void UpdateVertexStream (object_id_t id, const void* buffer, size_t buffer_size);
    void Update             (const media::geometry::IndexBuffer& buffer);
    void UpdateIndexBuffer  (object_id_t id, const void* buffer, size_t buffer_size);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Удаление буферов
///////////////////////////////////////////////////////////////////////////////////////////////////        
    void Remove    (const media::geometry::VertexStream& buffer);
    void Remove    (const media::geometry::VertexBuffer& buffer);    
    void Remove    (const media::geometry::IndexBuffer& buffer);
    void RemoveAll ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Резервирование буферов для динамических примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void         ReserveDynamicBuffers (unsigned int vertices_count, unsigned int indices_count);
    unsigned int DynamicVerticesCount  () const;
    unsigned int DynamicIndicesCount   () const;
      
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (PrimitiveBuffers&);
    
  protected:
    PrimitiveBuffers (PrimitiveBuffersImpl*);

  private:
    PrimitiveBuffersImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (PrimitiveBuffers&, PrimitiveBuffers&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Меш
///////////////////////////////////////////////////////////////////////////////////////////////////
class Primitive
{
  friend class Wrappers;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Primitive  (const Primitive&);
    ~Primitive ();

    Primitive& operator = (const Primitive&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя примитива
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name    () const;
    void        SetName (const char* id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Буферы примитива
///////////////////////////////////////////////////////////////////////////////////////////////////
    PrimitiveBuffers Buffers () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с мешами
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t MeshesCount     () const;
    size_t AddMesh         (const media::geometry::Mesh&,
                            MeshBufferUsage vb_usage = MeshBufferUsage_Default,
                            MeshBufferUsage ib_usage = MeshBufferUsage_Default,
                            unsigned int updatable_primitive_buffer_types = 0);
    void   RemoveMeshes    (size_t first_mesh, size_t meshes_count);
    void   RemoveAllMeshes (unsigned int updatable_primitive_buffer_types = 0);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа со спрайтами
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t     SpriteListsCount        () const;
    SpriteList AddStandaloneSpriteList (SpriteMode mode, const math::vec3f& up, MeshBufferUsage vb_usage = MeshBufferUsage_Default, MeshBufferUsage ib_usage = MeshBufferUsage_Default);
    SpriteList AddBatchingSpriteList   (SpriteMode mode, const math::vec3f& up);
    void       RemoveSpriteList        (SpriteList&);
    void       RemoveAllSpriteLists    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с линиями
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t   LineListsCount        () const;
    LineList AddStandaloneLineList (MeshBufferUsage vb_usage = MeshBufferUsage_Default, MeshBufferUsage ib_usage = MeshBufferUsage_Default);
    LineList AddBatchingLineList   ();
    void     RemoveLineList        (LineList&);
    void     RemoveAllLineLists    ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление кэшированием
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateCache ();
    void ResetCache  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Primitive&);
    
  private:
    Primitive (PrimitiveImpl*);

  private:
    PrimitiveImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Primitive&, Primitive&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Группа динамических примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class SimplePrimitiveList
{
  friend class Wrappers;
  friend class Primitive;
  public:
    typedef T Item;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    SimplePrimitiveList  (const SimplePrimitiveList&);
    ~SimplePrimitiveList ();

    SimplePrimitiveList& operator = (const SimplePrimitiveList&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Материал
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetMaterial (const char* material);
    const char* Material    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление / обновление примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Add    (size_t count, const Item* items);
    void   Update (size_t first, size_t count, const Item* items);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Удаление примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Remove (size_t first, size_t count = 1);
    void Clear  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Резервируемое пространство
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   Reserve  (size_t count);
    size_t Capacity () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Отсоединение от примитива   
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RemoveFromPrimitive ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (SimplePrimitiveList&);  

  private:
    typedef SimplePrimitiveListImpl<Item> Impl;

    SimplePrimitiveList (Impl*);

  private:
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> void swap (SimplePrimitiveList<T>&, SimplePrimitiveList<T>&);

}

}

#endif
