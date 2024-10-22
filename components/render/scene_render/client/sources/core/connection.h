///////////////////////////////////////////////////////////////////////////////////////////////////
///Соединение с сервером
///////////////////////////////////////////////////////////////////////////////////////////////////
class Connection: public xtl::noncopyable, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Connection  (const char* connection_name, const char* init_string, size_t logon_timeout_ms);
    ~Connection ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Description ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Рендер сцены
///////////////////////////////////////////////////////////////////////////////////////////////////
    ClientImpl& Client ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Контекст
///////////////////////////////////////////////////////////////////////////////////////////////////
    client::Context& Context ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Ожидание ответа от сервера
///////////////////////////////////////////////////////////////////////////////////////////////////
    void WaitServerFeedback    ();
    bool TryWaitServerFeedback (size_t timeout_ms);

  private:
    struct Impl;
    Impl* impl;  
};

typedef xtl::intrusive_ptr<Connection> ConnectionPtr;
