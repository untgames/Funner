#include "shared.h"

using namespace render;
using namespace render::render2d;
using namespace scene_graph;

namespace
{

/*
    Константы
*/

const char* DYNAMIC_STRING_MARKER = "dynamic"; //маркер динамической текстуры
const char* VIDEO_STRING_MARKER   = "video";   //маркер видео текстуры

/*
    Динамическая текстура
*/

class DynamicTexturePrerender: public IRenderablePrerequisite
{
  public:
    DynamicTexturePrerender (const RenderQueryPtr& in_query) : query (in_query) {}
    
    void Update ()
    {
      query->Update ();
    }
  
  private:
    RenderQueryPtr query;
};

/*
    Видео текстура
*/

class VideoTexturePrerender: public IRenderablePrerequisite
{
  public:
    VideoTexturePrerender (Renderable* in_renderable, const char* video_file_name, render::mid_level::renderer2d::IRenderer& renderer)
      : renderable (in_renderable)
      , stream (video_file_name)
      , buffer (stream.Width (), stream.Height (), 1, media::PixelFormat_RGBA8)
      , texture (renderer.CreateTexture (buffer), false)
      , current_frame (stream.FramesCount ())
    {
    }
    
    void Update ()
    {
      if (!stream.FramesCount ())
        return;
      
      try
      {
        size_t frame = ((size_t)(renderable->VideoPosition () * stream.FramesPerSecond ())) % stream.FramesCount ();
        
        if (frame == current_frame)
          return;

        stream.Decode (frame, buffer);        

        texture->Update (buffer);

        current_frame = frame;        
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::VideoTexturePrerender::Update");
        throw;
      }
    }
    
    render::mid_level::renderer2d::ITexture* GetTexture () { return texture.get (); }

  private:  
    Renderable*        renderable;
    media::VideoStream stream;
    media::Image       buffer;
    TexturePtr         texture;
    size_t             current_frame;
};

}

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
    Создание дочерних запросов
*/

void Render::SetQueryHandler (const QueryFunction& in_query_handler)
{
  query_handler = in_query_handler;
}

const ICustomSceneRender::QueryFunction& Render::GetQueryHandler ()
{
  return query_handler;
}

/*
    Работа с ресурсами
*/

void Render::LoadResource (const char* tag, const char* file_name)
{
  try
  {
    const char* MATERIAL_LIBRARY_TAG = "material_library";
    const char* TEXTURE_TAG          = "texture";    

    if (!strcmp (tag, "auto"))
    {
        //автоматическое определение тэга
        
      stl::string suffix = common::suffix (file_name);        

      if (suffix == ".xmtl")
        tag = MATERIAL_LIBRARY_TAG;        
        
      if (suffix == ".png" || suffix == ".tga" || suffix == ".jpg" || suffix == ".bmp")
        tag = TEXTURE_TAG; 
    }

      //диспетчеризация тэгов

    if (!strcmp (tag, MATERIAL_LIBRARY_TAG))
    {
      LoadMaterialLibrary (file_name);
    }
    
    if (!strcmp (tag, TEXTURE_TAG))
    {
      if (xtl::xstrncmp (file_name, VIDEO_STRING_MARKER, strlen (VIDEO_STRING_MARKER)))
      {      
        GetTexture (file_name, true);
      }
    }    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::render2d::Render::LoadResource");
    throw;
  }
}

void Render::UnloadResource (const char* tag, const char* file_name)
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
      UnloadMaterialLibrary (file_name);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::render2d::Render::UnloadResource");
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
      {
        InsertMaterial (library.ItemId (iter), sprite_material);

        //предзагрузка текстуры

        try
        {
          if (xtl::xstrncmp (sprite_material->Image (), VIDEO_STRING_MARKER, strlen (VIDEO_STRING_MARKER)))
          {
            GetTexture (sprite_material->Image (), sprite_material->BlendMode () == media::rfx::SpriteBlendMode_AlphaClamp ? true : false); //???? не загружается альфа текстур текста
          }
        }
        catch (std::exception& exception)
        {
          LogPrintf ("%s\n    at Render::LoadMaterialLibrary('%s')", exception.what (), file_name);
        }
        catch (...)
        {
          LogPrintf ("unknown exception\n    at Render::LoadMaterialLibrary('%s')", file_name);
        }
      }
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::render2d::Render::LoadMaterialLibrary(file_name='%s')", file_name);
    throw;
  }
}

void Render::UnloadMaterialLibrary (const char* file_name)
{
  try
  {
    media::rfx::MaterialLibrary library (file_name, log_handler);

    for (media::rfx::MaterialLibrary::Iterator iter=library.CreateIterator (); iter; ++iter)
    {
      media::rfx::Material::Pointer material        = *iter;
      media::rfx::SpriteMaterial*   sprite_material = dynamic_cast<media::rfx::SpriteMaterial*> (&*material);

      if (sprite_material)
      {
        RemoveMaterial (library.ItemId (iter));

        textures.erase (sprite_material->Image ());
      }
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::render2d::Render::UnloadMaterialLibrary(file_name='%s')", file_name);
    throw;
  }
}

void Render::InsertMaterial (const char* id, const media::rfx::SpriteMaterial::Pointer& material)
{
  materials [id] = material;
}

void Render::RemoveMaterial (const char* id)
{
  materials.erase (id);
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

RenderableFont* Render::GetFont (const char* name)
{
  static const char* METHOD_NAME = "render::render2d::Render::GetFont";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

    //попытка найти объект в кэше

  RenderableFontMap::iterator iter = fonts.find (name);

  if (iter != fonts.end ())
    return iter->second.get ();

  RenderableFontPtr new_font = RenderableFontPtr (new RenderableFont (name, *this));

  fonts[name] = new_font;

  return fonts[name].get ();
}

/*
    Работа с кэшем
*/

Renderable* Render::GetRenderable (scene_graph::SpriteModel* entity)
{
    //попытка найти объект в кэше

  RenderableMap::iterator iter = renderables_cache.find (entity);

  if (iter != renderables_cache.end ())
    return iter->second.renderable.get ();

    //создание нового спрайта

  RenderablePtr renderable (new RenderableSpriteModel (entity, *this), false);

  InsertRenderable (entity, renderable);

  return &*renderable;
}

Renderable* Render::GetRenderable (scene_graph::HeightMap* entity)
{
    //попытка найти объект в кэше

  RenderableMap::iterator iter = renderables_cache.find (entity);

  if (iter != renderables_cache.end ())
    return iter->second.renderable.get ();

    //создание нового спрайта

  RenderablePtr renderable (new RenderableHeightMap (entity, *this), false);

  InsertRenderable (entity, renderable);

  return &*renderable;
}

Renderable* Render::GetRenderable (scene_graph::TextLine* entity)
{
    //попытка найти объект в кэше

  RenderableMap::iterator iter = renderables_cache.find (entity);

  if (iter != renderables_cache.end ())
    return iter->second.renderable.get ();

    //создание нового спрайта

  RenderablePtr renderable (new RenderableTextLine (entity, *this), false);

  InsertRenderable (entity, renderable);

  return &*renderable;
}

void Render::InsertRenderable (scene_graph::Entity* entity, const RenderablePtr& renderable)
{
  xtl::auto_connection after_destroy_connection = entity->RegisterEventHandler (NodeEvent_AfterDestroy, xtl::bind (&Render::RemoveRenderable, this, entity));

  renderables_cache.insert_pair (entity, RenderableHolder (renderable)).first->second.on_destroy.swap (after_destroy_connection);
}

void Render::RemoveRenderable (scene_graph::Entity* entity)
{
  renderables_cache.erase (entity);
}

ITexture* Render::GetTexture (const char* file_name, bool need_alpha, Renderable* renderable)
{
    //попытка найти текстуру в кеше

  TextureMap::iterator iter = textures.find (file_name);

  if (iter != textures.end ())
  {
    TextureHolder& holder = iter->second;

    if (holder.prerender)
      renderable->AddPrerender (holder.prerender);

      //если альфа-канал не требуется - возвращаем базовую текстуру

    if (!need_alpha)
      return holder.base_texture.get ();

      //если требуется альфа канал и соответствующая текстура присутствует - возвращаем её

    if (holder.alpha_texture)
      return holder.alpha_texture.get ();

      //если альфа-текстура отсутствует - создаём её и возвращаем

    bool has_alpha = false, is_shared = false;
    
    PrerequisitePtr prerender;

    holder.alpha_texture = CreateTexture (file_name, true, has_alpha, is_shared, renderable, prerender);

    return holder.alpha_texture.get ();
  }

  try
  {
      //создание новой текстуры

    bool has_alpha = false, is_shared = false;
    
    PrerequisitePtr prerender;

    TexturePtr texture = CreateTexture (file_name, need_alpha, has_alpha, is_shared, renderable, prerender);    
    
    if (is_shared)
    {
        //добавление текстуры в кэш

      textures.insert_pair (file_name, TextureHolder (texture, has_alpha ? texture : TexturePtr (), prerender));
    }    

    if (prerender && renderable)
    {
      renderable->AddPrerender (prerender);
    }

    return texture.get ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::render2d::Render::GetTexture");
    throw;
  }
}

/*
    Создание текстур
*/

TexturePtr Render::CreateTexture (const char* file_name, bool need_alpha, bool& has_alpha, bool& is_shared, Renderable* renderable, PrerequisitePtr& prerender)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");
     
    is_shared = true;

      //проверка маски динамической текстуры

    if (!xtl::xstrncmp (file_name, DYNAMIC_STRING_MARKER, strlen (DYNAMIC_STRING_MARKER)))
    {
      has_alpha = true;            

      return CreateDynamicTexture (file_name, prerender);
    }
        
      //проверка маски видео текстуры

    if (!xtl::xstrncmp (file_name, VIDEO_STRING_MARKER, strlen (VIDEO_STRING_MARKER)))
    {
      has_alpha = true;
      is_shared = false;

      return CreateVideoTexture (file_name, renderable, prerender);
    }    

    static common::ComponentLoader loader ("media.compressed_image.*");
    
    if (media::CompressedImageManager::FindLoader (file_name, common::SerializerFindMode_ByName))
    {
        //загрузка сжатой картинки

      media::CompressedImage image (file_name);

      return TexturePtr (renderer->CreateTexture (image), false);
    }
    else
    {
        //загрузка не сжатой картинки

      bool need_alpha_generation = false;

      media::Image image (file_name);

      switch (image.Format ())
      {
        case media::PixelFormat_BGR8:
          image.Convert (need_alpha ? media::PixelFormat_RGBA8 : media::PixelFormat_RGB8);
          need_alpha_generation = need_alpha;
          break;
        case media::PixelFormat_BGRA8:
          image.Convert (media::PixelFormat_RGBA8);
          has_alpha = true;
          break;
        case media::PixelFormat_RGBA8:
        case media::PixelFormat_A8:
        case media::PixelFormat_L8:
        case media::PixelFormat_LA8:
          has_alpha = true;
          break;
        case media::PixelFormat_RGB8:
          if (need_alpha)
            image.Convert (media::PixelFormat_RGBA8);

          need_alpha_generation = need_alpha;

          break;
        default:
          throw xtl::format_operation_exception ("", "Unknown texture format=%d", image.Format ());
      }

      /*
        Если нужна генерация альфа-канала - генерируем канал таким образом, что все пикселы с цветом нижнего левого
        имеют alpha = 0, а остальные - alpha = 255
      */

      if (need_alpha_generation)
      {
        #pragma pack(1)

        struct RGBA
        {
          unsigned char red, green, blue, alpha;
        };

        RGBA* pixel       = reinterpret_cast<RGBA*> (image.Bitmap ());
        RGBA  alpha_color = *pixel;

        for (size_t count=image.Width () * image.Height (); count--; pixel++)
        {
          if (pixel->red == alpha_color.red && pixel->green == alpha_color.green && pixel->blue == alpha_color.blue)
            pixel->alpha = 0;
          else
            pixel->alpha = 255;
        }

        has_alpha = true;
      }


        //создание текстуры

      return TexturePtr (renderer->CreateTexture (image), false);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::render2d::Render::CreateTexture(file_name='%s', renderer='%s')", file_name, renderer->GetDescription ());
    throw;
  }
}

TexturePtr Render::CreateDynamicTexture (const char* name, PrerequisitePtr& prerender)
{
    //разбор запроса динамического рендеринга

  common::StringArray tokens = common::parse (name , "dynamic *\\( *([[:digit:]]*) *, *([[:digit:]]*) *, *'(.*)' *\\).*");

  if (tokens.Size () != 4)
    throw xtl::make_argument_exception ("render::render2d::Render::CreateDynamicTexture", "name", name,
      "Wrong format. Expected: dynamic(<width>,<height>,'<query_string>')");

  size_t texture_width  = xtl::io::get<size_t> (tokens [1]),
         texture_height = xtl::io::get<size_t> (tokens [2]);

  const char* query_string = tokens [3];

    //создание динамической текстуры и вспомогательного буфера глубины

  TexturePtr       texture (renderer->CreateTexture (texture_width, texture_height, render::mid_level::PixelFormat_RGBA8), false);
  RenderTargetPtr  depth_stencil_buffer (renderer->CreateDepthStencilBuffer (texture_width, texture_height), false);

    //создание запроса

  RenderQueryPtr query (query_handler (texture.get (), depth_stencil_buffer.get (), query_string), false);
  
    //создание пререндера
    
  prerender = PrerequisitePtr (new DynamicTexturePrerender (query), false);

  return texture;
}

TexturePtr Render::CreateVideoTexture (const char* name, Renderable* renderable, PrerequisitePtr& prerender)
{
  try
  {
    if (!renderable)
      throw xtl::make_null_argument_exception ("", "renderable");

      //разбор запроса динамического рендеринга

    common::StringArray tokens = common::parse (name , "video *\\( *'(.*)' *\\).*");

    if (tokens.Size () != 2)
      throw xtl::make_argument_exception ("", "name", name, "Wrong format. Expected: video('video_file_name')");

    const char* video_file_name = tokens [1];
    
      //создание пререндера
      
    xtl::intrusive_ptr<VideoTexturePrerender> video_prerender (new VideoTexturePrerender (renderable, video_file_name, *renderer), false);
    
    prerender = video_prerender;

    return video_prerender->GetTexture ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::render2d::Render::CreateVideoTexture");
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
