#include <stdio.h>
#include <stl/string>
#include <render/low_level/common.h>

using namespace render::low_level;

const char* get_flag_name (size_t flag)
{
  switch (flag)
  {
    case BindFlag_VertexBuffer:   return "BindFlag_VertexBuffer";
    case BindFlag_IndexBuffer:    return "BindFlag_IndexBuffer";
    case BindFlag_ConstantBuffer: return "BindFlag_ConstantBuffer";
    case BindFlag_Texture:        return "BindFlag_Texture";
    case BindFlag_RenderTarget:   return "BindFlag_RenderTarget";
    case BindFlag_DepthStencil:   return "BindFlag_DepthStencil";
    default:                      return "";
  }
}

void print_flags (size_t flags, stl::string& buffer)
{
  buffer.clear ();

  for (size_t i=0; i<32; i++)
  {
    size_t flag = 1 << i;
    
    if (flags & flag)
    {
      const char* name = get_flag_name (flag);
      
      if (!*name)
        continue;

      if (!buffer.empty ())
        buffer += " | ";
        
      buffer += get_flag_name (flag);
    }
  }
}

int main ()
{
  printf ("const char* get_name (BindFlag param)\n{\n");
  printf ("  switch (param)\n  {\n");
  printf ("    case 0: return \"0\";\n");
  
  stl::string buffer;
  
  buffer.reserve (1024);
  
  size_t all_flags = BindFlag_VertexBuffer | BindFlag_IndexBuffer | BindFlag_ConstantBuffer | BindFlag_Texture |
                     BindFlag_RenderTarget | BindFlag_DepthStencil;
                 
  for (size_t flags=1; flags<=all_flags; flags++)
  {
    print_flags (flags, buffer);
    
    printf ("    case %s: return \"%s\";\n", buffer.c_str (), buffer.c_str ());
  }

  printf ("    default:\n");
  printf ("      RaiseInvalidArgument (\"render::low_level::get_name(BindFlag)\", \"param\", param);\n");
  printf ("      return \"\";\n");
  printf ("  }\n");
  printf ("}\n");

  return 0;
}
