class TargetConstantBuffer;

typedef xtl::com_ptr<TargetConstantBuffer> TargetConstantBufferPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Прототип целевого буфера
///////////////////////////////////////////////////////////////////////////////////////////////////
class TargetConstantBufferPrototype: public xtl::reference_counter, public xtl::trackable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    TargetConstantBufferPrototype  (ShaderLibrary& library, const ProgramParametersLayout& src_layout, const ProgramBufferLayout& dst_layout);
    ~TargetConstantBufferPrototype ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение буфера
///////////////////////////////////////////////////////////////////////////////////////////////////
    TargetConstantBuffer& GetBuffer (const SourceConstantBufferPtr buffers [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT], ShaderLibrary& library) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение списка индексов константных буферов
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char        GetSourceBuffersCount   () const { return indices.size (); }
    const unsigned char* GetSourceBuffersIndices () const { return indices.empty () ? (unsigned char*)0 : &indices [0]; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение минимального размера буфера для указанного слота
/// (без проверок корректности аргументов)
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetMinSourceBufferSize      (size_t index) const { return synchronizer.GetMinSourceBufferSize (indices [index]); }
    size_t GetMinDestinationBufferSize ()             const { return synchronizer.GetMinDestinationBufferSize (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Синхронизация
/// (без проверок корректности)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CopyTo (size_t index, const char* src_buffer, char* dst_buffer) const
    {
      synchronizer.CopyTo (indices [index], src_buffer, dst_buffer);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Биндинг
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind (ID3D11Buffer& buffer, ID3D11Buffer* context_buffers [ShaderType_Num][DEVICE_CONSTANT_BUFFER_SLOTS_COUNT]) const;

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание буфера
///////////////////////////////////////////////////////////////////////////////////////////////////
    TargetConstantBufferPtr CreateBuffer (const DeviceManager& device_manager, const SourceConstantBufferPtr buffers [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT]) const;

  private:
    typedef stl::vector<unsigned char> IndexArray;
    typedef ProgramBufferLayout::Slot  Slot;

  private:
    ShaderBuffersSynchronizer& synchronizer; //синхронизатор
    const ProgramBufferLayout& dst_layout;   //целевой лэйаут
    IndexArray                 indices;      //индексы
    size_t                     slots_count;  //количество слотов
    const Slot*                slots;        //слоты
};

typedef xtl::intrusive_ptr<TargetConstantBufferPrototype> TargetConstantBufferPrototypePtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Целевой буфер
///////////////////////////////////////////////////////////////////////////////////////////////////
class TargetConstantBuffer: public DeviceObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    TargetConstantBuffer  (const DeviceManager&, const TargetConstantBufferPrototype& prototype, const SourceConstantBufferPtr buffers [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT]);
    ~TargetConstantBuffer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Биндинг
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind (ID3D11DeviceContext&, ID3D11Buffer* context_buffers [ShaderType_Num][DEVICE_CONSTANT_BUFFER_SLOTS_COUNT]);

  private:
    struct SourceBufferDesc
    {
      SourceConstantBuffer* buffer; //исходный буфер
      size_t                hash;   //хэш искходного буфера

      SourceBufferDesc (SourceConstantBuffer* in_buffer)
        : buffer (in_buffer)
        , hash ()
      {
      }
    };

    typedef stl::vector<SourceBufferDesc> BufferDescArray;

  private:
    const TargetConstantBufferPrototype& prototype;   //прототип
    DxBufferPtr                          dst_buffer;  //целевой буфер  
    BufferDescArray                      src_buffers; //исходные буферы
};
