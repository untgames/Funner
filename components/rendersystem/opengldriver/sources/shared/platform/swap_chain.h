#ifndef RENDER_GL_DRIVER_SWAP_CHAIN_HEADER
#define RENDER_GL_DRIVER_SWAP_CHAIN_HEADER

#include <render/low_level/driver.h>
#include <shared/platform/output_manager.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание цепочки обмена
///////////////////////////////////////////////////////////////////////////////////////////////////
ISwapChain* create_swap_chain (OutputManager& output_manager, const SwapChainDesc& swap_chain_desc);

}

}

}

#endif
