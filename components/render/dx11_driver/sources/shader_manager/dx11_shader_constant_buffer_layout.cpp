#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор / деструктор
*/

ConstantBufferLayout::ConstantBufferLayout (ID3D11ShaderReflectionConstantBuffer& buffer)
{
  try
  {
      //получение описания буфера

    D3D11_SHADER_BUFFER_DESC desc;

    check_errors ("ID3D11ShaderReflectionConstantBuffer::GetDesc", buffer.GetDesc (&desc));

    type        = desc.Type;
    buffer_size = desc.Size;

      //получение списка параметров

    strings.Reserve (desc.Variables);
    params.reserve (desc.Variables);

    for (size_t i=0; i<desc.Variables; i++)
    {
        //получение информации о переменной

      ID3D11ShaderReflectionVariable* var = buffer.GetVariableByIndex (i);

      if (!var)
        throw xtl::format_operation_exception ("", "ID3D11ShaderReflectionConstantBuffer::GetVariableByIndex failed");

      D3D11_SHADER_VARIABLE_DESC desc;

      check_errors ("ID3D11ShaderReflectionVariable::GetDesc", var->GetDesc (&desc));

      ID3D11ShaderReflectionType* type = var->GetType ();

      if (!type)
        throw xtl::format_operation_exception ("", "ID3D11ShaderReflectionVariable::GetType failed");

      D3D11_SHADER_TYPE_DESC type_desc;

      check_errors ("ID3D11ShaderReflectionType::GetDesc", type->GetDesc (&type_desc));

        //преобразование переменной в параметр шейдера      

      ShaderParameter param;

      memset (&param, 0, sizeof (param));

      strings.Add (desc.Name);

      switch (type_desc.Type)
      {
        case D3D10_SVT_INT:
        case D3D10_SVT_FLOAT:
          break;
        default:
          throw xtl::format_not_supported_exception ("", "Type %u is not supported for variable '%s'", type_desc.Type, desc.Name);
      }

      switch (type_desc.Class)
      {
        case D3D10_SVC_SCALAR:
          switch (type_desc.Type)
          {
            case D3D10_SVT_INT:   param.type = ProgramParameterType_Int;   break;
            case D3D10_SVT_FLOAT: param.type = ProgramParameterType_Float; break;
            default:              break;
          }

          break;
        case D3D10_SVC_VECTOR:
          switch (type_desc.Columns)
          {
            case 2:
              switch (type_desc.Type)
              {
                case D3D10_SVT_INT:   param.type = ProgramParameterType_Int2;   break;
                case D3D10_SVT_FLOAT: param.type = ProgramParameterType_Float2; break;
                default:              break;
              }
              break;
            case 3:
              switch (type_desc.Type)
              {
                case D3D10_SVT_INT:   param.type = ProgramParameterType_Int3;   break;
                case D3D10_SVT_FLOAT: param.type = ProgramParameterType_Float3; break;
                default:              break;
              }
              break;
            case 4:
              switch (type_desc.Type)
              {
                case D3D10_SVT_INT:   param.type = ProgramParameterType_Int4;   break;
                case D3D10_SVT_FLOAT: param.type = ProgramParameterType_Float4; break;
                default:              break;
              }
              break;
            default:
              throw xtl::format_not_supported_exception ("", "Vector rank %u is not supported for shader variable '%s'", type_desc.Columns, desc.Name);
          }

          break;
        case D3D10_SVC_MATRIX_ROWS:
          if (type_desc.Columns != type_desc.Rows)
            throw xtl::format_not_supported_exception ("", "Non square matrices %ux%u are not supported for shader variable '%s'", type_desc.Rows, type_desc.Columns, desc.Name);

          switch (type_desc.Type)
          {
            case D3D10_SVT_FLOAT:
              break;
            default:
              throw xtl::format_not_supported_exception ("", "Type %u is not supported for matrix variable '%s'", type_desc.Type, desc.Name);
          }

          switch (type_desc.Columns)
          {
            case 2:  param.type = ProgramParameterType_Float2x2; break;
            case 3:  param.type = ProgramParameterType_Float3x3; break;
            case 4:  param.type = ProgramParameterType_Float4x4; break;
            default: throw xtl::format_not_supported_exception ("", "Matrix rank %u is not supported for shader variable '%s'", type_desc.Columns, desc.Name);
          }

          break;
        default:
          throw xtl::format_not_supported_exception ("", "Class %u is not supported for shader variable '%s'", type_desc.Class, desc.Name);
      }

      param.count  = type_desc.Elements;
      param.offset = desc.StartOffset;

        //добавление параметра

      params.push_back (param);
    }

      //обновление имен

    const char** name = strings.Data ();

    for (ShaderParameterArray::iterator iter=params.begin (), end=params.end (); iter!=end; ++iter, ++name)
      iter->name = *name;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ConstantBufferLayout::ConstantBufferLayout");
    throw;
  }
}

ConstantBufferLayout::~ConstantBufferLayout ()
{
}
