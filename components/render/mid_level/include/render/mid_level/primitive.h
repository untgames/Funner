#ifndef RENDER_MID_LEVEL_PRIMITIVE_HEADER
#define RENDER_MID_LEVEL_PRIMITIVE_HEADER

#include <render/mid_level/material.h>

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

namespace mid_level
{

//implementation forwards
class PrimitiveImpl;
class Wrappers;

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
///Спрайт
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Sprite
{
  math::vec3f position;   //положение центра спрайта
  math::vec2f size;       //размер спрайта
  math::vec4f color;      //цвет спрайта
  math::vec2f tex_offset; //смещение начала спрайта в текстуре [0;1]
  math::vec2f tex_size;   //размер спрайта в текстуре [0;1]
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
  float     width;     //ширина линии
};

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
///Работа с мешами
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t MeshesCount     () const;
    size_t AddMesh         (const media::geometry::Mesh&, MeshBufferUsage vb_usage = MeshBufferUsage_Default, MeshBufferUsage ib_usage = MeshBufferUsage_Default);
    void   UpdateMesh      (size_t mesh_index, const media::geometry::Mesh& mesh);
    void   RemoveMeshes    (size_t first_mesh, size_t meshes_count);
    void   RemoveAllMeshes ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с динамическими буферами меша
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddMeshBuffer        (const media::geometry::VertexStream& buffer, MeshBufferUsage usage = MeshBufferUsage_Default);
    void AddMeshBuffer        (const media::geometry::VertexBuffer& buffer, MeshBufferUsage usage = MeshBufferUsage_Default);
    void AddMeshBuffer        (const media::geometry::IndexBuffer& buffer, MeshBufferUsage usage = MeshBufferUsage_Default);
    void UpdateMeshBuffer     (const media::geometry::VertexStream& buffer);
    void UpdateMeshBuffer     (const media::geometry::VertexBuffer& buffer);
    void UpdateMeshBuffer     (const media::geometry::IndexBuffer& buffer);
    void RemoveMeshBuffer     (const media::geometry::VertexStream& buffer);
    void RemoveMeshBuffer     (const media::geometry::VertexBuffer& buffer);
    void RemoveMeshBuffer     (const media::geometry::IndexBuffer& buffer);
    void RemoveAllMeshBuffers ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с линиями
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t LinesCount       () const;
    size_t AddLines         (size_t lines_count, const Line* Lines, const Material& material);
    void   UpdateLines      (size_t first_lines, size_t lines_count, const Line* Lines);
    void   SetLinesMaterial (size_t first_lines, size_t lines_count, const Material& material);
    void   RemoveLines      (size_t first_lines, size_t lines_count);
    void   RemoveAllLines   ();
    void   ReserveLines     (size_t lines_count);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа со спрайтами
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t SpritesCount       () const;
    size_t AddSprites         (size_t sprites_count, const Sprite* sprites, const Material& material);
    void   UpdateSprites      (size_t first_sprite, size_t sprites_count, const Sprite* sprites);
    void   SetSpritesMaterial (size_t first_sprite, size_t sprites_count, const Material& material);
    void   RemoveSprites      (size_t first_sprite, size_t sprites_count);
    void   RemoveAllSprites   ();
    void   ReserveSprites     (size_t sprites_count);

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

}

}

#endif
