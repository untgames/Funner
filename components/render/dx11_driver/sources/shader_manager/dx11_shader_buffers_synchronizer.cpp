#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    ѕредикат сравнени€ двух блоков
*/

inline bool ShaderBuffersSynchronizer::Chunk::operator < (const Chunk& another) const
{
  if (slot != another.slot)
    return slot < another.slot;

  return src_offset < another.src_offset;
}

namespace
{

typedef stl::pair<size_t, size_t> Span;
typedef stl::deque<Span>          SpanQueue;

Span merge (const Span& s1, const Span& s2)
{
  if (s1.first > s2.first)
    return merge (s2, s1);

  if (s1.second < s2.first)
    return Span (0, 0);

  //pre-cond: s1.first <= s2.first, s1.second >= s2.first

  size_t finish = s1.second > s2.second ? s1.second : s2.second;

  return Span (s1.first, finish);
}

void merge (const ShaderBuffersSynchronizer::Chunk* first_chunk, size_t count, SpanQueue& out_spans)
{
    //build spans

  SpanQueue spans;

  for (size_t i=0; i<count; i++)
  {
    const ShaderBuffersSynchronizer::Chunk& chunk = first_chunk [i];

    Span span (chunk.dst_offset, chunk.dst_offset + chunk.size);
    bool merged = false;

    for (SpanQueue::iterator iter=spans.begin (), end=spans.end (); iter!=end; ++iter)
    {
      if (iter->second < span.first)
        continue;

      Span merged_span = merge (span, *iter);

      if (merged_span.first == merged_span.second) spans.insert (iter, span);
      else                                         *iter = merged_span;

      merged = true;

      break;
    }

    if (!merged)
      spans.push_back (span);
  }

    //merge spans

  Span span (0, 0);

  out_spans.clear ();

  for (SpanQueue::iterator iter=spans.begin (), end=spans.end (); iter!=end; ++iter)
  {
    if (span.first == span.second)
    {
      span = *iter;
      continue;
    }

    Span merged_span = merge (span, *iter);

    if (merged_span.first == merged_span.second)
    {
      out_spans.push_back (span);

      span = *iter;

      continue;
    }
  }

  if (span.first != span.second)
    out_spans.push_back (span);
}

void merge (const ShaderBuffersSynchronizer::Chunk* first_chunk, size_t count, stl::vector<ShaderBuffersSynchronizer::Chunk>& chunks, SpanQueue& tmp_spans)
{
  merge (first_chunk, count, tmp_spans);

  chunks.reserve (chunks.size () + tmp_spans.size ());

  size_t offset = 0;

  for (SpanQueue::iterator iter=tmp_spans.begin (), end=tmp_spans.end (); iter!=end; ++iter)
  {
    ShaderBuffersSynchronizer::Chunk chunk;

    chunk.slot        = first_chunk->slot;
    chunk.src_offset  = first_chunk->src_offset + offset;
    chunk.dst_offset  = iter->first;
    chunk.size        = iter->second - iter->first;
    offset           += chunk.size;

    chunks.push_back (chunk);
  }
}

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

    ChunkArray tmp_chunks;

    tmp_chunks.reserve (dst_layout.GetParamsCount ());

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

      tmp_chunks.push_back (chunk);

        //обновление параметров слота

      Slot& slot = slots [src_param->slot];

      if (chunk.src_offset + param_size > slot.min_src_buffer_size)
        slot.min_src_buffer_size = chunk.src_offset + param_size;

      if (dst_param->offset + param_size > min_dst_buffer_size)
        min_dst_buffer_size = dst_param->offset + param_size;
    }

      //упор€дочивание блоков по источнику

    stl::sort (tmp_chunks.begin (), tmp_chunks.end ());

      //оптимизаци€ блоков

    SpanQueue tmp_spans;
    
    Chunk* first_chunk = 0;

    for (ChunkArray::iterator iter=tmp_chunks.begin (), end=tmp_chunks.end (); iter!=end; ++iter)
    {
      if (!first_chunk || iter [-1].slot != iter->slot || iter [-1].src_offset + iter [-1].size != iter->src_offset)
      {
        if (first_chunk)
          merge (first_chunk, iter - first_chunk, chunks, tmp_spans);

        first_chunk = &*iter;
      }      
    }

    if (first_chunk)
      merge (first_chunk, tmp_chunks.end () - first_chunk, chunks, tmp_spans);

      //обновление слотов

    size_t current_slot = DEVICE_CONSTANT_BUFFER_SLOTS_COUNT;

    for (ChunkArray::const_iterator iter=chunks.begin (), end=chunks.end (); iter!=end; ++iter)
      if (current_slot != iter->slot)
      {
        slots [iter->slot].first_chunk = iter - chunks.begin ();
        current_slot                   = iter->slot;
      }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderBuffersSynchronizer::ShaderBuffersSynchronizer");
    throw;
  }
}
