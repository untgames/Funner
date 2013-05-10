#ifndef RENDER_LOW_LEVEL_DEVICE_HEADER
#define RENDER_LOW_LEVEL_DEVICE_HEADER

#include <exception>
#include <render/low_level/context.h>

namespace render
{

namespace low_level
{

//forward declarations
class ISwapChain;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Таблица возможностей устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
struct DeviceCaps
{
  size_t max_texture_1d_size;            //максимальная длина одномерной текстуры
  size_t max_texture_2d_size;            //максимальный размер ребра двумерной текстуры
  size_t max_texture_3d_size;            //максимальный размер ребра трёхмерной текстуры
  size_t max_texture_cubemap_size;       //максимальный размер ребра кубической текстуры
  size_t max_texture_anisotropy;         //максимальный уровень анизотропии
  size_t samplers_count;                 //количество текстурных сэмплеров
  bool   has_advanced_blend;             //поддерживаются сложные режимы смешивания цветов
  bool   has_depth_texture;              //поддерживаются текстуры с информацией о глубине
  bool   has_multisample;                //поддерживается мультисэмплинг
  bool   has_occlusion_queries;          //поддерживаются запросы видимости геометрии
  bool   has_shadow_maps;                //поддерживаются карты теней
  bool   has_texture_anisotropic_filter; //поддерживается анизотропная фильтрация
  bool   has_texture_cube_map;           //поддерживаются кубические текстуры
  bool   has_texture_lod_bias;           //поддерживаются уровни детализации текстур
  bool   has_texture_mirrored_repeat;    //поддерживается отражение текстурных координат
  bool   has_texture_non_power_of_two;   //поддерживаются текстуры с размерами не кратными степени двойки
  bool   has_texture1d;                  //поддерживаются одномерные текстуры
  bool   has_texture3d;                  //поддерживаются трёхмерные текстуры
  bool   has_two_side_stencil;           //поддерживается двусторонний буфер трафарета
  bool   has_right_hand_viewport;        //находится ли область вывода в правосторонней системе координат
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Идентификаторы строк возможностей устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
enum DeviceCapString
{
  DeviceCapString_ShaderProfiles, //строка со списком профилей шейдеров
  
  DeviceCapString_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Устройство отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
class IDevice: virtual public IObject
{
  public:  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* GetName () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение возможностей устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void        GetCaps                        (DeviceCaps&) = 0;
    virtual const char* GetCapString                   (DeviceCapString) = 0;
    virtual const char* GetVertexAttributeSemanticName (VertexAttributeSemantic semantic) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IInputLayout*             CreateInputLayout             (const InputLayoutDesc&) = 0;
    virtual IProgramParametersLayout* CreateProgramParametersLayout (const ProgramParametersLayoutDesc&) = 0;
    virtual IRasterizerState*         CreateRasterizerState         (const RasterizerDesc&) = 0;
    virtual IBlendState*              CreateBlendState              (const BlendDesc&) = 0;
    virtual IDepthStencilState*       CreateDepthStencilState       (const DepthStencilDesc&) = 0;
    virtual ISamplerState*            CreateSamplerState            (const SamplerDesc&) = 0;
    virtual IBuffer*                  CreateBuffer                  (const BufferDesc&, const void* buffer_data = 0) = 0;
    virtual IProgram*                 CreateProgram                 (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log) = 0;
    virtual ITexture*                 CreateTexture                 (const TextureDesc&) = 0;
    virtual ITexture*                 CreateTexture                 (const TextureDesc&, const TextureData&) = 0;
    virtual ITexture*                 CreateTexture                 (const TextureDesc&, IBuffer* buffer, size_t buffer_offset, const size_t* mip_sizes) = 0;
    virtual ITexture*                 CreateRenderTargetTexture     (ISwapChain* swap_chain, size_t buffer_index) = 0;
    virtual ITexture*                 CreateDepthStencilTexture     (ISwapChain* swap_chain) = 0;
    virtual IView*                    CreateView                    (ITexture* texture, const ViewDesc& desc) = 0;
    virtual IPredicate*               CreatePredicate               () = 0;
    virtual IQuery*                   CreateQuery                   (QueryType type) = 0;
    virtual IStateBlock*              CreateStateBlock              (const StateBlockMask& mask) = 0;
    virtual IDeviceContext*           CreateDeferredContext         () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение непосредственного контекста выполнения операций
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IDeviceContext* GetImmediateContext () = 0;
};

}

}

#endif
