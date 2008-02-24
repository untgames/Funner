#include "shared.h"

const size_t ARRAYS_RESERVE_SIZE = 2048;

enum RenderTargetType
{
  RenderTargetType_Null,
  RenderTargetType_Color,
  RenderTargetType_DepthStencil
};

void dump (IView* view)
{
  ViewDesc     view_desc;
  TextureDesc  tex_desc;      
  
  if (view)
  {      
    view->GetDesc (view_desc);
    view->GetTexture ()->GetDesc (tex_desc);
    
    printf ("{%s, %s, %ux%ux%u, %u}", get_name (tex_desc.dimension), get_name (tex_desc.format),
           tex_desc.width, tex_desc.height, tex_desc.layers, view_desc.layer);
  }
  else printf ("null");
}

RenderTargetType get_target_type (IView* view)
{
  TextureDesc tex_desc;
  
  if (view)
  {
    view->GetTexture ()->GetDesc (tex_desc);
    
    switch (tex_desc.format)
    {
      case PixelFormat_RGB8:
      case PixelFormat_RGBA8:
      case PixelFormat_L8:
      case PixelFormat_A8:
      case PixelFormat_LA8:
      case PixelFormat_DXT1:
      case PixelFormat_DXT3:
      case PixelFormat_DXT5:
        return RenderTargetType_Color;
      case PixelFormat_D16:
      case PixelFormat_D24X8:
      case PixelFormat_D24S8:
      case PixelFormat_S8:
        return RenderTargetType_DepthStencil;
      default:
        common::RaiseNotSupported ("get_target_type", "Unknown format=%d", tex_desc.format);
        return RenderTargetType_Null;
    }
  }
  else return RenderTargetType_Null;
}

int main ()
{
  try
  {
    printf ("Results of render_targets3_test:\n");
    
    Test test (L"OpenGL device test window (render_targets3)");
    
    typedef stl::vector<TexturePtr> TextureList;        
    
      //создание текстур
      
    printf ("Create textures:\n");
    
    TextureList textures;
    
    textures.reserve (ARRAYS_RESERVE_SIZE);    
    
    static size_t tex_sizes [2] = {36, 32};
    
    for (int i=0; i<2; i++)
    {    
      size_t tex_size = tex_sizes [i];
      
      for (int dim=0; dim<TextureDimension_Num; dim++)
      {
        TextureDesc texture_desc;    

        memset (&texture_desc, 0, sizeof (texture_desc));

        texture_desc.dimension  = (TextureDimension)dim;
        texture_desc.bind_flags = BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil;

        switch (dim)
        {
          case TextureDimension_1D:
            texture_desc.width  = tex_size;
            texture_desc.height = 1;
            texture_desc.layers = 1;
            break;
          case TextureDimension_2D:
            texture_desc.width  = tex_size;
            texture_desc.height = tex_size;
            texture_desc.layers = 1;
            break;
          case TextureDimension_Cubemap:
            texture_desc.width  = tex_size;
            texture_desc.height = tex_size;
            texture_desc.layers = 6;
            break;          
          case TextureDimension_3D:
            texture_desc.width  = tex_size;
            texture_desc.height = tex_size;
            texture_desc.layers = 2;
            break;
          default:
            continue;
        }
        
        for (int format=0; format<PixelFormat_Num; format++)
        {
          texture_desc.format = (PixelFormat)format;                
          
          try
          {
            printf ("Create texture dimension=%s format=%s %ux%ux%u: ", get_name (texture_desc.dimension),
                    get_name (texture_desc.format), texture_desc.width, texture_desc.height, texture_desc.layers);

            TexturePtr texture (test.device->CreateTexture (texture_desc), false);

            textures.push_back (texture);                    

            printf ("Ok!\n");
          }
          catch (std::exception& exception)
          {
            printf ("Fail!\n%s\n", exception.what ());
          }
        }
      }
    }
    
      //создание отображений
      
    printf ("Create views:\n");
    
    typedef stl::vector<ViewPtr>    ViewList;    
    
    ViewList views;
    
    views.reserve (ARRAYS_RESERVE_SIZE);
    
    views.push_back (ViewPtr ()); //добавление пустого отображения
    
    for (TextureList::iterator iter=textures.begin (); iter!=textures.end (); ++iter)
    {
      TexturePtr texture = *iter;
      
      TextureDesc texture_desc;
      
      memset (&texture_desc, 0, sizeof (texture_desc));
      
      texture->GetDesc (texture_desc);
      
      for (size_t layer=0; layer<texture_desc.layers; layer++)
      {
        ViewDesc view_desc;
        
        memset (&view_desc, 0, sizeof (view_desc));

        view_desc.layer = layer;
        
        printf ("Create view dimension=%s format=%s layer=%u: ", get_name (texture_desc.dimension), get_name (texture_desc.format),
          layer);

        try
        {
          ViewPtr view (test.device->CreateView (texture.get (), view_desc), false);
        
          views.push_back (view);
        
          printf ("Ok!\n");
        }
        catch (std::exception& exception)
        {
          printf ("Fail!\n%s\n", exception.what ());
        }
      }
    }
    
      //тестирование различных конфигураций отображений
      
    printf ("Set render targets:\n");
      
    typedef stl::vector<bool> StatusTable;
    
    StatusTable status (views.size () * views.size ());
    
    for (size_t i=0; i<views.size (); i++)
    {      
      ViewPtr color_view = views [i];
      
      for (size_t j=0; j<views.size (); j++)
      {
        ViewPtr depth_stencil_view = views [j];
                
        printf ("%02ux%02u) ", i+1, j+1);
        printf ("render-target=");
        dump   (color_view.get ());
        printf (" depth-stencil=");
        dump   (depth_stencil_view.get ());
        printf (": ");

        try
        {
          test.device->OSSetRenderTargets (color_view.get (), depth_stencil_view.get ());
          test.device->Draw (PrimitiveType_PointList, 0, 0);

          status [i*views.size () + j] = true;

          printf ("Ok!\n");          
        }
        catch (std::exception& exception)
        {
          printf ("Fail!\n%s\n", exception.what ());
        }
      }
    }

      //вывод статуса        

    printf ("Status info:\n");

    for (size_t i=0; i<views.size (); i++)
    {
      printf ("%03u) ", i+1);
      dump   (views [i].get ());
      printf ("\n");
    }

    printf ("Status table (rows - render-target view, columns - depth-stencil view):\n");
    printf ("    ");
    
    for (size_t i=0; i<views.size (); i++)
      switch (get_target_type (views [i].get ()))
      {
        case RenderTargetType_Null:
        case RenderTargetType_DepthStencil:
          printf ("%03u ", i+1);
          break;
        default:
          break;
      }

    printf ("\n");

    for (size_t i=0; i<views.size (); i++)
    {
      switch (get_target_type (views [i].get ()))
      {
        case RenderTargetType_Null:
        case RenderTargetType_Color:
          printf ("%03u| ", i+1);
          break;
        default:
          continue;
      }
      
      for (size_t j=0; j<views.size (); j++)
      {
        switch (get_target_type (views [j].get ()))
        {
          case RenderTargetType_Null:
          case RenderTargetType_DepthStencil:
            break;
          default:
            continue;
        }
        
        if (status [i*views.size () + j]) printf ("+   ");
        else                              printf ("-   ");
      }

      printf ("\n");
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
