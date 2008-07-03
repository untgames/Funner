#include "shared.h"

using namespace render;
using namespace render::render2d;
using namespace scene_graph;

/*
    Конструктор / деструктор
*/

Render::Render (mid_level::IRenderer* in_renderer)
{
  static const char* METHOD_NAME = "render::render2d::Render::Render";

  if (!in_renderer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "renderer");

  IRenderer* casted_renderer = dynamic_cast<IRenderer*> (in_renderer);

  if (!casted_renderer)
    throw xtl::make_argument_exception (METHOD_NAME, "renderer", typeid (in_renderer).name (),
      "Renderer type is incompatible with render::renderer2d::IRenderer");

  renderer = casted_renderer;
}

Render::~Render ()
{
}

/*
    Подсчёт ссылок
*/

void Render::AddRef ()
{
  addref (this);
}

void Render::Release ()
{
  release (this);
}

/*
    Создание областей вывода
*/

IRenderView* Render::CreateRenderView (scene_graph::Scene* scene)
{
  return new RenderView (scene, this);
}

/*
    Установка цвета очистка буфера кадра
*/

void Render::SetBackgroundColor (const math::vec4f& color)
{
  clear_color = color;
}

void Render::GetBackgroundColor (math::vec4f& color)
{
  color = clear_color;
}

/*
    Установка функции отладочного протоколирования
*/

void Render::SetLogHandler (const LogFunction& log)
{
  log_handler = log;
}

const Render::LogFunction& Render::GetLogHandler ()
{
  return log_handler;
}

void Render::LogPrintf (const char* format, ...)
{
  va_list args;
  
  va_start (args, format);
  
  try
  {
    log_handler (common::vformat (format, args).c_str ());
  }
  catch (...)
  {
  }
}

/*
    Работа с ресурсами
*/

void Render::LoadResource (const char* tag, const char* file_name)
{
  try
  {
    const char* MATERIAL_LIBRARY_TAG = "material_library";      

    if (!strcmp (tag, "auto"))
    {
        //автоматическое определение тэга

      if (common::suffix (file_name) == ".xmtl")
        tag = MATERIAL_LIBRARY_TAG;
    }

      //диспетчеризация тэгов
      
    if (!strcmp (tag, MATERIAL_LIBRARY_TAG))
    {
      LoadMaterialLibrary (file_name);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::render2d::Render::LoadResource");
    throw;
  }
}

void Render::LoadMaterialLibrary (const char* file_name)
{
  try
  {
    media::rfx::MaterialLibrary library (file_name, log_handler);
    
    for (media::rfx::MaterialLibrary::Iterator iter=library.CreateIterator (); iter; ++iter)
    {
      media::rfx::Material::Pointer material        = *iter;
      media::rfx::SpriteMaterial*   sprite_material = dynamic_cast<media::rfx::SpriteMaterial*> (&*material);

      if (sprite_material)
        InsertMaterial (library.ItemId (iter), sprite_material);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::render2d::Render::LoadMaterialLibrary(file_name='%s')", file_name);
    throw;
  }
}

void Render::InsertMaterial (const char* id, const media::rfx::SpriteMaterial::Pointer& material)
{
  materials [id] = material;
}

SpriteMaterial* Render::GetMaterial (const char* name)
{
  static const char* METHOD_NAME = "render::render2d::Render::GetMaterial";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

    //попытка найти объект в кэше

  MaterialMap::iterator iter = materials.find (name);

  if (iter != materials.end ())
    return iter->second.get ();

  throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Material not found");
}

/*
    Работа с кэшем
*/

Renderable* Render::GetRenderable (scene_graph::Sprite* entity)
{
    //попытка найти объект в кэше

  RenderableMap::iterator iter = renderables_cache.find (entity);
  
  if (iter != renderables_cache.end ())
    return iter->second.renderable.get ();

    //создание нового спрайта

  RenderablePtr renderable (new RenderableSprite (entity, *this), false);

  InsertRenderable (entity, renderable);

  return &*renderable;
}

void Render::InsertRenderable (scene_graph::Entity* entity, const RenderablePtr& renderable)
{  
  renderables_cache.insert_pair (entity, RenderableHolder (renderable,
    entity->RegisterEventHandler (NodeEvent_AfterDestroy, xtl::bind (&Render::RemoveRenderable, this, entity))));
}

void Render::RemoveRenderable (scene_graph::Entity* entity)
{
  renderables_cache.erase (entity);
}

ITexture* Render::GetTexture (const char* file_name)
{
    //попытка найти текстуру в кеше

  TextureMap::iterator iter = textures.find (file_name);
  
  if (iter != textures.end ())
    return &*iter->second;
    
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");
    
      //загрузка картинки
      
    media::Image image (file_name);
    
    switch (image.Format ())
    {
      case media::PixelFormat_BGR8:
        image.Convert (media::PixelFormat_RGB8);
        break;
      case media::PixelFormat_BGRA8:
        image.Convert (media::PixelFormat_RGBA8);
        break;
      case media::PixelFormat_RGBA8:        
      case media::PixelFormat_RGB8:        
      case media::PixelFormat_L8:
      case media::PixelFormat_A8:
      case media::PixelFormat_LA8:
        break;
      default:
        throw xtl::format_operation_exception ("", "Unknown texture format=%d", image.Format ());
    }

      //создание новой текстуры

    TexturePtr texture (renderer->CreateTexture (image), false);
    
      //добавление текстуры в кэш

    textures.insert_pair (file_name, texture);

    return texture.get ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::render2d::Render::GetTexture(file_name='%s', renderer='%s')", file_name, renderer->GetDescription ());
    throw;
  }
}

/*
    Добавление кадра на отрисовку    
*/

void Render::AddFrame (IFrame* frame)
{
  renderer->AddFrame (frame);
}
