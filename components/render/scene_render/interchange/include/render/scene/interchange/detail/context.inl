namespace detail
{

/// Интерфейс получения сообщений
template <class Deserializer> class IIncomingCommandsProcessor
{
  public:
    virtual ~IIncomingCommandsProcessor () {}

    virtual void ProcessFeedback (Deserializer& deserializer, common::Log& log) = 0;
};

/// Обработчик входящих команд
template <class Processor, class Deserializer> class IncomingCommandsProcessor: public IIncomingCommandsProcessor<Deserializer>
{
  public:
    IncomingCommandsProcessor (Processor* in_processor) : processor (in_processor) {}

    void ProcessFeedback (Deserializer& deserializer, common::Log& log)
    {
      while (deserializer.Available ())
      {
        const Command& command   = read (deserializer, xtl::type<Command> ());
        size_t         body_size = command.command_size - sizeof (Command);

        if (deserializer.Available () < body_size)
        {
          const char* command_name = get_command_name ((CommandId)command.command_id);

          if (command_name) log.Printf ("Can't read command %s with size %u: only %u bytes is available", command_name, command.command_size, body_size);
          else              log.Printf ("Can't read command %u with size %u: only %u bytes is available", command.command_id, command.command_size, body_size);

          return; //no more data
        }

        if (!deserializer.Deserialize ((CommandId)command.command_id, *processor))
        {
          log.Printf ("Unknown command %u with size %u. Will be skipped", command.command_id, command.command_size);

          deserializer.Skip (body_size);
        }
      }
    }

  private:
    Processor* processor;
};

}

/*
    Конструктор / деструктор
*/

template <class Serializer, class Deserializer> template <class Processor>
inline Context<Serializer, Deserializer>::Context (Processor& processor)
  : incoming_commands_processor (new detail::IncomingCommandsProcessor<Processor, Deserializer> (&processor))
  , log ("render.scene.interchange.Context")
{
}

template <class Serializer, class Deserializer>
inline Context<Serializer, Deserializer>::~Context ()
{
  try
  {
    SetConnection (ConnectionPtr ());
  }
  catch (...)
  {
  }
}

/*
    Установка активного соединения
*/

template <class Serializer, class Deserializer>
inline void Context<Serializer, Deserializer>::SetConnection (const ConnectionPtr& in_connection)
{
  if (connection)
  {
    connection->DetachListener (this);
    connection = ConnectionPtr ();
  }

  if (in_connection)
    in_connection->AttachListener (this);

  connection = in_connection;
}

template <class Serializer, class Deserializer>
inline const typename Context<Serializer, Deserializer>::ConnectionPtr& Context<Serializer, Deserializer>::Connection () const
{
  return connection;
}

/*
    Отсылка исходящих команд на выполнение
*/

template <class Serializer, class Deserializer>
inline void Context<Serializer, Deserializer>::Flush ()
{
  try
  {
    if (!connection)
      return;

    CommandBuffer buffer = command_buffer_pool.CreateBuffer ();

    Serializer::Swap (buffer);

    try
    {
      connection->Send (buffer);
    }
    catch (...)
    {
      Serializer::Swap (buffer);
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::Context::Flush");
    throw;
  }
}

/*
    Обработка входящих команд
*/

template <class Serializer, class Deserializer>
inline void Context<Serializer, Deserializer>::ProcessIncomingCommands ()
{
  try
  {
    if (!connection)
      return;

    connection->ProcessIncomingCommands ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::Context::ProcessIncomingCommands");
    throw;
  }
}

template <class Serializer, class Deserializer>
inline void Context<Serializer, Deserializer>::OnIncomingCommands (const CommandBuffer& buffer)
{
  try
  {
    Deserializer::Reset (buffer);

    incoming_commands_processor->ProcessFeedback (*this, log); 
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::Context::OnIncomingCommands");
    throw;
  }
}
