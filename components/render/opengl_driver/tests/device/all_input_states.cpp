#include "shared.h"

OutputMode Mode = OutputMode_Default;

void print_input_layout_desc(const InputLayoutDesc& desc)
{
  static const char* header_names[] = {"Buffer", "Data type", "Data format", "Semantic", "Slot", "Offset", "Stride"};
  static const char* buffer_names[] = {"Index", "Vertex 0", "Vertex 1", "Vertex 2", "Vertex 3", "Vertex 4", "Vertex 5", "Vertex 6", "Vertex 7"};
  static const char* filler = " ";

  printf("InputLayout parameters:\n");
  printf("=====================================================================================================================================\n");
  printf(" %-10s  | %-24s | %-24s | %-36s | %-4s | %-8s | %-6s\n",
    header_names[0], header_names[1], header_names[2], header_names[3], header_names[4], header_names[5], header_names[6]);
  printf("-------------------------------------------------------------------------------------------------------------------------------------\n");
  printf(" %-10s  | %-24s | %-24s | %-36s | %-4s | %-8d | %-6s\n",
    buffer_names[0], get_name(desc.index_type), filler, filler, filler, desc.index_buffer_offset, filler);
  for (int i = 0; i < desc.vertex_attributes_count; i++)
  {
    printf(" %-10s  | %-24s | %-24s | %-36s | %-4d | %-8d | %-6d\n",
      buffer_names[i + 1],
      get_name(desc.vertex_attributes[i].type),
      get_name(desc.vertex_attributes[i].format),
      get_name(desc.vertex_attributes[i].semantic),
      desc.vertex_attributes[i].slot,
      desc.vertex_attributes[i].offset,
      desc.vertex_attributes[i].stride
    );
  }
  printf("=====================================================================================================================================\n");
  fflush(stdout);
}

void test_input_layout_desc(const InputLayoutDesc& desc, IDevice* device)
{
  try
  {
    device->ISSetInputLayout(device->CreateInputLayout(desc));
    device->Draw(PrimitiveType_PointList, 0, 0);
    if (Mode != OutputMode_FailOnly)
    {
      print_input_layout_desc(desc);
      printf("Testing... OK\n");
    }
  }
  catch (std::exception& exception)
  {
    if (Mode != OutputMode_SuccessOnly)
    {
      print_input_layout_desc(desc);
      printf ("Testing... FAIL! exception: %s\n", exception.what ());
    }
  }
}

int main ()
{
  printf ("Results of all_input_states_test:\n");

  try
  {
    Test test (L"OpenGL device test window (all_input_states_test)");
    
    Mode = test.log_mode;
    
    InputLayoutDesc desc;
    BufferDesc index, vertex;
    VertexAttribute attribs[8];
    
    index.size = 1;
    index.usage_mode = UsageMode_Default;
    index.bind_flags = BindFlag_IndexBuffer;
    index.access_flags = AccessFlag_ReadWrite;
    
    vertex.size = 1;
    vertex.usage_mode = UsageMode_Default;
    vertex.bind_flags = BindFlag_VertexBuffer;
    vertex.access_flags = AccessFlag_ReadWrite;
    
    desc.vertex_attributes = attribs;
    desc.vertex_attributes_count = 2;
    
    for (int i = 0; i < desc.vertex_attributes_count; i++)
    {
      attribs[i].slot = 0;
      attribs[i].offset = 0;
      attribs[i].stride = 0;
    }
    
    desc.index_buffer_offset = 0;
    bool next = true;
    
    
    test.device.get()->ISSetVertexBuffer(0, test.device.get()->CreateBuffer(vertex));
    test.device.get()->ISSetIndexBuffer(test.device.get()->CreateBuffer(index));
    
    for (int itype = 0; itype < InputDataType_Num; itype++)
    {
      desc.index_type = (InputDataType) itype;
      
      for (int type1 = 0; type1 < InputDataType_Num; type1++)
      {
        attribs[0].type = (InputDataType) type1;
        
        for (int format1 = 0; format1 < InputDataFormat_Num; format1++)
        {
          attribs[0].format = (InputDataFormat) format1;
          
          for (int semantic1 = 0; semantic1 < VertexAttributeSemantic_Num; semantic1++)
          {
            attribs[0].semantic = (VertexAttributeSemantic) semantic1;
            
            for (int type2 = 0; type2 < InputDataType_Num; type2++)
            {
              attribs[1].type = (InputDataType) type2;
              
              for (int format2 = 0; format2 < InputDataFormat_Num; format2++)
              {
                attribs[1].format = (InputDataFormat) format2;
                
                for (int semantic2 = 0; semantic2 < VertexAttributeSemantic_Num; semantic2++)
                {
                  attribs[1].semantic = (VertexAttributeSemantic) semantic2;
                  
                  test_input_layout_desc(desc, test.device.get());
                }
              }
            }
          }
        }
      }
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
