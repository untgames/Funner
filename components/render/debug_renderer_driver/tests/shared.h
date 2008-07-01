#include <cstdio>
#include <exception>

#include <xtl/intrusive_ptr.h>
#include <xtl/function.h>
#include <xtl/connection.h>

#include <common/log.h>

#include <render/mid_level/driver.h>

using namespace render::mid_level;

typedef xtl::com_ptr<IDriver>   DriverPtr;
typedef xtl::com_ptr<IRenderer> RendererPtr;

//общий код тестов
class Test
{
  public:
    Test ()
    {
      log_connection = common::LogSystem::RegisterLogHandler ("render.mid_level.debug", &Print);
    }
    
    ~Test ()
    {
      DriverManager::UnregisterDriver ("render.mid_level.debug"); //проверка корректности удаления
    }    

  private:
    static void Print (const char*, const char* message)
    {
      printf ("%s\n", message);
    }
  
  private:
    xtl::auto_connection log_connection;  
};
