#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Константы
*/

const size_t RESERVED_MACRO_SIZE = 16;

/*
    Конструктор / деструктор
*/

namespace
{

typedef stl::vector<D3D10_SHADER_MACRO> MacroArray;

/// Список макросов
struct ShaderMacroMapper
{
  common::StringArray strings;
  MacroArray          macroses;

  ShaderMacroMapper ()
  {
    strings.Reserve (RESERVED_MACRO_SIZE * 2);
  }

  void operator () (const char* name, const char* value)
  {
    size_t index = strings.Size ();

    strings.Add (name);
    strings.Add (value);
  }

  D3D10_SHADER_MACRO* Build ()
  {
    macroses.reserve (strings.Size () / 2 + 1); 

    if (strings.Size () >= 2)
    {
      for (size_t i=0, count=strings.Size (); i<count-1; i++)
      {
        size_t index = i * 2;

        D3D10_SHADER_MACRO macro;

        macro.Name       = strings [index];
        macro.Definition = strings [index + 1];

        macroses.push_back (macro);
      }
    }

    D3D10_SHADER_MACRO macro;    

    memset (&macro, 0, sizeof (macro));

    macroses.push_back (macro);

    return &macroses [0];
  }
};

/// Интерфейс подключаемых модулей
class IncludeManager: public ID3D10Include
{
  public:
    HRESULT __stdcall Open (D3D_INCLUDE_TYPE include_type, LPCSTR file_name, LPCVOID parent_data, LPCVOID* data, UINT* data_size)
    {
      try
      {
        if (!file_name)
          throw xtl::make_null_argument_exception ("", "file_name");

        if (!data)
          throw xtl::make_null_argument_exception ("", "data");

        if (!data_size)
          throw xtl::make_null_argument_exception ("", "data_size");

        switch (include_type)
        {
          case D3D_INCLUDE_LOCAL: // #include "FILE"
          case D3D_INCLUDE_SYSTEM: // #include <FILE>
            break;
          default:
            throw xtl::make_argument_exception ("", "include_type", include_type);
        }

        common::InputFile file (file_name, common::FileMode_ReadOnly);

        size_t file_size = file.Size ();

        char* buffer = new char [file_size + 1];

        size_t read_size = file.Read (buffer, file_size);

        buffer [file_size] = 0;

        *data      = buffer;
        *data_size = file_size;

        return S_OK;
      }
      catch (std::exception& e)
      {
        Log ().Printf ("%s\n    at render::low_level::dx11::IncludeManager::Open", e.what ());
      }
      catch (...)
      {
        Log ().Printf ("unknown exception\n    at render::low_level::dx11::IncludeManager::Open");
      }

      return E_FAIL;
    }
    
    HRESULT __stdcall Close (LPCVOID data)
    {
      delete [] (char*)data;

      return S_OK;
    }
};

}

ShaderCode::ShaderCode 
 (const DeviceManager& device_manager,
  const char*          name, 
  const char*          profile,
  const char*          source_code,
  size_t               source_code_length,
  const char*          options,
  const LogFunction&   log_fn,
  size_t               flags1,
  size_t               flags2)
   : DeviceObject (device_manager)
{
  try
  {
      //проверка корректности аргументов

    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    if (!profile)
      throw xtl::make_null_argument_exception ("", "profile");

    if (!source_code)
      throw xtl::make_null_argument_exception ("", "source_code");

    if (source_code_length == (size_t)-1)
      source_code_length = strlen (source_code);

      //построение списка макросов

    if (!options)
      options = "";      

    ShaderMacroMapper macro_mapper;

    common::parse_init_string (options, xtl::ref (macro_mapper));

      //подготовка интерфейса получения подключаемых модулей

    IncludeManager include_manager;

      //компиляция шейдера

    ID3D10Blob *dx_shader_blob = 0, *dx_error_msg_blob = 0;

    check_errors ("D3DX11CompileFromMemory", D3DX11CompileFromMemory (source_code, source_code_length, name, macro_mapper.Build (), &include_manager, 0,
      profile, flags1, flags2, 0, &dx_shader_blob, &dx_error_msg_blob, 0));

    if (!dx_shader_blob)
      throw xtl::format_operation_exception ("", "D3DX11CompileFromMemory failed");

    data = DxBlobPtr (dx_shader_blob, false);
    hash = common::crc32 (data->GetBufferPointer (), data->GetBufferSize ());

    DxBlobPtr err_blob (dx_error_msg_blob, false);
    
    if (dx_error_msg_blob)
    {
      const char* err_data = (const char*)dx_error_msg_blob->GetBufferPointer ();
      size_t      err_size = dx_error_msg_blob->GetBufferSize ();

      if (err_data && err_size)
      {
        stl::string message = common::format ("Shader '%s' compilation errors:\n%s", name, stl::string (err_data, err_size).c_str ());

        if (log_fn) log_fn (message.c_str ());
        else        Log ().Print (message.c_str ());
      }
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderCode::ShaderCode");
    throw;
  }
}

ShaderCode::~ShaderCode ()
{
}

/*
    Получение скомпилированного шейдера
*/

const void* ShaderCode::GetCompiledData () const
{
  return data->GetBufferPointer ();
}

size_t ShaderCode::GetCompiledDataSize () const
{
  return data->GetBufferSize ();
}

/*
    Хэш данных
*/

size_t ShaderCode::GetCompiledDataHash () const
{
  return hash;
}
