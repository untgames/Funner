#ifndef RENDER_SCENE_INTERCHANGE_CONTEXT_HEADER
#define RENDER_SCENE_INTERCHANGE_CONTEXT_HEADER

#include <stl/auto_ptr.h>

#include <xtl/trackable_ptr.h>

#include <common/log.h>

#include <render/scene/interchange/command_buffer.h>
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
class Context: public Serializer, private Deserializer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Processor> Context  (Processor&);
                               ~Context ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Отсылка исходящих команд на выполнение
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Flush ();

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработка входящих команд
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ProcessCommands (const CommandBuffer&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка связанного соединения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void         SetCounterparty (IConnection* connection);
    IConnection* Counterparty () const;

  private:
    Context (const Context&); //no impl
    Context& operator = (const Context&); //no impl

  private:
    typedef detail::IIncomingCommandsProcessor<Deserializer> IncomingCommandsProcessor;
    typedef stl::auto_ptr<IncomingCommandsProcessor>         IncomingCommandsProcessorPtr;
    typedef xtl::trackable_ptr<IConnection>                  ConnectionPtr;

  private:
    ConnectionPtr                connection;
    CommandBufferPool            command_buffer_pool;
    IncomingCommandsProcessorPtr incoming_commands_processor;
    common::Log                  log;
};

#include <render/scene/interchange/detail/context.inl>

}

}

}

#endif
