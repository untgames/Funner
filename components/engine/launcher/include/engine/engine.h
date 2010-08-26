#ifndef FUNNER_ENGINE_HEADER
#define FUNNER_ENGINE_HEADER

#ifdef _WIN32
  #define FUNNER_C_API extern "C" __declspec(dllexport)
#elif __GNUC__
  #define FUNNER_C_API extern "C"
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
    virtual void SetSize (size_t width, size_t height) = 0;
    
    ///Получение ширины окна
    virtual size_t GetWidth () = 0;
    
    ///Получение высоты окна
    virtual size_t GetHeight () = 0;
    
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
    virtual bool ParseCommandLine (unsigned int arguments_count, const char** arguments) = 0;

    ///Запуск главного цикла
    virtual void Run () = 0;

    ///Создание интерфейса поддержки внешних окон
    virtual IWindow* CreateWindow (const char* name) = 0;
};

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание приложения
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef engine::IEngine* (*FunnerInitProc)();

FUNNER_C_API engine::IEngine* FunnerInit ();

#endif
