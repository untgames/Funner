#ifndef RENDER_DX11_DRIVER_INPUT_STAGE_SHARED_HEADER
#define RENDER_DX11_DRIVER_INPUT_STAGE_SHARED_HEADER

#include <shared/input_manager.h>

namespace render
{

namespace low_level
{

namespace dx11
{

//получение семантики по имени (returns (VertexAttributeSemantic)-1 if fails)
VertexAttributeSemantic get_semantic_by_name (const char* name);

//получение имени семантики
const char* get_semantic_name (VertexAttributeSemantic semantic);

}

}

}

#endif

