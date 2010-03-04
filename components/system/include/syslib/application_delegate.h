#ifndef SYSLIB_APPLICATION_DELEGATE_HEADER
#define SYSLIB_APPLICATION_DELEGATE_HEADER

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Слушатель событий приложения
///////////////////////////////////////////////////////////////////////////////////////////////////
class IApplicationListener
{
  public:
///Очередь сообщений пуста
    virtual void OnIdle () {}    
    
///Приложение получило сигнал завершиться
    virtual void OnExit (int code) {}

  protected:
    virtual ~IApplicationListener () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Делегат приложения
///////////////////////////////////////////////////////////////////////////////////////////////////
class IApplicationDelegate
{
  public:
///Запуск цикла обработки сообщений
    virtual void Run () = 0;

///Выход из приложения
    virtual void Exit (int code) = 0;
    
///Установка необходимости вызова событий idle
    virtual void SetIdleState (bool state) = 0;

///Установка слушателя событий приложения
    virtual void SetListener (IApplicationListener* listener) = 0;
    
///Подсчёт ссылок
    virtual void AddRef  () = 0;
    virtual void Release () = 0;

  protected:
    virtual ~IApplicationDelegate () {}
};

}

#endif
