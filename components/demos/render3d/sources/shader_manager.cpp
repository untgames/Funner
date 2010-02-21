#include "shared.h"

namespace
{

void print (const char* message)
{
  printf ("Shader message: '%s'\n", message);
}

//чтение ихсодного текста шейдера в строку
stl::string read_shader (const char* file_name)
{
  common::InputFile file (file_name);

  stl::string buffer (file.Size (), ' ');

  file.Read (&buffer [0], file.Size ());
  
  for (;;)
  {
    char* include_directive = (char*)strstr (buffer.c_str (), "#include");
    
    if (!include_directive)
      break;
      
    char* start = strstr (include_directive, "\"");
    
    if (!start)
      break;
      
    start++;
    
    char* finish = (char*)strstr (start, "\"");
    
    if (!finish)
      break;      
      
    stl::string include_file_name (start, finish);
    
    ++finish;

    include_file_name = common::dir (file_name) + include_file_name;

    stl::string include_buffer = read_shader (include_file_name.c_str ());

    buffer.replace (include_directive, finish, include_buffer);
  }

  return buffer;
}


}

ShaderManager::ShaderManager (Test& in_test)
  : test (in_test)
{
}

void ShaderManager::SetShadersDir (const char* name)
{
  shaders_dir = name ? name : ".";
}

const char* ShaderManager::ShadersDir ()
{
  return shaders_dir.c_str ();
}

ProgramPtr ShaderManager::CreateProgram (const char* name)
{
  if (!name)
    return ProgramPtr ();
    
  printf ("Create shader '%s'\n", name);

  stl::string vertex_shader_name   = common::format ("%s/%s.vert", shaders_dir.c_str (), name),
              pixel_shader_name    = common::format ("%s/%s.frag", shaders_dir.c_str (), name),
              vertex_shader_source = read_shader (vertex_shader_name.c_str ()),
              pixel_shader_source  = read_shader (pixel_shader_name.c_str ());
              
  ShaderDesc shader_descs [] = {
    {vertex_shader_name.c_str (), size_t (-1), vertex_shader_source.c_str (), "glsl.vs", ""},
    {vertex_shader_name.c_str (), size_t (-1), pixel_shader_source.c_str (), "glsl.ps", ""},
  };

  return ProgramPtr (test.device->CreateProgram (sizeof shader_descs / sizeof *shader_descs, shader_descs, &print), false);
}

///Загрузка шейдера  
void ShaderManager::LoadShader (const char* name)
{
  ModelShaderPtr shader = FindShader (name);
  
  bool need_insert = false;
  
  if (!shader)
  {
    shader = ModelShaderPtr (new ModelShader);
    
    shader->name = name;        
    
    need_insert = true;
  }

  shader->program = CreateProgram (name);
  
  if (need_insert)
    shaders.push_back (shader);
}

///Поиск шейдера
ModelShaderPtr ShaderManager::FindShader (const char* name)
{
  if (!name)
    return ModelShaderPtr ();
    
  for (ShaderArray::iterator iter=shaders.begin (), end=shaders.end (); iter!=end; ++iter)
    if ((*iter)->name == name)
      return *iter;
            
  return ModelShaderPtr ();
}

ModelShaderPtr ShaderManager::GetShader (const char* name)
{
  ModelShaderPtr shader = FindShader (name);
  
  if (shader)
    return shader;
    
  LoadShader (name);
  
  return FindShader (name);
}

///Перезагрузка шейдеров
void ShaderManager::ReloadShaders ()
{
  for (ShaderArray::iterator iter=shaders.begin (), end=shaders.end (); iter!=end; ++iter)
  {
    LoadShader ((*iter)->name.c_str ());
  }
}
