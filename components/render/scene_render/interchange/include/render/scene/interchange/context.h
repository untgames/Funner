#ifndef RENDER_SCENE_INTERCHANGE_CONTEXT_HEADER
#define RENDER_SCENE_INTERCHANGE_CONTEXT_HEADER

#include <common/log.h>

#include <render/scene/interchange/connection.h>
#include <render/scene/interchange/streams.h>

namespace render
{

namespace scene
{

namespace interchange
{

namespace detail
{

//implementation forwards
template <class Deserializer> class IIncomingCommandsProcessor;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Контекст обработки потоков данных рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Serializer, class Deserializer>
class Context: public Serializer, private Deserializer, private IConnectionListener
{
  public:
    typedef xtl::com_ptr<IConnection> ConnectionPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Processor> Context (const ConnectionPtr&, Processor&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Отсылка исходящих команд на выполнение
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Flush ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработка входящих команд
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ProcessIncomingCommands ();

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработка входящих команд
///////////////////////////////////////////////////////////////////////////////////////////////////
    void OnIncomingCommands (const CommandBuffer&);

  private:
    typedef detail::IIncomingCommandsProcessor<Deserializer> IncomingCommandsProcessor;
    typedef stl::auto_ptr<IncomingCommandsProcessor>         IncomingCommandsProcessorPtr;

  private:
    ConnectionPtr                connection;
    CommandBufferPool            command_buffer_pool;
    IncomingCommandsProcessorPtr incoming_commands_processor;
    common::Log                  log;
};

#include <render/scene/interchange/context.inl>

}

}

}

#endif
