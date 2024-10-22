///////////////////////////////////////////////////////////////////////////////////////////////////
///Программа
///////////////////////////////////////////////////////////////////////////////////////////////////
class Program: virtual public IProgram, public DeviceObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Program  (ShaderLibrary& library, size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log);
    ~Program ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка программы в контекст
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind (ID3D11DeviceContext& context) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перечисление константных буферов
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t                     GetConstantBufferLayoutsCount () const { return buffer_layouts.size (); }
    const ProgramBufferLayout& GetConstantBufferLayout       (size_t index) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение входного лэйаута
///////////////////////////////////////////////////////////////////////////////////////////////////
    DxInputLayoutPtr GetInputLayout (ShaderLibrary& library, const InputLayout& input_layout) const;

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание входного лэйаута
///////////////////////////////////////////////////////////////////////////////////////////////////
    DxInputLayoutPtr CreateInputLayout (const D3D11_INPUT_ELEMENT_DESC* descs, size_t descs_count) const;

  private:
    struct ShaderSlot
    {
      ShaderPtr          holder;
      ID3D11DeviceChild* shader;

      ShaderSlot () : shader () {}
    };

    template <class T, ShaderType Type> T* Get () const;

    typedef stl::vector<ProgramBufferLayoutPtr> BufferLayoutArray;

  private:
    ShaderSlot        shaders [ShaderType_Num]; //использованные шейдеры
    BufferLayoutArray buffer_layouts;           //лэйауты буферов
    size_t            vertex_shader_code_hash;  //хэш кода вершинного шейдера
};
