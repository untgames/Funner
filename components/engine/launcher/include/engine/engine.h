#ifndef FUNNER_ENGINE_HEADER
#define FUNNER_ENGINE_HEADER

#include <cstddef>

#ifdef _WIN32
  #define FUNNER_C_API extern "C" __declspec(dllexport)
#elif __GNUC__
  #define FUNNER_C_API extern "C" __attribute__ ((visibility("default")))
#else
  #error "Unknown platform"
#endif

namespace engine
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Кнопки мыши
///////////////////////////////////////////////////////////////////////////////////////////////////
enum MouseButton
{
  MouseButton_Left,
  MouseButton_Right,
  MouseButton_Middle,
  MouseButton_X1,
  MouseButton_X2
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Слушатель событий окна
///////////////////////////////////////////////////////////////////////////////////////////////////
class IWindowListener
{
  public:
    ///Обработчик события перерисовки окна
    virtual void OnPaint () {}
    
    ///Обработчик изменения размеров окна
    virtual void OnSize (size_t width, size_t height) {}

    ///Обработчики событий ввода
    virtual void OnMouseMove        (int x, int y) {}
    virtual void OnMouseEnter       (int x, int y) {}
    virtual void OnMouseHover       (int x, int y) {}
    virtual void OnMouseLeave       (int x, int y) {}
    virtual void OnMouseDown        (MouseButton button, int x, int y) {}
    virtual void OnMouseUp          (MouseButton button, int x, int y) {}
    virtual void OnMouseDoubleClick (MouseButton button, int x, int y) {}
    virtual void OnKeyDown          (const char* key) {}
    virtual void OnKeyUp            (const char* key) {}

  protected:
    virtual ~IWindowListener () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Слушатель сообщений приложения
///////////////////////////////////////////////////////////////////////////////////////////////////
class INotificationListener
{
  public:
    ///Обработчик события перерисовки окна
    virtual void OnNotification (const char* notification) = 0;

  protected:
    virtual ~INotificationListener () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интерфейс доступа к внешним окнам
///////////////////////////////////////////////////////////////////////////////////////////////////
class IWindow
{
  public:
    ///Деструктор
    virtual ~IWindow () {}
    
    ///Показ / скрытие окна
    virtual void Show (bool state) = 0;
    
    ///Установка дескриптора родительского окна
    virtual void SetParentHandle (void* handle) = 0;
    
    ///Получение дескриптора родительского окна
    virtual void* GetParentHandle () = 0;
    
    ///Установка размеров окна
    virtual void SetSize (unsigned int width, unsigned int height) = 0;
    
    ///Получение ширины окна
    virtual unsigned int GetWidth () = 0;
    
    ///Получение высоты окна
    virtual unsigned int GetHeight () = 0;
    
    ///Установка положения окна
    virtual void SetPosition (int x, int y) = 0;

    ///Получение абсциссы положения окна
    virtual int GetX () = 0;

    ///Получение ординаты положения окна
    virtual int GetY () = 0;

    ///Добавление слушателя событий
    virtual void AttachListener (IWindowListener* listener) = 0;
    
    ///Удаление слушателя событий
    virtual void DetachListener (IWindowListener* listener) = 0;
    
    ///Удаление всех слушателей событий
    virtual void DetachAllListeners () = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интерфейс доступа к движку
///////////////////////////////////////////////////////////////////////////////////////////////////
class IEngine
{
  public:
    ///Деструктор
    virtual ~IEngine () {}
    
    ///Установка базовой директории
    virtual void SetBaseDir (const char* dir_name) = 0;
    
    ///Получение базовой директории
    virtual const char* GetBaseDir () = 0;

    ///Разбор командной строки и конфигурирование запуска движка
    virtual bool ParseCommandLine (unsigned int arguments_count, const char** arguments, const char** env = 0) = 0;

    ///Запуск главного цикла
    virtual void Run () = 0;

    ///Создание интерфейса поддержки внешних окон
    virtual IWindow* CreateWindow (const char* name) = 0;

    ///Запуск главного цикла
    virtual void Execute (const char* command) = 0;

    ///Посылка оповещения
    virtual void PostNotification (const char* notification) = 0;

    ///Добавление слушателя оповещений
    virtual void AttachNotificationListener (const char* notification_wildcard, INotificationListener* listener) = 0;

    ///Удаление слушателя оповещений
    virtual void DetachNotificationListener (const char* notfication_wildcard, INotificationListener* listener) = 0;
    virtual void DetachNotificationListener (INotificationListener* listener) = 0;

    ///Удаление всех слушателей оповещений
    virtual void DetachAllNotificationListeners () = 0;
};

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание приложения
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef engine::IEngine* (*FunnerInitProc)();

FUNNER_C_API engine::IEngine* FunnerInit ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Эмуляция запуска main функции
///////////////////////////////////////////////////////////////////////////////////////////////////
FUNNER_C_API int FunnerMain (int argc, const char** argv, const char** env = 0);

#endif
