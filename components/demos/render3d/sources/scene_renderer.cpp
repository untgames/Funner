#include "shared.h"

namespace
{

enum ConstantBufferSemantic
{
  ConstantBufferSemantic_Common,
  ConstantBufferSemantic_Transformations,
  ConstantBufferSemantic_Material,
};

struct CommonShaderParams
{
  math::vec3f light_pos;
  math::vec3f light_dir;
  int         diffuse_sampler;
  int         specular_sampler;
  int         bump_sampler;
  int         ambient_sampler;
  int         emission_sampler;
  int         reflection_sampler;
};

struct TransformationsShaderParams
{
  math::mat4f object_tm;
  math::mat4f view_tm;  
  math::mat4f model_view_tm;
  math::mat4f model_view_proj_tm;
};

struct SpriteVertex
{
  math::vec3f position;
  math::vec4f tex_coord;
  math::vec4f color;
};

class Renderable : public xtl::reference_counter
{
  public:
///Деструктор
    virtual ~Renderable () {}

///Отрисовка
    virtual void Draw (Test& test, const math::mat4f& view_tm, const math::mat4f& view_projection_tm) = 0;

///Получение материала
    virtual ModelMaterial* Material () = 0;

    void AddRef  () { addref  (this); }
    void Release () { release (this); }
};

class RenderablePrimitive : public Renderable
{
  public:
    RenderablePrimitive (const scene_graph::VisualModel& in_model, const ModelPrimitive& in_primitive)
      : model (in_model), primitive (in_primitive)
      {}

///Отрисовка
    void Draw (Test& test, const math::mat4f& view_tm, const math::mat4f& view_projection_tm)
    {
      TransformationsShaderParams params;

      IBuffer* cb = test.device->SSGetConstantBuffer (ConstantBufferSemantic_Transformations);

      if (!cb)
      {
        printf ("Null transformations constant buffer\n");
        return;
      }

      params.object_tm          = model.WorldTM ();
      params.view_tm            = transpose (view_tm);
      params.model_view_tm      = transpose (view_tm * model.WorldTM ());
      params.model_view_proj_tm = transpose (view_projection_tm * model.WorldTM ());

      cb->SetData (0, sizeof params, &params);

      ModelMeshPtr mesh = test.mesh_manager.FindMesh (model.MeshName ());

      if (!mesh)
      {
        printf ("Can't find mesh '%s'\n", model.MeshName ());
        return;
      }

      IDevice& device = *test.device;

      device.ISSetIndexBuffer (mesh->index_buffer.get ());

      ModelVertexBuffer& vb       = *primitive.vertex_buffer;
      ModelMaterial&     material = *primitive.material;

      if (!&material || !&*material.shader || !&*material.shader->program)
        return;

      device.ISSetInputLayout (vb.input_layout.get ());

      for (size_t i=0; i<vb.vertex_streams.size (); i++)
      {
        BufferPtr vs = vb.vertex_streams [i];

        device.ISSetVertexBuffer (i, vs.get ());
      }

      if (mesh->index_buffer)
      {
        device.DrawIndexed (primitive.type, primitive.first, primitive.count, 0);
      }
      else
      {
        device.Draw (primitive.type, primitive.first, primitive.count);
      }
    }

///Получение материала
    ModelMaterial* Material ()
    {
      return primitive.material.get ();
    }

  private:
    const scene_graph::VisualModel& model;
    const ModelPrimitive&           primitive;
};

class RenderableBillboardSpriteList : public Renderable
{
  public:
    RenderableBillboardSpriteList (Test& in_test, const scene_graph::SpriteList& in_sprite_list, InputLayoutPtr in_particle_vertices_input_layout)
      : test (in_test), sprite (in_sprite_list), particle_vertices_input_layout (in_particle_vertices_input_layout)
      {}

///Отрисовка
    void Draw (Test&, const math::mat4f& view_tm, const math::mat4f& view_projection_tm)
    {
      TransformationsShaderParams params;

      IBuffer* cb = test.device->SSGetConstantBuffer (ConstantBufferSemantic_Transformations);

      if (!cb)
      {
        printf ("Null transformations constant buffer\n");
        return;
      }

      math::quatf billboard_quat = inverse (sprite.WorldOrientation ()) * test.camera->WorldOrientation ();
      math::mat4f billboard_tm   = to_matrix (billboard_quat);

      params.object_tm          = sprite.WorldTM ();
      params.view_tm            = transpose (view_tm);
      params.model_view_tm      = transpose (view_tm * sprite.WorldTM ());
      params.model_view_proj_tm = transpose (view_projection_tm * sprite.WorldTM ());

      cb->SetData (0, sizeof params, &params);

      IDevice& device = *test.device;

      size_t sprites_count  = sprite.SpritesCount (),
             vertices_count = sprites_count * 6;

      xtl::uninitialized_storage<SpriteVertex> vertices (vertices_count);

      SpriteVertex*                               current_vertex      = vertices.data ();
      const scene_graph::SpriteModel::SpriteDesc* current_sprite_desc = sprite.Sprites ();

      math::vec4f ort_x (1, 0, 0, 0),
                  ort_y (0, 1, 0, 0);

      ort_x = billboard_tm * ort_x;
      ort_y = billboard_tm * ort_y;

      for (size_t i = 0; i < sprites_count; i++, current_vertex += 6, current_sprite_desc++)
      {
        math::vec3f right = ort_x * current_sprite_desc->size.x / 2.f,
                    up    = ort_y * current_sprite_desc->size.y / 2.f;

        current_vertex [0].position = current_sprite_desc->position - right + up;
        current_vertex [1].position = current_sprite_desc->position + right + up;
        current_vertex [2].position = current_sprite_desc->position - right - up;
        current_vertex [3].position = current_sprite_desc->position + right + up;
        current_vertex [4].position = current_sprite_desc->position + right - up;
        current_vertex [5].position = current_sprite_desc->position - right - up;

        current_vertex [0].tex_coord = math::vec2f (0, 1);
        current_vertex [1].tex_coord = math::vec2f (1, 1);
        current_vertex [2].tex_coord = math::vec2f (0, 0);
        current_vertex [3].tex_coord = math::vec2f (1, 1);
        current_vertex [4].tex_coord = math::vec2f (1, 0);
        current_vertex [5].tex_coord = math::vec2f (0, 0);

        current_vertex [0].color = current_sprite_desc->color;
        current_vertex [1].color = current_sprite_desc->color;
        current_vertex [2].color = current_sprite_desc->color;
        current_vertex [3].color = current_sprite_desc->color;
        current_vertex [4].color = current_sprite_desc->color;
        current_vertex [5].color = current_sprite_desc->color;
      }

      BufferDesc vb_desc;

      memset (&vb_desc, 0, sizeof vb_desc);

      vb_desc.size         = sizeof (SpriteVertex) * vertices_count;
      vb_desc.usage_mode   = UsageMode_Default;
      vb_desc.bind_flags   = BindFlag_VertexBuffer;
      vb_desc.access_flags = AccessFlag_Read | AccessFlag_Write;

      BufferPtr vb (device.CreateBuffer (vb_desc), false);

      vb->SetData (0, vb_desc.size, vertices.data ());

      device.ISSetInputLayout  (particle_vertices_input_layout.get ());
      device.ISSetVertexBuffer (0, vb.get ());

      device.Draw (PrimitiveType_TriangleList, 0, vertices_count);
    }

///Получение материала
    ModelMaterial* Material ()
    {
      return test.material_manager.FindMaterial (sprite.Material ()).get ();
    }

  private:
    Test&                          test;
    const scene_graph::SpriteList& sprite;
    InputLayoutPtr                 particle_vertices_input_layout;
};

class RenderableShotGfx : public Renderable
{
  public:
    RenderableShotGfx (Test& in_test, const scene_graph::ShotGfx& in_sprite_list, InputLayoutPtr in_particle_vertices_input_layout)
      : test (in_test), sprite (in_sprite_list), particle_vertices_input_layout (in_particle_vertices_input_layout)
      {}

///Отрисовка
    void Draw (Test& test, const math::mat4f& view_tm, const math::mat4f& view_projection_tm)
    {
      TransformationsShaderParams params;

      IBuffer* cb = test.device->SSGetConstantBuffer (ConstantBufferSemantic_Transformations);

      if (!cb)
      {
        printf ("Null transformations constant buffer\n");
        return;
      }

      params.object_tm          = sprite.WorldTM ();
      params.view_tm            = transpose (view_tm);
      params.model_view_tm      = transpose (view_tm * sprite.WorldTM ());
      params.model_view_proj_tm = transpose (view_projection_tm * sprite.WorldTM ());

      cb->SetData (0, sizeof params, &params);

      IDevice& device = *test.device;

      size_t sprites_count  = sprite.SpritesCount (),
             vertices_count = sprites_count * 6;

      xtl::uninitialized_storage<SpriteVertex> vertices (vertices_count);

      SpriteVertex*                               current_vertex      = vertices.data ();
      const scene_graph::SpriteModel::SpriteDesc* current_sprite_desc = sprite.Sprites ();

/*      printf ("rendering shot, ort_x = %.3f %.3f %.3f, ort_y = %.3f %.3f %.3f, camera_ort = %.3f %.3f %.3f\n",
              ort_x.x, ort_x.y, ort_x.z, ort_y.x, ort_y.y, ort_y.z, test.camera->WorldOrtZ ().x, test.camera->WorldOrtZ ().y, test.camera->WorldOrtZ ().z);*/

      for (size_t i = 0; i < sprites_count; i++, current_vertex += 6, current_sprite_desc++)
      {
        math::vec3f ort_x (sprite.ShotDirection (i)),
                    ort_y (normalize (math::cross (test.camera->WorldOrtZ (), ort_x)));

        math::vec3f right = ort_x * current_sprite_desc->size.x / 2.f,
                    up    = ort_y * current_sprite_desc->size.y / 2.f;

//        printf ("up = %.3f %.3f %.3f\n", up.x, up.y, up.z);

        current_vertex [0].position = current_sprite_desc->position - right + up;
        current_vertex [1].position = current_sprite_desc->position + right + up;
        current_vertex [2].position = current_sprite_desc->position - right - up;
        current_vertex [3].position = current_sprite_desc->position + right + up;
        current_vertex [4].position = current_sprite_desc->position + right - up;
        current_vertex [5].position = current_sprite_desc->position - right - up;

        current_vertex [0].tex_coord = math::vec2f (0, 1);
        current_vertex [1].tex_coord = math::vec2f (1, 1);
        current_vertex [2].tex_coord = math::vec2f (0, 0);
        current_vertex [3].tex_coord = math::vec2f (1, 1);
        current_vertex [4].tex_coord = math::vec2f (1, 0);
        current_vertex [5].tex_coord = math::vec2f (0, 0);

        current_vertex [0].color = current_sprite_desc->color;
        current_vertex [1].color = current_sprite_desc->color;
        current_vertex [2].color = current_sprite_desc->color;
        current_vertex [3].color = current_sprite_desc->color;
        current_vertex [4].color = current_sprite_desc->color;
        current_vertex [5].color = current_sprite_desc->color;
      }

      BufferDesc vb_desc;

      memset (&vb_desc, 0, sizeof vb_desc);

      vb_desc.size         = sizeof (SpriteVertex) * vertices_count;
      vb_desc.usage_mode   = UsageMode_Default;
      vb_desc.bind_flags   = BindFlag_VertexBuffer;
      vb_desc.access_flags = AccessFlag_Read | AccessFlag_Write;

      BufferPtr vb (device.CreateBuffer (vb_desc), false);

      vb->SetData (0, vb_desc.size, vertices.data ());

      device.ISSetInputLayout  (particle_vertices_input_layout.get ());
      device.ISSetVertexBuffer (0, vb.get ());

      device.Draw (PrimitiveType_TriangleList, 0, vertices_count);
    }

///Получение материала
    ModelMaterial* Material ()
    {
      return test.material_manager.FindMaterial (sprite.Material ()).get ();
    }

  private:
    Test&                       test;
    const scene_graph::ShotGfx& sprite;
    InputLayoutPtr              particle_vertices_input_layout;
};

}

typedef xtl::com_ptr <Renderable>   RenderablePtr;
typedef stl::vector <RenderablePtr> RenderablesArray;

struct SceneRenderer::Impl : public xtl::visitor<void, scene_graph::VisualModel, scene_graph::BillboardSpriteList, scene_graph::ShotGfx>
{
  Test&                      test;
  math::mat4f                view_projection_tm;
  math::mat4f                view_tm;
  scene_graph::Camera*       camera;
  RasterizerStatePtr         rasterizer;
  ProgramParametersLayoutPtr program_parameters_layout;
  BufferPtr                  common_cb;
  BufferPtr                  transformations_cb;
  InputLayoutPtr             particle_vertices_input_layout;
  BlendStatePtr              no_blend_state;
  BlendStatePtr              additive_blend_state;
  BlendStatePtr              transparency_blend_state;  
  RenderablesArray           renderables;
  DepthStencilStatePtr       depth_write_enabled_state;
  DepthStencilStatePtr       depth_write_disabled_state;

  Impl (Test& in_test)
    : test (in_test)
  {
    renderables.reserve (32);
  }

///Инициализация ресурсов
  void InitializeResources ()
  {
    printf ("Setup rasterizer stage\n");

    RasterizerDesc rasterizer_desc;

    memset (&rasterizer_desc, 0, sizeof (rasterizer_desc));

    rasterizer_desc.fill_mode               = FillMode_Solid;
    rasterizer_desc.cull_mode               = CullMode_None;
    rasterizer_desc.front_counter_clockwise = true;
    rasterizer_desc.depth_bias              = 0;
    rasterizer_desc.scissor_enable          = false;
    rasterizer_desc.multisample_enable      = true;
    rasterizer_desc.antialiased_line_enable = false;

    rasterizer = RasterizerStatePtr (test.device->CreateRasterizerState (rasterizer_desc), false);

    test.device->RSSetState (rasterizer.get ());

    printf ("Setup shader stage\n");

    static ProgramParameter shader_parameters[] = {
      {"ModelViewProjectionMatrix", ProgramParameterType_Float4x4, ConstantBufferSemantic_Transformations, 1, TEST_OFFSETOF (TransformationsShaderParams, model_view_proj_tm)},
      {"ModelViewMatrix", ProgramParameterType_Float4x4, ConstantBufferSemantic_Transformations, 1, TEST_OFFSETOF (TransformationsShaderParams, model_view_tm)},
      {"ViewMatrix", ProgramParameterType_Float4x4, ConstantBufferSemantic_Transformations, 1, TEST_OFFSETOF (TransformationsShaderParams, view_tm)},

      {"LightPosition", ProgramParameterType_Float3, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, light_pos)},
      {"LightDirection", ProgramParameterType_Float3, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, light_dir)},
      {"BumpTexture", ProgramParameterType_Int, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, bump_sampler)},
      {"DiffuseTexture", ProgramParameterType_Int, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, diffuse_sampler)},
      {"AmbientTexture", ProgramParameterType_Int, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, ambient_sampler)},
      {"SpecularTexture", ProgramParameterType_Int, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, specular_sampler)},
      {"EmissionTexture", ProgramParameterType_Int, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, emission_sampler)},
      {"ReflectionTexture", ProgramParameterType_Int, ConstantBufferSemantic_Common, 1, TEST_OFFSETOF (CommonShaderParams, reflection_sampler)},

      {"Reflectivity", ProgramParameterType_Float, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, reflectivity)},
      {"Transparency", ProgramParameterType_Float, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, transparency)},
      {"Shininess", ProgramParameterType_Float, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, shininess)},
      {"BumpAmount", ProgramParameterType_Float, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, bump_amount)},
      {"BumpTextureChannel", ProgramParameterType_Int, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, bump_texture_channel)},
      {"DiffuseTextureChannel", ProgramParameterType_Int, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, diffuse_texture_channel)},
      {"AmbientTextureChannel", ProgramParameterType_Int, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, ambient_texture_channel)},
      {"SpecularTextureChannel", ProgramParameterType_Int, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, specular_texture_channel)},
      {"EmissionTextureChannel", ProgramParameterType_Int, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, emission_texture_channel)},
      {"DiffuseColor", ProgramParameterType_Float4, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, diffuse_color)},
      {"AmbientColor", ProgramParameterType_Float4, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, ambient_color)},
      {"SpecularColor", ProgramParameterType_Float4, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, specular_color)},
      {"EmissionColor", ProgramParameterType_Float4, ConstantBufferSemantic_Material, 1, TEST_OFFSETOF (MaterialShaderParams, emission_color)},
    };

    ProgramParametersLayoutDesc program_parameters_layout_desc = {sizeof shader_parameters / sizeof *shader_parameters, shader_parameters};

    program_parameters_layout = ProgramParametersLayoutPtr (test.device->CreateProgramParametersLayout (program_parameters_layout_desc), false);

    BufferDesc cb_desc;

    memset (&cb_desc, 0, sizeof cb_desc);

    cb_desc.size         = sizeof (CommonShaderParams);
    cb_desc.usage_mode   = UsageMode_Default;
    cb_desc.bind_flags   = BindFlag_ConstantBuffer;
    cb_desc.access_flags = AccessFlag_ReadWrite;

    common_cb = BufferPtr (test.device->CreateBuffer (cb_desc), false);

    CommonShaderParams common_shader_params;

    common_shader_params.bump_sampler       = SamplerChannel_Bump;
    common_shader_params.diffuse_sampler    = SamplerChannel_Diffuse;
    common_shader_params.specular_sampler   = SamplerChannel_Specular;
    common_shader_params.ambient_sampler    = SamplerChannel_Ambient;
    common_shader_params.emission_sampler   = SamplerChannel_Emission;
    common_shader_params.reflection_sampler = SamplerChannel_Reflection;

    common_cb->SetData (0, sizeof common_shader_params, &common_shader_params);

    test.device->SSSetProgramParametersLayout (program_parameters_layout.get ());
    test.device->SSSetConstantBuffer (ConstantBufferSemantic_Common, common_cb.get ());

    memset (&cb_desc, 0, sizeof cb_desc);

    cb_desc.size         = sizeof (TransformationsShaderParams);
    cb_desc.usage_mode   = UsageMode_Default;
    cb_desc.bind_flags   = BindFlag_ConstantBuffer;
    cb_desc.access_flags = AccessFlag_ReadWrite;

    transformations_cb = BufferPtr (test.device->CreateBuffer (cb_desc), false);

    test.device->SSSetConstantBuffer (ConstantBufferSemantic_Transformations, transformations_cb.get ());

    VertexAttribute particle_vertices_input_layout_attributes [] = {
      {VertexAttributeSemantic_Position,  InputDataFormat_Vector3, InputDataType_Float, 0, offsetof (SpriteVertex, position), sizeof (SpriteVertex)},
      {VertexAttributeSemantic_TexCoord0, InputDataFormat_Vector2, InputDataType_Float, 0, offsetof (SpriteVertex, tex_coord), sizeof (SpriteVertex)},
      {VertexAttributeSemantic_Color,     InputDataFormat_Vector4, InputDataType_Float, 0, offsetof (SpriteVertex, color), sizeof (SpriteVertex)},
    };

    InputLayoutDesc particle_vertices_input_layout_desc;

    memset (&particle_vertices_input_layout_desc, 0, sizeof particle_vertices_input_layout_desc);

    particle_vertices_input_layout_desc.vertex_attributes_count = sizeof particle_vertices_input_layout_attributes / sizeof *particle_vertices_input_layout_attributes;
    particle_vertices_input_layout_desc.vertex_attributes       = particle_vertices_input_layout_attributes;
    particle_vertices_input_layout_desc.index_type              = InputDataType_UByte;

    particle_vertices_input_layout = InputLayoutPtr (test.device->CreateInputLayout (particle_vertices_input_layout_desc), false);

    BlendDesc blend_desc;

    memset (&blend_desc, 0, sizeof (blend_desc));

    blend_desc.blend_enable     = false;
    blend_desc.color_write_mask = ColorWriteFlag_All;

    no_blend_state = BlendStatePtr (test.device->CreateBlendState (blend_desc), false);

    blend_desc.blend_enable                     = true;
    blend_desc.blend_color_operation            = BlendOperation_Add;
    blend_desc.blend_alpha_operation            = BlendOperation_Add;
    blend_desc.blend_color_source_argument      = BlendArgument_SourceAlpha;
    blend_desc.blend_color_destination_argument = BlendArgument_One;
    blend_desc.blend_alpha_source_argument      = BlendArgument_SourceAlpha;
    blend_desc.blend_alpha_destination_argument = BlendArgument_One;

    additive_blend_state = BlendStatePtr (test.device->CreateBlendState (blend_desc), false);

    blend_desc.blend_enable                     = true;
    blend_desc.blend_color_operation            = BlendOperation_Add;
    blend_desc.blend_alpha_operation            = BlendOperation_Add;
    blend_desc.blend_color_source_argument      = BlendArgument_SourceAlpha;
    blend_desc.blend_color_destination_argument = BlendArgument_InverseSourceAlpha;
    blend_desc.blend_alpha_source_argument      = BlendArgument_SourceAlpha;
    blend_desc.blend_alpha_destination_argument = BlendArgument_InverseSourceAlpha;

    transparency_blend_state = BlendStatePtr (test.device->CreateBlendState (blend_desc), false);    

    DepthStencilDesc depth_stencil_desc;

    memset (&depth_stencil_desc, 0, sizeof (depth_stencil_desc));

    depth_stencil_desc.depth_test_enable  = true;
    depth_stencil_desc.depth_write_enable = true;
    depth_stencil_desc.depth_compare_mode = CompareMode_Less;

    depth_write_enabled_state = DepthStencilStatePtr (test.device->CreateDepthStencilState (depth_stencil_desc), false);

    depth_stencil_desc.depth_write_enable = false;

    depth_write_disabled_state = DepthStencilStatePtr (test.device->CreateDepthStencilState (depth_stencil_desc), false);
  }

  void SetMaterial (IDevice& device, ModelMaterial& material)
  {
    device.SSSetProgram (material.shader->program.get ());

    int samplers_count = stl::min ((int)MAX_SAMPLERS, (int)SamplerChannel_Num);

    for (int i = 0; i < samplers_count; i++)
      if (material.texmaps [i].texture && material.texmaps [i].sampler)
      {
        device.SSSetTexture (i, &*material.texmaps [i].texture);
        device.SSSetSampler (i, &*material.texmaps [i].sampler);
      }
      else
      {
        device.SSSetTexture (i, 0);
        device.SSSetSampler (i, 0);
      }

    device.SSSetConstantBuffer (ConstantBufferSemantic_Material, &*material.constant_buffer);
  }

///Рисование сцены
  void Draw (scene_graph::Camera& in_camera)
  {
    camera = &in_camera;

    if (!camera->Scene ())
      return;

    view_tm            = inverse (camera->WorldTM ());
    view_projection_tm = camera->ProjectionMatrix () * view_tm;

    UpdateLights ();

    renderables.clear ();

    camera->Scene ()->VisitEach (*this);

    DrawCore ();
  }

  void DrawCore ()
  {
    IDevice& device = *test.device;

    device.OSSetDepthStencilState (depth_write_enabled_state.get ());
    device.OSSetBlendState (no_blend_state.get ());

    for (RenderablesArray::iterator iter = renderables.begin (), end = renderables.end (); iter != end; ++iter)
    {
      ModelMaterial* material = (*iter)->Material ();
      
      if (!material || material->blend_mode != BlendMode_No)
        continue;

      SetMaterial (device, *material);

      (*iter)->Draw (test, view_tm, view_projection_tm);
    }

    device.OSSetDepthStencilState (depth_write_disabled_state.get ());

    for (RenderablesArray::iterator iter = renderables.begin (), end = renderables.end (); iter != end; ++iter)
    {
      ModelMaterial* material = (*iter)->Material ();

      if (!material)
        continue;
        
      switch (material->blend_mode)
      {
        case BlendMode_Additive:
          device.OSSetBlendState (additive_blend_state.get ());        
          break;
        case BlendMode_Transparency:
          device.OSSetBlendState (transparency_blend_state.get ());        
          break;
        default:
          continue;
      }

      SetMaterial (device, *material);

      (*iter)->Draw (test, view_tm, view_projection_tm);
    }

    renderables.clear ();
  }

  void UpdateLights ()
  {
    CommonShaderParams my_shader_parameters;

    IBuffer* cb = test.device->SSGetConstantBuffer (0);

    if (!cb)
    {
      printf ("Null constant buffer #0\n");
      return;
    }

    cb->GetData (0, sizeof my_shader_parameters, &my_shader_parameters);

    my_shader_parameters.light_pos = test.light->WorldPosition ();
    my_shader_parameters.light_dir = test.light->WorldOrtZ ();

    cb->SetData (0, sizeof my_shader_parameters, &my_shader_parameters);
  }

  void visit (scene_graph::VisualModel& model)
  {
    ModelMeshPtr mesh = test.mesh_manager.FindMesh (model.MeshName ());

    if (!mesh)
    {
      printf ("Can't find mesh '%s'\n", model.MeshName ());
      return;
    }

    for (PrimitiveArray::const_iterator iter=mesh->primitives.begin (); iter!=mesh->primitives.end (); ++iter)
    {
      RenderablePtr renderable (new RenderablePrimitive (model, *iter), false);

      renderables.push_back (renderable);
    }
  }

  void visit (scene_graph::BillboardSpriteList& sprite)
  {
    RenderablePtr renderable (new RenderableBillboardSpriteList (test, sprite, particle_vertices_input_layout), false);

    renderables.push_back (renderable);
  }

  void visit (scene_graph::ShotGfx& sprite)
  {
    RenderablePtr renderable (new RenderableShotGfx (test, sprite, particle_vertices_input_layout), false);

    renderables.push_back (renderable);
  }
};

SceneRenderer::SceneRenderer (Test& test)
  : impl (new Impl (test))
  {}

SceneRenderer::~SceneRenderer ()
{
  delete impl;
}

///Инициализация ресурсов
void SceneRenderer::InitializeResources ()
{
  impl->InitializeResources ();
}

///Рисование сцены
void SceneRenderer::Draw (scene_graph::Camera& camera)
{
  impl->Draw (camera);
}
