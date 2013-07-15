#ifndef RENDER_CUSTOM_SCENE_RENDER_COMMANDS_HEADER
#define RENDER_CUSTOM_SCENE_RENDER_COMMANDS_HEADER

#include <render/scene/custom_render.h>

namespace render
{

namespace scene
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Идентификаторы доступных команд
///////////////////////////////////////////////////////////////////////////////////////////////////
enum CommandId
{
  CommandId_LoadResource,
  CommandId_UnloadResource,
  CommandId_GetDescription,
  CommandId_GetDescriptionResponse,

  CommandId_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Доступные команды
///////////////////////////////////////////////////////////////////////////////////////////////////

/// Загрузка ресурса
struct CmdLoadUnloadResource: public Command
{
  uint16 name_size; //длина имени ресурса
  char   name [1];  //имя ресурса
};

struct CmdLoadResource:   public CmdLoadUnloadResource   { enum { ID = CommandId_LoadResource }; };
struct CmdUnloadResource: public CmdUnloadUnloadResource { enum { ID = CommandId_UnloadResource }; };

/// Получение информации о рендере
struct CmdGetDescription: public Command { enum { ID = CommandId_GetDescription }; };

struct CmdGetDescriptionResponse: public Command
{ 
  enum { ID = CommandId_GetDescriptionResponse }; 

  uint16 description_size; //длина строки с описанием
  char   name [1];         //описание
};

/// Установка глобальных настроек
struct CmdSettings: public Command
{
  uint16 max_draw_depth; //максимальное количество уровней отрисовки
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интерфейс рендера сцены
///////////////////////////////////////////////////////////////////////////////////////////////////
class ISceneRender
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Отсылка команд / прием данных
///////////////////////////////////////////////////////////////////////////////////////////////////
     virtual void   SendCommands   (const void* buffer, size_t size) = 0;
     virtual size_t ReceiveResults (void* buffer, size_t max_size) = 0;

  protected:
    virtual ~ISceneRender () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер рендеров сцены
///////////////////////////////////////////////////////////////////////////////////////////////////
class SceneRenderManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация рендеров
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterRender       (const char* name, ISceneRender* render);
    static void UnregisterRender     (const char* name);
    static void UnregisterAllRenders ();
};

}

}

#endif

