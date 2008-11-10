//базовый класс исключений, транслированных из .NET Framework
struct DotNetException: public xtl::message_exception_base
{
  DotNetException (const char* source, System::Exception^ exception) :
    message_exception_base (source, AutoString (exception->Message).Data ()) {}
};
