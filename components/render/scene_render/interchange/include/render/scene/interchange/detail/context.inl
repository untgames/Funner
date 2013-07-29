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
        Command& command   = read (deserializer, xtl::type<Command> ());
        size_t   body_size = command.command_size - sizeof (Command);

        if (deserializer.Available () < body_size)
        {
          const char* command_name = Deserializer::GetCommandName (command.command_id);

          if (command_name) log.Printf ("Can't read command %s with size %u: only %u bytes is available", command_name, command.command_size, body_size);
          else              log.Printf ("Can't read command %u with size %u: only %u bytes is available", command.command_id, command.command_size, body_size);

          return; //no more data
        }

        if (!deserializer.Deserialize (command.command_id, *processor))
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
