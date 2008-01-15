#include "shared.h"

namespace render
{

namespace low_level
{

namespace opengl
{

/*
    Базовый массив соответствий имён расширений их идентификаторам
*/

struct ExtensionPair
{
  const char* name;
  GlExtension id;
};

ExtensionPair extension_pairs [] =
{
  {"GL_VERSION_1_1", GlExtension_Version_1_1},
  {"GL_VERSION_1_2", GlExtension_Version_1_2},
  {"GL_VERSION_1_3", GlExtension_Version_1_3},
  {"GL_VERSION_1_4", GlExtension_Version_1_4},
  {"GL_VERSION_1_5", GlExtension_Version_1_5},
  {"GL_VERSION_2_0", GlExtension_Version_2_0},
  {"GL_VERSION_2_1", GlExtension_Version_2_1},
  {"GL_3DFX_multisample", GlExtension_3DFX_multisample},
  {"GL_3DFX_tbuffer", GlExtension_3DFX_tbuffer},
  {"GL_3DFX_texture_compression_FXT1", GlExtension_3DFX_texture_compression_FXT1},
  {"GL_APPLE_client_storage", GlExtension_APPLE_client_storage},
  {"GL_APPLE_element_array", GlExtension_APPLE_element_array},
  {"GL_APPLE_fence", GlExtension_APPLE_fence},
  {"GL_APPLE_float_pixels", GlExtension_APPLE_float_pixels},
  {"GL_APPLE_pixel_buffer", GlExtension_APPLE_pixel_buffer},
  {"GL_APPLE_specular_vector", GlExtension_APPLE_specular_vector},
  {"GL_APPLE_texture_range", GlExtension_APPLE_texture_range},
  {"GL_APPLE_transform_hint", GlExtension_APPLE_transform_hint},
  {"GL_APPLE_vertex_array_object", GlExtension_APPLE_vertex_array_object},
  {"GL_APPLE_vertex_array_range", GlExtension_APPLE_vertex_array_range},
  {"GL_APPLE_ycbcr_422", GlExtension_APPLE_ycbcr_422},
  {"GL_ARB_color_buffer_float", GlExtension_ARB_color_buffer_float},
  {"GL_ARB_depth_texture", GlExtension_ARB_depth_texture},
  {"GL_ARB_draw_buffers", GlExtension_ARB_draw_buffers},
  {"GL_ARB_fragment_program", GlExtension_ARB_fragment_program},
  {"GL_ARB_fragment_program_shadow", GlExtension_ARB_fragment_program_shadow},
  {"GL_ARB_fragment_shader", GlExtension_ARB_fragment_shader},
  {"GL_ARB_half_float_pixel", GlExtension_ARB_half_float_pixel},
  {"GL_ARB_imaging", GlExtension_ARB_imaging},
  {"GL_ARB_matrix_palette", GlExtension_ARB_matrix_palette},
  {"GL_ARB_multisample", GlExtension_ARB_multisample},
  {"GL_ARB_multitexture", GlExtension_ARB_multitexture},
  {"GL_ARB_occlusion_query", GlExtension_ARB_occlusion_query},
  {"GL_ARB_pixel_buffer_object", GlExtension_ARB_pixel_buffer_object},
  {"GL_ARB_point_parameters", GlExtension_ARB_point_parameters},
  {"GL_ARB_point_sprite", GlExtension_ARB_point_sprite},
  {"GL_ARB_shader_objects", GlExtension_ARB_shader_objects},
  {"GL_ARB_shading_language_100", GlExtension_ARB_shading_language_100},
  {"GL_ARB_shadow", GlExtension_ARB_shadow},
  {"GL_ARB_shadow_ambient", GlExtension_ARB_shadow_ambient},
  {"GL_ARB_texture_border_clamp", GlExtension_ARB_texture_border_clamp},
  {"GL_ARB_texture_compression", GlExtension_ARB_texture_compression},
  {"GL_ARB_texture_cube_map", GlExtension_ARB_texture_cube_map},
  {"GL_ARB_texture_env_add", GlExtension_ARB_texture_env_add},
  {"GL_ARB_texture_env_combine", GlExtension_ARB_texture_env_combine},
  {"GL_ARB_texture_env_crossbar", GlExtension_ARB_texture_env_crossbar},
  {"GL_ARB_texture_env_dot3", GlExtension_ARB_texture_env_dot3},
  {"GL_ARB_texture_float", GlExtension_ARB_texture_float},
  {"GL_ARB_texture_mirrored_repeat", GlExtension_ARB_texture_mirrored_repeat},
  {"GL_ARB_texture_non_power_of_two", GlExtension_ARB_texture_non_power_of_two},
  {"GL_ARB_texture_rectangle", GlExtension_ARB_texture_rectangle},
  {"GL_ARB_transpose_matrix", GlExtension_ARB_transpose_matrix},
  {"GL_ARB_vertex_blend", GlExtension_ARB_vertex_blend},
  {"GL_ARB_vertex_buffer_object", GlExtension_ARB_vertex_buffer_object},
  {"GL_ARB_vertex_program", GlExtension_ARB_vertex_program},
  {"GL_ARB_vertex_shader", GlExtension_ARB_vertex_shader},
  {"GL_ARB_window_pos", GlExtension_ARB_window_pos},
  {"GL_ATIX_point_sprites", GlExtension_ATIX_point_sprites},
  {"GL_ATIX_texture_env_combine3", GlExtension_ATIX_texture_env_combine3},
  {"GL_ATIX_texture_env_route", GlExtension_ATIX_texture_env_route},
  {"GL_ATIX_vertex_shader_output_point_size", GlExtension_ATIX_vertex_shader_output_point_size},
  {"GL_ATI_draw_buffers", GlExtension_ATI_draw_buffers},
  {"GL_ATI_element_array", GlExtension_ATI_element_array},
  {"GL_ATI_envmap_bumpmap", GlExtension_ATI_envmap_bumpmap},
  {"GL_ATI_fragment_shader", GlExtension_ATI_fragment_shader},
  {"GL_ATI_map_object_buffer", GlExtension_ATI_map_object_buffer},
  {"GL_ATI_pn_triangles", GlExtension_ATI_pn_triangles},
  {"GL_ATI_separate_stencil", GlExtension_ATI_separate_stencil},
  {"GL_ATI_shader_texture_lod", GlExtension_ATI_shader_texture_lod},
  {"GL_ATI_text_fragment_shader", GlExtension_ATI_text_fragment_shader},
  {"GL_ATI_texture_compression_3dc", GlExtension_ATI_texture_compression_3dc},
  {"GL_ATI_texture_env_combine3", GlExtension_ATI_texture_env_combine3},
  {"GL_ATI_texture_float", GlExtension_ATI_texture_float},
  {"GL_ATI_texture_mirror_once", GlExtension_ATI_texture_mirror_once},
  {"GL_ATI_vertex_array_object", GlExtension_ATI_vertex_array_object},
  {"GL_ATI_vertex_attrib_array_object", GlExtension_ATI_vertex_attrib_array_object},
  {"GL_ATI_vertex_streams", GlExtension_ATI_vertex_streams},
  {"GL_EXT_422_pixels", GlExtension_EXT_422_pixels},
  {"GL_EXT_Cg_shader", GlExtension_EXT_Cg_shader},
  {"GL_EXT_abgr", GlExtension_EXT_abgr},
  {"GL_EXT_bgra", GlExtension_EXT_bgra},
  {"GL_EXT_bindable_uniform", GlExtension_EXT_bindable_uniform},
  {"GL_EXT_blend_color", GlExtension_EXT_blend_color},
  {"GL_EXT_blend_equation_separate", GlExtension_EXT_blend_equation_separate},
  {"GL_EXT_blend_func_separate", GlExtension_EXT_blend_func_separate},
  {"GL_EXT_blend_logic_op", GlExtension_EXT_blend_logic_op},
  {"GL_EXT_blend_minmax", GlExtension_EXT_blend_minmax},
  {"GL_EXT_blend_subtract", GlExtension_EXT_blend_subtract},
  {"GL_EXT_clip_volume_hint", GlExtension_EXT_clip_volume_hint},
  {"GL_EXT_cmyka", GlExtension_EXT_cmyka},
  {"GL_EXT_color_subtable", GlExtension_EXT_color_subtable},
  {"GL_EXT_compiled_vertex_array", GlExtension_EXT_compiled_vertex_array},
  {"GL_EXT_convolution", GlExtension_EXT_convolution},
  {"GL_EXT_coordinate_frame", GlExtension_EXT_coordinate_frame},
  {"GL_EXT_copy_texture", GlExtension_EXT_copy_texture},
  {"GL_EXT_cull_vertex", GlExtension_EXT_cull_vertex},
  {"GL_EXT_depth_bounds_test", GlExtension_EXT_depth_bounds_test},
  {"GL_EXT_draw_buffers2", GlExtension_EXT_draw_buffers2},
  {"GL_EXT_draw_instanced", GlExtension_EXT_draw_instanced},
  {"GL_EXT_draw_range_elements", GlExtension_EXT_draw_range_elements},
  {"GL_EXT_fog_coord", GlExtension_EXT_fog_coord},
  {"GL_EXT_fragment_lighting", GlExtension_EXT_fragment_lighting},
  {"GL_EXT_framebuffer_blit", GlExtension_EXT_framebuffer_blit},
  {"GL_EXT_framebuffer_multisample", GlExtension_EXT_framebuffer_multisample},
  {"GL_EXT_framebuffer_object", GlExtension_EXT_framebuffer_object},
  {"GL_EXT_framebuffer_sRGB", GlExtension_EXT_framebuffer_sRGB},
  {"GL_EXT_geometry_shader4", GlExtension_EXT_geometry_shader4},
  {"GL_EXT_gpu_program_parameters", GlExtension_EXT_gpu_program_parameters},
  {"GL_EXT_gpu_shader4", GlExtension_EXT_gpu_shader4},
  {"GL_EXT_histogram", GlExtension_EXT_histogram},
  {"GL_EXT_index_array_formats", GlExtension_EXT_index_array_formats},
  {"GL_EXT_index_func", GlExtension_EXT_index_func},
  {"GL_EXT_index_material", GlExtension_EXT_index_material},
  {"GL_EXT_index_texture", GlExtension_EXT_index_texture},
  {"GL_EXT_light_texture", GlExtension_EXT_light_texture},
  {"GL_EXT_misc_attribute", GlExtension_EXT_misc_attribute},
  {"GL_EXT_multi_draw_arrays", GlExtension_EXT_multi_draw_arrays},
  {"GL_EXT_multisample", GlExtension_EXT_multisample},
  {"GL_EXT_packed_depth_stencil", GlExtension_EXT_packed_depth_stencil},
  {"GL_EXT_packed_float", GlExtension_EXT_packed_float},
  {"GL_EXT_packed_pixels", GlExtension_EXT_packed_pixels},
  {"GL_EXT_paletted_texture", GlExtension_EXT_paletted_texture},
  {"GL_EXT_pixel_buffer_object", GlExtension_EXT_pixel_buffer_object},
  {"GL_EXT_pixel_transform", GlExtension_EXT_pixel_transform},
  {"GL_EXT_pixel_transform_color_table", GlExtension_EXT_pixel_transform_color_table},
  {"GL_EXT_point_parameters", GlExtension_EXT_point_parameters},
  {"GL_EXT_polygon_offset", GlExtension_EXT_polygon_offset},
  {"GL_EXT_rescale_normal", GlExtension_EXT_rescale_normal},
  {"GL_EXT_scene_marker", GlExtension_EXT_scene_marker},
  {"GL_EXT_secondary_color", GlExtension_EXT_secondary_color},
  {"GL_EXT_separate_specular_color", GlExtension_EXT_separate_specular_color},
  {"GL_EXT_shadow_funcs", GlExtension_EXT_shadow_funcs},
  {"GL_EXT_shared_texture_palette", GlExtension_EXT_shared_texture_palette},
  {"GL_EXT_stencil_clear_tag", GlExtension_EXT_stencil_clear_tag},
  {"GL_EXT_stencil_two_side", GlExtension_EXT_stencil_two_side},
  {"GL_EXT_stencil_wrap", GlExtension_EXT_stencil_wrap},
  {"GL_EXT_subtexture", GlExtension_EXT_subtexture},
  {"GL_EXT_texture", GlExtension_EXT_texture},
  {"GL_EXT_texture3D", GlExtension_EXT_texture3D},
  {"GL_EXT_texture_array", GlExtension_EXT_texture_array},
  {"GL_EXT_texture_buffer_object", GlExtension_EXT_texture_buffer_object},
  {"GL_EXT_texture_compression_dxt1", GlExtension_EXT_texture_compression_dxt1},
  {"GL_EXT_texture_compression_latc", GlExtension_EXT_texture_compression_latc},
  {"GL_EXT_texture_compression_rgtc", GlExtension_EXT_texture_compression_rgtc},
  {"GL_EXT_texture_compression_s3tc", GlExtension_EXT_texture_compression_s3tc},
  {"GL_EXT_texture_cube_map", GlExtension_EXT_texture_cube_map},
  {"GL_EXT_texture_edge_clamp", GlExtension_EXT_texture_edge_clamp},
  {"GL_EXT_texture_env", GlExtension_EXT_texture_env},
  {"GL_EXT_texture_env_add", GlExtension_EXT_texture_env_add},
  {"GL_EXT_texture_env_combine", GlExtension_EXT_texture_env_combine},
  {"GL_EXT_texture_env_dot3", GlExtension_EXT_texture_env_dot3},
  {"GL_EXT_texture_filter_anisotropic", GlExtension_EXT_texture_filter_anisotropic},
  {"GL_EXT_texture_integer", GlExtension_EXT_texture_integer},
  {"GL_EXT_texture_lod_bias", GlExtension_EXT_texture_lod_bias},
  {"GL_EXT_texture_mirror_clamp", GlExtension_EXT_texture_mirror_clamp},
  {"GL_EXT_texture_object", GlExtension_EXT_texture_object},
  {"GL_EXT_texture_perturb_normal", GlExtension_EXT_texture_perturb_normal},
  {"GL_EXT_texture_rectangle", GlExtension_EXT_texture_rectangle},
  {"GL_EXT_texture_sRGB", GlExtension_EXT_texture_sRGB},
  {"GL_EXT_texture_shared_exponent", GlExtension_EXT_texture_shared_exponent},
  {"GL_EXT_timer_query", GlExtension_EXT_timer_query},
  {"GL_EXT_vertex_array", GlExtension_EXT_vertex_array},
  {"GL_EXT_vertex_shader", GlExtension_EXT_vertex_shader},
  {"GL_EXT_vertex_weighting", GlExtension_EXT_vertex_weighting},
  {"GL_GREMEDY_string_marker", GlExtension_GREMEDY_string_marker},
  {"GL_HP_convolution_border_modes", GlExtension_HP_convolution_border_modes},
  {"GL_HP_image_transform", GlExtension_HP_image_transform},
  {"GL_HP_occlusion_test", GlExtension_HP_occlusion_test},
  {"GL_HP_texture_lighting", GlExtension_HP_texture_lighting},
  {"GL_IBM_cull_vertex", GlExtension_IBM_cull_vertex},
  {"GL_IBM_multimode_draw_arrays", GlExtension_IBM_multimode_draw_arrays},
  {"GL_IBM_rasterpos_clip", GlExtension_IBM_rasterpos_clip},
  {"GL_IBM_static_data", GlExtension_IBM_static_data},
  {"GL_IBM_texture_mirrored_repeat", GlExtension_IBM_texture_mirrored_repeat},
  {"GL_IBM_vertex_array_lists", GlExtension_IBM_vertex_array_lists},
  {"GL_INGR_color_clamp", GlExtension_INGR_color_clamp},
  {"GL_INGR_interlace_read", GlExtension_INGR_interlace_read},
  {"GL_INTEL_parallel_arrays", GlExtension_INTEL_parallel_arrays},
  {"GL_INTEL_texture_scissor", GlExtension_INTEL_texture_scissor},
  {"GL_KTX_buffer_region", GlExtension_KTX_buffer_region},
  {"GL_MESAX_texture_stack", GlExtension_MESAX_texture_stack},
  {"GL_MESA_pack_invert", GlExtension_MESA_pack_invert},
  {"GL_MESA_resize_buffers", GlExtension_MESA_resize_buffers},
  {"GL_MESA_window_pos", GlExtension_MESA_window_pos},
  {"GL_MESA_ycbcr_texture", GlExtension_MESA_ycbcr_texture},
  {"GL_NV_blend_square", GlExtension_NV_blend_square},
  {"GL_NV_copy_depth_to_color", GlExtension_NV_copy_depth_to_color},
  {"GL_NV_depth_buffer_float", GlExtension_NV_depth_buffer_float},
  {"GL_NV_depth_clamp", GlExtension_NV_depth_clamp},
  {"GL_NV_depth_range_unclamped", GlExtension_NV_depth_range_unclamped},
  {"GL_NV_evaluators", GlExtension_NV_evaluators},
  {"GL_NV_fence", GlExtension_NV_fence},
  {"GL_NV_float_buffer", GlExtension_NV_float_buffer},
  {"GL_NV_fog_distance", GlExtension_NV_fog_distance},
  {"GL_NV_fragment_program", GlExtension_NV_fragment_program},
  {"GL_NV_fragment_program2", GlExtension_NV_fragment_program2},
  {"GL_NV_fragment_program4", GlExtension_NV_fragment_program4},
  {"GL_NV_fragment_program_option", GlExtension_NV_fragment_program_option},
  {"GL_NV_framebuffer_multisample_coverage", GlExtension_NV_framebuffer_multisample_coverage},
  {"GL_NV_geometry_program4", GlExtension_NV_geometry_program4},
  {"GL_NV_geometry_shader4", GlExtension_NV_geometry_shader4},
  {"GL_NV_gpu_program4", GlExtension_NV_gpu_program4},
  {"GL_NV_half_float", GlExtension_NV_half_float},
  {"GL_NV_light_max_exponent", GlExtension_NV_light_max_exponent},
  {"GL_NV_multisample_filter_hint", GlExtension_NV_multisample_filter_hint},
  {"GL_NV_occlusion_query", GlExtension_NV_occlusion_query},
  {"GL_NV_packed_depth_stencil", GlExtension_NV_packed_depth_stencil},
  {"GL_NV_parameter_buffer_object", GlExtension_NV_parameter_buffer_object},
  {"GL_NV_pixel_data_range", GlExtension_NV_pixel_data_range},
  {"GL_NV_point_sprite", GlExtension_NV_point_sprite},
  {"GL_NV_primitive_restart", GlExtension_NV_primitive_restart},
  {"GL_NV_register_combiners", GlExtension_NV_register_combiners},
  {"GL_NV_register_combiners2", GlExtension_NV_register_combiners2},
  {"GL_NV_texgen_emboss", GlExtension_NV_texgen_emboss},
  {"GL_NV_texgen_reflection", GlExtension_NV_texgen_reflection},
  {"GL_NV_texture_compression_vtc", GlExtension_NV_texture_compression_vtc},
  {"GL_NV_texture_env_combine4", GlExtension_NV_texture_env_combine4},
  {"GL_NV_texture_expand_normal", GlExtension_NV_texture_expand_normal},
  {"GL_NV_texture_rectangle", GlExtension_NV_texture_rectangle},
  {"GL_NV_texture_shader", GlExtension_NV_texture_shader},
  {"GL_NV_texture_shader2", GlExtension_NV_texture_shader2},
  {"GL_NV_texture_shader3", GlExtension_NV_texture_shader3},
  {"GL_NV_transform_feedback", GlExtension_NV_transform_feedback},
  {"GL_NV_vertex_array_range", GlExtension_NV_vertex_array_range},
  {"GL_NV_vertex_array_range2", GlExtension_NV_vertex_array_range2},
  {"GL_NV_vertex_program", GlExtension_NV_vertex_program},
  {"GL_NV_vertex_program1_1", GlExtension_NV_vertex_program1_1},
  {"GL_NV_vertex_program2", GlExtension_NV_vertex_program2},
  {"GL_NV_vertex_program2_option", GlExtension_NV_vertex_program2_option},
  {"GL_NV_vertex_program3", GlExtension_NV_vertex_program3},
  {"GL_NV_vertex_program4", GlExtension_NV_vertex_program4},
  {"GL_OES_byte_coordinates", GlExtension_OES_byte_coordinates},
  {"GL_OES_compressed_paletted_texture", GlExtension_OES_compressed_paletted_texture},
  {"GL_OES_read_format", GlExtension_OES_read_format},
  {"GL_OES_single_precision", GlExtension_OES_single_precision},
  {"GL_OML_interlace", GlExtension_OML_interlace},
  {"GL_OML_resample", GlExtension_OML_resample},
  {"GL_OML_subsample", GlExtension_OML_subsample},
  {"GL_PGI_misc_hints", GlExtension_PGI_misc_hints},
  {"GL_PGI_vertex_hints", GlExtension_PGI_vertex_hints},
  {"GL_REND_screen_coordinates", GlExtension_REND_screen_coordinates},
  {"GL_S3_s3tc", GlExtension_S3_s3tc},
  {"GL_SGIS_color_range", GlExtension_SGIS_color_range},
  {"GL_SGIS_detail_texture", GlExtension_SGIS_detail_texture},
  {"GL_SGIS_fog_function", GlExtension_SGIS_fog_function},
  {"GL_SGIS_generate_mipmap", GlExtension_SGIS_generate_mipmap},
  {"GL_SGIS_multisample", GlExtension_SGIS_multisample},
  {"GL_SGIS_pixel_texture", GlExtension_SGIS_pixel_texture},
  {"GL_SGIS_sharpen_texture", GlExtension_SGIS_sharpen_texture},
  {"GL_SGIS_texture4D", GlExtension_SGIS_texture4D},
  {"GL_SGIS_texture_border_clamp", GlExtension_SGIS_texture_border_clamp},
  {"GL_SGIS_texture_edge_clamp", GlExtension_SGIS_texture_edge_clamp},
  {"GL_SGIS_texture_filter4", GlExtension_SGIS_texture_filter4},
  {"GL_SGIS_texture_lod", GlExtension_SGIS_texture_lod},
  {"GL_SGIS_texture_select", GlExtension_SGIS_texture_select},
  {"GL_SGIX_async", GlExtension_SGIX_async},
  {"GL_SGIX_async_histogram", GlExtension_SGIX_async_histogram},
  {"GL_SGIX_async_pixel", GlExtension_SGIX_async_pixel},
  {"GL_SGIX_blend_alpha_minmax", GlExtension_SGIX_blend_alpha_minmax},
  {"GL_SGIX_clipmap", GlExtension_SGIX_clipmap},
  {"GL_SGIX_depth_texture", GlExtension_SGIX_depth_texture},
  {"GL_SGIX_flush_raster", GlExtension_SGIX_flush_raster},
  {"GL_SGIX_fog_offset", GlExtension_SGIX_fog_offset},
  {"GL_SGIX_fog_texture", GlExtension_SGIX_fog_texture},
  {"GL_SGIX_fragment_specular_lighting", GlExtension_SGIX_fragment_specular_lighting},
  {"GL_SGIX_framezoom", GlExtension_SGIX_framezoom},
  {"GL_SGIX_interlace", GlExtension_SGIX_interlace},
  {"GL_SGIX_ir_instrument1", GlExtension_SGIX_ir_instrument1},
  {"GL_SGIX_list_priority", GlExtension_SGIX_list_priority},
  {"GL_SGIX_pixel_texture", GlExtension_SGIX_pixel_texture},
  {"GL_SGIX_pixel_texture_bits", GlExtension_SGIX_pixel_texture_bits},
  {"GL_SGIX_reference_plane", GlExtension_SGIX_reference_plane},
  {"GL_SGIX_resample", GlExtension_SGIX_resample},
  {"GL_SGIX_shadow", GlExtension_SGIX_shadow},
  {"GL_SGIX_shadow_ambient", GlExtension_SGIX_shadow_ambient},
  {"GL_SGIX_sprite", GlExtension_SGIX_sprite},
  {"GL_SGIX_tag_sample_buffer", GlExtension_SGIX_tag_sample_buffer},
  {"GL_SGIX_texture_add_env", GlExtension_SGIX_texture_add_env},
  {"GL_SGIX_texture_coordinate_clamp", GlExtension_SGIX_texture_coordinate_clamp},
  {"GL_SGIX_texture_lod_bias", GlExtension_SGIX_texture_lod_bias},
  {"GL_SGIX_texture_multi_buffer", GlExtension_SGIX_texture_multi_buffer},
  {"GL_SGIX_texture_range", GlExtension_SGIX_texture_range},
  {"GL_SGIX_texture_scale_bias", GlExtension_SGIX_texture_scale_bias},
  {"GL_SGIX_vertex_preclip", GlExtension_SGIX_vertex_preclip},
  {"GL_SGIX_vertex_preclip_hint", GlExtension_SGIX_vertex_preclip_hint},
  {"GL_SGIX_ycrcb", GlExtension_SGIX_ycrcb},
  {"GL_SGI_color_matrix", GlExtension_SGI_color_matrix},
  {"GL_SGI_color_table", GlExtension_SGI_color_table},
  {"GL_SGI_texture_color_table", GlExtension_SGI_texture_color_table},
  {"GL_SUNX_constant_data", GlExtension_SUNX_constant_data},
  {"GL_SUN_convolution_border_modes", GlExtension_SUN_convolution_border_modes},
  {"GL_SUN_global_alpha", GlExtension_SUN_global_alpha},
  {"GL_SUN_mesh_array", GlExtension_SUN_mesh_array},
  {"GL_SUN_read_video_pixels", GlExtension_SUN_read_video_pixels},
  {"GL_SUN_slice_accum", GlExtension_SUN_slice_accum},
  {"GL_SUN_triangle_list", GlExtension_SUN_triangle_list},
  {"GL_SUN_vertex", GlExtension_SUN_vertex},
  {"GL_WIN_phong_shading", GlExtension_WIN_phong_shading},
  {"GL_WIN_specular_fog", GlExtension_WIN_specular_fog},
  {"GL_WIN_swap_hint", GlExtension_WIN_swap_hint},
};

/*
    Хранилище ассоциативного массива соответствий между строковым именем расширения и его идентификатором
*/

namespace
{

class ExtensionMap
{
  public:
    ExtensionMap ()
    {
      static const size_t EXTENSION_PAIRS_NUM = sizeof (extension_pairs) / sizeof (*extension_pairs);
      
      const ExtensionPair* pair = extension_pairs;
      
      map.resize (EXTENSION_PAIRS_NUM);      
      
      for (size_t i=0; i<EXTENSION_PAIRS_NUM; i++, pair++)
        map [common::strihash (pair->name)] = pair->id;
    }
    
    GlExtension FindExtension (const char* name, size_t length) const
    {
      Map::const_iterator iter = map.find (common::strnihash (name, length));

      if (iter == map.end ())
        return GlExtension_Unknown;

      return iter->second;
    }

  private:
    typedef stl::hash_map<size_t, GlExtension> Map;
    
  private:
    Map map;
};

}

/*
    Получение идентификатора расширения по строковому имени
*/

GlExtension get_extension_id (const char* full_name, size_t length)
{
  if (!full_name || !*full_name)
    return GlExtension_Num;

  typedef common::Singleton<ExtensionMap> ExtensionMapSingleton;
  
  return ExtensionMapSingleton::Instance ().FindExtension (full_name, length);
}

GlExtension get_extension_id (const char* full_name)
{
  if (!full_name || !*full_name)
    return GlExtension_Num;    

  return get_extension_id (full_name, strlen (full_name));
}

/*
    Получение имени расширения по идентификатору
*/

const char* get_extension_name (GlExtension id)
{
  switch (id)
  {
    case GlExtension_Version_1_1: return "GL_VERSION_1_1";
    case GlExtension_Version_1_2: return "GL_VERSION_1_2";
    case GlExtension_Version_1_3: return "GL_VERSION_1_3";
    case GlExtension_Version_1_4: return "GL_VERSION_1_4";
    case GlExtension_Version_1_5: return "GL_VERSION_1_5";
    case GlExtension_Version_2_0: return "GL_VERSION_2_0";
    case GlExtension_Version_2_1: return "GL_VERSION_2_1";
    case GlExtension_3DFX_multisample: return "GL_3DFX_multisample";
    case GlExtension_3DFX_tbuffer: return "GL_3DFX_tbuffer";
    case GlExtension_3DFX_texture_compression_FXT1: return "GL_3DFX_texture_compression_FXT1";
    case GlExtension_APPLE_client_storage: return "GL_APPLE_client_storage";
    case GlExtension_APPLE_element_array: return "GL_APPLE_element_array";
    case GlExtension_APPLE_fence: return "GL_APPLE_fence";
    case GlExtension_APPLE_float_pixels: return "GL_APPLE_float_pixels";
    case GlExtension_APPLE_pixel_buffer: return "GL_APPLE_pixel_buffer";
    case GlExtension_APPLE_specular_vector: return "GL_APPLE_specular_vector";
    case GlExtension_APPLE_texture_range: return "GL_APPLE_texture_range";
    case GlExtension_APPLE_transform_hint: return "GL_APPLE_transform_hint";
    case GlExtension_APPLE_vertex_array_object: return "GL_APPLE_vertex_array_object";
    case GlExtension_APPLE_vertex_array_range: return "GL_APPLE_vertex_array_range";
    case GlExtension_APPLE_ycbcr_422: return "GL_APPLE_ycbcr_422";
    case GlExtension_ARB_color_buffer_float: return "GL_ARB_color_buffer_float";
    case GlExtension_ARB_depth_texture: return "GL_ARB_depth_texture";
    case GlExtension_ARB_draw_buffers: return "GL_ARB_draw_buffers";
    case GlExtension_ARB_fragment_program: return "GL_ARB_fragment_program";
    case GlExtension_ARB_fragment_program_shadow: return "GL_ARB_fragment_program_shadow";
    case GlExtension_ARB_fragment_shader: return "GL_ARB_fragment_shader";
    case GlExtension_ARB_half_float_pixel: return "GL_ARB_half_float_pixel";
    case GlExtension_ARB_imaging: return "GL_ARB_imaging";
    case GlExtension_ARB_matrix_palette: return "GL_ARB_matrix_palette";
    case GlExtension_ARB_multisample: return "GL_ARB_multisample";
    case GlExtension_ARB_multitexture: return "GL_ARB_multitexture";
    case GlExtension_ARB_occlusion_query: return "GL_ARB_occlusion_query";
    case GlExtension_ARB_pixel_buffer_object: return "GL_ARB_pixel_buffer_object";
    case GlExtension_ARB_point_parameters: return "GL_ARB_point_parameters";
    case GlExtension_ARB_point_sprite: return "GL_ARB_point_sprite";
    case GlExtension_ARB_shader_objects: return "GL_ARB_shader_objects";
    case GlExtension_ARB_shading_language_100: return "GL_ARB_shading_language_100";
    case GlExtension_ARB_shadow: return "GL_ARB_shadow";
    case GlExtension_ARB_shadow_ambient: return "GL_ARB_shadow_ambient";
    case GlExtension_ARB_texture_border_clamp: return "GL_ARB_texture_border_clamp";
    case GlExtension_ARB_texture_compression: return "GL_ARB_texture_compression";
    case GlExtension_ARB_texture_cube_map: return "GL_ARB_texture_cube_map";
    case GlExtension_ARB_texture_env_add: return "GL_ARB_texture_env_add";
    case GlExtension_ARB_texture_env_combine: return "GL_ARB_texture_env_combine";
    case GlExtension_ARB_texture_env_crossbar: return "GL_ARB_texture_env_crossbar";
    case GlExtension_ARB_texture_env_dot3: return "GL_ARB_texture_env_dot3";
    case GlExtension_ARB_texture_float: return "GL_ARB_texture_float";
    case GlExtension_ARB_texture_mirrored_repeat: return "GL_ARB_texture_mirrored_repeat";
    case GlExtension_ARB_texture_non_power_of_two: return "GL_ARB_texture_non_power_of_two";
    case GlExtension_ARB_texture_rectangle: return "GL_ARB_texture_rectangle";
    case GlExtension_ARB_transpose_matrix: return "GL_ARB_transpose_matrix";
    case GlExtension_ARB_vertex_blend: return "GL_ARB_vertex_blend";
    case GlExtension_ARB_vertex_buffer_object: return "GL_ARB_vertex_buffer_object";
    case GlExtension_ARB_vertex_program: return "GL_ARB_vertex_program";
    case GlExtension_ARB_vertex_shader: return "GL_ARB_vertex_shader";
    case GlExtension_ARB_window_pos: return "GL_ARB_window_pos";
    case GlExtension_ATIX_point_sprites: return "GL_ATIX_point_sprites";
    case GlExtension_ATIX_texture_env_combine3: return "GL_ATIX_texture_env_combine3";
    case GlExtension_ATIX_texture_env_route: return "GL_ATIX_texture_env_route";
    case GlExtension_ATIX_vertex_shader_output_point_size: return "GL_ATIX_vertex_shader_output_point_size";
    case GlExtension_ATI_draw_buffers: return "GL_ATI_draw_buffers";
    case GlExtension_ATI_element_array: return "GL_ATI_element_array";
    case GlExtension_ATI_envmap_bumpmap: return "GL_ATI_envmap_bumpmap";
    case GlExtension_ATI_fragment_shader: return "GL_ATI_fragment_shader";
    case GlExtension_ATI_map_object_buffer: return "GL_ATI_map_object_buffer";
    case GlExtension_ATI_pn_triangles: return "GL_ATI_pn_triangles";
    case GlExtension_ATI_separate_stencil: return "GL_ATI_separate_stencil";
    case GlExtension_ATI_shader_texture_lod: return "GL_ATI_shader_texture_lod";
    case GlExtension_ATI_text_fragment_shader: return "GL_ATI_text_fragment_shader";
    case GlExtension_ATI_texture_compression_3dc: return "GL_ATI_texture_compression_3dc";
    case GlExtension_ATI_texture_env_combine3: return "GL_ATI_texture_env_combine3";
    case GlExtension_ATI_texture_float: return "GL_ATI_texture_float";
    case GlExtension_ATI_texture_mirror_once: return "GL_ATI_texture_mirror_once";
    case GlExtension_ATI_vertex_array_object: return "GL_ATI_vertex_array_object";
    case GlExtension_ATI_vertex_attrib_array_object: return "GL_ATI_vertex_attrib_array_object";
    case GlExtension_ATI_vertex_streams: return "GL_ATI_vertex_streams";
    case GlExtension_EXT_422_pixels: return "GL_EXT_422_pixels";
    case GlExtension_EXT_Cg_shader: return "GL_EXT_Cg_shader";
    case GlExtension_EXT_abgr: return "GL_EXT_abgr";
    case GlExtension_EXT_bgra: return "GL_EXT_bgra";
    case GlExtension_EXT_bindable_uniform: return "GL_EXT_bindable_uniform";
    case GlExtension_EXT_blend_color: return "GL_EXT_blend_color";
    case GlExtension_EXT_blend_equation_separate: return "GL_EXT_blend_equation_separate";
    case GlExtension_EXT_blend_func_separate: return "GL_EXT_blend_func_separate";
    case GlExtension_EXT_blend_logic_op: return "GL_EXT_blend_logic_op";
    case GlExtension_EXT_blend_minmax: return "GL_EXT_blend_minmax";
    case GlExtension_EXT_blend_subtract: return "GL_EXT_blend_subtract";
    case GlExtension_EXT_clip_volume_hint: return "GL_EXT_clip_volume_hint";
    case GlExtension_EXT_cmyka: return "GL_EXT_cmyka";
    case GlExtension_EXT_color_subtable: return "GL_EXT_color_subtable";
    case GlExtension_EXT_compiled_vertex_array: return "GL_EXT_compiled_vertex_array";
    case GlExtension_EXT_convolution: return "GL_EXT_convolution";
    case GlExtension_EXT_coordinate_frame: return "GL_EXT_coordinate_frame";
    case GlExtension_EXT_copy_texture: return "GL_EXT_copy_texture";
    case GlExtension_EXT_cull_vertex: return "GL_EXT_cull_vertex";
    case GlExtension_EXT_depth_bounds_test: return "GL_EXT_depth_bounds_test";
    case GlExtension_EXT_draw_buffers2: return "GL_EXT_draw_buffers2";
    case GlExtension_EXT_draw_instanced: return "GL_EXT_draw_instanced";
    case GlExtension_EXT_draw_range_elements: return "GL_EXT_draw_range_elements";
    case GlExtension_EXT_fog_coord: return "GL_EXT_fog_coord";
    case GlExtension_EXT_fragment_lighting: return "GL_EXT_fragment_lighting";
    case GlExtension_EXT_framebuffer_blit: return "GL_EXT_framebuffer_blit";
    case GlExtension_EXT_framebuffer_multisample: return "GL_EXT_framebuffer_multisample";
    case GlExtension_EXT_framebuffer_object: return "GL_EXT_framebuffer_object";
    case GlExtension_EXT_framebuffer_sRGB: return "GL_EXT_framebuffer_sRGB";
    case GlExtension_EXT_geometry_shader4: return "GL_EXT_geometry_shader4";
    case GlExtension_EXT_gpu_program_parameters: return "GL_EXT_gpu_program_parameters";
    case GlExtension_EXT_gpu_shader4: return "GL_EXT_gpu_shader4";
    case GlExtension_EXT_histogram: return "GL_EXT_histogram";
    case GlExtension_EXT_index_array_formats: return "GL_EXT_index_array_formats";
    case GlExtension_EXT_index_func: return "GL_EXT_index_func";
    case GlExtension_EXT_index_material: return "GL_EXT_index_material";
    case GlExtension_EXT_index_texture: return "GL_EXT_index_texture";
    case GlExtension_EXT_light_texture: return "GL_EXT_light_texture";
    case GlExtension_EXT_misc_attribute: return "GL_EXT_misc_attribute";
    case GlExtension_EXT_multi_draw_arrays: return "GL_EXT_multi_draw_arrays";
    case GlExtension_EXT_multisample: return "GL_EXT_multisample";
    case GlExtension_EXT_packed_depth_stencil: return "GL_EXT_packed_depth_stencil";
    case GlExtension_EXT_packed_float: return "GL_EXT_packed_float";
    case GlExtension_EXT_packed_pixels: return "GL_EXT_packed_pixels";
    case GlExtension_EXT_paletted_texture: return "GL_EXT_paletted_texture";
    case GlExtension_EXT_pixel_buffer_object: return "GL_EXT_pixel_buffer_object";
    case GlExtension_EXT_pixel_transform: return "GL_EXT_pixel_transform";
    case GlExtension_EXT_pixel_transform_color_table: return "GL_EXT_pixel_transform_color_table";
    case GlExtension_EXT_point_parameters: return "GL_EXT_point_parameters";
    case GlExtension_EXT_polygon_offset: return "GL_EXT_polygon_offset";
    case GlExtension_EXT_rescale_normal: return "GL_EXT_rescale_normal";
    case GlExtension_EXT_scene_marker: return "GL_EXT_scene_marker";
    case GlExtension_EXT_secondary_color: return "GL_EXT_secondary_color";
    case GlExtension_EXT_separate_specular_color: return "GL_EXT_separate_specular_color";
    case GlExtension_EXT_shadow_funcs: return "GL_EXT_shadow_funcs";
    case GlExtension_EXT_shared_texture_palette: return "GL_EXT_shared_texture_palette";
    case GlExtension_EXT_stencil_clear_tag: return "GL_EXT_stencil_clear_tag";
    case GlExtension_EXT_stencil_two_side: return "GL_EXT_stencil_two_side";
    case GlExtension_EXT_stencil_wrap: return "GL_EXT_stencil_wrap";
    case GlExtension_EXT_subtexture: return "GL_EXT_subtexture";
    case GlExtension_EXT_texture: return "GL_EXT_texture";
    case GlExtension_EXT_texture3D: return "GL_EXT_texture3D";
    case GlExtension_EXT_texture_array: return "GL_EXT_texture_array";
    case GlExtension_EXT_texture_buffer_object: return "GL_EXT_texture_buffer_object";
    case GlExtension_EXT_texture_compression_dxt1: return "GL_EXT_texture_compression_dxt1";
    case GlExtension_EXT_texture_compression_latc: return "GL_EXT_texture_compression_latc";
    case GlExtension_EXT_texture_compression_rgtc: return "GL_EXT_texture_compression_rgtc";
    case GlExtension_EXT_texture_compression_s3tc: return "GL_EXT_texture_compression_s3tc";
    case GlExtension_EXT_texture_cube_map: return "GL_EXT_texture_cube_map";
    case GlExtension_EXT_texture_edge_clamp: return "GL_EXT_texture_edge_clamp";
    case GlExtension_EXT_texture_env: return "GL_EXT_texture_env";
    case GlExtension_EXT_texture_env_add: return "GL_EXT_texture_env_add";
    case GlExtension_EXT_texture_env_combine: return "GL_EXT_texture_env_combine";
    case GlExtension_EXT_texture_env_dot3: return "GL_EXT_texture_env_dot3";
    case GlExtension_EXT_texture_filter_anisotropic: return "GL_EXT_texture_filter_anisotropic";
    case GlExtension_EXT_texture_integer: return "GL_EXT_texture_integer";
    case GlExtension_EXT_texture_lod_bias: return "GL_EXT_texture_lod_bias";
    case GlExtension_EXT_texture_mirror_clamp: return "GL_EXT_texture_mirror_clamp";
    case GlExtension_EXT_texture_object: return "GL_EXT_texture_object";
    case GlExtension_EXT_texture_perturb_normal: return "GL_EXT_texture_perturb_normal";
    case GlExtension_EXT_texture_rectangle: return "GL_EXT_texture_rectangle";
    case GlExtension_EXT_texture_sRGB: return "GL_EXT_texture_sRGB";
    case GlExtension_EXT_texture_shared_exponent: return "GL_EXT_texture_shared_exponent";
    case GlExtension_EXT_timer_query: return "GL_EXT_timer_query";
    case GlExtension_EXT_vertex_array: return "GL_EXT_vertex_array";
    case GlExtension_EXT_vertex_shader: return "GL_EXT_vertex_shader";
    case GlExtension_EXT_vertex_weighting: return "GL_EXT_vertex_weighting";
    case GlExtension_GREMEDY_string_marker: return "GL_GREMEDY_string_marker";
    case GlExtension_HP_convolution_border_modes: return "GL_HP_convolution_border_modes";
    case GlExtension_HP_image_transform: return "GL_HP_image_transform";
    case GlExtension_HP_occlusion_test: return "GL_HP_occlusion_test";
    case GlExtension_HP_texture_lighting: return "GL_HP_texture_lighting";
    case GlExtension_IBM_cull_vertex: return "GL_IBM_cull_vertex";
    case GlExtension_IBM_multimode_draw_arrays: return "GL_IBM_multimode_draw_arrays";
    case GlExtension_IBM_rasterpos_clip: return "GL_IBM_rasterpos_clip";
    case GlExtension_IBM_static_data: return "GL_IBM_static_data";
    case GlExtension_IBM_texture_mirrored_repeat: return "GL_IBM_texture_mirrored_repeat";
    case GlExtension_IBM_vertex_array_lists: return "GL_IBM_vertex_array_lists";
    case GlExtension_INGR_color_clamp: return "GL_INGR_color_clamp";
    case GlExtension_INGR_interlace_read: return "GL_INGR_interlace_read";
    case GlExtension_INTEL_parallel_arrays: return "GL_INTEL_parallel_arrays";
    case GlExtension_INTEL_texture_scissor: return "GL_INTEL_texture_scissor";
    case GlExtension_KTX_buffer_region: return "GL_KTX_buffer_region";
    case GlExtension_MESAX_texture_stack: return "GL_MESAX_texture_stack";
    case GlExtension_MESA_pack_invert: return "GL_MESA_pack_invert";
    case GlExtension_MESA_resize_buffers: return "GL_MESA_resize_buffers";
    case GlExtension_MESA_window_pos: return "GL_MESA_window_pos";
    case GlExtension_MESA_ycbcr_texture: return "GL_MESA_ycbcr_texture";
    case GlExtension_NV_blend_square: return "GL_NV_blend_square";
    case GlExtension_NV_copy_depth_to_color: return "GL_NV_copy_depth_to_color";
    case GlExtension_NV_depth_buffer_float: return "GL_NV_depth_buffer_float";
    case GlExtension_NV_depth_clamp: return "GL_NV_depth_clamp";
    case GlExtension_NV_depth_range_unclamped: return "GL_NV_depth_range_unclamped";
    case GlExtension_NV_evaluators: return "GL_NV_evaluators";
    case GlExtension_NV_fence: return "GL_NV_fence";
    case GlExtension_NV_float_buffer: return "GL_NV_float_buffer";
    case GlExtension_NV_fog_distance: return "GL_NV_fog_distance";
    case GlExtension_NV_fragment_program: return "GL_NV_fragment_program";
    case GlExtension_NV_fragment_program2: return "GL_NV_fragment_program2";
    case GlExtension_NV_fragment_program4: return "GL_NV_fragment_program4";
    case GlExtension_NV_fragment_program_option: return "GL_NV_fragment_program_option";
    case GlExtension_NV_framebuffer_multisample_coverage: return "GL_NV_framebuffer_multisample_coverage";
    case GlExtension_NV_geometry_program4: return "GL_NV_geometry_program4";
    case GlExtension_NV_geometry_shader4: return "GL_NV_geometry_shader4";
    case GlExtension_NV_gpu_program4: return "GL_NV_gpu_program4";
    case GlExtension_NV_half_float: return "GL_NV_half_float";
    case GlExtension_NV_light_max_exponent: return "GL_NV_light_max_exponent";
    case GlExtension_NV_multisample_filter_hint: return "GL_NV_multisample_filter_hint";
    case GlExtension_NV_occlusion_query: return "GL_NV_occlusion_query";
    case GlExtension_NV_packed_depth_stencil: return "GL_NV_packed_depth_stencil";
    case GlExtension_NV_parameter_buffer_object: return "GL_NV_parameter_buffer_object";
    case GlExtension_NV_pixel_data_range: return "GL_NV_pixel_data_range";
    case GlExtension_NV_point_sprite: return "GL_NV_point_sprite";
    case GlExtension_NV_primitive_restart: return "GL_NV_primitive_restart";
    case GlExtension_NV_register_combiners: return "GL_NV_register_combiners";
    case GlExtension_NV_register_combiners2: return "GL_NV_register_combiners2";
    case GlExtension_NV_texgen_emboss: return "GL_NV_texgen_emboss";
    case GlExtension_NV_texgen_reflection: return "GL_NV_texgen_reflection";
    case GlExtension_NV_texture_compression_vtc: return "GL_NV_texture_compression_vtc";
    case GlExtension_NV_texture_env_combine4: return "GL_NV_texture_env_combine4";
    case GlExtension_NV_texture_expand_normal: return "GL_NV_texture_expand_normal";
    case GlExtension_NV_texture_rectangle: return "GL_NV_texture_rectangle";
    case GlExtension_NV_texture_shader: return "GL_NV_texture_shader";
    case GlExtension_NV_texture_shader2: return "GL_NV_texture_shader2";
    case GlExtension_NV_texture_shader3: return "GL_NV_texture_shader3";
    case GlExtension_NV_transform_feedback: return "GL_NV_transform_feedback";
    case GlExtension_NV_vertex_array_range: return "GL_NV_vertex_array_range";
    case GlExtension_NV_vertex_array_range2: return "GL_NV_vertex_array_range2";
    case GlExtension_NV_vertex_program: return "GL_NV_vertex_program";
    case GlExtension_NV_vertex_program1_1: return "GL_NV_vertex_program1_1";
    case GlExtension_NV_vertex_program2: return "GL_NV_vertex_program2";
    case GlExtension_NV_vertex_program2_option: return "GL_NV_vertex_program2_option";
    case GlExtension_NV_vertex_program3: return "GL_NV_vertex_program3";
    case GlExtension_NV_vertex_program4: return "GL_NV_vertex_program4";
    case GlExtension_OES_byte_coordinates: return "GL_OES_byte_coordinates";
    case GlExtension_OES_compressed_paletted_texture: return "GL_OES_compressed_paletted_texture";
    case GlExtension_OES_read_format: return "GL_OES_read_format";
    case GlExtension_OES_single_precision: return "GL_OES_single_precision";
    case GlExtension_OML_interlace: return "GL_OML_interlace";
    case GlExtension_OML_resample: return "GL_OML_resample";
    case GlExtension_OML_subsample: return "GL_OML_subsample";
    case GlExtension_PGI_misc_hints: return "GL_PGI_misc_hints";
    case GlExtension_PGI_vertex_hints: return "GL_PGI_vertex_hints";
    case GlExtension_REND_screen_coordinates: return "GL_REND_screen_coordinates";
    case GlExtension_S3_s3tc: return "GL_S3_s3tc";
    case GlExtension_SGIS_color_range: return "GL_SGIS_color_range";
    case GlExtension_SGIS_detail_texture: return "GL_SGIS_detail_texture";
    case GlExtension_SGIS_fog_function: return "GL_SGIS_fog_function";
    case GlExtension_SGIS_generate_mipmap: return "GL_SGIS_generate_mipmap";
    case GlExtension_SGIS_multisample: return "GL_SGIS_multisample";
    case GlExtension_SGIS_pixel_texture: return "GL_SGIS_pixel_texture";
    case GlExtension_SGIS_sharpen_texture: return "GL_SGIS_sharpen_texture";
    case GlExtension_SGIS_texture4D: return "GL_SGIS_texture4D";
    case GlExtension_SGIS_texture_border_clamp: return "GL_SGIS_texture_border_clamp";
    case GlExtension_SGIS_texture_edge_clamp: return "GL_SGIS_texture_edge_clamp";
    case GlExtension_SGIS_texture_filter4: return "GL_SGIS_texture_filter4";
    case GlExtension_SGIS_texture_lod: return "GL_SGIS_texture_lod";
    case GlExtension_SGIS_texture_select: return "GL_SGIS_texture_select";
    case GlExtension_SGIX_async: return "GL_SGIX_async";
    case GlExtension_SGIX_async_histogram: return "GL_SGIX_async_histogram";
    case GlExtension_SGIX_async_pixel: return "GL_SGIX_async_pixel";
    case GlExtension_SGIX_blend_alpha_minmax: return "GL_SGIX_blend_alpha_minmax";
    case GlExtension_SGIX_clipmap: return "GL_SGIX_clipmap";
    case GlExtension_SGIX_depth_texture: return "GL_SGIX_depth_texture";
    case GlExtension_SGIX_flush_raster: return "GL_SGIX_flush_raster";
    case GlExtension_SGIX_fog_offset: return "GL_SGIX_fog_offset";
    case GlExtension_SGIX_fog_texture: return "GL_SGIX_fog_texture";
    case GlExtension_SGIX_fragment_specular_lighting: return "GL_SGIX_fragment_specular_lighting";
    case GlExtension_SGIX_framezoom: return "GL_SGIX_framezoom";
    case GlExtension_SGIX_interlace: return "GL_SGIX_interlace";
    case GlExtension_SGIX_ir_instrument1: return "GL_SGIX_ir_instrument1";
    case GlExtension_SGIX_list_priority: return "GL_SGIX_list_priority";
    case GlExtension_SGIX_pixel_texture: return "GL_SGIX_pixel_texture";
    case GlExtension_SGIX_pixel_texture_bits: return "GL_SGIX_pixel_texture_bits";
    case GlExtension_SGIX_reference_plane: return "GL_SGIX_reference_plane";
    case GlExtension_SGIX_resample: return "GL_SGIX_resample";
    case GlExtension_SGIX_shadow: return "GL_SGIX_shadow";
    case GlExtension_SGIX_shadow_ambient: return "GL_SGIX_shadow_ambient";
    case GlExtension_SGIX_sprite: return "GL_SGIX_sprite";
    case GlExtension_SGIX_tag_sample_buffer: return "GL_SGIX_tag_sample_buffer";
    case GlExtension_SGIX_texture_add_env: return "GL_SGIX_texture_add_env";
    case GlExtension_SGIX_texture_coordinate_clamp: return "GL_SGIX_texture_coordinate_clamp";
    case GlExtension_SGIX_texture_lod_bias: return "GL_SGIX_texture_lod_bias";
    case GlExtension_SGIX_texture_multi_buffer: return "GL_SGIX_texture_multi_buffer";
    case GlExtension_SGIX_texture_range: return "GL_SGIX_texture_range";
    case GlExtension_SGIX_texture_scale_bias: return "GL_SGIX_texture_scale_bias";
    case GlExtension_SGIX_vertex_preclip: return "GL_SGIX_vertex_preclip";
    case GlExtension_SGIX_vertex_preclip_hint: return "GL_SGIX_vertex_preclip_hint";
    case GlExtension_SGIX_ycrcb: return "GL_SGIX_ycrcb";
    case GlExtension_SGI_color_matrix: return "GL_SGI_color_matrix";
    case GlExtension_SGI_color_table: return "GL_SGI_color_table";
    case GlExtension_SGI_texture_color_table: return "GL_SGI_texture_color_table";
    case GlExtension_SUNX_constant_data: return "GL_SUNX_constant_data";
    case GlExtension_SUN_convolution_border_modes: return "GL_SUN_convolution_border_modes";
    case GlExtension_SUN_global_alpha: return "GL_SUN_global_alpha";
    case GlExtension_SUN_mesh_array: return "GL_SUN_mesh_array";
    case GlExtension_SUN_read_video_pixels: return "GL_SUN_read_video_pixels";
    case GlExtension_SUN_slice_accum: return "GL_SUN_slice_accum";
    case GlExtension_SUN_triangle_list: return "GL_SUN_triangle_list";
    case GlExtension_SUN_vertex: return "GL_SUN_vertex";
    case GlExtension_WIN_phong_shading: return "GL_WIN_phong_shading";
    case GlExtension_WIN_specular_fog: return "GL_WIN_specular_fog";
    case GlExtension_WIN_swap_hint: return "GL_WIN_swap_hint";
    default:
     common::RaiseInvalidArgument ("render::low_level::opengl::get_extension_name", "id", id);
     return"";
  }
}

}

}

}
