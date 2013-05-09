#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор / деструктор
*/

Query::Query (const DeviceManager& manager, QueryType in_type)
  : DeviceObject (manager)
  , type (in_type)
{
  try
  {
    D3D11_QUERY_DESC desc;

    memset (&desc, 0, sizeof (desc));

    switch (type)
    {
      case QueryType_Event:
        desc.Query = D3D11_QUERY_EVENT;
        break;
      case QueryType_Occlusion:
        desc.Query = D3D11_QUERY_OCCLUSION;
        break;
      case QueryType_OcclusionPredicate:
        desc.Query = D3D11_QUERY_OCCLUSION_PREDICATE;
        break;
      case QueryType_PipelineStatistics:
        desc.Query = D3D11_QUERY_PIPELINE_STATISTICS;
        break;
      case QueryType_StreamOutput0Statistics:
        desc.Query = D3D11_QUERY_SO_STATISTICS_STREAM0;
        break;
      case QueryType_StreamOutput1Statistics:
        desc.Query = D3D11_QUERY_SO_STATISTICS_STREAM1;
        break;
      case QueryType_StreamOutput2Statistics:
        desc.Query = D3D11_QUERY_SO_STATISTICS_STREAM2;
        break;
      case QueryType_StreamOutput3Statistics:
        desc.Query = D3D11_QUERY_SO_STATISTICS_STREAM3;
        break;
      default:
        throw xtl::make_argument_exception ("", "type", type);
    }
       
    switch (desc.Query)
    {
      case D3D11_QUERY_OCCLUSION_PREDICATE:
      case D3D11_QUERY_SO_OVERFLOW_PREDICATE:
      {
        ID3D11Predicate* dx_predicate = 0;

        desc.MiscFlags |= D3D11_QUERY_MISC_PREDICATEHINT;

        check_errors ("ID3D11Device::CreatePredicate", manager.GetDevice ().CreatePredicate (&desc, &dx_predicate));

        if (!dx_predicate)
          throw xtl::format_operation_exception ("", "ID3D11Device::CreatePredicate failed");

        query = DxQueryPtr (dx_predicate, false);
 
        break; 
      }
      default:
      {
        ID3D11Query* dx_query = 0;

        check_errors ("ID3D11Device::CreateQuery", manager.GetDevice ().CreateQuery (&desc, &dx_query));

        if (!dx_query)
          throw xtl::format_operation_exception ("", "ID3D11Device::CreateQuery failed");

        query = DxQueryPtr (dx_query, false);
 
        break; 
      }
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Query::Query");
    throw;
  }
}

Query::~Query ()
{
}

/*
    Получение данных
*/

void Query::GetData (size_t size, void* data)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Query::GetData");
    throw;
  }
}

bool Query::TryGetData (size_t size, void* data)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Query::TryGetData");
    throw;
  }
}
