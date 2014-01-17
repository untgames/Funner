#include "shared.h"

using namespace render::manager;

/*
===================================================================================================
    BatchStateBlock
===================================================================================================
*/

/*
    Описание реализации блока состояний пакета
*/

struct BatchStateBlock::Impl
{
};

/*
    Конструктор / деструктор
*/

BatchStateBlock::BatchStateBlock (BatchingManager& batching_manager, MaterialImpl& material)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

BatchStateBlock::~BatchStateBlock ()
{
}

/*
    Блок состояний
*/

LowLevelStateBlockPtr BatchStateBlock::StateBlock ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Управление кэшированием
*/

void BatchStateBlock::UpdateCacheCore ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void BatchStateBlock::ResetCacheCore ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
===================================================================================================
    BatchingManager
===================================================================================================
*/

/*
    Описание реализации менеджера пакетирования
*/

struct BatchingManager::Impl
{
};

/*
    Конструктор / деструктор
*/

BatchingManager::BatchingManager (const DeviceManagerPtr&)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

BatchingManager::~BatchingManager ()
{
}

/*
    Ссылка на менеджер устройства
*/

render::manager::DeviceManager& BatchingManager::DeviceManager ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Резервирование вспомогательных примитивов
*/

void BatchingManager::ReserveDynamicPrimitives (size_t sprites_count, size_t lines_count)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t BatchingManager::LinesCapacity ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t BatchingManager::SpritesCapacity ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Динамические буферы
*/

render::manager::DynamicVertexBuffer& BatchingManager::DynamicVertexBuffer ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

render::manager::DynamicIndexBuffer& BatchingManager::DynamicIndexBuffer ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Выделение вершин и индексов
*/

size_t BatchingManager::AllocateDynamicVertices (size_t count)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t BatchingManager::AllocateDynamicIndices (size_t count)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void BatchingManager::DeallocateDynamicVertices (size_t first, size_t count)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void BatchingManager::DeallocateDynamicIndices (size_t first, size_t count)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Получение блока состояний по материалу
*/

BatchStateBlockPtr BatchingManager::GetStateBlock (MaterialImpl* material)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
