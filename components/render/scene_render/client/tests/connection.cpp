#include "shared.h"

namespace
{

///Тестовое соединение с сервером
class MyConnection: public scene::interchange::IConnection, public xtl::reference_counter, public xtl::trackable
{
  public:
///Конструктор
    MyConnection (const char* init_string) 
    {
      common::PropertyMap properties = common::parse_init_string (init_string);
    
      const char* response_connection_name = properties.GetString ("initiator");

      response_connection = xtl::com_ptr<scene::interchange::IConnection> (scene::interchange::ConnectionManager::CreateConnection (response_connection_name, "initiator=MyConnection"), false);
    }

///Является ли соединение внутрипроцессным
    bool IsInprocessed () { return true; }

///Обработка входного потока данных
    void ProcessCommands (const scene::interchange::CommandBuffer& commands)
    {
    }

///Получение события оповещения об удалении
    xtl::trackable& GetTrackable () { return *this; }

///Подсчет ссылок
    void AddRef  () { addref (this); }
    void Release () { release (this); }

///Создание соединения
    static IConnection* CreateConnection (const char* name, const char* init_string)
    {
      return new MyConnection (init_string);
    }

  private:
    xtl::com_ptr<scene::interchange::IConnection> response_connection;
};

}

int main ()
{
  printf ("Results of connection_test:\n");

  try
  {
    static const char* CONNECTION_NAME = "my_test_connection";

    scene::interchange::ConnectionManager::RegisterConnection (CONNECTION_NAME, CONNECTION_NAME, &MyConnection::CreateConnection);

    Client render (CONNECTION_NAME);

    scene::interchange::ConnectionManager::UnregisterConnection (CONNECTION_NAME);

    printf ("description='%s'\n", render.Description ());
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
