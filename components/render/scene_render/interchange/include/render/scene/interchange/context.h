#ifndef RENDER_SCENE_CONTEXT_HEADER
#define RENDER_SCENE_CONTEXT_HEADER

//in progress

namespace render
{

namespace scene
{

template <class Serializer, class Deserializer>
class Context: public Serializer, private Deserializer, private IConnectionListener
{
  public:
    template <class Processor> Context (const ConnectionPtr&, Processor&);

    void Flush ();

    void WaitFeedback ();

  private:
    void OnFeedback (const CommandBuffer&);

  private:
    typedef IFeedbackProcessor<Deserializer> FeedbackProcessor;
    typedef stl::auto_ptr<FeedbackProcessor> FeedbackProcessorPtr;

  private:
///todo: impl
    ConnectionPtr connection;
    FeedbackProcessorPtr feedback_processor;
};

template <class Deserializer> class IFeedbackProcessor
{
  public:
    virtual ~IFeedbackProcessor () {}

    virtual void ProcessFeedback (Deserializer& deserializer) = 0;
};

template <class Processor, class Deserializer> class FeedbackProcessor: public IFeedbackProcessor<Deserializer>
{
  public:
    FeedbackProcessor (Processor* in_processor) : processor (in_processor) {}

    void ProcessFeedback (Deserializer& deserializer)
    {
      while (Command* command = deserializer.DeserializerHeader ())
      {
        if (deserializer.Available () < command->command_size - sizeof (Command))
          throw exception; 

        if (!deserializer.Deserialize (command->command_id, *processor))
          log error about unknown command id and skip;
      }
    }

  private:
    Processor* processor;
};

}

}

#endif
