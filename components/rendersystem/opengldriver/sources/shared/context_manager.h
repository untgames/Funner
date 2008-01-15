#ifndef RENDER_GL_DRIVER_CONTEXT_MANAGER_HEADER
#define RENDER_GL_DRIVER_CONTEXT_MANAGER_HEADER

#include <render/low_level/driver.h>
#include <gl/glew.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение контекста GLEW (имя функции выбрано исходя из требований GLEW)
///////////////////////////////////////////////////////////////////////////////////////////////////
const GLEWContext* glewGetContext ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Идентификаторы расширений OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
enum GlExtension
{
  GlExtension_Unknown = -1,  

  GlExtension_Version_1_1,
  GlExtension_Version_1_2,
  GlExtension_Version_1_3,
  GlExtension_Version_1_4,
  GlExtension_Version_1_5,
  GlExtension_Version_2_0,
  GlExtension_Version_2_1,    

  GlExtension_3DFX_multisample,
  GlExtension_3DFX_tbuffer,
  GlExtension_3DFX_texture_compression_FXT1,
  GlExtension_APPLE_client_storage,
  GlExtension_APPLE_element_array,
  GlExtension_APPLE_fence,
  GlExtension_APPLE_float_pixels,
  GlExtension_APPLE_pixel_buffer,
  GlExtension_APPLE_specular_vector,
  GlExtension_APPLE_texture_range,
  GlExtension_APPLE_transform_hint,
  GlExtension_APPLE_vertex_array_object,
  GlExtension_APPLE_vertex_array_range,
  GlExtension_APPLE_ycbcr_422,
  GlExtension_ARB_color_buffer_float,
  GlExtension_ARB_depth_texture,
  GlExtension_ARB_draw_buffers,
  GlExtension_ARB_fragment_program,
  GlExtension_ARB_fragment_program_shadow,
  GlExtension_ARB_fragment_shader,
  GlExtension_ARB_half_float_pixel,
  GlExtension_ARB_imaging,
  GlExtension_ARB_matrix_palette,
  GlExtension_ARB_multisample,
  GlExtension_ARB_multitexture,
  GlExtension_ARB_occlusion_query,
  GlExtension_ARB_pixel_buffer_object,
  GlExtension_ARB_point_parameters,
  GlExtension_ARB_point_sprite,
  GlExtension_ARB_shader_objects,
  GlExtension_ARB_shading_language_100,
  GlExtension_ARB_shadow,
  GlExtension_ARB_shadow_ambient,
  GlExtension_ARB_texture_border_clamp,
  GlExtension_ARB_texture_compression,
  GlExtension_ARB_texture_cube_map,
  GlExtension_ARB_texture_env_add,
  GlExtension_ARB_texture_env_combine,
  GlExtension_ARB_texture_env_crossbar,
  GlExtension_ARB_texture_env_dot3,
  GlExtension_ARB_texture_float,
  GlExtension_ARB_texture_mirrored_repeat,
  GlExtension_ARB_texture_non_power_of_two,
  GlExtension_ARB_texture_rectangle,
  GlExtension_ARB_transpose_matrix,
  GlExtension_ARB_vertex_blend,
  GlExtension_ARB_vertex_buffer_object,
  GlExtension_ARB_vertex_program,
  GlExtension_ARB_vertex_shader,
  GlExtension_ARB_window_pos,
  GlExtension_ATIX_point_sprites,
  GlExtension_ATIX_texture_env_combine3,
  GlExtension_ATIX_texture_env_route,
  GlExtension_ATIX_vertex_shader_output_point_size,
  GlExtension_ATI_draw_buffers,
  GlExtension_ATI_element_array,
  GlExtension_ATI_envmap_bumpmap,
  GlExtension_ATI_fragment_shader,
  GlExtension_ATI_map_object_buffer,
  GlExtension_ATI_pn_triangles,
  GlExtension_ATI_separate_stencil,
  GlExtension_ATI_shader_texture_lod,
  GlExtension_ATI_text_fragment_shader,
  GlExtension_ATI_texture_compression_3dc,
  GlExtension_ATI_texture_env_combine3,
  GlExtension_ATI_texture_float,
  GlExtension_ATI_texture_mirror_once,
  GlExtension_ATI_vertex_array_object,
  GlExtension_ATI_vertex_attrib_array_object,
  GlExtension_ATI_vertex_streams,
  GlExtension_EXT_422_pixels,
  GlExtension_EXT_Cg_shader,
  GlExtension_EXT_abgr,
  GlExtension_EXT_bgra,
  GlExtension_EXT_bindable_uniform,
  GlExtension_EXT_blend_color,
  GlExtension_EXT_blend_equation_separate,
  GlExtension_EXT_blend_func_separate,
  GlExtension_EXT_blend_logic_op,
  GlExtension_EXT_blend_minmax,
  GlExtension_EXT_blend_subtract,
  GlExtension_EXT_clip_volume_hint,
  GlExtension_EXT_cmyka,
  GlExtension_EXT_color_subtable,
  GlExtension_EXT_compiled_vertex_array,
  GlExtension_EXT_convolution,
  GlExtension_EXT_coordinate_frame,
  GlExtension_EXT_copy_texture,
  GlExtension_EXT_cull_vertex,
  GlExtension_EXT_depth_bounds_test,
  GlExtension_EXT_draw_buffers2,
  GlExtension_EXT_draw_instanced,
  GlExtension_EXT_draw_range_elements,
  GlExtension_EXT_fog_coord,
  GlExtension_EXT_fragment_lighting,
  GlExtension_EXT_framebuffer_blit,
  GlExtension_EXT_framebuffer_multisample,
  GlExtension_EXT_framebuffer_object,
  GlExtension_EXT_framebuffer_sRGB,
  GlExtension_EXT_geometry_shader4,
  GlExtension_EXT_gpu_program_parameters,
  GlExtension_EXT_gpu_shader4,
  GlExtension_EXT_histogram,
  GlExtension_EXT_index_array_formats,
  GlExtension_EXT_index_func,
  GlExtension_EXT_index_material,
  GlExtension_EXT_index_texture,
  GlExtension_EXT_light_texture,
  GlExtension_EXT_misc_attribute,
  GlExtension_EXT_multi_draw_arrays,
  GlExtension_EXT_multisample,
  GlExtension_EXT_packed_depth_stencil,
  GlExtension_EXT_packed_float,
  GlExtension_EXT_packed_pixels,
  GlExtension_EXT_paletted_texture,
  GlExtension_EXT_pixel_buffer_object,
  GlExtension_EXT_pixel_transform,
  GlExtension_EXT_pixel_transform_color_table,
  GlExtension_EXT_point_parameters,
  GlExtension_EXT_polygon_offset,
  GlExtension_EXT_rescale_normal,
  GlExtension_EXT_scene_marker,
  GlExtension_EXT_secondary_color,
  GlExtension_EXT_separate_specular_color,
  GlExtension_EXT_shadow_funcs,
  GlExtension_EXT_shared_texture_palette,
  GlExtension_EXT_stencil_clear_tag,
  GlExtension_EXT_stencil_two_side,
  GlExtension_EXT_stencil_wrap,
  GlExtension_EXT_subtexture,
  GlExtension_EXT_texture,
  GlExtension_EXT_texture3D,
  GlExtension_EXT_texture_array,
  GlExtension_EXT_texture_buffer_object,
  GlExtension_EXT_texture_compression_dxt1,
  GlExtension_EXT_texture_compression_latc,
  GlExtension_EXT_texture_compression_rgtc,
  GlExtension_EXT_texture_compression_s3tc,
  GlExtension_EXT_texture_cube_map,
  GlExtension_EXT_texture_edge_clamp,
  GlExtension_EXT_texture_env,
  GlExtension_EXT_texture_env_add,
  GlExtension_EXT_texture_env_combine,
  GlExtension_EXT_texture_env_dot3,
  GlExtension_EXT_texture_filter_anisotropic,
  GlExtension_EXT_texture_integer,
  GlExtension_EXT_texture_lod_bias,
  GlExtension_EXT_texture_mirror_clamp,
  GlExtension_EXT_texture_object,
  GlExtension_EXT_texture_perturb_normal,
  GlExtension_EXT_texture_rectangle,
  GlExtension_EXT_texture_sRGB,
  GlExtension_EXT_texture_shared_exponent,
  GlExtension_EXT_timer_query,
  GlExtension_EXT_vertex_array,
  GlExtension_EXT_vertex_shader,
  GlExtension_EXT_vertex_weighting,
  GlExtension_GREMEDY_string_marker,
  GlExtension_HP_convolution_border_modes,
  GlExtension_HP_image_transform,
  GlExtension_HP_occlusion_test,
  GlExtension_HP_texture_lighting,
  GlExtension_IBM_cull_vertex,
  GlExtension_IBM_multimode_draw_arrays,
  GlExtension_IBM_rasterpos_clip,
  GlExtension_IBM_static_data,
  GlExtension_IBM_texture_mirrored_repeat,
  GlExtension_IBM_vertex_array_lists,
  GlExtension_INGR_color_clamp,
  GlExtension_INGR_interlace_read,
  GlExtension_INTEL_parallel_arrays,
  GlExtension_INTEL_texture_scissor,
  GlExtension_KTX_buffer_region,
  GlExtension_MESAX_texture_stack,
  GlExtension_MESA_pack_invert,
  GlExtension_MESA_resize_buffers,
  GlExtension_MESA_window_pos,
  GlExtension_MESA_ycbcr_texture,
  GlExtension_NV_blend_square,
  GlExtension_NV_copy_depth_to_color,
  GlExtension_NV_depth_buffer_float,
  GlExtension_NV_depth_clamp,
  GlExtension_NV_depth_range_unclamped,
  GlExtension_NV_evaluators,
  GlExtension_NV_fence,
  GlExtension_NV_float_buffer,
  GlExtension_NV_fog_distance,
  GlExtension_NV_fragment_program,
  GlExtension_NV_fragment_program2,
  GlExtension_NV_fragment_program4,
  GlExtension_NV_fragment_program_option,
  GlExtension_NV_framebuffer_multisample_coverage,
  GlExtension_NV_geometry_program4,
  GlExtension_NV_geometry_shader4,
  GlExtension_NV_gpu_program4,
  GlExtension_NV_half_float,
  GlExtension_NV_light_max_exponent,
  GlExtension_NV_multisample_filter_hint,
  GlExtension_NV_occlusion_query,
  GlExtension_NV_packed_depth_stencil,
  GlExtension_NV_parameter_buffer_object,
  GlExtension_NV_pixel_data_range,
  GlExtension_NV_point_sprite,
  GlExtension_NV_primitive_restart,
  GlExtension_NV_register_combiners,
  GlExtension_NV_register_combiners2,
  GlExtension_NV_texgen_emboss,
  GlExtension_NV_texgen_reflection,
  GlExtension_NV_texture_compression_vtc,
  GlExtension_NV_texture_env_combine4,
  GlExtension_NV_texture_expand_normal,
  GlExtension_NV_texture_rectangle,
  GlExtension_NV_texture_shader,
  GlExtension_NV_texture_shader2,
  GlExtension_NV_texture_shader3,
  GlExtension_NV_transform_feedback,
  GlExtension_NV_vertex_array_range,
  GlExtension_NV_vertex_array_range2,
  GlExtension_NV_vertex_program,
  GlExtension_NV_vertex_program1_1,
  GlExtension_NV_vertex_program2,
  GlExtension_NV_vertex_program2_option,
  GlExtension_NV_vertex_program3,
  GlExtension_NV_vertex_program4,
  GlExtension_OES_byte_coordinates,
  GlExtension_OES_compressed_paletted_texture,
  GlExtension_OES_read_format,
  GlExtension_OES_single_precision,
  GlExtension_OML_interlace,
  GlExtension_OML_resample,
  GlExtension_OML_subsample,
  GlExtension_PGI_misc_hints,
  GlExtension_PGI_vertex_hints,
  GlExtension_REND_screen_coordinates,
  GlExtension_S3_s3tc,
  GlExtension_SGIS_color_range,
  GlExtension_SGIS_detail_texture,
  GlExtension_SGIS_fog_function,
  GlExtension_SGIS_generate_mipmap,
  GlExtension_SGIS_multisample,
  GlExtension_SGIS_pixel_texture,
  GlExtension_SGIS_sharpen_texture,
  GlExtension_SGIS_texture4D,
  GlExtension_SGIS_texture_border_clamp,
  GlExtension_SGIS_texture_edge_clamp,
  GlExtension_SGIS_texture_filter4,
  GlExtension_SGIS_texture_lod,
  GlExtension_SGIS_texture_select,
  GlExtension_SGIX_async,
  GlExtension_SGIX_async_histogram,
  GlExtension_SGIX_async_pixel,
  GlExtension_SGIX_blend_alpha_minmax,
  GlExtension_SGIX_clipmap,
  GlExtension_SGIX_depth_texture,
  GlExtension_SGIX_flush_raster,
  GlExtension_SGIX_fog_offset,
  GlExtension_SGIX_fog_texture,
  GlExtension_SGIX_fragment_specular_lighting,
  GlExtension_SGIX_framezoom,
  GlExtension_SGIX_interlace,
  GlExtension_SGIX_ir_instrument1,
  GlExtension_SGIX_list_priority,
  GlExtension_SGIX_pixel_texture,
  GlExtension_SGIX_pixel_texture_bits,
  GlExtension_SGIX_reference_plane,
  GlExtension_SGIX_resample,
  GlExtension_SGIX_shadow,
  GlExtension_SGIX_shadow_ambient,
  GlExtension_SGIX_sprite,
  GlExtension_SGIX_tag_sample_buffer,
  GlExtension_SGIX_texture_add_env,
  GlExtension_SGIX_texture_coordinate_clamp,
  GlExtension_SGIX_texture_lod_bias,
  GlExtension_SGIX_texture_multi_buffer,
  GlExtension_SGIX_texture_range,
  GlExtension_SGIX_texture_scale_bias,
  GlExtension_SGIX_vertex_preclip,
  GlExtension_SGIX_vertex_preclip_hint,
  GlExtension_SGIX_ycrcb,
  GlExtension_SGI_color_matrix,
  GlExtension_SGI_color_table,
  GlExtension_SGI_texture_color_table,
  GlExtension_SUNX_constant_data,
  GlExtension_SUN_convolution_border_modes,
  GlExtension_SUN_global_alpha,
  GlExtension_SUN_mesh_array,
  GlExtension_SUN_read_video_pixels,
  GlExtension_SUN_slice_accum,
  GlExtension_SUN_triangle_list,
  GlExtension_SUN_vertex,
  GlExtension_WIN_phong_shading,
  GlExtension_WIN_specular_fog,
  GlExtension_WIN_swap_hint,
  
  GlExtension_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение идентификатора расширения по строковому имени и обратная операция
///////////////////////////////////////////////////////////////////////////////////////////////////
GlExtension get_extension_id   (const char* full_name);
GlExtension get_extension_id   (const char* full_name, size_t length);
const char* get_extension_name (GlExtension id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Идентификатор таблицы локальных данных контекста
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ContextDataTable
{
  ContextDataTable_InputStage,
  ContextDataTable_ShaderStage,
  ContextDataTable_RasterizerStage,
  ContextDataTable_OutputStage,
  
  ContextDataTable_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество элементов в каждой из таблиц локальных данных контекста
///////////////////////////////////////////////////////////////////////////////////////////////////
const size_t CONTEXT_DATA_TABLE_SIZE = 64;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер контекстов OpenGL (политика копирования - подсчёт ссылок)
///////////////////////////////////////////////////////////////////////////////////////////////////
class ContextManager
{
  public:
    typedef xtl::function<void (const char*)> LogHandler;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    ContextManager  (const LogHandler& log_handler, const char* init_string="");
    ContextManager  (const ContextManager&);
    ~ContextManager ();

    ContextManager& operator = (const ContextManager&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание / удаление контекста
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t CreateContext (ISwapChain*);
    void   DeleteContext (size_t context_id);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание совместимой цепочки обмена
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISwapChain* CreateCompatibleSwapChain (size_t context_id);
    ISwapChain* CreateCompatibleSwapChain (ISwapChain*);
    ISwapChain* CreateCompatibleSwapChain (ISwapChain*, const SwapChainDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Выбор текущего контекста
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetContext       (size_t context_id, ISwapChain* draw_swap_chain, ISwapChain* read_swap_chain);
    ISwapChain* GetDrawSwapChain () const;
    ISwapChain* GetReadSwapChain () const;
    size_t      GetContextId     () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Активация текущего контекста
///////////////////////////////////////////////////////////////////////////////////////////////////
    void MakeContextCurrent () const;
    bool IsContextCurrent   () const; //убрать

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с таблицей локальных данных текущего контекста
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetContextData   (ContextDataTable table_id, size_t element_id, size_t value);
    size_t GetContextData   (ContextDataTable table_id, size_t element_id) const;
    void   ClearContextData (ContextDataTable table_id);
    void   ClearContextData ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение строки со списком расширений, зависящих от активной цепочки обмена
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetSwapChainsExtensionString () const; //убрать!!!

///////////////////////////////////////////////////////////////////////////////////////////////////
///Определение поддержки расширения контекстом
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsSupported (size_t context_id, GlExtension extension_id) const;
    bool IsSupported (GlExtension extension_id) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка совместимости контекста и цепочки обмена
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsCompatible (size_t context_id, ISwapChain* swap_chain) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка совместимости менеджеров контекстов
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsCompatible (const ContextManager&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Протоколирование
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LogPrintf  (const char* format, ...) const;
    void LogVPrintf (const char* format, va_list args) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка ошибок OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CheckErrors (const char* source) const;
    void RaiseError  (const char* source) const;

  private:
    struct Impl;
    Impl* impl;
};

}

}

}

#endif
