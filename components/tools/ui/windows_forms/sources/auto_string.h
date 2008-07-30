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
      str = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi (from);
    }

///Деструктор    
    ~AutoString ()
    {
      if (str)
        System::Runtime::InteropServices::Marshal::FreeHGlobal ((System::IntPtr)str);
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
