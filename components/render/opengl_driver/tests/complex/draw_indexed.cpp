#include "shared.h"

struct Vec3f
{
  float x, y, z;
};

struct Color4ub
{
  unsigned char red, green, blue, alpha;
};

struct MyVertex
{
  Vec3f    position;
  Vec3f    normal;
  Color4ub color;
};

void redraw (Test& test)
{
  test.device->DrawIndexed (PrimitiveType_TriangleList, 0, 3, 0);
}

int main ()
{
  printf ("Results of draw_indexed_test:\n");

  try
  {
    Test test (L"OpenGL device test window (draw_indexed)", &redraw);

    test.window.Show ();

    printf ("Create vertex buffer\n");

    static const size_t VERTICES_COUNT = 3;

    BufferDesc vb_desc;

    memset (&vb_desc, 0, sizeof vb_desc);

    vb_desc.size         = sizeof (MyVertex) * VERTICES_COUNT;
    vb_desc.usage_mode   = UsageMode_Default;
    vb_desc.bind_flags   = BindFlag_VertexBuffer;
    vb_desc.access_flags = AccessFlag_Read | AccessFlag_Write;

    BufferPtr vb (test.device->CreateBuffer (vb_desc), false);

    static const MyVertex verts [] = {
      {{-1, -1, 0}, {0, 0, 1}, {255, 0, 0, 0}},
      {{ 1, -1, 0}, {0, 0, 1}, {0, 255, 0, 0}},
      {{ 0, 1, 0}, {0, 0, 1}, {0, 0, 255, 0}},
    };

    vb->SetData (0, vb_desc.size, verts);

    printf ("Create index buffer\n");

    static size_t indices [] = {0, 1, 2};

    BufferDesc ib_desc;

    memset (&ib_desc, 0, sizeof ib_desc);

    ib_desc.size         = sizeof indices;
    ib_desc.usage_mode   = UsageMode_Default;
    ib_desc.bind_flags   = BindFlag_IndexBuffer;
    ib_desc.access_flags = AccessFlag_ReadWrite;

    BufferPtr ib (test.device->CreateBuffer (ib_desc), false);

    ib->SetData (0, ib_desc.size, indices);

    printf ("Set input-stage\n");

    VertexAttribute attributes [] = {
      {VertexAttributeSemantic_Normal, InputDataFormat_Vector3, InputDataType_Float, 0, offsetof (MyVertex, normal), sizeof (MyVertex)},
      {VertexAttributeSemantic_Position, InputDataFormat_Vector3, InputDataType_Float, 0, offsetof (MyVertex, position), sizeof (MyVertex)},
      {VertexAttributeSemantic_Color, InputDataFormat_Vector4, InputDataType_UByte, 0, offsetof (MyVertex, color), sizeof (MyVertex)},
    };

    InputLayoutDesc layout_desc;

    memset (&layout_desc, 0, sizeof layout_desc);

    layout_desc.vertex_attributes_count = sizeof attributes / sizeof *attributes;
    layout_desc.vertex_attributes       = attributes;
    layout_desc.index_type              = InputDataType_UInt;
    layout_desc.index_buffer_offset     = 0;

    InputLayoutPtr layout (test.device->CreateInputLayout (layout_desc), false);

    test.device->ISSetInputLayout (layout.get ());
    test.device->ISSetVertexBuffer (0, vb.get ());
    test.device->ISSetIndexBuffer (ib.get ());

    printf ("Main loop\n");

    syslib::Application::Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }

  return 0;
}
