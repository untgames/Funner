#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    �������� ���������� �������� �������
*/

typedef stl::vector<D3D11_INPUT_ELEMENT_DESC> InputElementArray;

struct InputLayout::Impl
{
  InputElementArray vertex_elements;     //��������� ��������
  DXGI_FORMAT       index_format;        //������ ��������
  size_t            index_buffer_offset; //�������� � ��������� ������    
};

/*
    �����������
*/

InputLayout::InputLayout (const InputLayoutDesc& desc)
  : impl (new Impl)
{
  try
  {
      //�������� ��������� ���������

    impl->vertex_elements.reserve (desc.vertex_attributes_count);

      //�������������� ��������� ���������

    if (desc.vertex_attributes_count)
    {
      if (!desc.vertex_attributes)
        throw xtl::make_null_argument_exception ("", "desc.vertex_attributes_count");

      size_t stride = 0;

      for (size_t i=0, count=desc.vertex_attributes_count; i<count; i++)
      {
        const VertexAttribute& src_va = desc.vertex_attributes [i];

        D3D11_INPUT_ELEMENT_DESC dst_va;

        memset (&dst_va, 0, sizeof (dst_va));

          //�������� ������������ ����������

        if (!src_va.semantic)
          throw xtl::format_exception<xtl::null_argument_exception> ("", "Null desc.vertex_attributes[%u].semantic", i);

        dst_va.SemanticName = src_va.semantic;

        if (src_va.slot >= DEVICE_VERTEX_BUFFER_SLOTS_COUNT)
          throw xtl::format_exception<xtl::argument_range_exception> ("", "Argument desc.vertex_attributes[%u].slot=%u is out of range [%u;%u)", i, src_va.slot, 0, DEVICE_VERTEX_BUFFER_SLOTS_COUNT);

        dst_va.InputSlot            = src_va.slot;
        dst_va.AlignedByteOffset    = src_va.offset;
        dst_va.InputSlotClass       = src_va.is_per_instance ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA ;
        dst_va.InstanceDataStepRate = src_va.instance_data_step_rate;

        if (!src_va.is_per_instance && src_va.instance_data_step_rate)
          throw xtl::format_exception<xtl::argument_exception> ("", "Invalid argument desc.vertex_attributes[%u].instance_data_step_rate=%u (for non instanced data)", i, src_va.instance_data_step_rate);

        size_t type_size = 0, elements_count = 0;

        switch (src_va.type)
        {
          case InputDataType_Byte:
          case InputDataType_UByte:
            type_size = sizeof (char);
            break;
          case InputDataType_Short:
          case InputDataType_UShort:
            type_size = sizeof (short);
            break;
          case InputDataType_Int:
          case InputDataType_UInt:
            type_size = sizeof (int);
            break;
          case InputDataType_Float:
            type_size = sizeof (float);
            break;
          default:
            throw xtl::format_exception<xtl::argument_exception> ("", "Invalid argument desc.vertex_attributes[%u].type=%d", i, src_va.type);
        }        

        switch (src_va.format)
        {
          case InputDataFormat_Vector1:
            elements_count = 1;

            switch (src_va.type)
            {
              case InputDataType_Byte:   dst_va.Format = DXGI_FORMAT_R8_SINT;   break;
              case InputDataType_UByte:  dst_va.Format = DXGI_FORMAT_R8_UINT;   break;
              case InputDataType_Short:  dst_va.Format = DXGI_FORMAT_R16_SINT;  break;
              case InputDataType_UShort: dst_va.Format = DXGI_FORMAT_R16_UINT;  break;
              case InputDataType_Int:    dst_va.Format = DXGI_FORMAT_R32_SINT;  break;
              case InputDataType_UInt:   dst_va.Format = DXGI_FORMAT_R32_UINT;  break;
              case InputDataType_Float:  dst_va.Format = DXGI_FORMAT_R32_FLOAT; break;
            }
            break;
          case InputDataFormat_Vector2:
            elements_count = 2;

            switch (src_va.type)
            {
              case InputDataType_Byte:   dst_va.Format = DXGI_FORMAT_R8G8_SINT;    break;
              case InputDataType_UByte:  dst_va.Format = DXGI_FORMAT_R8G8_UINT;    break;
              case InputDataType_Short:  dst_va.Format = DXGI_FORMAT_R16G16_SINT;  break;
              case InputDataType_UShort: dst_va.Format = DXGI_FORMAT_R16G16_UINT;  break;
              case InputDataType_Int:    dst_va.Format = DXGI_FORMAT_R32G32_SINT;  break;
              case InputDataType_UInt:   dst_va.Format = DXGI_FORMAT_R32G32_UINT;  break;
              case InputDataType_Float:  dst_va.Format = DXGI_FORMAT_R32G32_FLOAT; break;
            }
            break;
          case InputDataFormat_Vector3:
            elements_count = 3;

            switch (src_va.type)
            {
              case InputDataType_Int:    dst_va.Format = DXGI_FORMAT_R32G32B32_SINT;  break;
              case InputDataType_UInt:   dst_va.Format = DXGI_FORMAT_R32G32B32_UINT;  break;
              case InputDataType_Float:  dst_va.Format = DXGI_FORMAT_R32G32B32_FLOAT; break;
              case InputDataType_Byte:
              case InputDataType_UByte:
              case InputDataType_Short:
              case InputDataType_UShort:
                throw xtl::format_exception<xtl::not_supported_exception> ("", "Argument desc.vertex_attributes[%u].type=%s is not supported", i, get_name (src_va.type));
            }
            break;
          case InputDataFormat_Vector4:
            elements_count = 4;

            switch (src_va.type)
            {
              case InputDataType_Byte:   dst_va.Format = DXGI_FORMAT_R8G8B8A8_SINT;      break;
              case InputDataType_UByte:  dst_va.Format = DXGI_FORMAT_R8G8B8A8_UINT;      break;
              case InputDataType_Short:  dst_va.Format = DXGI_FORMAT_R16G16B16A16_SINT;  break;
              case InputDataType_UShort: dst_va.Format = DXGI_FORMAT_R16G16B16A16_UINT;  break;
              case InputDataType_Int:    dst_va.Format = DXGI_FORMAT_R32G32B32A32_SINT;  break;
              case InputDataType_UInt:   dst_va.Format = DXGI_FORMAT_R32G32B32A32_UINT;  break;
              case InputDataType_Float:  dst_va.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
            }
            break;
          default:
            throw xtl::format_exception<xtl::argument_exception> ("", "desc.vertex_attributes[%u].format", i, src_va.format);
        }

          //���������� ���� ����� ����������
                 
        size_t attribute_size = src_va.offset + elements_count * type_size;

        if (attribute_size > stride)
          stride = attribute_size;

          //���������� ��������

        impl->vertex_elements.push_back (dst_va);        
      }

        //TODO: dummy stride
      
      for (size_t i=0, count=desc.vertex_attributes_count; i<count; i++)
      {
        const VertexAttribute& src_va = desc.vertex_attributes [i];

        if (src_va.stride != stride)
          throw xtl::format_exception<xtl::not_supported_exception> ("", "Stride for desc.vertex_attributes[%u].stride=%u is not supported (not equal to total stride %u)", i, src_va.stride, stride);
      }
    }

      //�������������� ��������� ���������

    impl->index_buffer_offset = desc.index_buffer_offset;

    switch (desc.index_type)
    {
      case InputDataType_Byte:    impl->index_format = DXGI_FORMAT_R8_SINT;   break;
      case InputDataType_UByte:   impl->index_format = DXGI_FORMAT_R8_UINT;   break;
      case InputDataType_Short:   impl->index_format = DXGI_FORMAT_R16_SINT;  break;
      case InputDataType_UShort:  impl->index_format = DXGI_FORMAT_R16_UINT;  break;
      case InputDataType_Int:     impl->index_format = DXGI_FORMAT_R32_SINT;  break;
      case InputDataType_UInt:    impl->index_format = DXGI_FORMAT_R32_UINT;  break;
      case InputDataType_Float:   impl->index_format = DXGI_FORMAT_R32_FLOAT; break;
      default:                    throw xtl::make_argument_exception ("", "desc.index_type", desc.index_type);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::InputLayout::InputLayout");
    throw;
  }
}

InputLayout::~InputLayout ()
{
}

/*
    ��������� ���������� �������
*/

const D3D11_INPUT_ELEMENT_DESC* InputLayout::GetVertexElements () const
{
  return impl->vertex_elements.empty () ? (D3D11_INPUT_ELEMENT_DESC*)0 : &impl->vertex_elements [0];
}

size_t InputLayout::GetVertexElementsCount () const
{
  return impl->vertex_elements.size ();
}

/*
    ��������� ���������� �������
*/

DXGI_FORMAT InputLayout::GetIndexFormat () const
{
  return impl->index_format;
}

size_t InputLayout::GetIndexBufferOffset () const
{
  return impl->index_buffer_offset;
}
