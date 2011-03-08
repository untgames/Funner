#include "shared.h"

using namespace render;
using namespace render::low_level;

/*
    Описание реализации менеджера параметров программ шэйдинга
*/

typedef stl::hash_map<size_t, LowLevelProgramParametersLayoutPtr> LayoutMap;
typedef stl::hash_map<size_t, LowLevelBufferPtr>                  BufferMap;

struct ProgramParametersManager::Impl: public xtl::trackable
{
  LowLevelDevicePtr device;  //устройство визуализации
  LayoutMap         layouts; //лэйауты параметров различной конфигурации
  BufferMap         buffers; //константные буферы
  
///Конструктор
  Impl (const LowLevelDevicePtr& in_device) : device (in_device) {}
  
///Деструктор
  ~Impl ()
  {
      //TODO: проверить рекурсивный вызов RemoveLayout/RemoveConstantBuffer
    
    layouts.clear ();
    buffers.clear ();
  }
  
///Получение лэйаута
  LowLevelProgramParametersLayoutPtr GetLayout (const common::PropertyMap* properties [ProgramParametersSlot_Num])
  {
    try
    {
        //расчёт хэша конфигурации параметров
      
      size_t hash = 0xffffffff;
      
      for (size_t i=0; i<ProgramParametersSlot_Num; i++)
      {
        if (!properties [i])
          throw xtl::make_null_argument_exception ("", common::format ("parameters[%u]", i).c_str ());
          
        size_t parameters_hash = properties [i]->LayoutHash ();
          
        hash = common::crc32 (&parameters_hash, sizeof (parameters_hash), hash);
      }
      
        //поиск существующего лэйаута
        
      LayoutMap::iterator iter = layouts.find (hash);
      
      if (iter != layouts.end ())
        return iter->second;
        
        //резервирование числа параметров
      
      size_t parameters_count = 0;
      
      for (size_t i=0; i<ProgramParametersSlot_Num; i++)
        parameters_count += properties [i]->Size ();

      stl::vector<ProgramParameter> parameters;
      
      parameters.reserve (parameters_count);
      
        //преобразование параметров
        
      for (size_t i=0; i<ProgramParametersSlot_Num; i++)
      {
        const common::PropertyLayout& layout   = properties [i]->Layout ();
        const common::PropertyDesc*   property = layout.Properties ();
        
        for (size_t j=0, count=layout.Size (); j<count; j++, property++)
        {
          ProgramParameter parameter;
          
          memset (&parameter, 0, sizeof (parameter));
          
          parameter.name   = property->name;          
          parameter.slot   = i;
          parameter.count  = property->elements_count;
          parameter.offset = property->offset;
          
          switch (property->type)
          {            
            case common::PropertyType_String:
              continue; //строки не поддерживаются рендером
            case common::PropertyType_Integer:
              parameter.type = ProgramParameterType_Int;
              break;
            case common::PropertyType_Float:
              parameter.type = ProgramParameterType_Float;
              break;            
            case common::PropertyType_Vector:
              parameter.type = ProgramParameterType_Float4;
              break;            
            case common::PropertyType_Matrix:
              parameter.type = ProgramParameterType_Float4x4;
              break;            
            default:
              throw xtl::format_operation_exception ("", "Unexpected property[%u] type %s at parsing", j, get_name (property->type));
          }
          
          parameters.push_back (parameter);
        }
      }
      
        //создание нового лэйаута
        
      ProgramParametersLayoutDesc desc;
      
      memset (&desc, 0, sizeof (desc));

      desc.parameters_count = parameters.size ();
      desc.parameters       = parameters.empty () ? (ProgramParameter*)0 : &parameters [0];
      
      LowLevelProgramParametersLayoutPtr layout (device->CreateProgramParametersLayout (desc), false);      
      
        //регистрация обработчиков события удаления лэйаута / параметров
        
      xtl::trackable::function_type remover = xtl::bind (&Impl::RemoveLayout, this, hash);
        
      layout->RegisterDestroyHandler (remover, *this);
      
      for (size_t i=0; i<ProgramParametersSlot_Num; i++)
      {
        const common::PropertyLayout& layout = properties [i]->Layout ();
        
        layout.Trackable ().connect_tracker (remover, *this);
      }
      
        //регистрация лэйаута
      
      layouts.insert_pair (hash, layout);
      
      return layout;      
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::ProgramParametersManager::Impl::GetLayout");
      throw;
    }
  }
  
///Удаление лэйаута
  void RemoveLayout (size_t hash)
  {
    layouts.erase (hash);
  }
  
///Получение константного буфера
  LowLevelBufferPtr GetConstantBuffer (const common::PropertyMap& properties)
  {
    try
    {
        //поиск существующего буфера
      
      BufferMap::iterator iter = buffers.find (properties.Id ());
      
      if (iter != buffers.end ())
        return iter->second;
        
        //создание нового буфера
        
      BufferDesc desc;
      
      memset (&desc, 0, sizeof (desc));
      
      desc.size         = properties.Layout ().BufferSize ();
      desc.usage_mode   = UsageMode_Default;
      desc.bind_flags   = BindFlag_ConstantBuffer;
      desc.access_flags = AccessFlag_ReadWrite;
      
      LowLevelBufferPtr buffer (device->CreateBuffer (desc), false);
      
        //регистрация обработчика удаления
        
      buffer->RegisterDestroyHandler (xtl::bind (&Impl::RemoveConstantBuffer, this, properties.Id ()), *this);
      
        //регистрация буфера

      buffers.insert_pair (properties.Id (), buffer);
      
      return buffer;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::ProgramParametersManager::Impl::GetConstantBuffer");
      throw;
    }  
  }
  
///Удаление константного буфера
  void RemoveConstantBuffer (size_t id)
  {
    buffers.erase (id);
  }
};

/*
    Конструктор / деструктор
*/

ProgramParametersManager::ProgramParametersManager (const LowLevelDevicePtr& device)
  : impl (new Impl (device))
{
}

ProgramParametersManager::~ProgramParametersManager ()
{
}

/*
    Преобразование параметров
*/

void ProgramParametersManager::Convert
 (const common::PropertyMap*          in_parameters [ProgramParametersSlot_Num],
  LowLevelProgramParametersLayoutPtr& out_layout,
  LowLevelBufferPtr                   out_constant_buffers [ProgramParametersSlot_Num])
{
  try
  {
      //получение лэйаута
     
    out_layout = impl->GetLayout (in_parameters);
    
      //получение константных буферов
    
    for (size_t i=0; i<ProgramParametersSlot_Num; i++)
      out_constant_buffers [i] = impl->GetConstantBuffer (*in_parameters [i]);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::ProgramParametersManager::Convert");
    throw;
  }
}
