#include "shared.h"

using namespace render;
using namespace render::mid_level;

/*
    Описание реализации PrimitiveBuffers
*/

struct PrimitiveBuffers::Impl
{
};

/*
    Конструкторы / деструктор / присваивание
*/

PrimitiveBuffers  (const DeviceManagerPtr&);
~PrimitiveBuffers ();

/*
    Добавление буферов
*/

void Add (const media::geometry::VertexStream& buffer, MeshBufferUsage usage = MeshBufferUsage_Default);
void Add (const media::geometry::VertexBuffer& buffer, MeshBufferUsage usage = MeshBufferUsage_Default);
void Add (const media::geometry::IndexBuffer& buffer, MeshBufferUsage usage = MeshBufferUsage_Default);

/*
    Обновление буферов
*/

void Update (const media::geometry::VertexStream& buffer);
void Update (const media::geometry::VertexBuffer& buffer);
void Update (const media::geometry::IndexBuffer& buffer);

/*
    Удаление буферов
*/

void Remove (const media::geometry::VertexStream& buffer);
void Remove (const media::geometry::VertexBuffer& buffer);
void Remove (const media::geometry::IndexBuffer& buffer);
void Clear  ();

/*
    Резервирование вспомогательных примитивов
*/

void   ReserveLines    (size_t count);
void   ReserveSprites  (size_t count);
size_t LinesCapacity   ();
size_t SpritesCapacity ();

/*
    Режим использования буферов вспомогательных примитивов
*/

void            SetLinesBufferUsage   (MeshBufferUsage usage);
void            SetSpritesBufferUsage (MeshBufferUsage usage);
MeshBufferUsage LinesBufferUsage      ();
MeshBufferUsage SpritesBufferUsage    ();
