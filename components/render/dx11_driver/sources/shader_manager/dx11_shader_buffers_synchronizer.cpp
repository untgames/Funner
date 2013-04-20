#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    ѕредикат сравнени€ двух блоков
*/

inline bool ShaderBuffersSynchronizer::Chunk::operator < (const Chunk& another) const
{
  return dst_offset < another.dst_offset;
}

/*
     онструктор
*/

ShaderBuffersSynchronizer::ShaderBuffersSynchronizer (const ProgramParametersLayout& src_layout, const ConstantBufferLayout& dst_layout)
  : min_dst_buffer_size ()
{
  try
  {
      //резервирование количества блоков

    chunks.reserve (dst_layout.GetParamsCount ());

      //обход параметров

    const ShaderParameter* dst_param = dst_layout.GetParams ();

    for (size_t i=0, count=dst_layout.GetParamsCount (); i<count; i++, dst_param++)
    {
        //поиск параметра в исходном лэйауте

      const ProgramParameter* src_param = src_layout.FindParameter (dst_param->name);

      if (!src_param)
        throw xtl::format_operation_exception ("", "Can't find required shader parameter '%s' in program parameters layout", dst_param->name);

        //проверка возможности использовани€ параметра

      if (src_param->type != dst_param->type)
         throw xtl::format_operation_exception ("", "Shader parameter '%s' can't be used from program parameters layout. Types mistmatch: required %s but found %s",
           dst_param->name, get_name (dst_param->type), get_name (src_param->type));

      size_t param_size = 0;

      switch (dst_param->type)
      {
        case ProgramParameterType_Int:       param_size = sizeof (int);        break;
        case ProgramParameterType_Float:     param_size = sizeof (float);      break;
        case ProgramParameterType_Int2:      param_size = 2 * sizeof (int);    break;
        case ProgramParameterType_Float2:    param_size = 2 * sizeof (float);  break;
        case ProgramParameterType_Int3:      param_size = 3 * sizeof (int);    break;
        case ProgramParameterType_Float3:    param_size = 3 * sizeof (float);  break;
        case ProgramParameterType_Int4:      param_size = 4 * sizeof (int);    break;
        case ProgramParameterType_Float4:    param_size = 4 * sizeof (float);  break;
        case ProgramParameterType_Float2x2:  param_size = 4 * sizeof (float);  break;
        case ProgramParameterType_Float3x3:  param_size = 9 * sizeof (float);  break;
        case ProgramParameterType_Float4x4:  param_size = 16 * sizeof (float); break;
        default:                             throw xtl::format_operation_exception ("", "Unknown program parameter type '%s'", get_name (dst_param->type));
      }

      if (dst_param->count > src_param->count)
        throw xtl::format_operation_exception ("", "Shader parameter '%s' can't be used from program parameters layout. Required rank %u is larger than layout rank %u",
          dst_param->name, dst_param->count, src_param->count);

      param_size *= dst_param->count;

      if (src_param->slot >= DEVICE_CONSTANT_BUFFER_SLOTS_COUNT)
        throw xtl::format_operation_exception ("", "Shader parameter '%s' has wrong constant buffer slot index %u", dst_param->name, src_param->slot);

        //создание блока
      
      Chunk chunk;

      chunk.slot       = src_param->slot;
      chunk.src_offset = src_param->offset;
      chunk.dst_offset = dst_param->offset;
      chunk.size       = param_size;

      chunks.push_back (chunk);

        //обновление параметров слота

      Slot& slot = slots [src_param->slot];

      if (chunk.src_offset + param_size > slot.min_src_buffer_size)
        slot.min_src_buffer_size = chunk.src_offset + param_size;

      if (dst_param->offset + param_size > min_dst_buffer_size)
        min_dst_buffer_size = dst_param->offset + param_size;
    }

      //упор€дочивание блоков

    stl::sort (chunks.begin (), chunks.end ());

      //оптимизаци€ блоков

 /*   Chunk* first_chunk = 0;

    for (ChunkArray::iterator iter=chunks.begin (), end=chunks.end (); iter!=end; ++iter)
    {
      if (!first_chunk || first_chunk->slot != iter->slot || first_chunk->src_offset + first_chunk->size != iter->src_offset) //????
      {
        first_chunk = &*iter;
        continue;
      }

      if (first_chunk->slot != iter->slot)
      {
        first_chunk 
      }
    }*/

    //выделить Span
    //убрать slot из Chunk


      //обновление слотов

    size_t current_slot = DEVICE_CONSTANT_BUFFER_SLOTS_COUNT;

    for (ChunkArray::const_iterator iter=chunks.begin (), end=chunks.end (); iter!=end; ++iter)
      if (current_slot != iter->slot)
      {
        slots [iter->slot].first_chunk = iter - chunks.begin ();
        current_slot                   = iter->slot;
      }    

      //усечение "на месте"

    ChunkArray (chunks).swap (chunks);

    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderBuffersSynchronizer::ShaderBuffersSynchronizer");
    throw;
  }
}
