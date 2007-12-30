#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    ����������� / ����������
*/

View::View (ITexture* in_texture, const ViewDesc& in_desc)
  : base_texture (in_texture)
{
  static const char* METHOD_NAME = "render::low_level::opengl::View::View";

  if (!base_texture)
    RaiseNullArgument (METHOD_NAME, "texture");

     //�������� ������������ �����������

  TextureDesc texture_desc;

  base_texture->GetDesc (texture_desc);

  if (in_desc.layer >= texture_desc.layers)
    RaiseOutOfRange (METHOD_NAME, "desc.layer", in_desc.layer, texture_desc.layers);

  desc = in_desc;
    
    //����������� ���� ��������
    
  if (bindable_texture = dynamic_cast<IBindableTexture*> (in_texture))
  {
    type = ViewType_Texture;
  }
  else if (color_buffer = dynamic_cast<SwapChainColorBuffer*> (in_texture))
  {
    type = ViewType_SwapChainColorBuffer;
  }
  else if (depth_stencil_buffer = dynamic_cast<SwapChainDepthStencilBuffer*> (in_texture))
  {
    type = ViewType_SwapChainDepthStencilBuffer;
  }
  else
  {
    RaiseInvalidArgument (METHOD_NAME, "texture", typeid (in_texture).name (), "Unsupported texture type");
  }
}

/*
    ��������� �����������
*/

void View::GetDesc (ViewDesc& out_desc)
{
  out_desc = desc;
}
