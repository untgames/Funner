struct BindableProgramContext;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Программа, устанавливаемая в контекст
///////////////////////////////////////////////////////////////////////////////////////////////////
class BindableProgram: public DeviceObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    BindableProgram  (ShaderLibrary& library, const Program& program, const ProgramParametersLayout* layout);
    ~BindableProgram ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение программы и лэйаута
///////////////////////////////////////////////////////////////////////////////////////////////////
    const Program&                 GetProgram                 () const { return program; }
    const ProgramParametersLayout* GetProgramParametersLayout () const { return parameters_layout; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Биндинг в контекст
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind (ID3D11DeviceContext&          context,
               ShaderLibrary&                library,
               const SourceConstantBufferPtr buffers [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT],
               const InputLayout&            input_layout,
               BindableProgramContext&       program_context);

  private:
    typedef stl::vector<TargetConstantBufferPrototypePtr> BufferPrototypeArray;
    
  private:
    const Program&                 program;           //исходная программа
    const ProgramParametersLayout* parameters_layout; //лэйаут параметров программы (может быть нулевым)
    BufferPrototypeArray           buffer_prototypes; //прототипы буферов
};

typedef xtl::intrusive_ptr<BindableProgram> BindableProgramPtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Контекст приклепляемой программы
///////////////////////////////////////////////////////////////////////////////////////////////////
struct BindableProgramContext
{
  bool             has_dirty_buffers;                                  //присутствуют грязные буферы
  bool             program_binded;                                     //программа установлена в контекст
  bool             dirty_buffers [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT]; //"грязные" буферы
  DxInputLayoutPtr input_layout;                                       //входной лэйаут  

/// Конструктор
  BindableProgramContext ()
  {
    Reset ();
  }

/// Сброс контекста
  void Reset ()
  {
    program_binded    = false;
    has_dirty_buffers = true;

    for (size_t i=0; i<DEVICE_CONSTANT_BUFFER_SLOTS_COUNT; i++)
      dirty_buffers [i] = true;

    input_layout = DxInputLayoutPtr ();
  }
};
