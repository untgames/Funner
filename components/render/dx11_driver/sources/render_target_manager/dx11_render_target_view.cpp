#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
ID3D10RenderTargetView * pView;
D3D10_RENDER_TARGET_VIEW_DESC rtd;
rtd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
rtd.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
rtd.Texture2D.MipSlice = 0;
ThrowFailure( pD3D10Device->CreateRenderTargetView(pBB, &rtd, &pView), 
  "Couldn't create view" );    

*/

/*
    Конструктор / деструктор
*/

View::View (const DeviceManager& device_manager, ITexture* texture, const ViewDesc& desc)
  : DeviceObject (device_manager)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

View::~View ()
{
}

/*
    Получение целевой текстуры
*/

ITexture* View::GetTexture ()
{
  return texture.get ();
}

/*
    Получение дескриптора
*/

void View::GetDesc (ViewDesc& out_desc)
{
  out_desc = desc;
}
