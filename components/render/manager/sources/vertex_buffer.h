class InputLayoutManager;
class PrimitiveBuffersImpl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вершинный буфер
///////////////////////////////////////////////////////////////////////////////////////////////////
class VertexBuffer: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    VertexBuffer (const media::geometry::VertexBuffer& source, PrimitiveBuffersImpl& buffers, const DeviceManagerPtr& device_manager, MeshBufferUsage usage, bool updatable);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Вершинные потоки
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int             StreamsCount ();
    const LowLevelBufferPtr* Streams      ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение лэйаута
///////////////////////////////////////////////////////////////////////////////////////////////////
    LowLevelInputLayoutPtr CreateInputLayout (InputLayoutManager& layout_manager, low_level::InputDataType type);

  private:
    media::geometry::VertexFormat Clone (InputLayoutManager& manager, const media::geometry::VertexFormat& format);
    
  private:
    typedef stl::vector<LowLevelBufferPtr>              BufferArray;
    typedef stl::vector<low_level::VertexAttribute>     VertexAttributeArray;  
    typedef stl::vector<media::geometry::VertexFormat>  VertexFormatArray;  

  private:
    BufferArray            streams;
    VertexAttributeArray   attributes;
    VertexFormatArray      vertex_formats;
    size_t                 attributes_hash;
    LowLevelInputLayoutPtr layouts [low_level::InputDataType_Num];
};

typedef xtl::intrusive_ptr<VertexBuffer> VertexBufferPtr;
