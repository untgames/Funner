#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    TargetConstantBufferPrototype
*/

/*
    Конструктор / деструктор
*/

TargetConstantBufferPrototype::TargetConstantBufferPrototype (ShaderLibrary& library, const ProgramParametersLayout& src_layout, const ProgramBufferLayout& in_dst_layout)
  : synchronizer (library.GetSynchronizer (src_layout, in_dst_layout.GetLayout ()))
  , dst_layout (in_dst_layout)
{
  slots_count = dst_layout.GetSlotsCount ();
  slots       = dst_layout.GetSlots ();

  indices.reserve (DEVICE_CONSTANT_BUFFER_SLOTS_COUNT);

  const ShaderBuffersSynchronizer::Chunk* chunk = synchronizer.GetChunks ();

  size_t current_slot = DEVICE_CONSTANT_BUFFER_SLOTS_COUNT;

  for (size_t i=0, count=synchronizer.GetChunksCount (); i<count; i++, chunk++)
  {
    if (chunk->slot == current_slot)
      continue;

    indices.push_back (chunk->slot);

    current_slot = chunk->slot;
  }
}

TargetConstantBufferPrototype::~TargetConstantBufferPrototype ()
{
}

/*
    Биндинг
*/

inline void TargetConstantBufferPrototype::Bind (ID3D11Buffer& buffer, ID3D11Buffer* context_buffers [ShaderType_Num][DEVICE_CONSTANT_BUFFER_SLOTS_COUNT]) const
{
  const Slot* slot = slots;

  for (size_t i=0; i<slots_count; i++, slot++)
    context_buffers [slot->type][slot->slot] = &buffer;
}

/*
    Создание буфера
*/

TargetConstantBufferPtr TargetConstantBufferPrototype::CreateBuffer (const DeviceManager& device_manager, const SourceConstantBufferPtr buffers [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT]) const
{
  try
  {
    return TargetConstantBufferPtr (new TargetConstantBuffer (device_manager, *this, buffers), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::TargetConstantBufferPrototype::CreateBuffer");
    throw;
  }
}

/*
    Получение буфера
*/

TargetConstantBuffer& TargetConstantBufferPrototype::GetBuffer (const SourceConstantBufferPtr buffers [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT], ShaderLibrary& library) const
{
  try
  {
      //формирование хэша

    SourceConstantBuffer* buffers_hash_array [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT];

    memset (buffers_hash_array, 0, sizeof (buffers_hash_array));

    for (IndexArray::const_iterator iter=indices.begin (), end=indices.end (); iter!=end; ++iter)
    {
      if (!buffers [*iter])
        throw xtl::format_operation_exception ("", "Null constant buffer #%u", *iter);

      buffers_hash_array [*iter] = buffers [*iter].get ();
    }

    const TargetConstantBufferPrototype* prototype = this;

    size_t hash = common::crc32 (buffers_hash_array, sizeof (buffers_hash_array), common::crc32 (&prototype, 0, sizeof (prototype)));

      //поиск константного буфера

    TargetConstantBufferPtr buffer = library.FindConstantBuffer (hash);

    if (buffer)
      return *buffer;

      //создание константного буфера и его регистрация

    buffer = CreateBuffer (library.GetDeviceManager (), buffers);

    library.AddConstantBuffer (hash, buffer);

    try
    {
      xtl::trackable::function_type tracker = xtl::bind (&ShaderLibrary::RemoveConstantBuffer, &library, hash);

      for (IndexArray::const_iterator iter=indices.begin (), end=indices.end (); iter!=end; ++iter)
      {
        SourceConstantBufferPtr src_buffer = buffers [*iter];

        src_buffer->RegisterDestroyHandler (tracker, library.GetTrackable ());
      }

      const_cast<TargetConstantBufferPrototype&> (*this).connect_tracker (tracker, library.GetTrackable ());
    }
    catch (...)
    {
      library.RemoveConstantBuffer (hash);
      throw;
    }

    return *buffer;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::TargetConstantBufferPrototype::GetBuffer");
    throw;
  }  
}

/*
    TargetConstantBuffer
*/

/*
    Конструктор / деструктор
*/

TargetConstantBuffer::TargetConstantBuffer (const DeviceManager& device_manager, const TargetConstantBufferPrototype& in_prototype, const SourceConstantBufferPtr buffers [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT])
  : DeviceObject (device_manager)
  , prototype (in_prototype)
{
  try
  {
      //сохранение исходных буферов

    src_buffers.reserve (prototype.GetSourceBuffersCount ());

    const unsigned char* indices = prototype.GetSourceBuffersIndices ();

    UsageMode usage = UsageMode_Default;

    for (size_t i=0, count=prototype.GetSourceBuffersCount (); i<count; i++)
    {
      size_t slot = indices [i];

      SourceConstantBufferPtr src_buffer = buffers [slot];

      if (!src_buffer)
        throw xtl::format_exception<xtl::null_argument_exception> ("", "Null constant buffer #%u", slot);

      if (src_buffer->GetDataSize () < prototype.GetMinSourceBufferSize (i))
        throw xtl::format_operation_exception ("", "Source constant buffer size %u is less than required by program parameters layout size %u", src_buffer->GetDataSize (),
          prototype.GetMinSourceBufferSize (i));

      if (src_buffer->GetUsageMode () > usage)
        usage = src_buffer->GetUsageMode ();

      src_buffers.push_back (SourceBufferDesc (src_buffer.get ()));
    }

      //создание целевого буфера

    D3D11_BUFFER_DESC desc;

    memset (&desc, 0, sizeof (desc));

    desc.ByteWidth = prototype.GetMinDestinationBufferSize ();

    switch (usage)
    {
      default:
      case UsageMode_Static:   //desc.Usage = D3D11_USAGE_IMMUTABLE; break;
      case UsageMode_Default:  //desc.Usage = D3D11_USAGE_DEFAULT; break;
      case UsageMode_Dynamic:  desc.Usage = D3D11_USAGE_DYNAMIC; break;
      case UsageMode_Stream:   desc.Usage = D3D11_USAGE_STAGING; break;
    }

    desc.BindFlags       = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;

    ID3D11Buffer* dx_buffer = 0;

    check_errors ("ID3D11Device::CreateBuffer", GetDevice ().CreateBuffer (&desc, 0, &dx_buffer));

    if (!dx_buffer)
      throw xtl::format_operation_exception ("", "ID3D11Device::CreateBuffer failed");

    dst_buffer = DxBufferPtr (dx_buffer, false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::TargetConstantBuffer::TargetConstantBuffer");
    throw;
  }
}

TargetConstantBuffer::~TargetConstantBuffer ()
{
}

/*
    Биндинг
*/

void TargetConstantBuffer::Bind (ID3D11DeviceContext& context, ID3D11Buffer* context_buffers [ShaderType_Num][DEVICE_CONSTANT_BUFFER_SLOTS_COUNT])
{
  try
  {
      //синхронизация

    D3D11_MAPPED_SUBRESOURCE subresource;

    void* dst_buffer_data = 0;

    for (BufferDescArray::iterator iter=src_buffers.begin (), end=src_buffers.end (); iter!=end; ++iter)
    {
      SourceBufferDesc& desc = *iter;

      size_t actual_hash = desc.buffer->GetHash ();

      if (actual_hash == desc.hash)
        continue;

      if (!dst_buffer_data)
      {
        check_errors ("ID3D11DeviceContext::Map", context.Map (dst_buffer.get (), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource));        

        dst_buffer_data = subresource.pData;

        if (!dst_buffer_data)
          throw xtl::format_operation_exception ("", "ID3D11DeviceContext::Map failed");
      }

      prototype.CopyTo (iter - src_buffers.begin (), (const char*)desc.buffer->GetDataPointer (), (char*)dst_buffer_data);

      desc.hash = actual_hash;
    }

    if (dst_buffer_data)
      context.Unmap (dst_buffer.get (), 0);

      //биндинг

    prototype.Bind (*dst_buffer, context_buffers);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::TargetConstantBuffer::TargetConstantBuffer");
    throw;
  }
}
