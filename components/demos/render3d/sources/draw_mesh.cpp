#include "shared.h"

void draw (IDevice& device, ModelMesh& mesh)
{
  device.ISSetIndexBuffer (mesh.index_buffer.get ());

  for (PrimitiveArray::const_iterator iter=mesh.primitives.begin (); iter!=mesh.primitives.end (); ++iter)
  {
    const ModelPrimitive& primitive = *iter;
    ModelVertexBuffer&    vb        = *primitive.vertex_buffer;
    ModelMaterial&        material  = *primitive.material;
    
    if (!&material || !&*material.shader || !&*material.shader->program)
      continue;

    device.SSSetProgram (material.shader->program.get ());
    
    int samplers_count = stl::min ((int)MAX_SAMPLERS, (int)SamplerChannel_Num);

    for (int i=0; i<samplers_count; i++)
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
    
    device.ISSetInputLayout (vb.input_layout.get ());

    for (size_t i=0; i<vb.vertex_streams.size (); i++)
    {
      BufferPtr vs = vb.vertex_streams [i];

      device.ISSetVertexBuffer (i, vs.get ());
    }
    
    if (mesh.index_buffer)
    {
      device.DrawIndexed (primitive.type, primitive.first, primitive.count, 0);
    }
    else
    {
      device.Draw (primitive.type, primitive.first, primitive.count);
    }
  }
}
