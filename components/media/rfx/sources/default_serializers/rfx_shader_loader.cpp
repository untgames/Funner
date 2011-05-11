#include "shared.h"

using namespace media::rfx;
using namespace common;

namespace
{

/*
    Константы
*/

const char* COMPONENT_NAME = "media.rfx.shader.loaders.default"; //имя компонента

/*
    Загрузчик
*/

class DefaultShaderLoader
{
  public:
    DefaultShaderLoader (const char* name, Shader& shader, const char* profile)
    {
      try
      {
        stl::string content;
      
        FileSystem::LoadTextFile (name, content);
      
        shader.SetProfile    (profile);
        shader.SetSourceCode (content.c_str (), content.size ());        
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::rfx::DefaultShaderLoader(name='%s', profile='%s')", name, profile);
        throw;
      }
    }
};

/*
    Компонент
*/

class Component
{
  public:
    Component ()
    {
      ShaderManager::RegisterLoader ("vert", xtl::bind (&LoadShader, _1, _2, "glsl.vs"));
      ShaderManager::RegisterLoader ("frag", xtl::bind (&LoadShader, _1, _2, "glsl.ps"));
      ShaderManager::RegisterLoader ("ffp",  xtl::bind (&LoadShader, _1, _2, "ffp"));
    }

  private:
    static void LoadShader (const char* name, Shader& shader, const char* profile)
    {
      DefaultShaderLoader (name, shader, profile);
    }
};

}

extern "C"
{

ComponentRegistrator<Component> DefaultShaderLoader (COMPONENT_NAME);

}
