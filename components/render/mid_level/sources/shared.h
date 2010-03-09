#ifndef RENDER_MID_LEVEL_SHARED_HEADER
#define RENDER_MID_LEVEL_SHARED_HEADER

#include <stl/auto_ptr.h>

#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>

#include <media/image.h>

#include <media/geometry/mesh_library.h>

#include <media/rfx/effect_library.h>
#include <media/rfx/material_library.h>
#include <media/rfx/shader_library.h>

#include <syslib/window.h>

#include <render/low_level/driver.h>
#include <render/low_level/utils.h>

#include <render/mid_level/manager.h>

namespace render
{

namespace mid_level
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Указатели на объекты рендера среднего уровня
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef xtl::intrusive_ptr<EntityImpl>          EntityPtr;
typedef xtl::intrusive_ptr<FrameImpl>           FramePtr;
typedef xtl::intrusive_ptr<MaterialImpl>        MaterialPtr;
typedef xtl::intrusive_ptr<PrimitiveImpl>       PrimitivePtr;
typedef xtl::intrusive_ptr<RenderManagerImpl>   RenderManagerPtr;
typedef xtl::intrusive_ptr<RenderTargetImpl>    RenderTargetPtr;
typedef xtl::intrusive_ptr<ResourceLibraryImpl> ResourceLibraryPtr;
typedef xtl::intrusive_ptr<TextureImpl>         TexturePtr;
typedef xtl::intrusive_ptr<WindowImpl>          WindowPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Указатели на объекты рендера низкого уровня
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef xtl::com_ptr<low_level::IDriver>                  LowLevelDriverPtr;
typedef xtl::com_ptr<low_level::ISwapChain>               LowLevelSwapChainPtr;
typedef xtl::com_ptr<low_level::IDevice>                  LowLevelDevicePtr;
typedef xtl::com_ptr<low_level::ITexture>                 LowLevelTexturePtr;
typedef xtl::com_ptr<low_level::ISamplerState>            LowLevelSamplerStatePtr;
typedef xtl::com_ptr<low_level::IView>                    LowLevelViewPtr;
typedef xtl::com_ptr<low_level::IBlendState>              LowLevelBlendStatePtr;
typedef xtl::com_ptr<low_level::IDepthStencilState>       LowLevelDepthStencilStatePtr;
typedef xtl::com_ptr<low_level::IInputLayout>             LowLevelInputLayoutPtr;
typedef xtl::com_ptr<low_level::IBuffer>                  LowLevelBufferPtr;
typedef xtl::com_ptr<low_level::IRasterizerState>         LowLevelRasterizerStatePtr;
typedef xtl::com_ptr<low_level::IProgram>                 LowLevelProgramPtr;
typedef xtl::com_ptr<low_level::IProgramParametersLayout> LowLevelProgramParametersLayoutPtr;
typedef xtl::com_ptr<low_level::IPredicate>               LowLevelPredicatePtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обёртки
///////////////////////////////////////////////////////////////////////////////////////////////////
class Wrappers
{
  public:
    template <class Ret, class T>
    static xtl::intrusive_ptr<Ret> Unwrap (const T& object)
    {
      return object.impl;
    }

    template <class Ret, class T>
    static Ret Wrap (T* ptr)
    {
      return Ret (ptr);
    }
  
    template <class Ret, class T>
    static Ret Wrap (const xtl::intrusive_ptr<T>& ptr)
    {
      return Ret (ptr.get ());
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подключение заголовочных файлов реализации
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "object.h"

#include "entity.h"
#include "frame.h"
#include "manager.h"
#include "material.h"
#include "primitive.h"
#include "render_target.h"
#include "resource_library.h"
#include "texture.h"
#include "window.h"

}

}

#endif
