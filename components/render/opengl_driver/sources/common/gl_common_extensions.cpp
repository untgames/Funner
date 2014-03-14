#include "shared.h"

using namespace common;

/*
    Массив поддерживаемых расширений
*/

namespace
{

const char* extensions [] = {
  "GL_VERSION_1_1",
  "GL_VERSION_1_2",
  "GL_VERSION_1_3",
  "GL_VERSION_1_4",
  "GL_VERSION_1_5",
  "GL_VERSION_2_0",
  "GL_VERSION_2_1",
  "GL_VERSION_3_0",
  "GL_VERSION_3_1",
  "GL_VERSION_3_2",
  "GL_VERSION_3_3",
  "GL_VERSION_4_0",
  "GL_VERSION_4_1",
  "GL_VERSION_4_2",
  "GL_VERSION_4_3",  
  
  "GL_3DFX_multisample",
  "GL_3DFX_tbuffer",
  "GL_3DFX_texture_compression_FXT1",
  "GL_AMD_blend_minmax_factor",
  "GL_AMD_conservative_depth",
  "GL_AMD_debug_output",
  "GL_AMD_depth_clamp_separate",
  "GL_AMD_draw_buffers_blend",
  "GL_AMD_multi_draw_indirect",
  "GL_AMD_name_gen_delete",
  "GL_AMD_performance_monitor",
  "GL_AMD_pinned_memory",
  "GL_AMD_query_buffer_object",
  "GL_AMD_sample_positions",
  "GL_AMD_seamless_cubemap_per_texture",
  "GL_AMD_shader_stencil_export",
  "GL_AMD_shader_trinary_minmax",
  "GL_AMD_sparse_texture",
  "GL_AMD_stencil_operation_extended",
  "GL_AMD_texture_texture4",
  "GL_AMD_transform_feedback3_lines_triangles",
  "GL_AMD_vertex_shader_layer",
  "GL_AMD_vertex_shader_tesselator",
  "GL_AMD_vertex_shader_tessellator",
  "GL_AMD_vertex_shader_viewport_index",
  "GL_APPLE_aux_depth_stencil",
  "GL_APPLE_client_storage",
  "GL_APPLE_element_array",
  "GL_APPLE_fence",
  "GL_APPLE_float_pixels",
  "GL_APPLE_flush_buffer_range",
  "GL_APPLE_object_purgeable",
  "GL_APPLE_rgb_422",
  "GL_APPLE_row_bytes",
  "GL_APPLE_specular_vector",
  "GL_APPLE_texture_range",
  "GL_APPLE_transform_hint",
  "GL_APPLE_vertex_array_object",
  "GL_APPLE_vertex_array_range",
  "GL_APPLE_vertex_program_evaluators",
  "GL_APPLE_ycbcr_422",
  "GL_ARB_arrays_of_arrays",
  "GL_ARB_base_instance",
  "GL_ARB_blend_func_extended",
  "GL_ARB_cl_event",
  "GL_ARB_clear_buffer_object",
  "GL_ARB_color_buffer_float ",
  "GL_ARB_color_buffer_float",
  "GL_ARB_compatibility",
  "GL_ARB_compressed_texture_pixel_storage",
  "GL_ARB_compute_shader",
  "GL_ARB_conservative_depth",
  "GL_ARB_copy_buffer",
  "GL_ARB_copy_image",
  "GL_ARB_debug_output",
  "GL_ARB_depth_buffer_float",
  "GL_ARB_depth_clamp",
  "GL_ARB_depth_texture",
  "GL_ARB_draw_buffers",
  "GL_ARB_draw_buffers_blend",
  "GL_ARB_draw_elements_base_vertex",
  "GL_ARB_draw_indirect",
  "GL_ARB_draw_instanced",
  "GL_ARB_ES2_compatibility",
  "GL_ARB_ES3_compatibility",
  "GL_ARB_explicit_attrib_location",
  "GL_ARB_explicit_uniform_location",
  "GL_ARB_fragment_coord_conventions",
  "GL_ARB_fragment_layer_viewport",
  "GL_ARB_fragment_program",
  "GL_ARB_fragment_program_shadow",
  "GL_ARB_fragment_shader",
  "GL_ARB_framebuffer_no_attachments",
  "GL_ARB_framebuffer_object",
  "GL_ARB_framebuffer_sRGB ",
  "GL_ARB_framebuffer_sRGB",
  "GL_ARB_geometry_shader4",
  "GL_ARB_get_program_binary",
  "GL_ARB_gpu_shader_fp64",
  "GL_ARB_gpu_shader5",
  "GL_ARB_half_float_pixel",
  "GL_ARB_half_float_vertex",
  "GL_ARB_instanced_arrays",
  "GL_ARB_internalformat_query",
  "GL_ARB_internalformat_query2",
  "GL_ARB_invalidate_subdata",
  "GL_ARB_map_buffer_alignment",
  "GL_ARB_map_buffer_range",
  "GL_ARB_matrix_palette",
  "GL_ARB_multi_draw_indirect",
  "GL_ARB_multisample ",
  "GL_ARB_multisample",
  "GL_ARB_multitexture",
  "GL_ARB_occlusion_query",
  "GL_ARB_occlusion_query2",
  "GL_ARB_pixel_buffer_object",
  "GL_ARB_point_parameters",
  "GL_ARB_point_sprite",
  "GL_ARB_program_interface_query",
  "GL_ARB_provoking_vertex",
  "GL_ARB_robust_buffer_access_behavior",
  "GL_ARB_robustness",
  "GL_ARB_robustness_isolation",
  "GL_ARB_sample_shading",
  "GL_ARB_sampler_objects",
  "GL_ARB_seamless_cube_map",
  "GL_ARB_separate_shader_objects",
  "GL_ARB_shader_atomic_counters",
  "GL_ARB_shader_bit_encoding",
  "GL_ARB_shader_image_load_store",
  "GL_ARB_shader_image_size",
  "GL_ARB_shader_objects",
  "GL_ARB_shader_precision",
  "GL_ARB_shader_stencil_export",
  "GL_ARB_shader_storage_buffer_object",
  "GL_ARB_shader_subroutine",
  "GL_ARB_shader_texture_lod",
  "GL_ARB_shading_language_100",
  "GL_ARB_shading_language_420pack",
  "GL_ARB_shading_language_include",
  "GL_ARB_shading_language_packing",
  "GL_ARB_shadow",
  "GL_ARB_shadow_ambient",
  "GL_ARB_stencil_texturing",
  "GL_ARB_sync",
  "GL_ARB_tessellation_shader",
  "GL_ARB_texture_border_clamp",
  "GL_ARB_texture_buffer_object",
  "GL_ARB_texture_buffer_object_rgb32",
  "GL_ARB_texture_buffer_range",
  "GL_ARB_texture_compression",
  "GL_ARB_texture_compression_bptc",
  "GL_ARB_texture_compression_rgtc",
  "GL_ARB_texture_cube_map",
  "GL_ARB_texture_cube_map_array",
  "GL_ARB_texture_env_add",
  "GL_ARB_texture_env_combine",
  "GL_ARB_texture_env_crossbar",
  "GL_ARB_texture_env_dot3",
  "GL_ARB_texture_float",
  "GL_ARB_texture_gather",
  "GL_ARB_texture_mirrored_repeat",
  "GL_ARB_texture_multisample",
  "GL_ARB_texture_non_power_of_two",
  "GL_ARB_texture_query_levels",
  "GL_ARB_texture_query_lod",
  "GL_ARB_texture_rectangle",
  "GL_ARB_texture_rg",
  "GL_ARB_texture_rgb10_a2ui",
  "GL_ARB_texture_storage",
  "GL_ARB_texture_storage_multisample",
  "GL_ARB_texture_swizzle",
  "GL_ARB_texture_view",
  "GL_ARB_timer_query",
  "GL_ARB_transform_feedback_instanced",
  "GL_ARB_transform_feedback2",
  "GL_ARB_transform_feedback3",
  "GL_ARB_transpose_matrix",
  "GL_ARB_uniform_buffer_object",
  "GL_ARB_vertex_array_bgra",
  "GL_ARB_vertex_array_object",
  "GL_ARB_vertex_attrib_64bit",
  "GL_ARB_vertex_attrib_binding",
  "GL_ARB_vertex_blend",
  "GL_ARB_vertex_buffer_object",
  "GL_ARB_vertex_program",
  "GL_ARB_vertex_shader",
  "GL_ARB_vertex_type_2_10_10_10_rev",
  "GL_ARB_viewport_array",
  "GL_ARB_window_pos",
  "GL_ATI_draw_buffers",
  "GL_ATI_element_array",
  "GL_ATI_envmap_bumpmap",
  "GL_ATI_fragment_shader",
  "GL_ATI_map_object_buffer",
  "GL_ATI_meminfo",
  "GL_ATI_pixel_format_float",
  "GL_ATI_pn_triangles",
  "GL_ATI_separate_stencil",
  "GL_ATI_text_fragment_shader",
  "GL_ATI_texture_env_combine3",
  "GL_ATI_texture_float",
  "GL_ATI_texture_mirror_once",
  "GL_ATI_vertex_array_object",
  "GL_ATI_vertex_attrib_array_object",
  "GL_ATI_vertex_streams",
  "GL_EXT_422_pixels",
  "GL_EXT_abgr",
  "GL_EXT_bgra",
  "GL_EXT_bindable_uniform",
  "GL_EXT_blend_color",
  "GL_EXT_blend_equation_separate",
  "GL_EXT_blend_func_separate",
  "GL_EXT_blend_logic_op",
  "GL_EXT_blend_minmax",
  "GL_EXT_blend_subtract",
  "GL_EXT_clip_volume_hint",
  "GL_EXT_cmyka",
  "GL_EXT_color_subtable",
  "GL_EXT_compiled_vertex_array",
  "GL_EXT_convolution",
  "GL_EXT_coordinate_frame",
  "GL_EXT_copy_texture",
  "GL_EXT_cull_vertex",
  "GL_EXT_depth_bounds_test",
  "GL_EXT_direct_state_access",
  "GL_EXT_draw_buffers2",
  "GL_EXT_draw_instanced",
  "GL_EXT_draw_range_elements",
  "GL_EXT_fog_coord",
  "GL_EXT_fragment_lighting",
  "GL_EXT_framebuffer_blit",
  "GL_EXT_framebuffer_multisample",
  "GL_EXT_framebuffer_multisample_blit_scaled",
  "GL_EXT_framebuffer_object",
  "GL_EXT_framebuffer_sRGB ",
  "GL_EXT_framebuffer_sRGB",
  "GL_EXT_geometry_shader4",
  "GL_EXT_gpu_program_parameters",
  "GL_EXT_gpu_shader4",
  "GL_EXT_histogram",
  "GL_EXT_index_array_formats",
  "GL_EXT_index_func",
  "GL_EXT_index_material",
  "GL_EXT_index_texture",
  "GL_EXT_light_texture",
  "GL_EXT_misc_attribute",
  "GL_EXT_multi_draw_arrays ",
  "GL_EXT_multi_draw_arrays",
  "GL_EXT_multisample",
  "GL_EXT_packed_depth_stencil",
  "GL_EXT_packed_float ",
  "GL_EXT_packed_float",
  "GL_EXT_packed_pixels",
  "GL_EXT_paletted_texture",
  "GL_EXT_pixel_buffer_object",
  "GL_EXT_pixel_transform",
  "GL_EXT_pixel_transform_color_table",
  "GL_EXT_point_parameters",
  "GL_EXT_polygon_offset",
  "GL_EXT_provoking_vertex",
  "GL_EXT_rescale_normal",
  "GL_EXT_scene_marker ",
  "GL_EXT_secondary_color",
  "GL_EXT_separate_shader_objects",
  "GL_EXT_separate_specular_color",
  "GL_EXT_shader_image_load_store",
  "GL_EXT_shadow_funcs",
  "GL_EXT_shared_texture_palette",
  "GL_EXT_stencil_clear_tag",
  "GL_EXT_stencil_two_side",
  "GL_EXT_stencil_wrap",
  "GL_EXT_subtexture",
  "GL_EXT_swap_control",
  "GL_EXT_texture",
  "GL_EXT_texture_array",
  "GL_EXT_texture_buffer_object",
  "GL_EXT_texture_compression_dxt1",
  "GL_EXT_texture_compression_latc",
  "GL_EXT_texture_compression_rgtc",
  "GL_EXT_texture_compression_s3tc",
  "GL_EXT_texture_cube_map",
  "GL_EXT_texture_env",
  "GL_EXT_texture_env_add",
  "GL_EXT_texture_env_combine",
  "GL_EXT_texture_env_dot3",
  "GL_EXT_texture_filter_anisotropic",
  "GL_EXT_texture_integer",
  "GL_EXT_texture_lod_bias",
  "GL_EXT_texture_mirror_clamp",
  "GL_EXT_texture_object",
  "GL_EXT_texture_perturb_normal",
  "GL_EXT_texture_shared_exponent",
  "GL_EXT_texture_snorm",
  "GL_EXT_texture_sRGB",
  "GL_EXT_texture_sRGB_decode",
  "GL_EXT_texture_swizzle",
  "GL_EXT_texture3D",
  "GL_EXT_timer_query",
  "GL_EXT_transform_feedback",
  "GL_EXT_vertex_array",
  "GL_EXT_vertex_array_bgra",
  "GL_EXT_vertex_attrib_64bit",
  "GL_EXT_vertex_shader",
  "GL_EXT_vertex_weighting",
  "GL_EXT_x11_sync_object",
  "GL_FfdMaskSGIX",
  "GL_GREMEDY_frame_terminator",
  "GL_GREMEDY_string_marker",
  "GL_HP_convolution_border_modes",
  "GL_HP_image_transform",
  "GL_HP_occlusion_test",
  "GL_HP_texture_lighting",
  "GL_IBM_cull_vertex",
  "GL_IBM_multimode_draw_arrays",
  "GL_IBM_rasterpos_clip",
  "GL_IBM_static_data",
  "GL_IBM_texture_mirrored_repeat",
  "GL_IBM_vertex_array_lists",
  "GL_INGR_blend_func_separate",
  "GL_INGR_color_clamp",
  "GL_INGR_interlace_read",
  "GL_INGR_palette_buffer",
  "GL_INTEL_map_texture",
  "GL_INTEL_parallel_arrays",
  "GL_INTEL_texture_scissor",
  "GL_KHR_debug",
  "GL_KHR_texture_compression_astc_ldr",
  "GL_MESA_pack_invert",
  "GL_MESA_resize_buffers",
  "GL_MESA_window_pos",
  "GL_MESA_ycbcr_texture",
  "GL_MESAX_texture_stack",
  "GL_NV_bindless_texture",
  "GL_NV_blend_square",
  "GL_NV_compute_program5",
  "GL_NV_conditional_render",
  "GL_NV_copy_depth_to_color",
  "GL_NV_copy_image ",
  "GL_NV_copy_image",
  "GL_NV_deep_texture3D",
  "GL_NV_depth_buffer_float",
  "GL_NV_depth_clamp",
  "GL_NV_draw_texture",
  "GL_NV_evaluators",
  "GL_NV_explicit_multisample",
  "GL_NV_fence",
  "GL_NV_float_buffer ",
  "GL_NV_float_buffer",
  "GL_NV_fog_distance",
  "GL_NV_fragment_program",
  "GL_NV_fragment_program_option",
  "GL_NV_fragment_program2",
  "GL_NV_fragment_program4",
  "GL_NV_framebuffer_multisample_coverage",
  "GL_NV_geometry_program4",
  "GL_NV_geometry_shader4",
  "GL_NV_gpu_program4",
  "GL_NV_gpu_program5",
  "GL_NV_gpu_shader5",
  "GL_NV_half_float",
  "GL_NV_light_max_exponent",
  "GL_NV_multisample_coverage",
  "GL_NV_multisample_filter_hint",
  "GL_NV_nvx_conditional_render",
  "GL_NV_occlusion_query",
  "GL_NV_packed_depth_stencil",
  "GL_NV_parameter_buffer_object",
  "GL_NV_parameter_buffer_object2",
  "GL_NV_path_rendering",
  "GL_NV_pixel_data_range",
  "GL_NV_point_sprite",
  "GL_NV_present_video ",
  "GL_NV_present_video",
  "GL_NV_primitive_restart",
  "GL_NV_register_combiners",
  "GL_NV_register_combiners2",
  "GL_NV_shader_atomic_counters",
  "GL_NV_shader_atomic_float",
  "GL_NV_shader_buffer_load",
  "GL_NV_shader_buffer_store",
  "GL_NV_shader_storage_buffer_object",
  "GL_NV_tessellation_program5",
  "GL_NV_texgen_emboss",
  "GL_NV_texgen_reflection",
  "GL_NV_texture_barrier",
  "GL_NV_texture_compression_vtc",
  "GL_NV_texture_env_combine4",
  "GL_NV_texture_expand_normal",
  "GL_NV_texture_multisample",
  "GL_NV_texture_rectangle",
  "GL_NV_texture_shader",
  "GL_NV_texture_shader2",
  "GL_NV_texture_shader3",
  "GL_NV_transform_feedback",
  "GL_NV_transform_feedback2",
  "GL_NV_vdpau_interop",
  "GL_NV_vertex_array_range",
  "GL_NV_vertex_array_range2",
  "GL_NV_vertex_attrib_integer_64bit",
  "GL_NV_vertex_buffer_unified_memory",
  "GL_NV_vertex_program",
  "GL_NV_vertex_program1_1",
  "GL_NV_vertex_program2",
  "GL_NV_vertex_program2_option",
  "GL_NV_vertex_program3",
  "GL_NV_vertex_program4",
  "GL_NV_video_capture ",
  "GL_NV_video_capture",
  "GL_OES_byte_coordinates",
  "GL_OES_compressed_paletted_texture",
  "GL_OES_fixed_point",
  "GL_OES_query_matrix",
  "GL_OES_read_format",
  "GL_OES_single_precision",
  "GL_OML_interlace",
  "GL_OML_resample",
  "GL_OML_subsample",
  "GL_PGI_misc_hints",
  "GL_PGI_vertex_hints",
  "GL_REND_screen_coordinates",
  "GL_S3_s3tc",
  "GL_SGI_color_matrix",
  "GL_SGI_color_table",
  "GL_SGI_depth_pass_instrument",
  "GL_SGI_texture_color_table",
  "GL_SGIS_color_range",
  "GL_SGIS_detail_texture",
  "GL_SGIS_fog_function",
  "GL_SGIS_generate_mipmap",
  "GL_SGIS_multisample ",
  "GL_SGIS_multisample",
  "GL_SGIS_pixel_texture",
  "GL_SGIS_point_line_texgen",
  "GL_SGIS_point_parameters",
  "GL_SGIS_sharpen_texture",
  "GL_SGIS_texture_border_clamp",
  "GL_SGIS_texture_color_mask",
  "GL_SGIS_texture_edge_clamp",
  "GL_SGIS_texture_filter4",
  "GL_SGIS_texture_lod",
  "GL_SGIS_texture_select",
  "GL_SGIS_texture4D",
  "GL_SGIX_async",
  "GL_SGIX_async_histogram",
  "GL_SGIX_async_pixel",
  "GL_SGIX_blend_alpha_minmax",
  "GL_SGIX_calligraphic_fragment",
  "GL_SGIX_clipmap",
  "GL_SGIX_convolution_accuracy",
  "GL_SGIX_depth_pass_instrument",
  "GL_SGIX_depth_texture",
  "GL_SGIX_flush_raster",
  "GL_SGIX_fog_offset",
  "GL_SGIX_fog_scale",
  "GL_SGIX_fragment_lighting",
  "GL_SGIX_framezoom",
  "GL_SGIX_igloo_interface",
  "GL_SGIX_impact_pixel_texture",
  "GL_SGIX_instruments",
  "GL_SGIX_interlace",
  "GL_SGIX_ir_instrument1",
  "GL_SGIX_list_priority",
  "GL_SGIX_pixel_texture",
  "GL_SGIX_pixel_texture_bits",
  "GL_SGIX_pixel_tiles",
  "GL_SGIX_polynomial_ffd",
  "GL_SGIX_reference_plane",
  "GL_SGIX_resample",
  "GL_SGIX_scalebias_hint",
  "GL_SGIX_shadow",
  "GL_SGIX_shadow_ambient",
  "GL_SGIX_sprite",
  "GL_SGIX_subsample",
  "GL_SGIX_tag_sample_buffer",
  "GL_SGIX_texture_add_env",
  "GL_SGIX_texture_coordinate_clamp",
  "GL_SGIX_texture_lod_bias",
  "GL_SGIX_texture_multi_buffer",
  "GL_SGIX_texture_range",
  "GL_SGIX_texture_scale_bias",
  "GL_SGIX_texture_select",
  "GL_SGIX_vertex_preclip ",
  "GL_SGIX_vertex_preclip",
  "GL_SGIX_vertex_preclip_hint",
  "GL_SGIX_ycrcb",
  "GL_SGIX_ycrcb_subsample",
  "GL_SGIX_ycrcba",
  "GL_SUN_convolution_border_modes",
  "GL_SUN_global_alpha",
  "GL_SUN_mesh_array",
  "GL_SUN_multi_draw_arrays",
  "GL_SUN_slice_accum",
  "GL_SUN_triangle_list",
  "GL_SUN_vertex",
  "GL_SUNX_constant_data",
  "GL_WIN_phong_shading",
  "GL_WIN_specular_fog",
  "GLU_EXT_nurbs_tessellator",
  "GLU_EXT_object_space_tess",
  "GLU_SGI_filter4_parameters",
  "GLX_AMD_gpu_association",
  "GLX_ARB_create_context",
  "GLX_ARB_create_context_profile",
  "GLX_ARB_create_context_robustness",
  "GLX_ARB_fbconfig_float",
  "GLX_ARB_framebuffer_sRGB ",
  "GLX_ARB_get_proc_address",
  "GLX_ARB_multisample ",
  "GLX_ARB_robustness_application_isolation ",
  "GLX_ARB_robustness_share_group_isolation",
  "GLX_EXT_buffer_age",
  "GLX_EXT_create_context_es_profile",
  "GLX_EXT_create_context_es2_profile ",
  "GLX_EXT_fbconfig_packed_float",
  "GLX_EXT_framebuffer_sRGB ",
  "GLX_EXT_import_context",
  "GLX_EXT_scene_marker",
  "GLX_EXT_swap_control_tear",
  "GLX_EXT_texture_from_pixmap",
  "GLX_EXT_visual_info",
  "GLX_EXT_visual_rating",
  "GLX_INTEL_swap_event",
  "GLX_MESA_agp_offset",
  "GLX_MESA_copy_sub_buffer",
  "GLX_MESA_pixmap_colormap",
  "GLX_MESA_release_buffers",
  "GLX_MESA_set_3dfx_mode",
  "GLX_NV_copy_image",
  "GLX_NV_present_video ",
  "GLX_NV_swap_group",
  "GLX_NV_video_capture ",
  "GLX_NV_video_output",
  "GLX_OML_swap_method",
  "GLX_OML_sync_control",
  "GLX_SGI_cushion",
  "GLX_SGI_make_current_read",
  "GLX_SGI_swap_control",
  "GLX_SGI_video_sync",
  "GLX_SGIS_blended_overlay",
  "GLX_SGIS_color_range ",
  "GLX_SGIS_multisample",
  "GLX_SGIX_dm_buffer",
  "GLX_SGIX_fbconfig",
  "GLX_SGIX_hyperpipe",
  "GLX_SGIX_pbuffer",
  "GLX_SGIX_swap_barrier",
  "GLX_SGIX_swap_group",
  "GLX_SGIX_video_resize",
  "GLX_SGIX_video_source",
  "GLX_SGIX_visual_select_group",
  "GLX_SUN_get_transparent_index",
  "WGL_3DL_stereo_control",
  "WGL_AMD_gpu_association",
  "WGL_ARB_buffer_region",
  "WGL_ARB_create_context",
  "WGL_ARB_create_context_profile",
  "WGL_ARB_create_context_robustness",
  "WGL_ARB_extensions_string",
  "WGL_ARB_framebuffer_sRGB",
  "WGL_ARB_make_current_read",
  "WGL_ARB_multisample",
  "WGL_ARB_pbuffer",
  "WGL_ARB_pixel_format",
  "WGL_ARB_pixel_format_float ",
  "WGL_ARB_render_texture",
  "WGL_ARB_robustness_application_isolation ",
  "WGL_ARB_robustness_share_group_isolation",
  "WGL_ATI_pixel_format_float",
  "WGL_EXT_create_context_es_profile",
  "WGL_EXT_create_context_es2_profile ",
  "WGL_EXT_depth_float",
  "WGL_EXT_display_color_table",
  "WGL_EXT_extensions_string",
  "WGL_EXT_framebuffer_sRGB",
  "WGL_EXT_make_current_read",
  "WGL_EXT_multisample ",
  "WGL_EXT_pbuffer",
  "WGL_EXT_pixel_format",
  "WGL_EXT_pixel_format_packed_float ",
  "WGL_EXT_swap_control",
  "WGL_EXT_swap_control_tear",
  "WGL_I3D_digital_video_control",
  "WGL_I3D_gamma",
  "WGL_I3D_genlock",
  "WGL_I3D_image_buffer",
  "WGL_I3D_swap_frame_lock",
  "WGL_I3D_swap_frame_usage",
  "WGL_NV_copy_image ",
  "WGL_NV_DX_interop",
  "WGL_NV_DX_interop2",
  "WGL_NV_float_buffer",
  "WGL_NV_gpu_affinity",
  "WGL_NV_present_video",
  "WGL_NV_render_depth_texture",
  "WGL_NV_render_texture_rectangle",
  "WGL_NV_swap_group",
  "WGL_NV_video_capture",
  "WGL_NV_video_output",
  "WGL_OML_sync_control",

  "GLBUG_texture3D_get_tex_image",
  "GLBUG_texture_no_subimage",
  "GLBUG_texture_no_mipmap",
  "GLBUG_swap_buffers_twice_call",
  
#ifdef OPENGL_ES_SUPPORT
  "GL_OES_blend_equation_separate",
  "GL_OES_blend_func_separate",
  "GL_OES_blend_subtract",
  "GL_OES_byte_coordinates",
  "GL_OES_compressed_ETC1_RGB8_texture",
  "GL_OES_compressed_paletted_texture",
  "GL_OES_draw_texture",
  "GL_OES_extended_matrix_palette",
  "GL_OES_fixed_point",
  "GL_OES_framebuffer_object",
  "GL_OES_matrix_get",
  "GL_OES_matrix_palette",
  "GL_OES_point_size_array",
  "GL_OES_point_sprite",
  "GL_OES_query_matrix",
  "GL_OES_read_format",
  "GL_OES_single_precision",
  "GL_OES_stencil_wrap",
  "GL_OES_texture_cube_map",
  "GL_OES_texture_env_crossbar",
  "GL_OES_texture_mirrored_repeat",
  "GL_OES_EGL_image",
  "GL_OES_depth24",
  "GL_OES_depth32",
  "GL_OES_element_index_uint",
  "GL_OES_fbo_render_mipmap",
  "GL_OES_fragment_precision_high",
  "GL_OES_mapbuffer",
  "GL_OES_rgb8_rgba8",
  "GL_OES_stencil1",
  "GL_OES_stencil4",
  "GL_OES_stencil8",
  "GL_OES_texture_3D",
  "GL_OES_texture_float_linear",
  "GL_OES_texture_half_float_linear",
  "GL_OES_texture_float",
  "GL_OES_texture_half_float",
  "GL_OES_texture_npot",
  "GL_OES_vertex_half_float",
  "GL_AMD_compressed_3DC_texture",
  "GL_AMD_compressed_ATC_texture",
  "GL_EXT_texture_filter_anisotropic",
  "GL_EXT_texture_type_2_10_10_10_REV",
  "GL_OES_depth_texture",
  "GL_OES_packed_depth_stencil",
  "GL_OES_standard_derivatives",
  "GL_OES_vertex_type_10_10_10_2",
  "GL_OES_get_program_binary",
  "GL_AMD_program_binary_Z400",
  "GL_AMD_performance_monitor",
  "GL_EXT_texture_format_BGRA8888",
  "GL_NV_fence",
  "GL_IMG_read_format",
  "GL_IMG_texture_compression_pvrtc",
  "GL_QCOM_driver_control",
  "GL_QCOM_performance_monitor_global_mode",
  "GL_IMG_user_clip_plane",
  "GL_IMG_texture_env_enhanced_fixed_function",
  "GL_APPLE_texture_2D_limited_npot",
  "GL_EXT_texture_lod_bias",
  "GL_AMD_compressed_ATC_texture",
  "GL_ATI_texture_compression_atitc",
#endif

#ifdef OPENGL_ES2_SUPPORT
  "GL_KHR_debug",
  "GL_KHR_texture_compression_astc_hdr",
  "GL_KHR_texture_compression_astc_ldr",
  "GL_OES_EGL_image",
  "GL_OES_EGL_image_external",
  "GL_OES_compressed_ETC1_RGB8_texture",
  "GL_OES_compressed_paletted_texture",
  "GL_OES_depth24",
  "GL_OES_depth32",
  "GL_OES_depth_texture",
  "GL_OES_element_index_uint",
  "GL_OES_fbo_render_mipmap",
  "GL_OES_fragment_precision_high",
  "GL_OES_get_program_binary",
  "GL_OES_mapbuffer",
  "GL_OES_packed_depth_stencil",
  "GL_OES_required_internalformat",
  "GL_OES_rgb8_rgba8",
  "GL_OES_standard_derivatives",
  "GL_OES_stencil1",
  "GL_OES_stencil4",
  "GL_OES_surfaceless_context",
  "GL_OES_texture_3D",
  "GL_OES_texture_compression_astc",
  "GL_OES_texture_float",
  "GL_OES_texture_float_linear",
  "GL_OES_texture_half_float",
  "GL_OES_texture_half_float_linear",
  "GL_OES_texture_npot",
  "GL_OES_vertex_array_object",
  "GL_OES_vertex_half_float",
  "GL_OES_vertex_type_10_10_10_2",
  "GL_AMD_compressed_3DC_texture",
  "GL_AMD_compressed_ATC_texture",

  "GL_AMD_program_binary_Z400",
  "GL_ANGLE_depth_texture",
  "GL_ANGLE_framebuffer_blit",
  "GL_ANGLE_framebuffer_multisample",
  "GL_ANGLE_instanced_arrays",
  "GL_ANGLE_pack_reverse_row_order",
  "GL_ANGLE_program_binary",
  "GL_ANGLE_texture_compression_dxt3",
  "GL_ANGLE_texture_compression_dxt5",
  "GL_ANGLE_texture_usage",
  "GL_ANGLE_translated_shader_source",
  "GL_APPLE_copy_texture_levels",
  "GL_APPLE_framebuffer_multisample",

  "GL_APPLE_sync",
  "GL_APPLE_texture_format_BGRA8888",
  "GL_APPLE_texture_max_level",
  "GL_ARM_mali_program_binary",
  "GL_ARM_mali_shader_binary",
  "GL_ARM_rgba8",
  "GL_DMP_shader_binary",

  "GL_EXT_color_buffer_half_float",
  "GL_EXT_debug_label",
  "GL_EXT_debug_marker",
  "GL_EXT_discard_framebuffer",
  "GL_EXT_disjoint_timer_query",
  "GL_EXT_draw_buffers",

  "GL_EXT_instanced_arrays",
  "GL_EXT_map_buffer_range",

  "GL_EXT_multisampled_render_to_texture",
  "GL_EXT_multiview_draw_buffers",
  "GL_EXT_occlusion_query_boolean",
  "GL_EXT_pvrtc_sRGB",
  "GL_EXT_read_format_bgra",
  "GL_EXT_robustness",
  "GL_EXT_sRGB",
  "GL_EXT_sRGB_write_control",

  "GL_EXT_shader_framebuffer_fetch",
  "GL_EXT_shader_integer_mix",
  "GL_EXT_shader_texture_lod",
  "GL_EXT_shadow_samplers",



  "GL_EXT_texture_format_BGRA8888",
  "GL_EXT_texture_rg",

  "GL_EXT_texture_storage",
  "GL_EXT_texture_type_2_10_10_10_REV",
  "GL_EXT_unpack_subimage",
  "GL_FJ_shader_binary_GCCSO",
  "GL_IMG_multisampled_render_to_texture",
  "GL_IMG_program_binary",
  "GL_IMG_read_format",
  "GL_IMG_shader_binary",
  "GL_IMG_texture_compression_pvrtc",
  "GL_IMG_texture_compression_pvrtc2",
  "GL_INTEL_performance_query",
  "GL_NV_blend_equation_advanced",
  "GL_NV_blend_equation_advanced_coherent",
  "GL_NV_copy_buffer",
  "GL_NV_coverage_sample",
  "GL_NV_depth_nonlinear",
  "GL_NV_draw_buffers",
  "GL_NV_draw_instanced",
  "GL_NV_explicit_attrib_location",
  "GL_NV_fbo_color_attachments",

  "GL_NV_framebuffer_blit",
  "GL_NV_framebuffer_multisample",
  "GL_NV_generate_mipmap_sRGB",
  "GL_NV_instanced_arrays",
  "GL_NV_non_square_matrices",
  "GL_NV_read_buffer",
  "GL_NV_read_buffer_front",
  "GL_NV_read_depth",
  "GL_NV_read_depth_stencil",
  "GL_NV_read_stencil",
  "GL_NV_sRGB_formats",
  "GL_NV_shadow_samplers_array",
  "GL_NV_shadow_samplers_cube",
  "GL_NV_texture_border_clamp",
  "GL_NV_texture_compression_s3tc_update",
  "GL_NV_texture_npot_2D_mipmap",
  "GL_QCOM_alpha_test",
  "GL_QCOM_binning_control",
  "GL_QCOM_driver_control",
  "GL_QCOM_extended_get",
  "GL_QCOM_extended_get2",
  "GL_QCOM_perfmon_global_mode",
  "GL_QCOM_tiled_rendering",
  "GL_QCOM_writeonly_rendering",
  "GL_VIV_shader_binary",
#endif
};

const size_t EXTENSIONS_COUNT = sizeof (extensions) / sizeof (*extensions);      

/*
    Хранилище ассоциативного массива соответствий между строковым именем расширения и его идентификатором
*/

class ExtensionMap
{
  public:
    ExtensionMap ()
    {
      map.resize (EXTENSIONS_COUNT);
      
      for (size_t i=0; i<EXTENSIONS_COUNT; i++)
        map [strihash (extensions [i])] = i;
    }
    
    size_t FindExtensionId (const char* name, size_t length) const
    {
      Map::const_iterator iter = map.find (strnihash (name, length));

      if (iter == map.end ())
        return EXTENSIONS_COUNT;

      return iter->second;
    }

  private:
    typedef stl::hash_map<size_t, size_t> Map;
    
  private:
    Map map;
};

/*
    Список псевдо-расширений (багов)
*/

class BugExtensionSet: public render::low_level::opengl::ExtensionSet
{
  public:
    BugExtensionSet ()
    {
      for (size_t i=0; i<EXTENSIONS_COUNT; i++)
        if (strstr (extensions [i], "GLBUG_") == extensions [i])
          Set (i, true);
    }
};

}

namespace render
{

namespace low_level
{

namespace opengl
{

/*
    Получение идентификатора расширения по строковому имени
*/

size_t get_extension_id (const char* full_name, size_t length)
{
  static const char* METHOD_NAME = "render::low_level::opengl::get_extension_id";

  if (!full_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "full_name");  

  typedef Singleton<ExtensionMap> ExtensionMapSingleton;

  return ExtensionMapSingleton::Instance ()->FindExtensionId (full_name, length);
}

size_t get_extension_id (const char* full_name)
{
  return get_extension_id (full_name, full_name ? strlen (full_name) : 0);
}

/*
    Получение имени расширения по идентификатору
*/

const char* get_extension_name (size_t id)
{
  if (id >= EXTENSIONS_COUNT)
    throw xtl::make_range_exception ("render::low_level::opengl::get_extension_name", "id", id, EXTENSIONS_COUNT);

  return extensions [id];
}

/*
    class Extension
*/

//конструкторы
Extension::Extension (size_t in_id)
  : id (in_id)
{
  if (id >= EXTENSIONS_COUNT)
    throw xtl::make_range_exception ("render::low_level::opengl::Extension::Extension", "id", id, EXTENSIONS_COUNT);
}

Extension::Extension (const char* full_name)
  : id (get_extension_id (full_name))
{
  if (id >= EXTENSIONS_COUNT)
    throw xtl::format_not_supported_exception ("render::low_level::opengl::Extension::Extension", "Extension '%s' not supported", full_name);
}

//имя расширения
const char* Extension::Name () const
{
  return extensions [id];
}

/*
    class ExtensionSet
*/

struct ExtensionSet::Impl
{
  typedef stl::bitset<EXTENSIONS_COUNT> Bitset;
  
  Bitset flags;
};

//конструкторы
ExtensionSet::ExtensionSet ()
  : impl (new Impl)
{
}

ExtensionSet::ExtensionSet (const ExtensionSet& set)
  : impl (new Impl (*set.impl))
{
}

//деструктор
ExtensionSet::~ExtensionSet ()
{
  delete impl;
}

//присваивание
ExtensionSet& ExtensionSet::operator = (const ExtensionSet& set)
{
  ExtensionSet (set).Swap (*this);
  
  return *this;
}

//количество расширений в множестве
size_t ExtensionSet::Size ()
{
  return EXTENSIONS_COUNT;
}

//установка флагов
void ExtensionSet::Set (const Extension& extension, bool state)
{
  impl->flags.set (extension.Id (), state);
}

bool ExtensionSet::Get (const Extension& extension) const
{
  return impl->flags.test (extension.Id ());
}

void ExtensionSet::Set (bool state)
{
  if (state) impl->flags.set ();
  else       impl->flags.reset ();
}

//установка флагов по строке содержащей имена расширений и wildcard маски
void ExtensionSet::SetGroup (const char* extension_names, bool state)
{
  stl::string mask;

  for (const char* pos=extension_names; *pos;)
  {
    for (;*pos && *pos==' '; ++pos);

    const char* first    = pos;
    bool        wildcard = false;

    for (bool loop=true; loop;)
      switch (*pos)
      {
        case '\0':
        case ' ':
          loop = false;
          break;
        case '?':
        case '*':
          wildcard = true;
          ++pos;
        default:
          ++pos;
          break;
      }

    if (!wildcard)
    {
      size_t extension_id = get_extension_id (first, size_t (pos-first));

      if (extension_id >= EXTENSIONS_COUNT)
        continue;

      impl->flags [extension_id] = state;
    }
    else
    {
      mask.assign (first, size_t (pos - first));

      if (!strcmp (mask.c_str (), "*")) //обработка специального случая очистки / установки всех флагов
      {
        Set (state);
      }
      else
      {
        for (size_t id=0; id<EXTENSIONS_COUNT; id++)
          if (wcimatch (extensions [id], mask.c_str ()))
            impl->flags.set (id, state);
      }
    }    
  }
}

//логические операции
ExtensionSet& ExtensionSet::operator |= (const ExtensionSet& set)
{
  impl->flags |= set.impl->flags;

  return *this;
}

ExtensionSet& ExtensionSet::operator &= (const ExtensionSet& set)
{
  impl->flags &= set.impl->flags;
  
  return *this;
}

//получение списка псевдо-расширений (багов)
const ExtensionSet& ExtensionSet::BugExtensions ()
{
  return *Singleton<BugExtensionSet>::Instance ();
}

//обмен
void ExtensionSet::Swap (ExtensionSet& set)
{
  Impl* tmp = set.impl;
  set.impl  = impl;
  impl      = tmp;
}

void swap (ExtensionSet& set1, ExtensionSet& set2)
{
  set1.Swap (set2);
}

}

}

}
