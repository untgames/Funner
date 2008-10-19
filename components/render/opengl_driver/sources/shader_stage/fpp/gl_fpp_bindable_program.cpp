#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Описание параметра и группы параметров fpp-программы
*/

struct FppBindableProgram::Parameter
{
  const FppDynamicParameter* location; //указатель на динамический параметр шейдера
  size_t                     offset;   //смещение относительно начала константного буфера
  size_t                     size;     //размер параметра в байтах
};

struct FppBindableProgram::Group
{
  size_t     slot;        //номер слота с константым буфером
  size_t     data_hash;   //хеш данных константного буфера
  size_t     count;       //количество элементов группы
  Parameter* parameters;  //указатель на начало области с элементами
};

/*
    Конструктор
*/

FppBindableProgram::FppBindableProgram
 (const ContextManager&    context_manager,
  FppProgram&              program,
  ProgramParametersLayout* parameters_layout)
   : ContextObject (context_manager)
{
  try
  {
      //получение базового состояния фиксированной программы шейдинга
    
    fpp_state = program.GetBaseState ();  
    
    identity_matrix (view_object_matrix);

      //обновление хэшей

    UpdateHashes ();  

      //обработка ситуации отсутствия параметров

    if (!parameters_layout || !parameters_layout->GetParametersCount ())
      return;

      //резервирование памяти для хранения параметров и групп
    
    parameters.reserve (parameters_layout->GetParametersCount ());
    groups.reserve     (parameters_layout->GetGroupsCount ());
    
      //преобразование параметров
      
    const ProgramParameterGroup* src_groups = parameters_layout->GetGroups ();  
      
    for (size_t i=0, count=parameters_layout->GetGroupsCount (); i<count; i++)
    {
      const ProgramParameterGroup& src_group = src_groups [i];
      
      size_t start_parameters_count = parameters.size ();
      
      for (size_t j=0; j<src_group.count; j++)
      {
        const ProgramParameter& src_param = src_group.parameters [j];
        Parameter               dst_param;
        
          //поиск параметра в шейдере        

        dst_param.location = program.FindDynamicParameter (src_param.name);
        dst_param.offset   = src_param.offset;

        if (!dst_param.location)
          continue; //если параметр отсутствует - игнорируем его 

          //проверка соответствия типов
          
        const FppDynamicParameter& dyn_param = *dst_param.location;

        bool check_status = false;

        switch (src_param.type)
        {
          case ProgramParameterType_Int:
            check_status = dyn_param.type == FppDynamicParameterType_Int && dyn_param.count == 1;
            break;
          case ProgramParameterType_Float:
            check_status = dyn_param.type == FppDynamicParameterType_Float && dyn_param.count == 1;
            break;
          case ProgramParameterType_Int2:
            check_status = dyn_param.type == FppDynamicParameterType_Int && dyn_param.count == 2;
            break;
          case ProgramParameterType_Float2:
            check_status = dyn_param.type == FppDynamicParameterType_Float && dyn_param.count == 2;
            break;
          case ProgramParameterType_Int3:
            check_status = dyn_param.type == FppDynamicParameterType_Int && dyn_param.count == 3;
            break;
          case ProgramParameterType_Float3:
            check_status = dyn_param.type == FppDynamicParameterType_Float && dyn_param.count == 3;
            break;
          case ProgramParameterType_Int4:
            check_status = dyn_param.type == FppDynamicParameterType_Int && dyn_param.count == 4;
            break;
          case ProgramParameterType_Float4:
            check_status = dyn_param.type == FppDynamicParameterType_Float && dyn_param.count == 4;
            break;
          case ProgramParameterType_Float2x2:
            check_status = dyn_param.type == FppDynamicParameterType_Float && dyn_param.count == 4;
            break;
          case ProgramParameterType_Float3x3:
            check_status = dyn_param.type == FppDynamicParameterType_Float && dyn_param.count == 9;
            break;
          case ProgramParameterType_Float4x4:
            check_status = dyn_param.type == FppDynamicParameterType_Float && dyn_param.count == 16;
            break;
          default:
            LogPrintf ("Internal error: undefined program parameter type %d", src_param.type);
            continue;
        }
        
        switch (dyn_param.type)
        {
          case FppDynamicParameterType_Int:
            dst_param.size = dyn_param.count * sizeof (int);
            break;
          case FppDynamicParameterType_Float:
            dst_param.size = dyn_param.count * sizeof (float);
            break;         
          default:
            dst_param.size = 0;
            break;
        }
        
        if (!check_status)
        {
          const char* dyn_param_type_name;

          switch (dyn_param.type)
          {
            case FppDynamicParameterType_Int:    dyn_param_type_name = "int"; break;
            case FppDynamicParameterType_Float:  dyn_param_type_name = "float"; break;         
            default:                             dyn_param_type_name = "__unknown__"; break;
          }

          LogPrintf ("Shader parameter '%s' declaration %sx%u mismatch with layout definition %s",
                     src_param.name, dyn_param_type_name, dyn_param.count, get_name (src_param.type));

          continue;
        }

          //добавляем созданный параметр

        parameters.push_back (dst_param);
      }
      
        //добавление новой группы
        
      size_t group_parameters_count = parameters.size () - start_parameters_count;

      if (!group_parameters_count)
        continue; //игнорируем пустые группы

      Group dst_group;

      dst_group.slot       = src_group.slot;
      dst_group.data_hash  = 0;
      dst_group.count      = group_parameters_count;
      dst_group.parameters = &parameters [start_parameters_count];

      groups.push_back (dst_group);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::FppBindableProgram::FppBindableProgram");
    throw;
  }
}

/*
    Биндинг
*/

namespace
{

//эмуляция загрузки транспонированной матрицы в контекст OpenGL
void load_transpose_matrix (Matrix4f matrix, PFNGLLOADTRANSPOSEMATRIXFPROC fn)
{
  if (fn)
  {
    fn (&matrix [0][0]);
    
    return;
  }
  
  Matrix4f transposed_matrix;

  transpose_matrix (matrix, transposed_matrix);

  glLoadMatrixf (&transposed_matrix [0][0]);
}

}

void FppBindableProgram::Bind (ConstantBufferPtr* constant_buffers)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FppBindableProgram::Bind";
  
    //проверка корректности аргументов
    
  if (!constant_buffers)
    throw xtl::make_null_argument_exception (METHOD_NAME, "constant_buffers");
  
    //получение кэш переменных

  const ContextCaps& caps = GetCaps ();
                   
  const size_t current_program            = GetContextCacheValue (CacheEntry_UsedProgram),
               current_viewer_hash        = GetContextCacheValue (CacheEntry_FppViewerStateHash),
               current_object_hash        = GetContextCacheValue (CacheEntry_FppObjectStateHash),
               current_rasterization_hash = GetContextCacheValue (CacheEntry_FppRasterizationStateHash),
               current_material_hash      = GetContextCacheValue (CacheEntry_FppMaterialStateHash),
               current_lighting_hash      = GetContextCacheValue (CacheEntry_FppLightingStateHash),
               current_texmaps_hash       = GetContextCacheValue (CacheEntry_FppTexmapsStateHash),
               current_modes_hash         = GetContextCacheValue (CacheEntry_FppModesStateHash);

    //отключение glsl-шейдеров  

  if (current_program != GetId ())
  {    
    if (caps.has_arb_shading_language_100)
      caps.glUseProgram_fn (0);

    SetContextCacheValue (CacheEntry_UsedProgram, 0);
  }

    //извлечение параметров из кэша расположения параметров
    
  char* dst_data = (char*)&fpp_state;

    //обновление динамических параметров
    
  bool need_update_hashes = false;

  for (GroupArray::iterator iter=groups.begin (), end=groups.end (); iter!=end; ++iter)
  {
    Group& group = *iter;

    IBindableBuffer* buffer = constant_buffers [group.slot].get ();

      //проверка наличия требуемого константного буфера

    if (!buffer)
      throw xtl::format_operation_exception (METHOD_NAME, "Null constant buffer #%u", group.slot);

      //проверка необходимости обновления параметров

    if (group.data_hash == buffer->GetDataHash ())
      continue;
      
    need_update_hashes = true;

      //получение базового адреса константного буфера
      
    char* buffer_base = (char*)buffer->GetDataPointer ();
 
    if (!buffer_base)
      throw xtl::format_operation_exception (METHOD_NAME, "Null constant buffer #%u data pointer", group.slot);
      
      //обновление динамических параметров

    for (size_t j=0; j<group.count; j++)
    {
      const Parameter& param      = group.parameters [j];
      const char*      src_data   = buffer_base + param.offset;

        //обновление всех полей FppState, ассоциированных с динамическим параметром

      const size_t* offset      = &param.location->field_offsets [0],
                    update_size = param.size;

      for (size_t k=0, count=param.location->field_offsets.size (); k<count; k++, offset++)
        memcpy (dst_data + *offset, src_data, update_size);
    }

      //обновление хэша данных группы параметров

    group.data_hash = buffer->GetDataHash ();    
  }
  
    //обновление хэшей FppState
    
  if (need_update_hashes)
  {
    UpdateHashes ();
  }

    //установка состояния в контекст OpenGL    
    
  bool need_update_modelview_matrix = false,
       need_update_modes            = current_modes_hash != modes_hash,
       need_update_viewer           = current_viewer_hash != viewer_hash,
       need_update_object           = current_object_hash != object_hash,       
       need_update_rasterization    = current_rasterization_hash != rasterization_hash,
       need_update_material         = current_material_hash != material_hash,
       need_update_lighting         = current_lighting_hash != lighting_hash || need_update_viewer,
       need_update_texmaps          = current_texmaps_hash != texmaps_hash;    
    
    //установка режимов отрисовки
    
  if (need_update_modes)
  {
    if (fpp_state.modes.normalize) glEnable  (GL_NORMALIZE);
    else                           glDisable (GL_NORMALIZE);

    SetContextCacheValue (CacheEntry_FppModesStateHash, modes_hash);
  }    
  
    //установка параметров растеризации
    
  if (need_update_rasterization)
  {
    float point_size = fpp_state.rasterization.point_size,
          line_width = fpp_state.rasterization.line_width;
          
    unsigned short line_stipple_factor = fpp_state.rasterization.line_stipple_factor;        

    if (point_size <= 0.0f)        point_size = 1.0f;
    if (line_width <= 0.0f)        line_width = 1.0f;    
    if (line_stipple_factor < 1)   line_stipple_factor = 1;
    if (line_stipple_factor > 255) line_stipple_factor = 255;

    glPointSize (point_size);
    glLineWidth (line_width);
    
      //проблемы с поддержкой на MSOGL
//    glLineStipple (line_stipple_factor, static_cast<unsigned short> (fpp_state.rasterization.line_stipple_pattern));

    SetContextCacheValue (CacheEntry_FppRasterizationStateHash, rasterization_hash);
  }

    //установка параметров источников освещения

  if (need_update_lighting)
  {
    bool lighting = false;
    
    for (size_t i=0; i<FPP_MAX_LIGHTS_COUNT; i++)
      if (fpp_state.lights [i].enable)
      {
        lighting = true;
        break;
      }
      
    if (lighting)
    {
        //включение освещения
      
      glEnable      (GL_LIGHTING);
      glLightModeli (GL_LIGHT_MODEL_TWO_SIDE,     GL_TRUE);
      glLightModeli (GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
      
        //установка преобразования наблюдателя

      glMatrixMode (GL_MODELVIEW);

      load_transpose_matrix (fpp_state.viewer.view_matrix, caps.glLoadTransposeMatrixf_fn);

      need_update_modelview_matrix = true;
      
        //установка параметров источника

      for (size_t i=0; i<FPP_MAX_LIGHTS_COUNT; i++)
      {
        const LightDesc& light    = fpp_state.lights [i];
        GLenum           light_id = GL_LIGHT0 + i;

        if (!light.enable)
        {
          glDisable (light_id);
          continue;
        }

        float position [4] = {light.position [0], light.position [1], light.position [2], light.type != LightType_Remote};

        glEnable  (light_id);
        glLightfv (light_id, GL_POSITION,              position);
        glLightfv (light_id, GL_SPOT_DIRECTION,        light.direction);
        glLightfv (light_id, GL_AMBIENT,               (GLfloat*)&light.ambient_color);
        glLightfv (light_id, GL_DIFFUSE,               (GLfloat*)&light.diffuse_color);
        glLightfv (light_id, GL_SPECULAR,              (GLfloat*)&light.specular_color);
        glLightf  (light_id, GL_SPOT_CUTOFF,           light.type != LightType_Point ? light.angle : 180.0f);
        glLightf  (light_id, GL_SPOT_EXPONENT,         light.exponent);
        glLightf  (light_id, GL_CONSTANT_ATTENUATION,  light.constant_attenuation);
        glLightf  (light_id, GL_LINEAR_ATTENUATION,    light.linear_attenuation);
        glLightf  (light_id, GL_QUADRATIC_ATTENUATION, light.quadratic_attenuation);
      }      
    }
    else
    {
      glDisable (GL_LIGHTING);    
    }

    SetContextCacheValue (CacheEntry_FppLightingStateHash, lighting_hash);
  }    
  
    //установка параметров материала

  if (need_update_material)
  {
    glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION,  (GLfloat*)&fpp_state.material.emission_color);
    glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT,   (GLfloat*)&fpp_state.material.ambient_color);
    glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE,   (GLfloat*)&fpp_state.material.diffuse_color);
    glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR,  (GLfloat*)&fpp_state.material.specular_color);
    glMaterialf  (GL_FRONT_AND_BACK, GL_SHININESS, fpp_state.material.shininess);

      //настройка передачи цвета материала

    if (fpp_state.material.color_material == ColorMaterial_Explicit)
    {
      glDisable (GL_COLOR_MATERIAL);
    }
    else
    {
      GLenum mode;
      
      switch (fpp_state.material.color_material)
      {
        case ColorMaterial_Emission:          mode = GL_EMISSION; break;
        case ColorMaterial_Ambient:           mode = GL_AMBIENT; break;
        default:
        case ColorMaterial_Diffuse:           mode = GL_DIFFUSE; break;
        case ColorMaterial_Specular:          mode = GL_SPECULAR; break;
        case ColorMaterial_AmbientAndDiffuse: mode = GL_AMBIENT_AND_DIFFUSE; break;
      }
      
      glEnable        (GL_COLOR_MATERIAL);
      glColorMaterial (GL_FRONT_AND_BACK, mode);
    }

      //включение параметров альфа-теста

    if (fpp_state.material.alpha_compare_mode != CompareMode_AlwaysPass)
    {
      glEnable (GL_ALPHA_TEST);
      
      GLenum gl_mode;
      
      switch (fpp_state.material.alpha_compare_mode)
      {      
        case CompareMode_AlwaysFail:   gl_mode = GL_NEVER;    break;
        default:
        case CompareMode_AlwaysPass:   gl_mode = GL_ALWAYS;   break;
        case CompareMode_Equal:        gl_mode = GL_EQUAL;    break;
        case CompareMode_NotEqual:     gl_mode = GL_NOTEQUAL; break;
        case CompareMode_Less:         gl_mode = GL_LESS;     break;
        case CompareMode_LessEqual:    gl_mode = GL_LEQUAL;   break;
        case CompareMode_Greater:      gl_mode = GL_GREATER;  break;
        case CompareMode_GreaterEqual: gl_mode = GL_GEQUAL;   break;
      }

      glAlphaFunc (gl_mode, fpp_state.material.alpha_reference);
    }
    else
    {
      glDisable (GL_ALPHA_TEST);
    }

    SetContextCacheValue (CacheEntry_FppMaterialStateHash, material_hash);
  }  

    //установка параметров текстурирования
    
  if (need_update_texmaps)
  {    
    size_t       texture_units_count      = caps.has_arb_multitexture ? caps.texture_units_count : 1;
    const size_t active_texture_slot      = GetContextCacheValue (CacheEntry_ActiveTextureSlot),
                 current_enabled_textures = GetContextCacheValue (CacheEntry_EnabledTextures);

    if (texture_units_count > DEVICE_SAMPLER_SLOTS_COUNT)
      texture_units_count = DEVICE_SAMPLER_SLOTS_COUNT;

    glMatrixMode (GL_TEXTURE);

    for (size_t i=0; i<texture_units_count; i++)
    {
        //установка активного слота текстурирования

      if (active_texture_slot != i)
      {
        caps.glActiveTexture_fn (GL_TEXTURE0 + i);

        SetContextCacheValue (CacheEntry_ActiveTextureSlot, i);
      }

        //если текстура не установлена - отключение текстурирования канала

      if (!(current_enabled_textures & (1 << i)))
      {
        glDisable (GL_TEXTURE_2D);

        continue;
      }
      
        //включение текстурирования на канале
        
      glEnable (GL_TEXTURE_2D);        
      
      TexmapDesc& texmap = fpp_state.maps [i];
      
        //установка параметров смешивания цветов
        
      GLenum blend_mode;
        
      switch (texmap.blend)
      {
        default:
        case TextureBlend_Replace:
          blend_mode = GL_REPLACE;
          break;
        case TextureBlend_Modulate:
          blend_mode = GL_MODULATE;
          break;
        case TextureBlend_Blend:
          blend_mode = GL_BLEND;
          break;
      }    

      glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, blend_mode);

        //установка параметров генерации текстурных координат

      load_transpose_matrix (texmap.transform, caps.glLoadTransposeMatrixf_fn);
      
      TexcoordSource source [] = {texmap.source_u, texmap.source_v, texmap.source_w};
      
      for (size_t i=0; i<3; i++)
      {
        GLenum coord = GL_S + i, coord_gen_mode = GL_TEXTURE_GEN_S + i;

        switch (source [i])
        {
          case TexcoordSource_Explicit:
            glDisable (coord_gen_mode);
            break;
          case TexcoordSource_SphereMap:
          case TexcoordSource_ReflectionMap:
            glEnable  (coord_gen_mode);
            glTexGeni (coord, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
            break;
          case TexcoordSource_ObjectSpace:
            glEnable   (coord_gen_mode);
            glTexGeni  (coord, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
            glTexGenfv (coord, GL_OBJECT_PLANE, &texmap.texgen [i][0]);
            break;
          case TexcoordSource_ViewerSpace:
            glEnable   (coord_gen_mode);
            glTexGeni  (coord, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
            glTexGenfv (coord, GL_EYE_PLANE, &texmap.texgen [i][0]);
            break;
          default:
            break;
        }
      }
    }
    
    SetContextCacheValue (CacheEntry_FppTexmapsStateHash, texmaps_hash);
  }
  
    //установка параметров наблюдателя    
    
  bool need_compute_view_object_matrix = false;

  if (need_update_viewer)
  {    
    glMatrixMode          (GL_PROJECTION);
    load_transpose_matrix (fpp_state.viewer.projection_matrix, caps.glLoadTransposeMatrixf_fn);

    SetContextCacheValue (CacheEntry_FppViewerStateHash, viewer_hash);

    need_compute_view_object_matrix = true;
  }

    //установка параметров объекта

  if (need_update_object)
  {
    SetContextCacheValue (CacheEntry_FppObjectStateHash, object_hash);

    need_compute_view_object_matrix = true;
  }

  if (need_compute_view_object_matrix)
  {
    mult_matrix (fpp_state.viewer.view_matrix, fpp_state.object.matrix, view_object_matrix);

    need_update_modelview_matrix = true;
  }

  if (need_update_modelview_matrix)
  {
    glMatrixMode          (GL_MODELVIEW);
    load_transpose_matrix (view_object_matrix, caps.glLoadTransposeMatrixf_fn);
  }  
  
    //проверка ошибок
    
  CheckErrors (METHOD_NAME);
}

/*
    Обновление хэшей
*/

void FppBindableProgram::UpdateHashes ()
{
  viewer_hash        = crc32 (&fpp_state.viewer, sizeof (fpp_state.viewer));
  object_hash        = crc32 (&fpp_state.object, sizeof (fpp_state.object));
  rasterization_hash = crc32 (&fpp_state.rasterization, sizeof (fpp_state.rasterization));
  material_hash      = crc32 (&fpp_state.material, sizeof (fpp_state.material));
  lighting_hash      = crc32 (fpp_state.lights, sizeof (fpp_state.lights));
  texmaps_hash       = crc32 (fpp_state.maps, sizeof (fpp_state.maps));
  modes_hash         = crc32 (&fpp_state.modes, sizeof (fpp_state.modes));
}
