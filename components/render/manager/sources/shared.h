#ifndef RENDER_MANAGER_SHARED_HEADER
#define RENDER_MANAGER_SHARED_HEADER

#include <stl/auto_ptr.h>
#include <stl/hash_map>
#include <stl/map>
#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/iterator.h>
#include <xtl/ref.h>
#include <xtl/shared_ptr.h>
#include <xtl/signal.h>
#include <xtl/trackable.h>

#include <common/hash.h>
#include <common/log.h>
#include <common/parser.h>
#include <common/strlib.h>
#include <common/time.h>

#include <media/image.h>
#include <media/compressed_image.h>
#include <media/geometry/mesh_library.h>
#include <media/rfx/material_library.h>
#include <media/rfx/shader_library.h>

#include <syslib/window.h>

#include <render/low_level/driver.h>
#include <render/low_level/utils.h>

#include <render/manager.h>

namespace render
{


//implementation forwards
class CacheManager;
class DeviceManager;
class DynamicTextureImpl;
class Effect;
class EffectLoaderLibrary;
class EffectManager;
class EffectPass;
class EffectRenderer;
class MaterialManager;
class PrimitiveManager;
class Program;
class ProgramManager;
class ProgramParametersLayout;
class PropertyCache;
class RenderingContext;
class Settings;
class ShaderOptionsCache;
class TextureManager;
class InstantiatedEffect;
struct RendererOperationList;
struct RenderTargetDesc;
struct ShaderOptions;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �� ������� ������� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef xtl::intrusive_ptr<CacheManager>            CacheManagerPtr;
typedef xtl::intrusive_ptr<DeviceManager>           DeviceManagerPtr;
typedef xtl::intrusive_ptr<DynamicTextureImpl>      DynamicTexturePtr;
typedef xtl::intrusive_ptr<EffectPass>              EffectPassPtr;
typedef xtl::intrusive_ptr<Effect>                  EffectPtr;
typedef xtl::intrusive_ptr<EffectManager>           EffectManagerPtr;
typedef xtl::intrusive_ptr<EffectRenderer>          EffectRendererPtr;
typedef xtl::intrusive_ptr<EntityImpl>              EntityPtr;
typedef xtl::intrusive_ptr<FrameImpl>               FramePtr;
typedef xtl::intrusive_ptr<InstantiatedEffect>      InstantiatedEffectPtr;
typedef xtl::intrusive_ptr<MaterialImpl>            MaterialPtr;
typedef xtl::intrusive_ptr<MaterialManager>         MaterialManagerPtr;
typedef xtl::intrusive_ptr<PrimitiveImpl>           PrimitivePtr;
typedef xtl::intrusive_ptr<PrimitiveBuffersImpl>    PrimitiveBuffersPtr;
typedef xtl::intrusive_ptr<PrimitiveManager>        PrimitiveManagerPtr;
typedef xtl::intrusive_ptr<Program>                 ProgramPtr;
typedef xtl::intrusive_ptr<ProgramManager>          ProgramManagerPtr;
typedef xtl::intrusive_ptr<ProgramParametersLayout> ProgramParametersLayoutPtr;
typedef xtl::intrusive_ptr<PropertyCache>           PropertyCachePtr;
typedef xtl::intrusive_ptr<RectAreaImpl>            RectAreaPtr;
typedef xtl::intrusive_ptr<RenderManagerImpl>       RenderManagerPtr;
typedef xtl::intrusive_ptr<RenderTargetImpl>        RenderTargetPtr;
typedef xtl::intrusive_ptr<RenderTargetDesc>        RenderTargetDescPtr;
typedef xtl::intrusive_ptr<Settings>                SettingsPtr;
typedef xtl::intrusive_ptr<TextureImpl>             TexturePtr;
typedef xtl::intrusive_ptr<TextureManager>          TextureManagerPtr;
typedef xtl::intrusive_ptr<ViewportImpl>            ViewportPtr;
typedef xtl::intrusive_ptr<WindowImpl>              WindowPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �� ������� ������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef xtl::com_ptr<low_level::IDriver>                  LowLevelDriverPtr;
typedef xtl::com_ptr<low_level::IAdapter>                 LowLevelAdapterPtr;
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
typedef xtl::com_ptr<low_level::IStateBlock>              LowLevelStateBlockPtr;
typedef xtl::com_ptr<low_level::IProgram>                 LowLevelProgramPtr;
typedef xtl::com_ptr<low_level::IProgramParametersLayout> LowLevelProgramParametersLayoutPtr;
typedef xtl::com_ptr<low_level::IPredicate>               LowLevelPredicatePtr;
typedef xtl::shared_ptr<low_level::TextureDesc>           LowLevelTextureDescPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
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
      if (!ptr)
        throw xtl::format_operation_exception ("render::Wrappers::Wrap", "Attempt to wrap null internal object for of type '%s'", typeid (T).name ());
    
      return Ret (ptr);
    }
  
    template <class Ret, class T>
    static Ret Wrap (const xtl::intrusive_ptr<T>& ptr)
    {
      if (!ptr)
        throw xtl::format_operation_exception ("render::Wrappers::Wrap", "Attempt to wrap null internal object for of type '%s'", typeid (T).name ());
    
      return Ret (ptr.get ());
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������, ���������� ���������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class DebugIdHolder: public xtl::noncopyable
{
  public:
////�����������
    DebugIdHolder ()
    {
      static size_t current_id = 1;
      
      id = current_id++;
      
      if (id == 0)
      {
        --current_id;

        throw xtl::format_operation_exception ("render::DebugIdHolder::DebugIdHolder", "Too many render objects");
      }
    }
    
///��������� ��������������
    size_t Id () { return id; }
  
  private:
    size_t id;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������������ ������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "object.h"
#include "device_manager.h"
#include "log.h"
#include "cache_holder.h"
#include "resource_proxy.h"

#include "cache_manager.h"
#include "cache_map.h"
#include "dynamic_texture.h"
#include "effect.h"
#include "effect_loader_library.h"
#include "effect_manager.h"
#include "entity.h"
#include "frame.h"
#include "input_layout_manager.h"
#include "program_parameters.h"
#include "manager.h"
#include "material.h"
#include "material_manager.h"
#include "vertex_buffer.h"
#include "primitive.h"
#include "primitive_buffers.h"
#include "primitive_manager.h"
#include "program.h"
#include "program_manager.h"
#include "property_buffer.h"
#include "property_cache.h"
#include "rect_area.h"
#include "render_target.h"
#include "rendering_context.h"
#include "settings.h"
#include "shader_options.h"
#include "texture.h"
#include "texture_manager.h"
#include "viewport.h"
#include "window.h"

}

#endif