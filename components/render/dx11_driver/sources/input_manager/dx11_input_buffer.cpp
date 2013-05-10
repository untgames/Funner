#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор / деструктор
*/

InputBuffer::InputBuffer (const DeviceManager& device_manager, const BufferDesc& in_desc, const void* data)
  : DeviceObject (device_manager)
  , desc (in_desc)
{
  try
  {
    D3D11_BUFFER_DESC dx_desc;

    memset (&dx_desc, 0, sizeof (dx_desc));

    dx_desc.ByteWidth = desc.size;

      //преобразование параметров

    switch (desc.usage_mode)
    {
      case UsageMode_Static:   dx_desc.Usage = D3D11_USAGE_IMMUTABLE; break;
      case UsageMode_Default:  dx_desc.Usage = D3D11_USAGE_DEFAULT; break;
      case UsageMode_Dynamic:  dx_desc.Usage = D3D11_USAGE_DYNAMIC; break;
      case UsageMode_Stream:   dx_desc.Usage = D3D11_USAGE_STAGING; break;
      default:
        throw xtl::make_argument_exception ("", "desc.usage_mode", desc.usage_mode);
    }

      //проверка корректности флагов доступа

    switch (desc.access_flags)
    {
      case 0:
      case AccessFlag_Read:
      case AccessFlag_Write:
      case AccessFlag_Read | AccessFlag_Write:
        break;
      default:
        throw xtl::make_argument_exception ("", "desc.access_flags", desc.access_flags);
    }

//    if (desc.access_flags & AccessFlag_Read)  dx_desc.CPUAccessFlags |= D3D11_CPU_ACCESS_READ;
    if (desc.access_flags & AccessFlag_Write) dx_desc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;

    struct FlagMap
    {
      BindFlag src_flag;
      UINT     dst_flag;
    };

    static const FlagMap flags [] = {
      {BindFlag_VertexBuffer,   D3D11_BIND_VERTEX_BUFFER},
      {BindFlag_IndexBuffer,    D3D11_BIND_INDEX_BUFFER},
      {BindFlag_ConstantBuffer, D3D11_BIND_CONSTANT_BUFFER},
      {BindFlag_Texture,        D3D11_BIND_SHADER_RESOURCE},
      {BindFlag_RenderTarget,   D3D11_BIND_RENDER_TARGET},
      {BindFlag_DepthStencil,   D3D11_BIND_DEPTH_STENCIL},
    };

    static size_t flags_count = sizeof (flags) / sizeof (*flags);

    for (size_t i=0; i<flags_count; i++)
      if (desc.bind_flags & flags [i].src_flag)
        dx_desc.BindFlags |= flags [i].dst_flag;    

      //создание буфера

    ID3D11Buffer* dx_buffer = 0;

    D3D11_SUBRESOURCE_DATA dx_data;

    memset (&dx_data, 0, sizeof (dx_data));

    dx_data.pSysMem = data;

    check_errors ("ID3D11Device::CreateBuffer", GetDevice ().CreateBuffer (&dx_desc, data ? &dx_data : (D3D11_SUBRESOURCE_DATA*)0, &dx_buffer));

    if (!dx_buffer)
      throw xtl::format_operation_exception ("", "ID3D11Device::CreateBuffer failed");

    buffer = DxBufferPtr (dx_buffer, false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::InputBuffer::InputBuffer");
    throw;
  }
}

InputBuffer::~InputBuffer ()
{
}

/*
    Получение дескриптора буфера
*/

void InputBuffer::GetDesc (BufferDesc& out_desc)
{
  out_desc = desc;
}

/*
    Чтение / запись из буфера
*/

void InputBuffer::SetData (size_t offset, size_t size, const void* data, IDeviceContext* context)
{
  try
  {
      //проверка возможности установки данных
      
    if (!(desc.access_flags & AccessFlag_Write))
      throw xtl::format_operation_exception ("", "Can't set buffer data (no AccessFlag_Write in desc.access_flags)");

      //отсечение

    if (offset >= desc.size)
      return;

    if (desc.size - offset < size)
      size = desc.size - offset;

    if (!size)
      return;

      //проверка корректности результирующего буфера

    if (!data)
      throw xtl::make_null_argument_exception ("", "data");

      //преобразование контекста

    ID3D11DeviceContext& dx_context = get_dx_context (*this, context);

      //запись

    D3D11_MAPPED_SUBRESOURCE subresource;

    check_errors ("ID3D11DeviceContext::Map", dx_context.Map (buffer.get (), 0, size == desc.size ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE, 0, &subresource));

    if (!subresource.pData)
      throw xtl::format_operation_exception ("", "ID3D11DeviceContext::Map failed");

    memcpy (reinterpret_cast<char*> (subresource.pData) + offset, reinterpret_cast<const char*> (data), size);

    dx_context.Unmap (buffer.get (), 0);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::InputBuffer::SetData");
    throw;
  }
}

void InputBuffer::GetData (size_t offset, size_t size, void* data, IDeviceContext* context)
{
  try
  {
      //проверка возможности установки данных
      
    if (!(desc.access_flags & AccessFlag_Read))
      throw xtl::format_operation_exception ("", "Can't get buffer data (no AccessFlag_Read in desc.access_flags)");

      //отсечение

    if (offset >= desc.size)
      return;

    if (desc.size - offset < size)
      size = desc.size - offset;

    if (!size)
      return;

      //проверка корректности результирующего буфера

    if (!data)
      throw xtl::make_null_argument_exception ("", "data");

      //преобразование контекста

    ID3D11DeviceContext& dx_context = get_dx_context (*this, context);

      //чтение

    D3D11_MAPPED_SUBRESOURCE subresource;

    check_errors ("ID3D11DeviceContext::Map", dx_context.Map (buffer.get (), 0, D3D11_MAP_READ, 0, &subresource));

    if (!subresource.pData)
      throw xtl::format_operation_exception ("", "ID3D11DeviceContext::Map failed");

    memcpy (reinterpret_cast<char*> (data), reinterpret_cast<const char*> (subresource.pData) + offset, size);

    dx_context.Unmap (buffer.get (), 0);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::InputBuffer::GetData");
    throw;
  }
}
