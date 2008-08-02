///////////////////////////////////////////////////////////////////////////////////////////////////
///Исключение, возникающее при невозможности создания строки
///////////////////////////////////////////////////////////////////////////////////////////////////
struct BadAutoString: public xtl::message_exception_base
{
  BadAutoString (const char* source) : message_exception_base (source, "Can't translate string from System::String^ to const char*") {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Класс для приведения CLR-строк в const char*
///////////////////////////////////////////////////////////////////////////////////////////////////
class AutoString
{
  public:
///Конструкторы
    AutoString () : str (0) { }

    AutoString (System::String^ from)
    {
      try
      {
        str = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi (from);
      }
      catch (...)
      {
        throw BadAutoString ("tools::ui::windows_forms::AutoString::AutoString");
      }
    }

///Деструктор    
    ~AutoString ()
    {
      try
      {
        if (str)
          System::Runtime::InteropServices::Marshal::FreeHGlobal ((System::IntPtr)str);
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }

///Данные
    const char* Data ()
    {
      return str ? str : "";
    }
    
///Обмен
    void Swap (AutoString& s)
    {
      stl::swap (str, s.str);
    }

  private:
    char* str;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение строки
///////////////////////////////////////////////////////////////////////////////////////////////////
inline const char* get_string (System::String^ from, AutoString& buffer)
{
  AutoString (from).Swap (buffer);
  
  return buffer.Data ();
}
