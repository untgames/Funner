#ifndef RENDER_DX11_DRIVER_SHADER_STAGE_SHARED_HEADER
#define RENDER_DX11_DRIVER_SHADER_STAGE_SHARED_HEADER

#include <D3Dcommon.h>
#include <D3DX11async.h>
#include <D3D11Shader.h>
#include <D3Dcompiler.h>

#include <stl/deque>
#include <stl/hash_map>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/ref.h>
#include <xtl/shared_ptr.h>
#include <xtl/uninitialized_storage.h>

#include <common/file.h>
#include <common/hash.h>
#include <common/string.h>
#include <common/strlib.h>

#include <render/low_level/utils.h>

#include <render/low_level/helpers/program_parameters_layout.h>

#include <shared/error.h>
#include <shared/log.h>
#include <shared/input_layout.h>
#include <shared/shader_manager.h>

#ifdef _MSC_VER
  #pragma warning (disable : 4503) //decorated name length exceeded, name was truncated
#endif

namespace render
{

namespace low_level
{

namespace dx11
{

//forwards
class ShaderLibrary;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Тип шейдера
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ShaderType
{
  ShaderType_Compute,
  ShaderType_Domain,
  ShaderType_Geometry,
  ShaderType_Hull,
  ShaderType_Pixel,
  ShaderType_Vertex,

  ShaderType_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание параметра шейдера
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ShaderParameter: public ProgramParameter {};

#include "shader_code.h"
#include "constant_buffer_layout.h"
#include "shader.h"
#include "program_buffer_layout.h"
#include "program.h"
#include "program_parameters_layout.h"
#include "source_constant_buffer.h"
#include "shader_buffers_synchronizer.h"
#include "target_constant_buffer.h"
#include "bindable_program.h"
#include "shader_library.h"

}

}

}

#endif
