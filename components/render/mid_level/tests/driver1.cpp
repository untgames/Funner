#include <cstdio>

#include <xtl/common_exceptions.h>
#include <xtl/reference_counter.h>
#include <xtl/intrusive_ptr.h>

#include <render/mid_level/driver.h>

using namespace render::mid_level;

typedef xtl::com_ptr<IDriver> DriverPtr;

class TestDriver: public IDriver, public xtl::reference_counter
{
  public:
///Конструктор / деструктор
    TestDriver ()
    {
      printf ("TestDriver::TestDriver\n");
    }
    
    ~TestDriver ()
    {
      printf ("TestDriver::~TestDriver\n");
    }
  
///Описание драйвера
    const char* GetDescription () { return "Test renderer driver"; }

///Перечисление доступных систем визуализации
    size_t GetRenderersCount () { return 2; }
    
    const char* GetRendererName (size_t index)
    {
      switch (index)
      {
        case 0:  return "Renderer #1";
        case 1:  return "Renderer #2";
        default: throw xtl::make_range_exception ("TestDriver::GetRendererName", "index", index, 2);
      }
    }

///Создание устройства визуализации
    IRenderer* CreateRenderer (const char* name)
    {
      printf ("Attempt to create renderer '%s'\n", name);
      throw xtl::make_not_implemented_exception ("TestDriver::CreateRenderer");
    }

///Подсчёт ссылок
    void AddRef ()
    {
      addref (this);
    }
    
    void Release ()
    {
      release (this);
    }
};

int main ()
{
  try
  {
    printf ("Results of driver1_test:\n");
    
    DriverPtr driver (new TestDriver, false);

    DriverManager::RegisterDriver ("test_drv", driver.get ());
    
    driver = 0;

    DriverManager::CreateRenderer ("*_drv", "*");
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
  }
  
  printf ("exit\n");

  return 0;
}

